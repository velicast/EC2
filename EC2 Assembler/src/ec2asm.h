/* 
 * File:   ec2asm.h
 *
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 *
 * Created on 30 de octubre de 2010, 12:13 PM
 *
 * Ensamblador basico para el Microprocesador EC-2
 */
#ifndef _EC2ASM_H
#define	_EC2ASM_H

#include <stdio.h>

#define MAX_SIMBOL 50
#define MAX_LINE 1024

#define LOAD  0x0
#define STORE 0x1
#define ADD   0x2
#define SUB   0x3
#define IN    0x4
#define JZ    0x5
#define JP    0x6
#define HALT  0x7

void st_add(char *, int);
int  st_remove(const char *, int *);
int  st_find(const char *, int *);
void st_print();

int is_rword(const char *);
int readl(FILE *, char *);
int invalid_name(const char *);
int parse_op(char *);
int check_line(char *, int, int, int *);
int encode(char *, int, int *, int *);
int assemble(FILE *, FILE *, int);

#endif	/* _EC2ASM_H */
