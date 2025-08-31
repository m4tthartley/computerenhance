
bits 16


; mov word [1000], 1
; mov word [1002], 2
; mov word [1004], 3
; mov word [1006], 4
; 
; mov bx, 1000
; mov word [bx + 4], 10
; 
; mov bx, word [1000]
; mov cx, word [1002]
; mov dx, word [1004]
; mov bp, word [1006]

; mov dx, 6
; mov bp, 1000
; 
; mov si, 0
; init_loop_start:
; 	mov word [bp + si], si
; 	add si, 2
; 	cmp si, dx
; 	jnz init_loop_start
; 
; mov bx, 0
; mov si, 0
; add_loop_start:
; 	mov cx, word [bp + si]
; 	add bx, cx
; 	add si, 2
; 	cmp si, dx
; 	jnz add_loop_start

mov dx, 6
mov bp, 1000

mov si, 0
init_loop_start:
	mov word [bp + si], si
	add si, 2
	cmp si, dx
	jnz init_loop_start

mov bx, 0
mov si, dx
sub bp, 2
add_loop_start:
	add bx, word [bp + si]
	sub si, 2
	jnz add_loop_start
