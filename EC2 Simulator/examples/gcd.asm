; Programa que calcula el Maximo comun divisor de dos numero positvos
; Autor: Eduar Castrillo Velilla
; Codigo: 258019
; Universidad Nacional de Colombia
	in a
	store a, x 	
	in a
	store a, y 		; comentario de una linea
loop:
	load a, x
	sub  a, y
	jz out
	jp xgty
	load  a, y
	sub   a, x
	store a, y 
	jp loop
xgty:
	load  a, x		
	sub   a, y
	store a, x
	jp loop
out:	
	load a, x	;hkjh
	halt
; Etiquetar dos bytes para variables
x db
y db
