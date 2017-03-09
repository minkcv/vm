# Sample Code

## Enabling the GPU (rendering)
```
LRC r1 #127 ; Segment address of the GPU flags
LRC r2 #17 ; Offset of the GPU flags
LDR r3 r1 r2 ; Read the current flags
LRC r4 #1 ; Set the enabled flag: 0000 0001
OR  r3 r3 r4 ; Or the enabled flag with the existing flags to preserve them
STR r3 r1 r2 ; Write the flags to memory
```

## Changing the Background color
```
LRC r1 #127 ; Segment address of background color
LRC r2 #16 ; Offset address of background color
LRC r3 #75 ; Light blue (An index into the 256 screen color palette)
STR r3 r1 r2 ; Write the byte to memory
```

## Creating a Sprite Attribute
```
; Set the width and height
LRC r1 #64 ; Segment addresses of sprite attribute
LRC r2 #3 ; Offset address of sprite width
LRC r3 #16 ; Width and height of 16x16 sprite
STR r3 r1 r2 ; Write the width to memory
LRC r2 #4 ; Offset address of sprite height
STR r3 r1 r2 ; Write the height to memory

; Set the location of the sprite data
LRC r2 #5 ; Offset address of sprite data segment
LRC r4 #128 ; Segment address of sprite data
STR r4 r1 r2 ; Write sprite segment address to memory

LRC r2 #6 ; Offset address of sprite data offset
LRC r4 #0 ; Offset address of sprite data
STR r4 r1 r2 ; Write sprite offset address to memory

; Set the first color of the sprite palette to white
LRC r2 #7 ; Offset address of sprite color 0
LRC r4 $FF ; White (An index into the 256 screen color palette)
STR r4 r1 r2 ; Write the color to memory

; Enable the sprite
LRC r2 #0 ; Offset of sprite flags
LRC r3 #128 ; Active flag for the sprite attributes: 1000 0000
STR r3 r1 r2 ; Write the flags to memory
```
## Reading Joystick 1 Input and Jumping on Button Press
(Not jumping as in making a character jump, but as in jumping to a section of code)
```
LRC r1 #127 ; Joysticks segment address
LRC r2 #0 ; Joystick 1 offset address
LRC r3 #2 ; Joystick 1 down button pressed state 0000 0010
LDR r4 r1 r2 ; Read Joystick 1 buttons states from memory
AND r4 r4 r3 ; Mask off the other button states
CMP r5 r4 r3 ; Compare the masked bits with the "down pressed" state
LRL r9 r10 @movedown ; Load r9 and r10 with the address of label @movedown
JEQ r5 r9 r10 ; Jump to the @movedown label if the comparison returned true
@donemovedown ; Jump back here when done with @movedown code
```

## Waiting Until the Screen Refreshes
Without any sort of waiting, a program will run at 500,000 instructions per second and a human player won't be able to keep up. We can execute our desired update code and then wait until the GPU toggles the "refreshed" flag.
```
@gameloop
; Your game code here
LRL r1 r2 @waitscreen
JMP r1 r2
@donewaitscreen
LRC r1 #127 ; Segment address of GPU flags
LRC r2 #17 ; Offset address of GPU flags
LDR r15 r1 r2 ; Read the flags from memory
LRL r1 r2 @gameloop
JMP r1 r2 ; Repeat the loop

@waitscreen ; Jump here at the end of your game loop
NOP ; Waste some time
NOP
NOP
NOP
LRC r1 #127 ; Segment address of GPU flags
LRC r2 #17 ; Offset address of GPU flags
LDR r3 r1 r2 ; Read the flags from memory
LRC r4 $2 ; 0000 0010
AND r3 r3 r4 ; Mask the GPU flags
AND r15 r15 r4 ; Mask the previous GPU flags
XOR r3 r3 r15 ; See if the current state differs from the previous state
CMP r1 r3 r4 ; The XOR will result in 0000 0010 if the states differ
LRL r9 r10 @donewaitscreen
JEQ r1 r9 r10 ; Return to the game loop if the screen refreshed
LRL r9 r10 @waitscreen
JMP r9 r10 ; Keep running the waitscreen code until the screen refreshes
```
