
bits 16

; Data stored at the second 64k page
mov ax, 0x1000
mov ds, ax

; Stack stored at the third 64k page
mov ax, 0x2000
mov ss, ax

mov ax, 0

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

; mov ax, 127
; shl al, 1
; mov cl, 6
; nop
; shl al, cl

; nop
; mov al, 3
; shl al, 1
; shr al, 1

; mov ax, -7
; sar ax, 1
; sar ax, 1

; mov ax, 3
; mov cx, 7
; and ax, cx

; mov ax, 7
; mov [0], word 3
; and ax, [0]

; mov ax, 0xFFFF
; and ax, 0x00FF

; mov ax, 0xFFFF
; test ax, 0x00

; mov ax, 0xFF00
; or ax, 0x00FF

; mov ax, 0xFFFF
; xor ax, 0x00FF

; mov ax, 0xFF00
; not ax

; inc ax
; inc cx
; inc dh
; inc al
; inc ah
; inc sp
; inc di

inc byte [bp + 1002]
inc word [bx + 39]
inc byte [bx + si + 5]
inc word [bp + di - 10044]
inc word [9349]
inc byte [bx]

; mov al, byte [bp + 1002]
; mov ax, word [bx + 39]
; mov al, byte [bx + si + 5]
; mov ax, word [bp + di - 10044]
; mov ax, word [9349]
; mov al, byte [bx]

dec ax
dec cx
dec dh
dec al
dec ah
dec sp
dec di
