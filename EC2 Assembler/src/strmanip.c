/*
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 */
#include "strmanip.h"
#include <stdio.h>

/*
 * Retorna el valor entero representado por la cadena HEX escrita
 * en base Hexadecimal. Utiliza el formato 0X1234ABCD
 * La cadena debe estar en Mayuscula.
 *
 * Retorna 0 si un error ocurre.
 */
int xatoi(const char *hex, int *value) {

    if(strlen(hex) < 3 || strncmp(hex, "0X", 2) != 0 ) {
        return 0;
    }
    *value = 0; // valor entero

    int fix, v;
    char *c = (char *)(hex + 2);
    
    for(; *c; ++c) {
        if(!isxdigit(*c)) {
            return 0;
        }
        fix = (*c > 0x40) ? 0x37 : 0x30;  // valores para Mayusculas solamente
        v = *c - fix;
        *value <<= 4;
        *value |= v;
    }
    return 1;
}

/*
 * Elimina los espacios al comienzo y final de linea, evita el texto precedido (incluido)
 * por el ';', restringe el espaciado a un caracter y elimina el caracter de fin de linea.
 */
char *trim(char *str) {

    if (!str) { // NULL String
        return 0;
    }
    if (!*str) { // String vacio
        return str;
    }

    char *ptr;

    // Evitar comentarios
    ptr = strchr(str, ';');
    if (ptr >= str) {
        *ptr = '\0';
    }
        // Cadena vacia
    if(!*str) {
        return str;
    }
    
    // Trim por izquierda
    while (*str == ' ' || *str == '\t') ++str;

    ptr = str;
    ptr += strlen(str) - 1;

    // Eliminar caracter de fin de linea
    if (*ptr == '\n') {
        *ptr = '\0';
        --ptr;
    }

    // Trim por derecha
    while (*ptr == ' ' || *ptr == '\t') --ptr;
    ptr[1] = '\0';

    // Trim interno a Un espacio
    int n;
    char *c;
    ptr = str;
    for (; *ptr; ++ptr) {
        if (*ptr == '\t') {
            *ptr = ' ';
        }
        if (*ptr == ' ') {
            c = ptr + 1;
            n = 0;
            for (; c[n] == ' ' || c[n] == '\t'; ++n);
            if (n) {
                for (; *c = *(c + n); ++c);     // desplazar caracteres a izquierda N espacio
            }
            ++ptr;
        }
    }
    return str;
}

/*
 * Convierte una cadena a mayusculas
 */
void uppercase(char *str) {

    for(; *str; ++str) {
        *str = toupper(*str);
    }
}

/*
 * Retorna el indice de la primera ocurrencia de C en STR. -1 si no se encuentran
 * en la cadena. Negativo si no se encuentra en la cadena.
 */
int indexof(const char *str, int c) {

    char *ptr = strchr(str, c);
    if(ptr < str) {
        return -1;
    }
    return ptr - str;
}

/*
 * Retorna el indice de la ultima ocurrencia de C en STR. -1 si no se encuentran
 * en la cadena.
 */
int lindexof(const char *str, int c) {

    int i = strlen(str) - 1;
    for (; i >= 0; --i) {
        if(str[i] == c) {
            return i;
        }
    }
    return -1;
}

/*
 * Determina si una cadena termina con el caracter C.
 *
 * Retorna 1 si verdadero; 0 en caso contrario
 */
inline int endwith(const char *str, int c) {
    return str[strlen(str) - 1] == c;
}

/*
 * Determina si una cadena inicia con el caracter C.
 *
 * Retorna 1 si verdadero; 0 en caso contrario
 */
inline int startwith(const char *str, int c) {
    return *str == c;
}

/*
 * Guarda en BUFF la subcadena delimitada por los indices S y E
 */
void substr(char *buff, const char *str, int s, int e) {

    int k = 0;
    int i = s;
    for(;i <= e; ++i, ++k) {
        buff[k] = str[i];
    }
    buff[k] = '\0';
}
