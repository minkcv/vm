#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <retro_endianness.h>
#include "libretro.h"
#include "constants.h"
#include "cpu.h"
#include "gpu.h"

#define VALID_EXTENSIONS "vaporbin"

#ifdef USE_RGB565
typedef unsigned short pixel_t;
#if defined(ABGR1555)
#define vRGB(r,g,b) ((((b) & 0xf8) << 7) | (((g) & 0xf8) << 2) | (((r) & 0xf8) >> 3))
#else
#define vRGB(r,g,b) ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))
#endif
#else
typedef unsigned int pixel_t;
#define vRGB(r,g,b) (((r) << 16) | ((g) << 8) | (b))
#endif
#define FPS 60
#define AUDIO_SAMPLE_RATE 44100
#define SAMPLES_PER_FRAME (AUDIO_SAMPLE_RATE / FPS)

static int16_t audio_buffer[2 * SAMPLES_PER_FRAME];

struct LIBRETRO_VM
{
    struct CPU cpu;
    // Memory is indexed by segment and then byte
    uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE];
    GPU* gpu;
};

static pixel_t frame[SCREEN_WIDTH * SCREEN_HEIGHT];

static retro_environment_t environ_cb;
static retro_log_printf_t log_cb;
static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;

static struct LIBRETRO_VM *vm;

static void fallback_log(enum retro_log_level level,
			 const char *fmt, ...) {
    va_list args;

    (void) level;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
   
static unsigned keyorder[] = {
    RETRO_DEVICE_ID_JOYPAD_UP,
    RETRO_DEVICE_ID_JOYPAD_DOWN,
    RETRO_DEVICE_ID_JOYPAD_LEFT,
    RETRO_DEVICE_ID_JOYPAD_RIGHT,
    RETRO_DEVICE_ID_JOYPAD_A,
    RETRO_DEVICE_ID_JOYPAD_B,
    RETRO_DEVICE_ID_JOYPAD_X,
    RETRO_DEVICE_ID_JOYPAD_Y,
};

static struct retro_input_descriptor input_desc[] = {
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "left" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "down" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "right" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "up" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "left" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "down" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "right" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "up" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
    { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },

    { 0 },
};

static struct LIBRETRO_VM* createVM(const uint16_t* code, uint8_t* rom, int debugMode)
{
    struct LIBRETRO_VM* ret_vm = (struct LIBRETRO_VM*)malloc(sizeof(struct LIBRETRO_VM));
    ret_vm->cpu.code = code;
    ret_vm->cpu.pc = code;
    ret_vm->cpu.debugMode = debugMode;
    ret_vm->cpu.breakState = 0;
    ret_vm->cpu.step = 0;
    ret_vm->gpu = createGPU();
    memset(ret_vm->memory, 0, sizeof(ret_vm->memory[0][0]) * MEMORY_SEGMENT_COUNT * MEMORY_SEGMENT_SIZE);
    memset(ret_vm->cpu.regs, 0, sizeof(ret_vm->cpu.regs[0]) * REGISTER_COUNT);
    if (rom != NULL) // ROM is optional
        memcpy(ret_vm->memory[128], rom, 128 * MEMORY_SEGMENT_SIZE);
    return ret_vm;
}

static void destroyVM(struct LIBRETRO_VM* a_vm)
{
    destroyGPU(a_vm->gpu);
    a_vm->gpu = NULL;
    free(a_vm);
}

void retro_set_environment(retro_environment_t fn)
{
    struct retro_log_callback log;

    environ_cb = fn;

    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
	log_cb = log.log;
    else
	log_cb = fallback_log;

    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, input_desc);
}

