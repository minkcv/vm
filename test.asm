; This is a test program
; These are comments
; 0x907B
LRC r0 123
; 0x1001
ADD r0 r0 r1
; 0x000
LRC r0 0
STR r1 r0 r0
HALT ; There can be comments on instruction lines
; But there must be a space before the semicolon
