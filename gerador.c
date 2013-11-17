#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gerador.h"

#define TAM_COD 2000
#define PRM_MAX 10

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
					cmd_atr(v0,i0,v1,i1,op,v2,i2,codigo,&idx,line);
		        }
        		break;
		    }
		    case 'r': {  /* ret */
				int i0, i1;
				char v0, v1;
				if (fscanf(f, "et? %c%d %c%d", &v0, &i0, &v1, &i1) != 4)
					error("comando invalido", line);
				printf("ret? %c%d %c%d\n", v0, i0, v1, i1);
				cmd_ret(v0,i0,v1,i1,codigo,&idx,line);
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
		printf("Resultado: %d\n",(*pFunc)(3,5,6));
	}

}

void cmd_ret(char v0, int i0, char v1, int i1, unsigned char * code, int * idx, int line){
	unsigned char bottom[] = {0x89, 0xec, 0x5d, 0xc3};
	switch(v0){
		case '$': {
			if(!i0){
				switch(v1){
					case '$':{	//const
						code[*idx] = 0xb8;
						(*idx)++;
						*( (int *) &code[*idx] ) = i1;  
						(*idx) += 4;
						break;						
					}
					case 'p':{ //parameter
						unsigned char mov_eax[] = {0x8b, 0x45};					
						if(i1 >= PRM_MAX)
							error("numero maximo de parametros excedido. ", line);						
						concat(code,mov_eax,idx,2);
						code[*idx] = (unsigned char) i1*4 + 8;
						(*idx)++;
						break;
					}
					case 'v':{ // var
						//TODO
						break;
					}
				}
			}
			break;
		}
	}
	concat(code,bottom,idx,4);
}



void concat (unsigned char * code1, unsigned char * code2, int * idx, int n){
	int i;
	for(i = 0;i < n && (*idx) < TAM_COD; i++) {
		code1[*idx] = code2[i];
		(*idx)++;
	}
}

void cmd_atr(char v0, int i0, char v1, int i1, char op, char v2, int i2, unsigned char * code, int * idx, int line){
	if(v0=='p'){//parameter
		//mov to p
		unsigned char movtop[] = {0x89, 0x55, 0x08};
		if(v1=='$'){
			//mov $i1, %edx
			code[*idx] = 0xba;
			(*idx)++;
			*( (int *) &code[*idx] ) = i1;  
			(*idx) += 4;
		}else if(v1=='p'){
			//mov 8+4*i1(%ebp),%edx
			unsigned char mov_edx[] = {0x8b, 0x55};
			concat(code,mov_edx,idx,2);
			code[*idx] = (unsigned char) i1*4 + 8;
			(*idx)++;
		}
		if(v2=='$'){
			//mov $i1, %ecx
			code[*idx] = 0xb9;
			(*idx)++;
			*( (int *) &code[*idx] ) = i2;  
			(*idx) += 4;
		}else if(v2=='p'){
			//mov 8+4*i1(%ebp),%ecx
			unsigned char mov_ecx[] = {0x8b, 0x4d};
			concat(code,mov_ecx,idx,2);
			code[*idx] = (unsigned char) i2*4 + 8;
			(*idx)++;
		}
		//make operations with %ecx and %edx
		if(op=='+'){
			unsigned char makeop[] = {0x01, 0xca};
			concat(code,makeop,idx,2);
		}else if(op=='-'){
			unsigned char makeop[] = {0x29, 0xca};
			concat(code,makeop,idx,2);
		}else if(op=='*'){
			unsigned char makeop[] = {0x0f, 0xaf, 0xd1};
			concat(code,makeop,idx,3);
		}
		//mov edx to p
		concat(code,movtop,idx,3);
	}else{
		//TODO - Var
	}
}
