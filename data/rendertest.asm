
bits 16

; Framebuffer starts at what I call 1 page 0x10000
mov ax, 0x1000
mov ds, ax
mov ss, ax

; Draw red blue pattern
mov ax, 0
mov dx, 64
y_loop_start:
	
	mov cx, 64
	x_loop_start:
		mov byte [bx + 0], al  ; Red
		mov byte [bx + 1], 0   ; Green
		mov byte [bx + 2], ah  ; Blue
		mov byte [bx + 3], 255 ; Alpha
		add bx, 4
		add al, 12
		add ah, 1
			
		loop x_loop_start
	
	sub dx, 1
	jnz y_loop_start

; Draw green line pattern
mov bx, 4*64 + 4
mov bp, 4*64 + 4
mov cx, 62
mov ax, 0
outline_loop_start:
	
	mov [bx + 1], ax ; Top line
	mov [bx + 61*64*4 + 1], ax ; Bottom line
	mov [bp + 1], ax ; Left line
	mov [bp + 61*4 + 1], ax ; Right  line
	
	add bx, 4
	add bp, 4*64
	add ax, 16
			
	loop outline_loop_start
