
bits 16

; mov cx, 200
; mov bx, cx
; add cx, 1000
; mov bx, 2000
; sub cx, bx

; mov cx, 3
; mov bx, 1000
; loop_start:
; add bx, 10
; sub cx, 1
; jnz loop_start

mov ax, 10
mov bx, 10
mov cx, 10

label_0:
cmp bx, cx
je label_1

add ax, 1
jp label_2

label_1:
sub bx, 5
jb label_3

label_2:
sub cx, 2

label_3:
loopnz label_0
