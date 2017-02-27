all: main assembler packrom

main: clean-main
	gcc main.c vm.c display.c gpu.c ipu.c `sdl-config --cflags --libs` -o main

assembler: clean-assembler
	gcc assembler.c -o assembler

packrom: clean-packrom
	gcc packrom.c -o packrom

clean-main:
	rm -f main

clean-assembler:
	rm -f assembler

clean-packrom:
	rm -f packrom

clean: clean-main clean-assembler clean-packrom
