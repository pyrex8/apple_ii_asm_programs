; Adapted from Bouncing Balls in Hires
; Advanced 6502 Assembly Programming for the Apple II, by Stephen Edwards
; http://www.cs.columbia.edu/~sedwards/2020-vcfw-6502/

cout = $FDED   ; Define cout label (character out)

; Numeric constants

NBALLS      = 30            ; Number of balls to bounce
COLUMNS     = 40            ; Number of columns/bytes per row
WHITE       = $7F           ; White hires byte
RKEY        = $D2           ; Code for when "R" is pressed
RWALL       = $f700
LWALL       = $0400
BOTTOM      = $B700
BOTTOM2     = $6E00         ; Bottom * 2, for bouncing

; Addresses

BALL        = $6            ; Current ball number
GBASL       = $26           ; Graphics base address
GBASH       = $27
HCOLOR1     = $1c           ; Color value
HGRX        = $e0           ; two-byte value
HGRY        = $e2

HGR1SCRN    = $2000         ; Start of hires page 1

KBD         = $c000         ; key code when MSB set
KBDSTRB     = $c010         ; clear keyboard buffer
TXTCLR      = $c050         ; graphics mode
TXTSET      = $c051         ; text mode
LOWSCR      = $c054         ; page 1
HIRES       = $c057         ; hires mode

balls:
            jsr hclear

; Draw the boundary

            lda #WHITE
            ldy #0
            jsr hline	; White line on the top row
            ldy #191
            jsr hline	; White line on the bottom row
            lda #%00000011
            ldy #0
            jsr vline	; White line on the left column
            lda #%01100000
            ldy #COLUMNS-1
            jsr vline	; White line on the right column



            ; Draw a horizontal line
            ; A = color byte to repeat, e.g., $7F
            ; Y = row (0-191) ($FF on exit)
            ;
            ; Uses GBASL, GBASH

hline:
            pha
            lda LKLO,y
            sta GBASL
            lda LKHI,y
            sta GBASH
            ldy #COLUMNS-1	; Width of screen in bytes
            pla
hl1:
            sta (GBASL),y
            dey
            bpl hl1
            rts

            ; Draw a vertical line on all but the topmost and bottom most rows
            ; A = byte to write in each position
            ; Y = column
            ;
            ; Uses GBASL, GBASH, HCOLOR1

vline:
            sta HCOLOR1
            ldx #190	; Start at second-to-last row
vl1:
            lda LKLO,x	; Get the row address
            sta GBASL
            lda LKHI,x
            sta GBASH
            lda HCOLOR1
            sta (GBASL),y	; Write the color byte
            dex 		; Previous row
            bne vl1
            rts

            ; Clear and switch to hires page 1
            ; Uses GBASL and GBASH

hclear:
            ldx #>HGR1SCRN	; $20, also the number of pages to clear
            stx GBASH
            lda #0		; Clear to black
            sta GBASL
            tay
hclr1:
            sta (GBASL),y
            iny
            bne hclr1	; Done with the page?
            inc GBASH
            dex
            bne hclr1	; Done with all pages?

            bit HIRES	; Switch to hires mode
            bit TXTCLR
            rts



            ; Hires row address lookup tables, after Pelczarski, Graphically Speaking

LKHI:
            .byte $20, $24, $28, $2c, $30, $34, $38, $3c, $20, $24, $28, $2c, $30, $34, $38, $3c
            .byte $21, $25, $29, $2d, $31, $35, $39, $3d, $21, $25, $29, $2d, $31, $35, $39, $3d
            .byte $22, $26, $2a, $2e, $32, $36, $3a, $3e, $22, $26, $2a, $2e, $32, $36, $3a, $3e
            .byte $23, $27, $2b, $2f, $33, $37, $3b, $3f, $23, $27, $2b, $2f, $33, $37, $3b, $3f
            .byte $20, $24, $28, $2c, $30, $34, $38, $3c, $20, $24, $28, $2c, $30, $34, $38, $3c
            .byte $21, $25, $29, $2d, $31, $35, $39, $3d, $21, $25, $29, $2d, $31, $35, $39, $3d
            .byte $22, $26, $2a, $2e, $32, $36, $3a, $3e, $22, $26, $2a, $2e, $32, $36, $3a, $3e
            .byte $23, $27, $2b, $2f, $33, $37, $3b, $3f, $23, $27, $2b, $2f, $33, $37, $3b, $3f
            .byte $20, $24, $28, $2c, $30, $34, $38, $3c, $20, $24, $28, $2c, $30, $34, $38, $3c
            .byte $21, $25, $29, $2d, $31, $35, $39, $3d, $21, $25, $29, $2d, $31, $35, $39, $3d
            .byte $22, $26, $2a, $2e, $32, $36, $3a, $3e, $22, $26, $2a, $2e, $32, $36, $3a, $3e
            .byte $23, $27, $2b, $2f, $33, $37, $3b, $3f, $23, $27, $2b, $2f, $33, $37, $3b, $3f

LKLO:
            .byte $00, $00, $00, $00, $00, $00, $00, $00, $80, $80, $80, $80, $80, $80, $80, $80
            .byte $00, $00, $00, $00, $00, $00, $00, $00, $80, $80, $80, $80, $80, $80, $80, $80
            .byte $00, $00, $00, $00, $00, $00, $00, $00, $80, $80, $80, $80, $80, $80, $80, $80
            .byte $00, $00, $00, $00, $00, $00, $00, $00, $80, $80, $80, $80, $80, $80, $80, $80
            .byte $28, $28, $28, $28, $28, $28, $28, $28, $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8
            .byte $28, $28, $28, $28, $28, $28, $28, $28, $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8
            .byte $28, $28, $28, $28, $28, $28, $28, $28, $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8
            .byte $28, $28, $28, $28, $28, $28, $28, $28, $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8
            .byte $50, $50, $50, $50, $50, $50, $50, $50, $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0
            .byte $50, $50, $50, $50, $50, $50, $50, $50, $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0
            .byte $50, $50, $50, $50, $50, $50, $50, $50, $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0
            .byte $50, $50, $50, $50, $50, $50, $50, $50, $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0
