/*
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 */
#include "ec2.h"
#include <stdio.h>
#include <string.h>

/*
 * Ejecuta el procesador EC2
 */
int main(int argc, char **argv) {

    FILE *f = 0;
    char *path = 0;
    int mk_cd = 0;  // hacer volcado de nucleo y memoria

    if(argc > 1) {

        int i = 1;
        for(; i < argc; ++i) {
            if(strcmp(argv[i], "-cd") == 0 || strcmp(argv[i], "--coredump") == 0) {
                mk_cd = 1;
            }
            else if((argv[i])[0] == '-') {
                printf("unknown --option: %s\n", argv[i]);
                return -1;
            } else {
                path = argv[i];
            }
        }
    }

    if(!path) {
        printf("ec2: no target to execute\n");
        return 0;
    }

    int exit_code = -1;
    f = fopen(path, "r");
    if(f) {
        if(load(f)) {
            exit_code = exec(mk_cd);
        }
        fclose(f);
    } else {
        printf("error when openning file %s\n", path);
    }
    return exit_code;
}