// Makes the physical screen match the emulator display.
static void draw_display(void)
{
    int x, y;

    updateGPU(vm->gpu, vm->memory);
    if (vm->gpu->active)
    {
	readSpritesFromMem(vm->gpu, vm->memory);
	drawSprites(vm->gpu, vm->memory);
	for (y = 0; y < SCREEN_HEIGHT; y++)
	{
	    uint8_t *iptr = vm->gpu->pixels + vm->gpu->pitch * y;
	    for (x = 0; x < SCREEN_WIDTH; x++)
	    {
		frame[x + y * SCREEN_WIDTH] = vRGB(iptr[0], iptr[1], iptr[2]);
		iptr += 3;
	    }
	}
    }
}

void retro_set_video_refresh(retro_video_refresh_t fn) { video_cb = fn; }
void retro_set_audio_sample(retro_audio_sample_t fn) { audio_cb = fn; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t fn) { audio_batch_cb = fn; }
void retro_set_input_poll(retro_input_poll_t fn) { input_poll_cb = fn; }
void retro_set_input_state(retro_input_state_t fn) { input_state_cb = fn; }

void retro_init(void)
{
}

static uint16_t *code_data = NULL;
static uint8_t *rom_data = NULL;

static void load_rom(void) {
    if (vm) {
	destroyVM(vm);
	vm = NULL;
    }
    vm = createVM(code_data, rom_data, false);
}

bool retro_load_game(const struct retro_game_info *info)
{
    const uint8_t *bundle;
    size_t bundle_size;
    size_t code_size;
    const uint8_t *rom_block = NULL;
    uint16_t rom_size = 0;

    /* If frontend does not support persistent
     * content data, must create a copy */
    if (!info || !info->data)
	return false;

    bundle = (const uint8_t *) info->data;
    bundle_size = info->size;
    code_size = 2 * (bundle[0] | (bundle[1] << 8));
    if (code_size + 2 > bundle_size) {
	log_cb(RETRO_LOG_ERROR, "Invalid bundle.\n");
	return false;
    }

    if (code_size + 4 < bundle_size) {
	rom_block = bundle + 2 + code_size;
	rom_size = rom_block[0] | (rom_block[1] << 8);
	if (code_size + 4 + rom_size > bundle_size) {
	    log_cb(RETRO_LOG_ERROR, "Invalid bundle.\n");
	    return false;
	}
    }

    code_data  = (uint16_t *) malloc(JUMP_SEGMENT_SIZE * JUMP_SEGMENT_SIZE * sizeof(uint16_t));
    rom_data  = (uint8_t *) malloc(128 * MEMORY_SEGMENT_SIZE * sizeof(uint16_t));

    if (!code_data || !rom_data) {
	log_cb(RETRO_LOG_ERROR, "Failed to allocate ROM buffer.\n");
	free(code_data);
	free(rom_data);
	return false;
    }

    memset(code_data, 0, JUMP_SEGMENT_SIZE * JUMP_SEGMENT_SIZE * sizeof(uint16_t));
    memset(rom_data, 0, 128 * MEMORY_SEGMENT_SIZE * sizeof(uint16_t));
	
    memcpy(code_data, bundle + 2, code_size);
    if (rom_size) {
	memcpy(rom_data, rom_block + 2, rom_size);
    }

    load_rom();

    return true;
}

void retro_unload_game(void)
{
    if (code_data)
	free(code_data);
    if (rom_data)
	free(rom_data);

    code_data = NULL;
    rom_data = NULL;

    if (vm) {
	destroyVM(vm);
	vm = NULL;
    }
}

void retro_run(void)
{
    unsigned i;
    uint8_t js1 = 0, js2 = 0;

    input_poll_cb();

    for (i = 0; i < 8; i++)
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, keyorder[i]))
	    js1 |= 1 << i;
    for (i = 0; i < 8; i++)
	if (input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, keyorder[i]))
	    js2 |= 1 << i;
    vm->memory[JOYSTICK_SEG][JOYSTICK_1_OFFSET] = js1;
    vm->memory[JOYSTICK_SEG][JOYSTICK_2_OFFSET] = js2;

    for (i = 0; i < 8064; i++) {
	uint16_t instr = retro_le_to_cpu16(*(vm->cpu.pc));
	Instruction decoded;
	decode(instr, &decoded);
	exec(&vm->cpu, &decoded, vm->memory);
	vm->cpu.pc++;
    }

    // Output video
    draw_display();
    video_cb(frame, SCREEN_WIDTH, SCREEN_HEIGHT, sizeof(pixel_t) * SCREEN_WIDTH);
    audio_batch_cb(audio_buffer, SAMPLES_PER_FRAME);
}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

