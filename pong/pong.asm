; Pong for Vapor Spec VM
; 2017 Will Smith (minkcv)

; Joystick 1 controls the right paddle
; Joystick 2 controls the left paddle

; Program memory map (0.0 - 63.255)
; 0.0: Saved GPU flags - updated every refresh
; 0.1: Left paddle y position
; 0.2: Right paddle y position
; 0.3: Ball x position
; 0.4: Ball y position
; 0.5: Ball x direction 0=left 2=right
; 0.6: Ball y direction 0=up 2=down

; Note: I like to leave r0 as zero, it's handy.

; Pong is just white on black so we don't even need sprite data,
; just set 00 as white in the sprite color map.

; Left paddle sprite
LRC r1 #64 ; Segment of sprite attributes
LRC r2 #128 ; 1000-0000 sprite attribute flags
STR r2 r1 r0 ; Set the flags
LRC r2 #1 ; Offset of sprite x position
LRC r3 #8 ; Sprite x position
STR r3 r1 r2 ; Set the x position to 8
LRC r2 #2 ; Offset of the sprite y position
LRC r3 #64 ; Start y position
STR r3 r1 r2 ; Set the y position to 64
LRC r2 #3 ; Offset of sprite width
LRC r3 #8 ; Width
STR r3 r1 r2 ; Set the width to 8
LRC r2 #4 ; Offset of sprite height
LRC r3 #64 ; Height, 64 is 1/3 of the screen height
STR r3 r1 r2 ; Set the height to 64
LRC r2 #5 ; Offset of sprite segment addresses
LRC r3 #128 ; Start of ROM
STR r3 r1 r2 ; Set the sprite's segment addresses
LRC r2 #7 ; Offset of color 0 in sprite color map
LRC r3 #255 ; White
STR r3 r1 r2 ; Set the 00 color in the color map to white

; Right paddle sprite
LRC r2 #16 ; Offset of sprite flags in second sprite attribute
LRC r3 #128 ; 1000-0000 sprite attribute flags
STR r3 r1 r2 ; Set the flags
LRC r2 #17 ; Offset of sprite x position
LRC r3 #240 ; Sprite x position
STR r3 r1 r2 ; Set the x position to 240
LRC r2 #18 ; Offset of the sprite y position
LRC r3 #64 ; Start y position
STR r3 r1 r2 ; Set the y position to 64
LRC r2 #19 ; Offset of sprite width
LRC r3 #8 ; Width
STR r3 r1 r2 ; Set the width to 8
LRC r2 #20 ; Offset of sprite height
LRC r3 #64 ; Height, 64 is 1/3 of the screen height
STR r3 r1 r2 ; Set the height to 64
LRC r2 #21 ; Offset of sprite segment addresses
LRC r3 #128 ; Start of ROM
STR r3 r1 r2 ; Set the sprite's segment addresses
LRC r2 #23 ; Offset of color 0 in sprite color map
LRC r3 #255 ; White
STR r3 r1 r2 ; Set the 00 color in the color map to white

; Ball sprite
LRC r2 #32 ; Offset of sprite flags in third sprite attribute
LRC r3 #128 ; 1000-0000 sprite attribute flags
STR r3 r1 r2 ; Set the flags
LRC r2 #33 ; Offset of sprite x position
LRC r3 #124 ; Sprite x position 256 / 2 - 4 (screen width / 2 - sprite width / 2)
STR r3 r1 r2 ; Set the x position to 124
LRC r2 #34 ; Offset of the sprite y position
LRC r3 #92 ; Start y position 192 / 2 - 4
STR r3 r1 r2 ; Set the y position to 92
LRC r2 #35 ; Offset of sprite width
LRC r3 #8 ; Width
STR r3 r1 r2 ; Set the width to 8
LRC r2 #36 ; Offset of sprite height
LRC r3 #8 ; Height
STR r3 r1 r2 ; Set the height to 8
LRC r2 #37 ; Offset of sprite segment addresses
LRC r3 #128 ; Start of ROM
STR r3 r1 r2 ; Set the sprite's segment addresses
LRC r2 #39 ; Offset of color 0 in sprite color map
LRC r3 #255 ; White
STR r3 r1 r2 ; Set the 00 color in the color map to white

