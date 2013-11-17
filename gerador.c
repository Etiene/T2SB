#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gerador.h"

#define TAM_COD 2000

static void error (const char *msg, int line) {
	fprintf(stderr, "erro %s na linha %d\n", msg, line);
	exit(EXIT_FAILURE);
}


void gera(FILE *f, void **code, funcp *entry){
	int c;
	int line = 1;
	int idx = 0;
	unsigned char top[] = {0x55, 0x89, 0xe5};

	//eu acho q esse codigo aqui é o code q é pra vir da main
	//mas ainda nao entendi isso direito e coloquei aqui por enquanto
	//pra ter algo funcionando
	unsigned char codigo[TAM_COD];


	concat(codigo,top,&idx,3);

	while ((c = fgetc(f)) != EOF) {
		switch (c) {
			case 'f': {  /* function */
		        char c0;
		        if (fscanf(f, "unction%c", &c0) != 1) 
		        	error("comando invalido", line);
		        printf("function\n");
		        break;
            }
		    case 'e': {  /* end */
            	char c0;
		        if (fscanf(f, "nd%c", &c0) != 1) 
		        	error("comando invalido", line);
		        printf("end\n");
		        break;
		    }
		    case 'v': 
		    case 'p': {  /* atribuicao */
		    	int i0;
		        char v0 = c, c0;

		        if (fscanf(f, "%d = %c", &i0, &c0) != 2)
		        	error("comando invalido", line);

		        if (c0 == 'c') { /* call */
					int fc, i1;
					char v1;
					if (fscanf(f, "all %d %c%d", &fc, &v1, &i1) != 3) 
						error("comando invalido", line);
					printf("%c%d = call %d %c%d\n", v0, i0, fc, v1, i1);
		        }
		        else { /* operacao aritmetica */
					int i1, i2;
					char v1 = c0, v2, op;
					if (fscanf(f, "%d %c %c%d", &i1, &op, &v2, &i2) != 4)
						error("comando invalido", line);
					printf("%c%d = %c%d %c %c%d\n", v0, i0, v1, i1, op, v2, i2);
		        }
        		break;
		    }
		    case 'r': {  /* ret */
				int i0, i1;
				char v0, v1;
				if (fscanf(f, "et? %c%d %c%d", &v0, &i0, &v1, &i1) != 4)
					error("comando invalido", line);
				printf("ret? %c%d %c%d\n", v0, i0, v1, i1);
				cmd_ret(v0,i0,v1,i1,codigo,&idx);
				break;
    		}
		    default: 
		    	error("comando desconhecido", line);
		}

  		line++;	
  		fscanf(f, " ");
	}

	//teste
	{
		funcp pFunc = (funcp) codigo;
		printf("Resultado: %d\n",(*pFunc)(3));
	}

}

void cmd_ret(char v0, int i0, char v1, int i1, unsigned char * code, int * idx){
	switch(v0){
		case '$': {
			if(!i0){
				switch(v1){
					case '$':{
						unsigned char bottom[] = {0x89, 0xec, 0x5d, 0xc3};
						code[*idx] = 0xb8;
						(*idx)++;
						*( (int *) &code[*idx] ) = i1;  
						(*idx) += 4;
						concat(code,bottom,idx,4);
						break;
					}
					default:
						break;

				}
			}
			break;
		}
		default:
			break;
	}
}

void concat (unsigned char * code1, unsigned char * code2, int * idx, int n){
	int i;
	for(i = 0;i < n && (*idx) < TAM_COD; i++) {
		code1[*idx] = code2[i];
		(*idx)++;
	}
}