	in a
	store a, x
	in a
	store a,y
	load a,x
	sub a, y
	jp else
	load a, x
	add a, x
	store a, z
	load a, cero
	jz endif
else:
	load a, y
	add a, tres
	store a, z
endif:
	load a, z
	halt
x db
y db
z db
cero db
tres db 0x3 