; Enable the GPU
LRC r1 #127 ; GPU flag segment
LRC r2 #17 ; GPU flag offset
LRC r3 #1 ; 0000-0001
STR r3 r1 r2

@reset
; Initialize left and right paddle y position to 64
LRC r1 #1
LRC r2 #64
STR r2 r0 r1
LRC r1 #2
STR r2 r0 r1
; Initialize ball x and y position to 124 and 92 respectively
LRC r1 #3
LRC r2 #124
STR r2 r0 r1
LRC r1 #4
LRC r2 #92
STR r2 r0 r1

; The main game loop
; - Read the joystick buttons and move paddles
; - Ball movement and collisions
; - Update sprites
; - Repeat
@gameloop
; Right paddle movement
LRC r1 #127
LDR r2 r1 r0 ; Load the joystick 1 button states into r2
LRC r3 #1 ; 0000-0001 - Up button
AND r4 r2 r3 ; Mask off buttons besides up
CMP r4 r4 r3 ; r4 is 1 if up is pressed
LRL r14 r15 @j1up
JEQ r4 r14 r15
@returnj1up
LRC r1 #127
LDR r2 r1 r0 ; Load the joystick 1 button states into r2
LRC r3 #2 ; 0000-0010 - Down button
AND r4 r2 r3 ; Mask off buttons besides down
CMP r4 r4 r3
LRL r14 r15 @j1down
JEQ r4 r14 r15
@returnj1down
; Left paddle movement
LRC r1 #127
LRC r2 #1
LDR r3 r1 r2 ; Load the joystick 2 button states into r3
AND r4 r3 r2 ; Mask off buttons besides up
CMP r4 r4 r2 ; r4 is 1 if up is pressed
LRL r14 r15 @j2up
JEQ r4 r14 r15
@returnj2up
LRC r1 #127
LRC r2 #1
LDR r3 r1 r2 ; Load the joystick 2 button states into r3
LRC r2 #2 ; 0000-0010
AND r4 r3 r2
CMP r4 r4 r2 ; r4 is 1 if down is pressed
LRL r14 r15 @j2down
JEQ r4 r14 r15
@returnj2down
; Ball movement and collisions
; Jump to left or right movement based on direction
LRC r1 #5
LDR r2 r0 r1 ; Load ball x direction from RAM
LRL r14 r15 @moveballleft
JLT r2 r14 r15 ; Ball is moving left
@returnmoveballleft
LRC r1 #5
LDR r2 r0 r1 ; Load ball x direction from RAM
LRL r14 r15 @moveballright
JGT r2 r14 r15
@returnmoveballright
LRC r1 #6
LDR r2 r0 r1 ; Load ball y direction from RAM
LRL r14 r15 @moveballup
JLT r2 r14 r15
@returnmoveballup
LRC r1 #6
LDR r2 r0 r1 ; Load again
LRL r14 r15 @moveballdown
JGT r2 r14 r15
@returnmoveballdown
; Update paddle sprites
LRC r1 #1
LDR r2 r0 r1 ; Load left paddle y position
; Right paddle sprite position
LRC r1 #2
LDR r2 r0 r1 ; Load right paddle y position
LRC r3 #64 ; Sprite attribute segment
LRC r4 #18 ; Right paddle y position offset
STR r2 r3 r4
; Left paddle sprite position
LRC r1 #1
LDR r2 r0 r1 ; Load right paddle y position
LRC r4 #2 ; Left paddle y position offset
STR r2 r3 r4
; Update ball sprite
LRC r1 #3
LDR r2 r0 r1 ; Load ball x position from RAM
LRC r4 #33 ; Ball sprite x position offset
STR r2 r3 r4
LRC r1 #4
LDR r2 r0 r1 ; Load ball y position from RAM
LRC r4 #34 ; Ball sprite y position offset
STR r2 r3 r4
; Wait for screen refresh
LRL r14 r15 @waitscreen
JMP r14 r15
@returnwaitscreen
; Save the GPU flags to RAM
LRC r1 #127 ; GPU flags segment
LRC r2 #17 ; GPU flags offset
LDR r3 r1 r2 ; Load the current GPU flags
STR r3 r0 r0 ; Store them in RAM 0.0
LRL r14 r15 @gameloop
JMP r14 r15

