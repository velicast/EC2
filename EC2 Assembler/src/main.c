/*
 * File:   main.c
 *
 * Eduar Castrillo Velilla
 * Ingenieria de Sistemas
 * Universidad Nacional de Colombia
 *
 * Created on 30 de octubre de 2010, 10:38 AM
 */

#include <stdio.h>
#include "ec2asm.h"

/*
 *
 */
int main(int argc, char** argv) {

    FILE *in, *out;
    int summary = 0;
    char *inpath = 0, *outpath = 0;

    if(argc > 1) {
        
        int i = 1;
        for(; i < argc; ++i) {
            if(strcmp(argv[i], "-sm") == 0 || strcmp(argv[i], "--summary") == 0) {
                summary = 1;
            }
            else if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
                if(++i < argc) {
                    outpath = argv[i];
                } else {
                    printf("ec2asm: no output file\n");
                    return 0;
                }
            }
            else if((argv[i])[0] == '-') {
                printf("unknown --option: %s\n", argv[i]);
                return -1;
            } else {
                inpath = argv[i];
            }
        }
    }

    if(!inpath) {
        printf("ec2asm: no input file\n");
        return 0;
    }

    if(!outpath) {
        outpath = "./out.init";
    }

    in = fopen(inpath, "r");
    if(!in) {
        printf("error when openning file %s\n", inpath);
        return -1;
    }

    out = fopen(outpath, "w");
    if(!out) {
        printf("error when openning file %s\n", outpath);
        fclose(in);
        return -1;
    }

    if(assemble(in, out, summary)) {
        if(summary) {
            printf("BUILD SUCCESSFUL\n");
        }
    } else {
        printf("BUILD FAILED\n");
    }
    fclose(in);
    fclose(out);

    return 0;
}
