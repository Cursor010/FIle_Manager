.code
;-------------------------------------------------------------------------------------------------------------
Get_Pos_Address proc
; Параметры:
; RCX - screen_buffer
; RDX - pos
; Возврат: RDI

	; 1. Вычисляем адрес вывода: address_offset = (pos.Y_Pos * pos.Screen_Width + pos.X_Pos) * 4
	; 1.1.Вычисляем pos.Y * pos.Screen_Width
	mov rax, rdx
	shr rax, 16  ; AX = pos.Y_Pos
	movzx rax, ax  ; RAX = AX = pos.Y_Pos

	mov rbx, rdx
	shr rbx, 32  ; BX = pos.Screen_Width
	movzx rbx, bx  ; RBX = BX = pos.Screen_Width

	imul rax, rbx  ; RAX = RAX * RBX = pos.Y_Pos * pos.Screen_Width

	; 1.2. Добавим pos.X к RAX
	movzx rbx, dx  ; RBX = DX = pox.X_Pos
	add rax, rbx  ; RAX = pos.Y_Pos * pos.Screen_Width + pox.X_Pos = смещение в символах

	; 1.3. RAX содержит смещение начала строки в символах, а надо - в байтах.
	; Т.к. каждый символ занимает 4 байта, надо умножить это смещение на 4
	shl rax, 2  ; RAX = RAX * 4 = address_offset

	mov rdi, rcx  ; RDI = screen_buffer
	add rdi, rax  ; RDI = screen_buffer + address_offset

	ret

Get_Pos_Address endp
;-------------------------------------------------------------------------------------------------------------
Draw_Start_Symbol proc
; Выводим стартовый символ
; Параметры:
; RDI - текущий адрес в буфере окна
; R8 - symbol
; Возврат: нет

	push rax
	push rbx

	mov eax, r8d
	mov rbx, r8
	shr rbx, 32  ; RBX = EBX = { symbol.Start_Symbol, symbol.End_Symbol }
	mov ax, bx  ; EAX = { symbol.Attributes, symbol.Start_Symbol }

	stosd

	pop rbx
	pop rax

	ret

Draw_Start_Symbol endp
;-------------------------------------------------------------------------------------------------------------
Draw_End_Symbol proc
; Выводим конечный символ
; Параметры:
; EAX - { symbol.Attributes, symbol.Main_Symbol }
; RDI - текущий адрес в буфере окна
; R8 - symbol
; Возврат: нет

	mov rbx, r8
	shr rbx, 48  ; RBX = BX = symbol.End_Symbol
	mov ax, bx  ; EAX = { symbol.Attributes, symbol.End_Symbol }

	stosd

	ret

Draw_End_Symbol endp
;-------------------------------------------------------------------------------------------------------------
Draw_Line_Horizontal proc
; extern "C" void Draw_Line_Horizontal(CHAR_INFO * screen_buffer, SPos pos, ASymbol symbol);
; Параметры:
; RCX - screen_buffer
; RDX - pos
; R8 - symbol
; Возврат: нет

	push rax
	push rbx
	push rcx
	push rdi

	; 1. Вычисляем адрес вывода
	call Get_Pos_Address  ; RDI = позиция символа в буфере screen_buffer в позиции pos

	; 2. Выводим стартовый символ
	call Draw_Start_Symbol

	; 3. Выводим символы symbol.Main_Symbol
	mov eax, r8d
	mov rcx, rdx
	shr rcx, 48  ; RCX = CX = pos.Len

	rep stosd

	; 4. Выводим конечный символ
	call Draw_End_Symbol

	pop rdi
	pop rcx
	pop rbx
	pop rax

	ret

Draw_Line_Horizontal endp
;-------------------------------------------------------------------------------------------------------------
Draw_Line_Vertical proc
; extern "C" void Draw_Line_Vertical(CHAR_INFO * screen_buffer, SPos pos, ASymbol symbol);
; Параметры:
; RCX - screen_buffer
; RDX - pos
; R8 - symbol
; Возврат: нет

	push rax
	push rcx
	push rdi
	push r11

	; 1. Вычисляем адрес вывода
	call Get_Pos_Address  ; RDI = позиция символа в буфере screen_buffer в позиции pos

	; 2. Вычисление коррекции позиции вывода
	call Get_Screen_Width_Size  ; R11 = pos.Screen_Width * 4 = Ширина экрана в байтах
	sub r11, 4

	; 3. Выводим стартовый символ
	call Draw_Start_Symbol

	add rdi, r11

	; 4. Готовим счётчик цикла
	mov rcx, rdx
	shr rcx, 48  ; RCX = CX = pos.Len

	mov eax, r8d  ; EAX = symbol

_1:
	stosd  ; Выводим символ
	add rdi, r11

	loop _1

	; 5. Выводим конечный символ
	call Draw_End_Symbol

	pop r11
	pop rdi
	pop rcx
	pop rax

	ret

