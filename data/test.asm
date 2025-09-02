
bits 16

; mov ax, 5
; mov bx, 4

; mul bx

; mov ax, 10
; mov bx, 32
; mul bx

; mov ax, -10
; mov bx, 50
; imul bx

; mov ax, -15
; mov bx, -2
; imul bx

; mov ax, 4000
; mov bx, 256
; imul bx

; mov dx, 0
; mov ax, 4000
; mov bx, -7
; idiv bx

mov ax, 127
shl al, 1
mov cl, 6
nop
shl al, cl

nop
mov al, 3
shl al, 1
shr al, 1

mov ax, -7
sar ax, 1
sar ax, 1
