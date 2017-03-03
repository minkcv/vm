# Vapor Spec
A virtual machine with no physical hardware spec. **The vm behavior is not yet finalized so please do not start development on a program yet.**

#### The vm has:
- A 16 bit instruction size
- 16 different general purpose registers
- 65536 bytes (64K) of addressable memory
- An 8-bit color depth screen for a palette of 256 colors
- Support for 256 concurrent sprites
- Support for sprites with 4 colors or 3 colors + alpha
- A screen resolution of 256 x 192
- An assembler to accompany it
- A maximum of 65536 instructions per program
- A speed of 500,000 Instructions per Second (0.5 MIPS)

### Why did you make this? Who is it for?
This is a hobby project to allow myself and others to write games in assembly without some of the annoyances that physical architectures impose.

This project can be viewed as a challenge to people like  [demoscene](https://en.wikipedia.org/wiki/Demoscene) members. Please let me know if you make a program.

### How do I build and run it?
#### Linux:
Install SDL1.2 if you don't already have it. Systems with apt should try:

    sudo apt install libsdl1.2-dev
Then run:

    make
To test the program:

    ./testvm.sh
#### Other Operating Systems:
No other operating systems are currently supported. You can wait for me to implement the vm for them or do it yourself based on the spec. The point of the vm is that all programs can be ported to other platforms simply by reimplementing the vm.

### How do I make a program/game?
1. Wait until the vm is fully implemented and the specifications finalized.
2. Have some assembly programming knowledge.
3. Check out the `/docs` folder.