@j1up ; Joystick 1 up button pressed
LRL r14 r15 @returnj1up
LRC r1 #2
LDR r2 r0 r1 ; Load 0.2 - right paddle y position
CMP r4 r2 r0
JEQ r4 r14 r15 ; Don't move up if we are at the top
SUBC r2 #4 ; Subtract 4 from the y position
STR r2 r0 r1 ; Write the y position back to memory
JMP r14 r15 ; Return

@j1down ; Joystick 1 down button pressed
LRL r14 r15 @returnj1down
LRC r1 #2
LDR r2 r0 r1 ; Load 0.2 - right paddle y position
LRC r3 #128 ; Screen height - sprite height
CMP r4 r2 r3
JEQ r4 r14 r15
ADDC r2 #4 ; Add 4 to the y position
STR r2 r0 r1 ; Write the y position back to memory
JMP r14 r15

@j2up
LRL r14 r15 @returnj2up
LRC r1 #1
LDR r2 r0 r1 ; Load 0.1 - left paddle y position
CMP r4 r2 r0
JEQ r4 r14 r15 ; Don't move up if we are at the top
SUBC r2 #4 ; Subtract 4 from the y position
STR r2 r0 r1 ; Write the y position back to memory
JMP r14 r15 ; Return

@j2down
LRL r14 r15 @returnj2down
LRC r1 #1
LDR r2 r0 r1 ; Load 0.1 - left paddle y position
LRC r3 #128 ; Screen height - sprite height
CMP r4 r2 r3
JEQ r4 r14 r15
ADDC r2 #4 ; Add 4 to the y position
STR r2 r0 r1 ; Write the y position back to memory
JMP r14 r15

@moveballleft
LRC r1 #3
LDR r2 r0 r1 ; Load ball x position into r2
LRC r3 #4
CMP r3 r2 r3
LRL r14 r15 @reset
JLT r3 r14 r15 ; Ball is past left paddle, reset
LRC r3 #16
CMP r3 r2 r3
LRL r14 r15 @nocollisionleft
JGT r3 r14 r15 ; Ball x is not left of the right side of the left paddle
LRC r1 #1
LDR r3 r0 r1 ; Load left paddle y into r3
LRC r1 #4
LDR r4 r0 r1 ; Load ball y into r4
ADDC r4 #8 ; r4 is now bottom y of ball
CMP r5 r4 r3 ; Compare ball bottom y with left paddle top y
LRC r1 #64 ; Paddle height
ADD r1 r1 r3 ; r1 is now the bottom y of the left paddle
SUBC r4 #8 ; r4 is now top y of ball
CMP r6 r4 r1 ; Compare ball y with left paddle bottom y
LRL r14 r15 @nocollisionleft
JLT r5 r14 r15 ; Above the left paddle
JGT r6 r14 r15 ; Below the left paddle
; Collision with left paddle, reverse x direction
LRC r1 #5
LRC r2 #2
STR r2 r0 r1 ; Set ball x direction to 2 (right)
LRL r14 r15 @returnmoveballleft
JMP r14 r15 ; Return
@nocollisionleft
; Move the ball left
LRC r1 #3
LDR r2 r0 r1 ; Load ball x position into r2
SUBC r2 #4 ; Move 4 pixels left
STR r2 r0 r1
LRL r14 r15 @returnmoveballleft
JMP r14 r15

