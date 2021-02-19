; This test program performs a few
; additions and subtractions in various
; conditions to test the math instructions.
;
; This includes binary-mode and decimal-mode.
;
; The results of which are noted on the comment
; at the respective points.
*=$0200

START:
	CLD
	CLC
	LDA #$32
	SBC #$11	; C=0 A=20

	CLD
	CLC
	LDA #0
	SBC #1		; C=0 A=FF

	SED
	CLC
	LDA #$32
	SBC #$02	; C=1 A=29

	SED
	SEC
	LDA #$40
	SBC #$13	; C=1 A=27

	SED
	CLC
	LDA #$21
	SBC #$34	; C=0 A=87

    BRK