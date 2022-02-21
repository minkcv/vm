all: main assembler convertsprite

main: clean-main
	gcc main.c machine/vm.c machine/display.c machine/gpu.c machine/ipu.c machine/cpu.c `sdl2-config --cflags --libs` -o main  -Ilibretro-common/include

assembler: clean-assembler
	gcc assembler.c -o assembler -Ilibretro-common/include

convertsprite: clean-convertsprite
	gcc convertsprite.c -o convertsprite -lpng -lz -lm

clean-main:
	rm -f main

clean-assembler:
	rm -f assembler

clean-convertsprite:
	rm -f convertsprite

clean: clean-main clean-assembler clean-convertsprite
