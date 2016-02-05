
; Implementacion de la multiplicacion de dos numeros 
	in a
	store a, x
	
	in a
	store a, y

	load a  , y
	store a, v
	load a, cero
	store 	a, z
	
start_while:	 
	load a,		 v		; Etiqueta...
	jz  end_while
	
	load a, z
	add a, x
	store a, z
	load a, v
	sub a , uno
	store a, v
	load a, cero
	jz start_while
	
end_while:
	load a, z
	halt
	
; variables
x db
y db
z db
v db
cero db 0x0
uno db 0x1
