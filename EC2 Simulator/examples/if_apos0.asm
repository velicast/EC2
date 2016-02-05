; algoritmo
    in A
    store A, X

    in A
    store A, Y

    load A, CUATRO
    sub A, X
  
    jp endif

    load A, SIETE
    store A, Y    

endif:
    load A, Y
    halt
    
X db
Y db
CUATRO db 0x04
SIETE db 0x07
