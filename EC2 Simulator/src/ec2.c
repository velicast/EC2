/*
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 */
#include "ec2.h"
#include <string.h>
#include <ctype.h>

#define MEM_MASK 0x1F

#define OPCODE(ir) (ir >> 5)
#define ADDRESS(ir) (ir & MEM_MASK)

// 32 bytes de memoria para el procesador
unsigned char memory[32];

/*
 * Carga el codigo maquina a la memoria del procesador.
 * El formato del codigo ensamblado es por valores INIT
 * para cada bit de la memoria.
 *
 * Retorna 1 si carga correcta; 0 en caso contrario
 */
int load(FILE *f) {

    unsigned init;
    int offset = 0, b;

    while(offset <= 7) {
        if(fscanf(f, "%X", &init) == 0) {
            fprintf(stderr, "Fatal error: Invalid init format\n");
            return 0;
        }
        b = 0;
        for(;b < 32; ++b) {
            memory[b] |= (init & 0x1) << offset;
            init >>= 1;
        }
        ++offset;
    }
    return 1;
}

/*
 * Ejecuta el programa cargado en la memoria del procesador
 * La ejecucion del programa comienzan en la posicion
 * 0 de la memoria.
 *
 * Retorna los 5 bits menos significativos de la instruccion HALT
 */
int exec(int mk_coredump) {
    
    char ip = 0;        // Puntero de instruccion
    unsigned char ir = 0;   // Registro de instruccion
    int reg_a;          // Regitro general A
    int opc, addr;      // Codigo de operacion y direccion

    do {
        ir = memory[ip];
        opc = OPCODE(ir);
        addr = ADDRESS(ir);

        if((opc == JZ && !reg_a)
            || (opc == JP && reg_a >= 0)) {
            ip = addr; // Salto!!   
        } else {
            if(opc == LOAD) {
                reg_a = memory[addr];
            }
            else if(opc == STORE) {
                memory[addr] = (char)reg_a;
            }
            else if(opc == ADD) {
                reg_a += memory[addr];
            }
            else if(opc == SUB) {
                reg_a -= memory[addr];
            }
            else if(opc == IN_A) {
                printf("Input A: ");
                scanf("%d", &reg_a);
            }
            ++ip; // siguiente instruccion
            printf("Reg A: %d\n", reg_a);
        }
    } while(opc != HALT);
    
    printf("System halted...\n");

    if(mk_coredump) {
        core_dump(ip, ir, reg_a);
    }
    return ADDRESS(ir);
}

/*
 * Imprime un volcado del nucleo de procesador y la memoria
 */
void core_dump(char ip, unsigned char ir, int reg_a) {

    printf("\n\n--- EC 2 Core Dump ---\n\nIP: 0x%X  |  IR: 0x%X  |  REG A: 0x%X\n\n", ip - 1, ir, reg_a);
    printf("Memory Content:\n");

    int col = 0, pos = 0;
    
    printf("   ");
    for(; col < 16; ++col) {
        printf("%02X  ", col);
    }
    printf("\n00 ");
    for(; pos < 16; ++pos) {
        printf("%02X  ", memory[pos]);
    }
    printf("\n01 ");
    for(; pos < 32; ++pos) {
        printf("%02X  ", memory[pos]);
    }
    printf("\n\n");
}
