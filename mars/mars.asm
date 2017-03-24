; This is a test program
; These are comments
;
; Enable the GPU
LRC r0 #127 ; GPU flag seg
LRC r1 #17 ; GPU flag offset
LRC r3 $1
STR r3 r0 r1

; Create a sprite attribute
LRC r0 #65 ; Segment address (draw the player on top of sprites in the 64 segment
LRC r1 #0 ; Byte address
LRC r3 #144; flags for sprite attrs: binary: 1001-0000
STR r3 r0 r1 ; set the flags
LRC r1 #1
LRC r3 #120 ; start x
STR r3 r0 r1
LRC r1 #2
LRC r3 #88 ; start y
STR r3 r0 r1
LRC r1 #3 ; byte address of width
LRC r3 #16
STR r3 r0 r1 ; set width
LRC r1 #4 ; byte address of height
STR r3 r0 r1 ; set height
LRC r4 #5 ; offset address of sprite segment address in sprite attribute
LRC r5 #128 ; Segment address of sprite
STR r5 r0 r4
LRC r4 #7 ; Byte address of color 0 in sprite
LRC r5 #255 ; Color
STR r5 r0 r4
LRC r4 #8 ; color 1 offset
LRC r5 #54 ; Color
STR r5 r0 r4
LRC r4 #9 ; color 2 offset
LRC r5 #11 ; Color
STR r5 r0 r4
; stars sprite attribute
; multiple times
; r6 is our looping index
; r2 is 16 * r6
; r7 is our max loops
LRC r2 #0
LRC r6 #0
LRC r7 #8
LRC r0 #64 ; below the player
@stars
LRC r0 #64
LRC r1 #16
ADD r1 r1 r2
LRC r3 #144
STR r3 r0 r1 ; flags
LRC r1 #17
ADD r1 r1 r2
LRC r3 #0
; move 32 * r6 right
ADD r3 r3 r2
ADD r3 r3 r2
STR r3 r0 r1 ; x
LRC r1 #19
ADD r1 r1 r2
LRC r3 #32
STR r3 r0 r1 ; width
LRC r1 #20
ADD r1 r1 r2
LRC r3 #64
STR r3 r0 r1 ; height
LRC r4 #21
ADD r4 r4 r2
LRC r5 #128
STR r5 r0 r4 ; segment
LRC r4 #22
ADD r4 r4 r2
LRC r5 #64
STR r5 r0 r4 ; offset
LRC r4 #23
ADD r4 r4 r2
LRC r5 #0
STR r5 r0 r4 ; color 0
LRC r4 #24
ADD r4 r4 r2
LRC r5 #255
STR r5 r0 r4 ; color 1
LRC r4 #25
ADD r4 r4 r2
LRC r5 #254
STR r5 r0 r4 ; color 2
ADDC r2 #16
ADDC r6 #1
CMP r8 r6 r7
LRL r14 r15 @stars
JLT r8 r14 r15
; rock sprite
LRC r1 #144 ; sprite attr offset, still segment 64
LRC r3 #144 ; 1001-0000
STR r3 r0 r1 ; flags
LRC r1 #145
LRC r3 #32
STR r3 r0 r1 ; x
LRC r1 #146
LRC r3 #96
STR r3 r0 r1 ; y
LRC r1 #147
LRC r3 #16
STR r3 r0 r1 ; width
LRC r1 #148
STR r3 r0 r1 ; height
LRC r1 #149
LRC r3 #130
STR r3 r0 r1 ; segment
LRC r1 #150
LRC r3 #64
STR r3 r0 r1 ; offset
LRC r1 #151
LRC r3 #96
STR r3 r0 r1 ; color 0
LRC r1 #152
LRC r3 #164
STR r3 r0 r1 ; color 1
; background color
LRC r0 #127
LRC r1 #16
LRC r2 #128 ; Color: dark red
STR r2 r0 r1
; Load the joystick 1 button address
LRC r0 #127
LRC r1 #0
; Loop forever until the user clicks the x
@forever; comments after label work
; comments below label work
LRC r3 #2 ; 0000 0010
LDR r2 r0 r1 ; r2 now holds the button states of joystick 1
AND r2 r2 r3
CMP r4 r2 r3 ; r4 is "equal" if joystick 1 up button is pressed
LRL r9 r10 @movedown
JEQ r4 r9 r10
@donemovedown
LDR r2 r0 r1 ; r2 now holds the button states of joystick 1
LRC r3 #1 ; 0000 0001
AND r2 r2 r3
CMP r4 r2 r3
LRL r9 r10 @moveup
JEQ r4 r9 r10
@donemoveup
LDR r2 r0 r1 ; r2 now holds the button states of joystick 1
LRC r3 #4 ; 0000 0100
AND r2 r2 r3
CMP r4 r2 r3
LRL r9 r10 @moveleft
JEQ r4 r9 r10
@donemoveleft
LDR r2 r0 r1 ; r2 now holds the button states of joystick 1
LRC r3 #8 ; 0000 1000
AND r2 r2 r3
CMP r4 r2 r3
LRL r9 r10 @moveright
JEQ r4 r9 r10
@donemoveright
NOP ; test nop
LRL r9 r10 @waitscreen
JMP r9 r10
@donewaitscreen
LRC r12 #127
LRC r13 #17
LDR r15 r12 r13 ; remember the current gpu flags
LRL r9 r10 @forever; back up
JMP r9 r10
;
@movedown
LRL r9 r10 @donemovedown
LRC r5 #65
LRC r6 #2
LDR r7 r5 r6
LRC r4 #174 ; 192 - 16 - 2
CMP r4 r7 r4
JGT r4 r9 r10
LRC r8 #2 ; move speed 2
ADD r7 r7 r8
STR r7 r5 r6
JMP r9 r10
;
@moveup
LRL r9 r10 @donemoveup
LRC r5 #65
LRC r6 #2
LDR r7 r5 r6
LRC r4 #64 ; sky tile height
CMP r4 r7 r4
JLT r4 r9 r10
LRC r8 #2 ; move speed 2
SUB r7 r7 r8
STR r7 r5 r6
JMP r9 r10
;
@moveleft
LRL r9 r10 @donemoveleft
LRC r5 #65
LRC r6 #1
LDR r7 r5 r6
LRC r4 #0
CMP r4 r7 r4
JEQ r4 r9 r10
LRC r8 #2 ; move speed 2
SUB r7 r7 r8
STR r7 r5 r6
JMP r9 r10
;
@moveright
LRL r9 r10 @donemoveright
LRC r5 #65
LRC r6 #1
LDR r7 r5 r6
LRC r4 #240 ; 256 - 16
CMP r4 r7 r4
JEQ r4 r9 r10
LRC r8 #2 ; move speed 2
ADD r7 r7 r8
STR r7 r5 r6
JMP r9 r10
;
@waitscreen
NOP ; easy to see in printouts
NOP
NOP
NOP
NOP
NOP
NOP
NOP
LRC r12 #127 ; gpu flags
LRC r13 #17
LDR r14 r12 r13
LRC r12 $2 ; 0000 0010
AND r14 r12 r14 ; mask r14 to 0000 0010
AND r15 r12 r15 ; mask r15 to 0000 0010
XOR r14 r14 r15 ; see if r14 and r15 differ
CMP r12 r14 r12
LRL r9 r10 @donewaitscreen
JEQ r12 r9 r10
LRL r9 r10 @waitscreen
JMP r9 r10
