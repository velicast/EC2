/* 
 * File:   ec2.h
 *
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 *
 * Created on 29 de octubre de 2010, 09:55 PM
 */

#ifndef _EC2_H
#define	_EC2_H

#include <stdio.h>

/*
 * Codigos de operacion para las instrucciones
 * del procesador
 */
#define LOAD  0x0
#define STORE 0x1
#define ADD   0x2
#define SUB   0x3
#define IN_A  0x4
#define JZ    0x5
#define JP    0x6
#define HALT  0x7

int load(FILE *);
int exec(int);
void core_dump(char, unsigned char, int);

#endif	/* _EC2_H */
