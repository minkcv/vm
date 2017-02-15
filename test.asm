; This is a test program
; These are comments
;
; Create a sprite attribute
LRC r0 #64 ; Segment address
LRC r1 #0 ; Byte address
LRC r3 #128 ; flags for sprite attrs: binary: 1000-0000
STR r3 r0 r1 ; set the flags
LRC r1 #3 ; byte address of width
LRC r3 #16
STR r3 r0 r1 ; set width
LRC r1 #4 ; byte address of height
STR r3 r0 r1 ; set height
LRC r4 #5 ; Segment address of sprite segment address in sprite attribute
LRC r5 #127 ; Segment address of sprite
STR r5 r0 r4
LRC r4 #7 ; Byte address of color 1 in sprite
LRC r5 $FF ; Color: white
STR r5 r0 r4
LRC r0 #80
LRC r1 #16
LRC r2 #75
STR r2 r0 r1
;
; Loop forever until the user clicks the x 
@forever
JMP @forever
; LRC r0 $7B ; Same as #123, but in hex
; ADD r0 r0 r1
; LRC r0 #0
; JMP @end ; jump to label @end
; STR r1 r0 r0
; @end ; a label
HALT ; There can be comments on instruction lines
; But there must be a space before the semicolon
