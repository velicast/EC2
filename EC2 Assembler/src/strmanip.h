/* 
 * File:   strmanip.h
 *
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 *
 * Created on 31 de octubre de 2010, 11:49 AM
 */
#ifndef _STRMANIP_H
#define	_STRMANIP_H

#include <string.h>
#include <ctype.h>

int xatoi(const char *, int *);
void parsex(char *, unsigned);

void uppercase(char *);
int indexof(const char *, int);
int lindexof(const char *, int);
int endwith(const char *, int);
int startwith(const char *, int);
void substr(char *, const char *, int, int);

#endif	/* _STRMANIP_H */
