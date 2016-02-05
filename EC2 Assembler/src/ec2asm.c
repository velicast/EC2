/*
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 */
#include "ec2asm.h"
#include "strmanip.h"
#include <malloc.h>

#define N_RWORDS 10
#define EQ(token, inst) (strcmp(token, inst) == 0)      // solo para comparar instrucciones

// -------------------------------
// - Mensajes de error formateados
// -------------------------------

#define PRINT_STDERR(error, line, src) fprintf(stderr, error, line, src); return 0;
                                       
#define ERR_INVALID_NAME(l, str)      PRINT_STDERR("error line %d: invalid name %s\n", l, str)
#define ERR_INVALID_ID(l, str)        PRINT_STDERR("error line %d: invalid identifier %s\n", l, str)
#define ERR_RWORD(l, str)             PRINT_STDERR("error line %d: %s is reserved word\n", l, str)
#define ERR_REDEFINED(l, str)         PRINT_STDERR("error line %d: simbol %s redefined here\n", l, str)
#define ERR_UNEXPECTED(l)             PRINT_STDERR("error line %d: unexpected \',\'\n", l, "")
#define ERR_MISMATCH(l)               PRINT_STDERR("error line %d: instruction operands mismatch\n", l, "")
#define ERR_DIRECTIVE_MISMATCH(l)     PRINT_STDERR("error line %d: directive operands mismatch\n", l, "")
#define ERR_INVALID_HEX(l, str)       PRINT_STDERR("error line %d: invalid hex-value %s\n", l, str)
#define ERR_UNDEFINED(l, str)         PRINT_STDERR("error line %d: undefined simbol %s\n", l, str)
#define ERR_INVALID_OP(l, str)        PRINT_STDERR("error line %d: invalid operand %s\n", l, str)
#define ERR_UNKNOWN(l, str)           PRINT_STDERR("error line %d: unknown instruction %s\n", l, str)

/* Palabras reservadas para el ensamblador */
const char *RWORDS[] = {"A", "JZ", "JP", "IN", "HALT",
                         "LOAD", "STORE", "ADD", "SUB", "DB"};

/* Lista ligada simple para los simbolos*/
typedef struct __simbol {
    char name[MAX_SIMBOL];
    int value;
    struct __simbol *next;
} node_simbol;

static node_simbol *s_table;

/*
 * Agrega el simbolo S a la tabla con su respectivo valor V
 */
void st_add(char *s, int v) {

    node_simbol *new_s = malloc(sizeof(node_simbol));
    strcpy(new_s->name, s);
    new_s->value = v;
    new_s->next = 0;

    if(s_table) {
        new_s->next = s_table;
    }
    s_table = new_s;
}

/*
 * Eliminar el simbolo S de la tabla y guarda su valor en V
 *
 * Retorna 1 si elimina; 0 en caso contrario
 */
int st_remove(const char *s, int *v) {

    node_simbol *ptr = s_table;
    if(!ptr) {
        return 0;
    }
    if(strcmp(ptr->name, s) == 0) {
        *v = s_table->value;
        s_table = s_table->next;
        free(ptr);
        return 1;
    } else {
        while (ptr->next) {
            node_simbol *to_free = ptr->next;

            if(strcmp(to_free->name, s) == 0) {
                *v = to_free->value;
                ptr->next = to_free->next;
                free(to_free);
                return 1;
            } else {
                ptr = to_free;
            }
        }
    }
    return 0;
}

/*
 * Busca en la tabla el simbolo S y guarda en V su valor
 *
 * Retorna 1 si encuentra el simbolo; 0 en caso contrario.
 */