Draw_Line_Vertical endp
;-------------------------------------------------------------------------------------------------------------
Show_Colors proc
; extern "C" void Show_Colors(CHAR_INFO * screen_buffer, SPos pos, CHAR_INFO symbol);
; Параметры:
; RCX - screen_buffer
; RDX - pos
; R8 - symbol
; Возврат: нет

	push rax
	push rbx
	push rcx
	push rdi
	push r10
	push r11

	; 1. Вычисляем адрес вывода
	call Get_Pos_Address  ; RDI = позиция символа в буфере screen_buffer в позиции pos

	mov r10, rdi

	; 2. Вычисление коррекции позиции вывода
	call Get_Screen_Width_Size  ; R11 = pos.Screen_Width * 4 = Ширина экрана в байтах

	; 3. Готовим циклы
	mov rax, r8  ; RAX = EAX = symbol

	; 3.1. Результат команды AND (логического "И")
	; 0 & 0 = 0
	; 0 & 1 = 0
	; 1 & 0 = 0
	; 1 & 1 = 1

	and rax, 0ffffh  ; Обнуляем все байты RAX, кроме 0 и 1
	mov rbx, 16

	xor rcx, rcx  ; RCX = 0

	; 0 XOR 0 = 0
	; 0 XOR 1 = 1
	; 1 XOR 0 = 1
	; 1 XOR 1 = 0

_0:
	mov cl, 16

_1:
	stosd
	add rax, 010000h  ; Единица, смещённая на 16 разрядов влево (т.е. элементарный шаг для аттрибутов)

	loop _1

	add r10, r11
	mov rdi, r10

	dec rbx
	jnz _0

	pop r11
	pop r10
	pop rdi
	pop rcx
	pop rbx
	pop rax

	ret

Show_Colors endp
;-------------------------------------------------------------------------------------------------------------
Get_Screen_Width_Size proc
; Вычисляет ширину экрана в байтах
; RDX - SPos pos или SArea_Pos pos
; Возврат: R11 = pos.Screen_Width * 4

	mov r11, rdx
	shr r11, 32  ; R11 = pos
	movzx r11, r11w  ; R11 = R11W = pos.Screen_Width
	shl r11, 2  ; R11 = pos.Screen_Width * 4 = Ширина экрана в байтах

	ret

Get_Screen_Width_Size endp
;-------------------------------------------------------------------------------------------------------------
Clear_Area proc
; extern "C" void Clear_Area(CHAR_INFO * screen_buffer, SArea_Pos area_pos, ASymbol symbol);
; Параметры:
; RCX - screen_buffer
; RDX - area_pos
; R8 - symbol
; Возврат: нет

	push rax
	push rbx
	push rcx
	push rdi
	push r10
	push r11

	; 1. Вычисляем адрес вывода
	call Get_Pos_Address  ; RDI = позиция символа в буфере screen_buffer в позиции pos

	mov r10, rdi

	; 2. Вычисление коррекции позиции вывода
	call Get_Screen_Width_Size  ; R11 = pos.Screen_Width * 4 = Ширина экрана в байтах

	; 3. Готовим циклы
	mov rax, r8  ; RAX = EAX = symbol

	mov rbx, rdx
	shr rbx, 48  ; BH = area_pos.Height, BL = area_pos.Width

	xor rcx, rcx  ; RCX = 0

_0:
	mov cl, bl
	rep stosd

	add r10, r11
	mov rdi, r10

	dec bh
	jnz _0

	pop r11
	pop r10
	pop rdi
	pop rcx
	pop rbx
	pop rax

	ret

Clear_Area endp
;-------------------------------------------------------------------------------------------------------------
Draw_Text proc
; extern "C" int Draw_Text(CHAR_INFO * screen_buffer, SText_Pos pos, const wchar_t * str);
; Параметры:
; RCX - screen_buffer
; RDX - pos
; R8 - str
; Возврат: RAX - длина строки str

	push rbx
	push rdi
	push r8

	; 1. Вычисляем адрес вывода
	call Get_Pos_Address  ; RDI = позиция символа в буфере screen_buffer в позиции pos

	mov rax, rdx
	shr rax, 32  ; Старшая половина EAX содержит pos.Attributes

	xor rbx, rbx  ; RBX = 0

_1:
	mov ax, [ r8 ]  ; AL = очередной символ из строки

	cmp ax, 0
	je _exit

	add r8, 2  ; Переводим указатель на следующий символ

	stosd
	inc rbx
	jmp _1

_exit:
	mov rax, rbx

	pop r8
	pop rdi
	pop rbx

	ret

Draw_Text endp
;-------------------------------------------------------------------------------------------------------------
Draw_Limited_Text proc
; extern "C" void Draw_Limited_Text(CHAR_INFO * screen_buffer, SText_Pos pos, const wchar_t * str, unsigned short limit);
; Параметры:
; RCX - screen_buffer
; RDX - pos
; R8 - str
; R9 - limit
; Возврат: RAX - длина строки str

	push rax
	push rcx
	push rdi
	push r8
	push r9

	; 1. Вычисляем адрес вывода
	call Get_Pos_Address  ; RDI = позиция символа в буфере screen_buffer в позиции pos

	mov rax, rdx
	shr rax, 32  ; Старшая половина EAX содержит pos.Attributes

_1:
	mov ax, [ r8 ]  ; AL = очередной символ из строки

	cmp ax, 0
	je _fill_spaces

	add r8, 2  ; Переводим указатель на следующий символ

	stosd

	dec r9
	cmp r9, 0
	je _exit  ; Прекращаем вывод, если строка достигла предела

	jmp _1

_fill_spaces:
	mov ax, 020h  ; Сохраняем пробелы
	mov rcx, r9  ; Количество оставшихся пробелов

	rep stosd

_exit:
	pop r9
	pop r8
	pop rdi
	pop rcx
	pop rax

	ret

Draw_Limited_Text endp
;-------------------------------------------------------------------------------------------------------------
end