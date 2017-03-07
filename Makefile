all: main assembler convertsprite

main: clean-main
	gcc main.c vm.c display.c gpu.c ipu.c `sdl-config --cflags --libs` -o main

assembler: clean-assembler
	gcc assembler.c -o assembler

convertsprite: clean-convertsprite
	gcc convertsprite.c -o convertsprite -lpng -lz

clean-main:
	rm -f main

clean-assembler:
	rm -f assembler

clean-convertsprite:
	rm -f convertsprite

clean: clean-main clean-assembler clean-convertsprite
