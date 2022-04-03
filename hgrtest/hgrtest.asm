; Adapted from 8 bit workshop HGR Test hgrtest.a
; https://8bitworkshop.com/v3.9.0/?file=hgrtest.a&platform=apple2

start:
        sta $c050       ; set graphics
        sta $c052       ; no mixed mode
        sta $c057       ; set hires

        ldx #0
loop:
        inc $2000,x     ; increment hgr byte
        inx
        bne loop
        lda loop+2     ; increment hi byte of instruction
        clc
        adc #1
        sta loop+2     ; self-modifying code
        cmp #$40
        bne loop
        lda #$20        ; reset to $2000
        sta loop+2
        jmp start
