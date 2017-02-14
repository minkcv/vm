all: main assembler

main: clean-main
	gcc main.c vm.c display.c gpu.c `sdl-config --cflags --libs` -o main

assembler: clean-assembler
	gcc assembler.c -o assembler

clean-main:
	rm -f main

clean-assembler:
	rm -f assembler

clean: clean-main clean-assembler