int st_find(const char *s, int *v) {

    node_simbol *ptr = s_table;
    while (ptr) {
        if(strcmp(ptr->name, s) == 0) {
            *v = ptr->value;
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

/*
 * Imprime el contenido de la tabla de simbolos
 */
void st_print() {

    printf("\nSimbol Table:\n");
    node_simbol *ptr = s_table;
    while(ptr) {
        printf("%s\t|\t0x%02X\n", ptr->name, ptr->value);
        ptr = ptr->next;
    }
}

/*
 * Determia si S es una palabra reservada de EC2ASM.
 * Esta funcion si es CaseSensitve.
 */
int is_rword(const char *s) {

    int i = N_RWORDS - 1;
    for(; i >= 0; --i) {
        if(strcmp(RWORDS[i], s) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Determina si la cadena STR es nombre sintacticamente invalido.
 * Los criterios de identificacion son los siguiente.
 *
 * - Debe comenzar con una letra del alfabeto o con el caracter '_'
 * _ Solo puede contener Letras, digitos o el caracter '_'
 *
 * Retorna 1 si es invalido; 0 en caso contrario
 */
int invalid_name(const char *str) {

    if(!isalpha(str[0]) && str[0] != '_') {
        return 1;
    } 
    int i = strlen(str) - 1;
    for (; i >= 1; --i) {
        if(!isalpha(str[i]) && !isdigit(str[i]) && str[i] != '_') {
            return 1;
        }
    }
    return 0;
}

/*
 * Formatea la cadena como lista de operandos.
 * La cantidad de espacios entre elementos de la cadena debe ser maximo de 1.
 * 
 * Retorna 1 si se formatea correctamente; 0 si encuenta ',' no esperadas
 */
int parse_op(char *str) {

    if(startwith(str, ',')
            || endwith(str, ','))
    {
        return 0;
    }
    int i = 0;
    int colon = 0;

    for (; str[i]; ++i) {

        if(str[i] == ',') {
            if(str[i + 1] == ',') {
                return 0;
            }
            colon = 1;
        }
        else if(str[i] == ' ') {
            if(colon && str[i + 1] == ',') {
                return 0;
            }
            str[i] = ',';
        } else {
            colon = 0;
        }
    }
    return 1;
}

/*
 * Chequea la sintaxis de la linea y actualiza la tabla de simbolos.
 *
 * Solo se puede colocar una etiqueta por linea.
 *
 * El formato de instruccion es...
 *
 * TAG: INST OP1, OP2, OP3 ...
 *
 * donde TAG: es opcional en la linea y la lista de operandos es variable.
 *
 * Los diferentes formatos de directivas son:
 *
 * TAG: X DB 0xA
 *
 * donde TAG: X y el valor Hexadecimal son opcionales en la linea
 *
 * Parametros:
 *
 * - LINE: linea a chequear
 * - LN: numero de linea
 * - ADDRESS: posicion de memoria donde debe quedar guardada la instruccion o directiva
 * - ONLY_TAG: guarda 1 si solo es una etiqueta; 0 en caso contrario
 *
 * Retorna 1 si correcto, 0 en caso contrario.
 */
int check_line(char *line, int ln, int address, int *only_tag) {

    //printf("\nLine %d: (%s)\n", ln, line);
    *only_tag = 1;
    char simbol[MAX_SIMBOL] = "";   // simbolo a registrar
    char inst[MAX_SIMBOL] = "";     // instruccion a ejecutar
    char *operands;   // operandos de la instruccion
    int match_ops = 0;            // 1 si se debe verificar concordancia de operandos

    uppercase(line);  // Todo Mayuculas

    char *token = strtok(line, " ");
    int ops_index = indexof(token, ':');
    
    // Verificar si es etiqueta tipo TAG:
    if(ops_index >= 0) {
        substr(simbol, token, 0, ops_index - 1);
        //printf("Tag: (%s)\n", simbol);
        if(invalid_name(simbol)) {
            ERR_INVALID_ID(ln, simbol);
        } 
        int tmp; // tmp value
        if(is_rword(simbol)) {
            ERR_RWORD(ln, simbol);
        }
        else if(st_find(simbol, &tmp)) {
            ERR_REDEFINED(ln, simbol);
        }
        st_add(simbol, address);

        int length = strlen(token);

        if(ops_index == length - 1) {
            token = strtok(NULL, " "); // pasar al siguiente token
            if(token) {
                strcpy(inst, token);
                ops_index = length + strlen(inst) + 2;
            }
        } else {
            substr(inst, token, ops_index + 1, length - 1);
            ops_index = length + 1;
        }
        if(!strlen(inst)) { // NO Hay mas tokens a parte de la etiqueta, retornar
            return 1;
        }
        *only_tag = 0;
    } else {
        strcpy(inst, token);
        ops_index = strlen(inst) + 1;
        *only_tag = 0;
    }
    
    //printf("Inst: (%s)\n", inst);
    if(invalid_name(inst)) {
        ERR_INVALID_NAME(ln, inst);
    }
    operands = line + ops_index;

    if(!parse_op(operands)) {
        ERR_UNEXPECTED(ln);
    }
    //printf("Operands: (%s)\n", operands);
    char *optoken = strtok(operands, ",");   // extraer primer operando

    if(EQ(inst, "IN")
            || EQ(inst, "JZ")
            || EQ(inst, "JP"))
    {
        if(optoken) {
            if(invalid_name(optoken)) {
                ERR_INVALID_NAME(ln, optoken);
            }
            match_ops = 1;
        } else {
            ERR_MISMATCH(ln);
        }
    }
    else if(EQ(inst, "HALT")) {

        if(optoken) { // Parametros de mas
            ERR_MISMATCH(ln);
    	}
    }
    else if(EQ(inst, "LOAD")
            || EQ(inst, "STORE")
            || EQ(inst, "ADD")
            || EQ(inst, "SUB"))
    {
        char dest[MAX_SIMBOL] = "";
        char src[MAX_SIMBOL] = "";
        int mismatch = 0;   // numero de operandos no concuerdan

        if(optoken) {
            strcpy(dest, optoken);
        } else {
            mismatch = 1;
        }

        optoken = strtok(NULL, ",");
        if(optoken) {
            strcpy(src, optoken);
        } else {
            mismatch = 1;
        }

        if(mismatch) {
            ERR_MISMATCH(ln);
        }
        if(invalid_name(dest)) {
            ERR_INVALID_NAME(ln, dest);
        }
        if(invalid_name(src)) {
            ERR_INVALID_NAME(ln, src);
        }
        match_ops = 1;
        //printf("  Dest: (%s)\n  Src: (%s)\n", dest, src);
    }
    else if(EQ(inst, "DB")) {   // forma DB o DB 0xF
        
        if(optoken) {
            int hex = 0;
            if(!xatoi(optoken, &hex)) {
                ERR_INVALID_HEX(ln, optoken);
            }
            //printf("Hex: (%s)\n", optoken);
        }
    } else {                    // forma X DB o X DB 0xDF
        if(optoken && EQ(optoken, "DB")) {
            //printf("Directive: (%s)\n", optoken);
            optoken = strtok(NULL, ",");
            if(optoken) {
                int hex = 0;
                if(!xatoi(optoken, &hex)) {
                    ERR_INVALID_HEX(ln, optoken);
                }
                //printf("Hex: (%s)\n", optoken);
                optoken = strtok(NULL, ",");
                if(optoken) { // Parametros de mas
                    ERR_DIRECTIVE_MISMATCH(ln);
                }
            }
            int v; // tmp value
            if(is_rword(inst)) {
                ERR_RWORD(ln, inst);
            }
            else if(st_find(inst, &v)) {
                ERR_REDEFINED(ln, inst);
            }
            st_add(inst, address);
            //printf("Simbol Registered (%s)\n", inst);‪
        }
    }

    if(match_ops) {
        // Verificar si hay parametros de sobra en la instruccion
        optoken = strtok(NULL, ",");
        if(optoken) { // Parametros de mas
            ERR_MISMATCH(ln);
        }
    }
    return 1;
}

/*
 * Codifica la linea en una instruccion de 1 byte de longitud.
 * El formato del byte es:
 *
 * 7-5 bits Codigo de operacion
 * 4-0 bits Direccion en memoria
 *
 * Retorna 1 si correcto; 0 en caso contrario
 */
int encode(char *line, int ln, int *code, int *only_tag) {

    //printf("\nLine %d: %s\n", ln, line);
    *only_tag = 1;
    char inst[MAX_SIMBOL] = "";
    char *operands;

    uppercase(line);

    char *token = strtok(line, " ");
    int ops_index = indexof(token, ':');

    if(ops_index >= 0) {        // Saltar etiqueta

        int length = strlen(token);
        //printf("Tag: (%s)\n", token);
        if(ops_index == length - 1) {
            token = strtok(NULL, " "); // pasar al siguiente token
            if(token) {
                strcpy(inst, token);
                ops_index = length + strlen(inst) + 2;
            }
        } else {
            substr(inst, token, ops_index + 1, length - 1);
            ops_index = length + 1;
        }
        if(!strlen(inst)) {
            return 1;
        }
        *only_tag = 0;
    } else {
        strcpy(inst, token);
        ops_index = strlen(inst) + 1;
        *only_tag = 0;
    }
    //printf("Inst: (%s)\n", inst);
    operands = line + ops_index;
    parse_op(operands);
    //printf("Operands: (%s)\n", operands);
    char *optoken = strtok(operands, ",");   // extraer primer operando
    
    if(EQ(inst, "JZ") || EQ(inst, "JP")) {
        
        int addr;
        if(st_find(optoken, &addr)) {
            *code = EQ(inst, "JZ") ? JZ : JP;
            *code <<= 5;
            *code |= addr;
        } else {
            ERR_UNDEFINED(ln, optoken);
        }
    }
    else if(EQ(inst, "IN")) {

        if(EQ(optoken, "A")) {
            *code = IN << 5;
        } else {
            ERR_INVALID_OP(ln, optoken);
        }
    }
    else if (EQ(inst, "HALT")) {
        *code = (HALT << 5) | 0x1F;
    }
    else if(EQ(inst, "LOAD")
            || EQ(inst, "STORE")
            || EQ(inst, "ADD")
            || EQ(inst, "SUB"))
    {
        char dest[MAX_SIMBOL] = "";
        char src[MAX_SIMBOL] = "";

        strcpy(dest, optoken);
        optoken = strtok(NULL, ",");
        strcpy(src, optoken);
        //printf("  Dest: (%s)\n  Src: (%s)\n", dest, src);
        if(EQ(dest, "A")) {     // Registro A
            int addr = 0;
            if(st_find(src, &addr)) {
                if(EQ(token, "LOAD")) {
                    *code = LOAD << 5;
                }
                else if(EQ(inst, "STORE")) {
                    *code = STORE << 5;
                }
                else if (EQ(inst, "ADD")) {
                    *code = ADD << 5;
                }
                else if(EQ(inst, "SUB")) {
                    *code = SUB << 5;
                }
                *code |= addr;
            } else {
                ERR_UNDEFINED(ln, src);
            }
        }
        /* Agregar mas simbolos reservados aqu'i */
        else {
            // Buscar en la tabla de simbolos
            // Si no se encuentra en la tabla de simbolos
            // error
            ERR_INVALID_OP(ln, dest);
        }
    }
    else if(EQ(inst, "DB")) {
        
        if(optoken) {
            xatoi(optoken,  code);
        } else {
            *code = 0;
        }
    }
    else {
        if(optoken && EQ(optoken, "DB")) {
            optoken = strtok(NULL, ",");
            if(optoken) {
                xatoi(optoken, code);
            } else {
                *code = 0;
            }
        } else {
            ERR_UNKNOWN(ln, inst);
        }
    }
    return 1;
}

/*
 * Ensambla el codigo almacenado en el archivo IN y lo guarda en el archivo OUT
 * El formato de ensamblado es por Valores INIT de la memoria.
 * Si el parametro SUMMARY esta en 1, imprimer el resumen del ensamblado cuando este
 * es correcto.
 * 
 * Retorna 1 si correcto; 0 en caso contrario.
 */
int assemble(FILE *in, FILE *out, int summary) {

    int size = 0;         		// tamanio del programa en bytes
    int line = 1;         		// Numero de linea actualmente procesada
    char buff[MAX_LINE];      	// linea de codigo que est'a siendo procesada
    int tag;
    int error = 0;
    int llength;           	// longitud de la linea leida
    char *codline;
    
    // PRIMERA PASADA
    // Revisar sintaxis y crear tabla de simbolos
    while(fgets(buff, MAX_LINE, in)) {

        codline = trim(buff);
        llength = strlen(codline);

        if(llength > 0) {
            if(check_line(codline, line, size, &tag)) {
                if(!tag) {
                    ++size;
                }
            } else {
                error = 1;
            }
        }
        ++line;
    }

    if(error) {
        return 0;   // error de sintaxis
    }

    rewind(in);
    
    // SEGUNDA PASADA
    // codificar cada instruccion a su byte correspondiente

    char *memory = malloc(size);  // crear espacio de memoria
    if(!memory) {
        return 0;   // error de memoria
    }

    int address = 0;
    int byte = 0;
    line = 1;

    while(fgets(buff, MAX_LINE, in)) {

        codline = trim(buff);
        llength = strlen(codline);

        if(llength > 0) {
            if(encode(codline, line, &byte, &tag)) {
                if(!tag) {
                    memory[address] = byte;
                    byte = 0;
                    ++address;
                }
            } else {
                error = 1;
            }
        }
        ++line;
    }
    
    if(error) {
        free(memory);
        return 0;       // error de ensamblado
    }

    if(size > 32) {
        printf("Warning! Program is too bigger, maybe not work properly\n");
    }
    
    // TERCERA PASADA.
    //Convertir a formato INIT el contenido de la memoria y escribir en archivo

    unsigned init = 0;   // Valor init
    int k = 0;
    int i;
    
    for(;k < 8; ++k) {
        i = size - 1;
        init = 0;
        for(; i >= 0; --i) {
            init <<= 1;
            init |= (memory[i] >> k) & 0x1;
        }
        fprintf(out, "%0#9X\n", init);
    }

    // mostrar informacion del proceso y codigo ensamblado
    if(summary) {
        printf("\n--- Assembly Summary ---\n\n");
        printf("Readed Lines: 0x%0X  |  Generated Bytes: 0x%0X\n", line - 1, size);
        st_print();
        printf("\n");
    }

    free(memory);
    free(s_table);
    
    return 1;
}
