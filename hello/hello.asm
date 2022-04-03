; Adapted from Advanced 6502 Assembly Programming for the Apple II
; by Stephen Edwards
; http://www.cs.columbia.edu/~sedwards/2020-vcfw-6502/

cout = $FDED   ; Define cout label (character out)

entry:
        ldx #0      ; Use X because cout leaves it unchanged
loop:
        lda msg, X  ; Get character from string
        beq done    ; Are we at the end?
        jsr cout    ; No: print the character
        inx         ; Go to next character in string
        bne loop    ; A trick: always taken
done:   rts         ; Return

msg:    .byte "Hello, World!"    ; Generate ASCII character codes
        .byte  $8D, 00           ; Define byte: 8D is return, 00 is end
