; Adapted from Bouncing Balls in Hires
; Advanced 6502 Assembly Programming for the Apple II, by Stephen Edwards
; http://www.cs.columbia.edu/~sedwards/2020-vcfw-6502/

cout = $FDED   ; Define cout label (character out)

; Numeric constants

NBALLS      = 2             ; Number of balls to bounce
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

; Initialize the ball positions and speeds

; Zero the first four tables
        ldx #NBALLS * 4 - 1
        lda #0
init0:
        sta BALLXL,x
        dex
        bpl init0

; Initialize non-zero entries

        ldx #NBALLS-1
        ldy #1
initl:
        tya		; Linear feedback shift register
        lsr
        bcc skip
        eor #$c0
skip:
        tay
        and #$1F
        adc #1
        sta BALLXH,x
        sta BALLDX,x
        tya
        lsr
        adc #10
        sta BALLYH,x
        dex
        bpl initl

; Draw each ball

drawall:
        lda #NBALLS-1
        sta BALL
da1:
        jsr xorball
        dec BALL
        bpl da1

update:
        lda #NBALLS-1
        sta BALL
uploop:
        jsr xorball	; erase the ball in its current position
        ldx BALL

; Update horizontal position

        lda BALLXL,x
        clc
        adc BALLDX,x
        bpl noxunderflow

        adc #56		; Correct for underflow; carry is clear
        sta BALLXL,x
        dec BALLXH,x
        bne xdone	; Hit the left wall?
        beq bouncex	; Yes: bounce

noxunderflow:
        sta BALLXL,x
        sec
        sbc #56
        bcc xdone	; No overflow?

        sta BALLXL,x
        inc BALLXH,x
        ldy BALLXH,x
        cpy #COLUMNS-2	; Hit the right wall?
        bne xdone
bouncex:
        sec
        lda #0
        sbc BALLDX,x
        sta BALLDX,x
xdone:

; Update the ball's vertical position

; Apply gravity
        inc BALLDYH,x

; Update position
        clc
        lda BALLYL,x
        adc BALLDYL,x
        sta BALLYL,x
        lda BALLYH,x
        adc BALLDYH,x
        sta BALLYH,x

        cmp #>BOTTOM
        bcc nobounce

; BALLY = 2 * bottom - BALLY

        sec
        lda #<BOTTOM2
        sbc BALLYL,x
        sta BALLYL,x
        lda #>BOTTOM2
        sbc BALLYH,x
        sta BALLYH,x

        sec
        lda #0
        sbc BALLDYL,x
        sta BALLDYL,x
        lda #0
        sbc BALLDYH,x
        sta BALLDYH,x
nobounce:
        jsr xorball 	; draw the ball in its new position

        dec BALL
        bpl uploop

        bit KBD		; Key pressed?
        bmi pressed
        jmp update	; No, update everything

pressed:
        lda KBD
        cmp #RKEY	; Was R pressed?
        bne quit
        bit KBDSTRB	; Clear the key
        jmp balls	; Restart

quit:
        bit KBDSTRB	; Clear the key
        bit TXTSET	; Switch to text mode
        rts

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

; Draw or erase a ball
; BALL = ball number
; BALLYH+BALL = top row
; BALLXH+BALL = left column (byte offset)
; BALLXL+BALL = bits 4-6 select one of the 7 shifted versions
;
; Uses GBASL, GBASH, HGRY, HGRX

xorball:
        ldy BALL
        lda BALLYH,y	; Get row
        sta HGRY
        lda BALLXH,y	; Get column
        sta HGRX
        lda BALLXL,y	; Get Shift (0,8,...,48)
        and #$38
        tax		; Offset into sprite table (pixel * 8)

xsplot:
        ldy HGRY	; Get the row address
        lda LKLO,y
        sta GBASL
        lda LKHI,y
        sta GBASH
        iny
        sty HGRY

        ldy HGRX	; XOR the two bytes onto the screen

; (GBASL, GBASH) = row address
; Y = byte offset into the row
; X = index into sprite tables

        lda (GBASL),y
        eor BALL0,x
        sta (GBASL),y
        iny
        lda (GBASL),y
        eor BALL1,x
        sta (GBASL),y
        inx

        txa
        and #7
        bne xsplot	; Stop at a multiple of 8 bytes
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

        ; Eight bytes per sprite, first bytes

BALL0:
        .byte %00111100, %01111111, %01111111, %01111111, %01111111, %01111111, %01111111, %00111100
        .byte %01111000, %01111110, %01111110, %01111110, %01111110, %01111110, %01111110, %01111000
        .byte %01110000, %01111100, %01111100, %01111100, %01111100, %01111100, %01111100, %01110000
        .byte %01100000, %01111000, %01111000, %01111000, %01111000, %01111000, %01111000, %01100000
        .byte %01000000, %01110000, %01110000, %01110000, %01110000, %01110000, %01110000, %01000000
        .byte %00000000, %01100000, %01100000, %01100000, %01100000, %01100000, %01100000, %00000000
        .byte %00000000, %01000000, %01000000, %01000000, %01000000, %01000000, %01000000, %00000000

BALL1:
        .byte %00000000, %00000001, %00000001, %00000001, %00000001, %00000001, %00000001, %00000000
        .byte %00000000, %00000011, %00000011, %00000011, %00000011, %00000011, %00000011, %00000000
        .byte %00000001, %00000111, %00000111, %00000111, %00000111, %00000111, %00000111, %00000001
        .byte %00000011, %00001111, %00001111, %00001111, %00001111, %00001111, %00001111, %00000011
        .byte %00000111, %00011111, %00011111, %00011111, %00011111, %00011111, %00011111, %00000111
        .byte %00001111, %00111111, %00111111, %00111111, %00111111, %00111111, %00111111, %00001111
        .byte %00011110, %01111111, %01111111, %01111111, %01111111, %01111111, %01111111, %00011110

        ; Position and velocity information for each ball

BALLXL:
        .byte NBALLS        ; 0,8,...,48 = horizontal shift
BALLYL:
        .byte NBALLS
BALLDYL:
        .byte NBALLS
BALLDYH:
        .byte NBALLS

BALLDX:
        .byte NBALLS        ; 8 = one pixel horizontal
BALLXH:
        .byte NBALLS        ; column/byte
BALLYH:
        .byte NBALLS        ; row