@moveballright
LRC r1 #3
LDR r2 r0 r1 ; Load ball x position into r2
LRC r3 #244
CMP r3 r2 r3
LRL r14 r15 @reset
JGT r3 r14 r15 ; Ball is past right paddle, reset
LRC r3 #232
CMP r3 r2 r3
LRL r14 r15 @nocollisionright
JLT r3 r14 r15 ; Ball x is not right of the left side of the right paddle
LRC r1 #2
LDR r3 r0 r1 ; Load right paddle y into r3
LRC r1 #4
LDR r4 r0 r1 ; Load ball y into r4
ADDC r4 #8 ; r4 is now bottom y of ball
CMP r5 r4 r3 ; Compare ball bottom y with right paddle top y
LRC r1 #64 ; Paddle height
ADD r1 r1 r3 ; r1 is now the bottom y of the right paddle
SUBC r4 #8 ; r4 is now top y of ball
CMP r6 r4 r1 ; Compare ball y with right paddle bottom y
LRL r14 r15 @nocollisionright
JLT r5 r14 r15 ; Above the right paddle
JGT r6 r14 r15 ; Below the right paddle
; Collision with right paddle, reverse x direction
LRC r1 #5
STR r0 r0 r1 ; Set ball x direction to 0 (left)
LRL r14 r15 @returnmoveballright
JMP r14 r15 ; Return
@nocollisionright
; Move the ball right
LRC r1 #3
LDR r2 r0 r1 ; Load ball x position into r2
ADDC r2 #4 ; Move 4 pixels right
STR r2 r0 r1
LRL r14 r15 @returnmoveballright
JMP r14 r15

@moveballup
LRC r1 #4
LDR r2 r0 r1 ; Load ball y position into r2
CMP r1 r0 r2
LRL r14 r15 @nocollisiontop
JLT r1 r14 r15 ; Ball is below the top of the screen
; Collision with top of screen, reverse y direction
LRC r1 #6
LRC r2 #2
STR r2 r0 r1 ; Set y direction to 2 (down)
LRL r14 r15 @returnmoveballup
JMP r14 r15
@nocollisiontop
; Move the ball up
LRC r1 #4
LDR r2 r0 r1 ; Load ball y position into r2
SUBC r2 #4 ; Move 4 pixels up
STR r2 r0 r1
LRL r14 r15 @returnmoveballup
JMP r14 r15

@moveballdown
LRC r1 #4
LDR r2 r0 r1 ; Load ball y position into r2
LRC r3 #184 ; 192 - 8 (screen height - ball height)
CMP r1 r2 r3
LRL r14 r15 @nocollisionbottom
JLT r1 r14 r15 ; Ball is above the bottom of the screen
; Collision with top of screen, reverse y direction
LRC r1 #6
STR r0 r0 r1 ; Set y direction to 0 (up)
LRL r14 r15 @returnmoveballup
JMP r14 r15
@nocollisionbottom
; Move the ball down
LRC r1 #4
LDR r2 r0 r1 ; Load ball y position into r2
ADDC r2 #4 ; Move 4 pixels down
STR r2 r0 r1
LRL r14 r15 @returnmoveballdown
JMP r14 r15

; Function for synchronizing with screen refresh
@waitscreen
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
LRC r1 #127 ; GPU flags segment
LRC r2 #17 ; GPU flags offset
LDR r3 r1 r2 ; Load the current GPU flags into r3
LDR r4 r0 r0 ; Load the previously saved GPU flags into r4
LRC r1 $2 ; 0000 0010
AND r3 r1 r3 ; Mask r3 to 0000 0010
AND r4 r1 r4 ; Mask r4 to 0000 0010
XOR r3 r3 r4 ; See if r3 and r4 differ
CMP r1 r3 r1
LRL r14 r15 @returnwaitscreen
JEQ r1 r14 r15
LRL r14 r15 @waitscreen
JMP r14 r15 ; Keep waiting