void retro_get_system_info(struct retro_system_info *info)
{
    memset(info, 0, sizeof(*info));
    info->library_name = "Vaporspec";
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
    info->library_version = "1.0" GIT_VERSION;
    info->valid_extensions = VALID_EXTENSIONS;
    info->need_fullpath = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
#ifdef USE_RGB565
    int pixelformat = RETRO_PIXEL_FORMAT_RGB565;
#else
    int pixelformat = RETRO_PIXEL_FORMAT_XRGB8888;
#endif	

    memset(info, 0, sizeof(*info));
    info->geometry.base_width   = SCREEN_WIDTH;
    info->geometry.base_height  = SCREEN_HEIGHT;
    info->geometry.max_width    = SCREEN_WIDTH;
    info->geometry.max_height   = SCREEN_HEIGHT;
    info->geometry.aspect_ratio = ((float)SCREEN_WIDTH) / ((float)SCREEN_HEIGHT);

    info->timing.fps = FPS;
    info->timing.sample_rate = AUDIO_SAMPLE_RATE;

    environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixelformat);
}


void retro_deinit(void) {  }

void retro_reset(void)
{
    load_rom();
}

struct serialized_state
{
    uint32_t pc;
    uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE];
    uint8_t regs[REGISTER_COUNT];
};

size_t retro_serialize_size(void)
{
    return sizeof (struct serialized_state);
}

bool retro_serialize(void *data, size_t size)
{
    if (size < sizeof (struct serialized_state))
	return false;

    struct serialized_state *st = (struct serialized_state *) data;
    memcpy(&st->memory, &vm->memory, sizeof(st->memory));
    memcpy(&st->regs, &vm->cpu.regs, sizeof(st->regs));
    st->pc = vm->cpu.pc - vm->cpu.code;
    return true;
}

bool retro_unserialize(const void *data, size_t size)
{
    if (size < sizeof (struct serialized_state))
	return false;

    const struct serialized_state *st = (struct serialized_state *) data;
    memcpy(&vm->memory, &st->memory, sizeof(st->memory));
    memcpy(&vm->cpu.regs, &st->regs, sizeof(st->regs));
    vm->cpu.pc = vm->cpu.code + st->pc;
    return true;
}

size_t retro_get_memory_size(unsigned id)
{
    switch(id)
    {
    case RETRO_MEMORY_SYSTEM_RAM: // System Memory
	return 64 * MEMORY_SEGMENT_SIZE;

    case RETRO_MEMORY_VIDEO_RAM: // VRAM
	return 64 * MEMORY_SEGMENT_SIZE;
    }
    return 0;
}

void *retro_get_memory_data(unsigned id)
{
    switch(id)
    {
    case RETRO_MEMORY_SYSTEM_RAM: // System Memory
	return &vm->memory[0][0];

    case RETRO_MEMORY_VIDEO_RAM: // VRAM
	return &vm->memory[64][0];
    }
    return 0;
}

/* Stubs */
unsigned int retro_api_version(void) { return RETRO_API_VERSION; }
void retro_cheat_reset(void) {  }
void retro_cheat_set(unsigned idx, bool enabled, const char *code) {
    (void) idx;
    (void) enabled;
    (void) code;
}
bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) {
    (void) game_type;
    (void) info;
    (void) num_info;
    return false;
}
void retro_set_controller_port_device(unsigned port, unsigned device) {
    (void) port;
    (void) device;
}
