
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

; inc byte [bp + 1002]
; inc word [bx + 39]
; inc byte [bx + si + 5]
; inc word [bp + di - 10044]
; inc word [9349]
; inc byte [bx]

; ; mov al, byte [bp + 1002]
; ; mov ax, word [bx + 39]
; ; mov al, byte [bx + si + 5]
; ; mov ax, word [bp + di - 10044]
; ; mov ax, word [9349]
; ; mov al, byte [bx]

; dec ax
; dec cx
; dec dh
; dec al
; dec ah
; dec sp
; dec di

mov bx, 5
sub bx, 5

mov di, 256
lea bx, [di + 1024]

mov [100], word 300
mov [102], word 50
lds si, [100]

mov [100], word 400
mov [102], word 25
les di, [100]

mov bx, 5
sub bx, 5
lahf

add bx, 20
sahf

pushf
pushf
mov bp, sp
mov cx, [bp]

; sub bx, 5
; popf

; mov bp, 0
; mov [bp + 0xFFFF - 2], word 4000

mov ax, 0x1234
push ax

mov [100], word 0x5608
push word [100]

mov ax, 0xBABE
mov es, ax
push es

pop dx

mov ax, 0xCAFE
push ax

pop word [100]
mov cx, [100]

pop es