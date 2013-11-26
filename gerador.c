/** Victor Yves		  	1212469 */
/** Etiene Dalcol   	1211996 */

#include <stdio.h>
#include <stdlib.h>
#include "gerador.h"

#define TAM_COD 2000
#define PRM_MAX 10
#define VAR_MAX 10		//não precisa conferir qtd de parametros, variaveis e talz, o arquivo de entrada esta sempre certo
#define MAX_FUNCS 33
#define DEBUG 1

void concat (unsigned char * code1, unsigned char * code2, int * idx, int n);
void cmd_ret(char v0, int i0, char v1, int i1, unsigned char * code, int * idx, int line);
void cmd_op(char v1, int i1, char op, char v2, int i2, unsigned char * code, int * idx);
void cmd_func(unsigned char ** func, int * funcIdx, unsigned char * code, int * idx);
void cmd_end(unsigned char * code, int * idx);
void cmd_call(int fc, char v1, int i1,unsigned char ** func, unsigned char *code, int * idx);
void cmd_atr(char v0, int i0, unsigned char *code, int * idx);
void ret_v1(char v1, int i1, unsigned char * code, int * idx, int line);

static void error (const char *msg, int line) {
	fprintf(stderr, "erro %s na linha %d\n", msg, line);
	exit(EXIT_FAILURE);
}


void gera(FILE *f, void **code, funcp *entry){
	int c;
	int line = 1, 
		idx = 0, 
		funcIdx = -1;
	unsigned char * func[MAX_FUNCS];
    unsigned char *codigo = (unsigned char *) malloc (TAM_COD);
    *code = codigo;

	while ((c = fgetc(f)) != EOF) {
		switch (c) {
			case 'f': {  /* function */
		        char c0;
		        if (fscanf(f, "unction%c", &c0) != 1) 
		        	error("comando invalido", line);
			    #if (DEBUG)
					printf("%d - %p:	",idx,&codigo[idx]);
				#endif
		        printf("function\n");
		        cmd_func(func,&funcIdx,codigo,&idx);
		        break;
            }
		    case 'e': {  /* end */
		        fscanf(f, "nd");
		        printf("end\n");
		        cmd_end(codigo,&idx);
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
					#if (DEBUG)
						printf("%d - %p:	",idx,&codigo[idx]);
					#endif
					printf("%c%d = call %d %c%d\n", v0, i0, fc, v1, i1);
					cmd_call(fc,v1,i1,func,codigo,&idx);
		        }
		        else { /* operacao aritmetica */
					int i1, i2;
					char v1 = c0, v2, op;
					if (fscanf(f, "%d %c %c%d", &i1, &op, &v2, &i2) != 4)
						error("comando invalido", line);
					#if (DEBUG)
						printf("%d - %p:	",idx, &codigo[idx]);
					#endif
					printf("%c%d = %c%d %c %c%d\n", v0, i0, v1, i1, op, v2, i2);
					cmd_op(v1,i1,op,v2,i2,codigo,&idx);
		        }
		        #if (DEBUG==2)
					printf(">>>Atribuindo\n");
				#endif
		        cmd_atr(v0,i0,codigo,&idx);
        		break;
		    }
		    case 'r': {  /* ret */
				int i0, i1;
				char v0, v1;
				if (fscanf(f, "et? %c%d %c%d", &v0, &i0, &v1, &i1) != 4)
					error("comando invalido", line);
				#if (DEBUG)
					printf("%d - %p:	",idx, &codigo[idx]);
				#endif
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

	(*entry) = (funcp) func[funcIdx];
}

void cmd_ret(char v0, int i0, char v1, int i1, unsigned char * code, int * idx, int line){
	switch(v0){
		case '$': {
			if(!i0){
				ret_v1(v1, i1, code, idx, line);
			}
			break;
		}
		//TODO quando o primeiro paramentro do ret não é constante , case v, case p
		case 'p': {
			#if (DEBUG == 2)
				printf(">>>>%d Antes do ret P %p:	<<<<<\n", *idx, &code[*idx]);
			#endif
			int bytesJump;
			unsigned char * enderecoJump;
			unsigned char compara[]={0x83,0x7d,(unsigned char) i0*4 + 8,0x00,0x75};
			unsigned char end[] = {0x89, 0xec, 0x5d, 0xc3}; // pop mov e ret

			if(i1 >= PRM_MAX)
				error("numero maximo de parametros excedido. ", line);
			concat(code,compara,idx,5);

			bytesJump = *idx;
			enderecoJump = &code[*idx];

			#if (DEBUG == 2)
				printf(">>>>%d Depois do ret P: %p<<<<\n", *idx, &code[*idx]);
			#endif

			//um espaço onde será colocado posteriormente os bytes de jump
			(*idx)++;

			ret_v1(v1, i1, code, idx, line);			
			concat(code,end,idx,4);
			*enderecoJump = *idx - bytesJump -1;
			#if (DEBUG == 2)
				printf(">>>>%d Depois  do ret P %p<<<<<\n", *idx, &code[*idx]);
			#endif
			break;
		}
		case 'v': {
			#if (DEBUG == 2)
				printf(">>>>%d Antes do ret V %p:	<<<<<\n", *idx, &code[*idx]);
			#endif
			int bytesJump;
			unsigned char * enderecoJump;
			unsigned char compara[]={0x83,0x7d,(unsigned char) i0*-4 -4,0x00,0x75};
			unsigned char end[] = {0x89, 0xec, 0x5d, 0xc3}; // pop mov e ret
			if(i1 >= PRM_MAX)
				error("numero maximo de parametros excedido. ", line);
			concat(code,compara,idx,5);

			bytesJump = *idx;
			enderecoJump = &code[*idx];
			//um espaço onde será colocado posteriormente os bytes de jump
			(*idx)++;

			ret_v1(v1, i1, code, idx, line);	
			concat(code,end,idx,4);
			*enderecoJump = *idx - bytesJump - 1;

			#if (DEBUG == 2)
				printf(">>>>%d Depois  do ret V %p<<<<<\n", *idx, &code[*idx]);
			#endif
			break;
		}
	}
}

void ret_v1(char v1, int i1, unsigned char * code, int * idx, int line){
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
			//TODO local var
			unsigned char mov_eax[] = {0x8b, 0x45};					
			if(i1 >= VAR_MAX)
				error("numero maximo de variaveis excedido. ", line);						
			concat(code,mov_eax,idx,2);
			code[*idx] = (unsigned char) i1*-4 - 4;
			(*idx)++;
			break;
		}
	}
}

void concat (unsigned char * code1, unsigned char * code2, int * idx, int n){
	int i;
	for(i = 0;i < n && (*idx) < TAM_COD; i++) {
		code1[*idx] = code2[i];
		(*idx)++;
	}
}

void cmd_op(char v1, int i1, char op, char v2, int i2, unsigned char * code, int * idx){
	
	if(v1=='$'){
		//mov $i1, %eax
		code[*idx] = 0xb8;
		(*idx)++;
		*( (int *) &code[*idx] ) = i1;  
		(*idx) += 4;
	}else if(v1=='p'){
		//mov 8+4*i1(%ebp),%eax
		unsigned char mov_eax[] = {0x8b, 0x45};
		concat(code,mov_eax,idx,2);
		code[*idx] = (unsigned char) i1*4 + 8;
		(*idx)++;
	}else if(v1=='v'){
		//TODO - mov -4+(-4)*i1(%ebp),%eax
		unsigned char mov_eax[] = {0x8b, 0x45};
		concat(code,mov_eax,idx,2);
		code[*idx] = (unsigned char) i1*-4 - 4;
		(*idx)++;
	}

	if(v2=='$'){
		//mov $i2, %edx
		code[*idx] = 0xba;
		(*idx)++;
		*( (int *) &code[*idx] ) = i2;  
		(*idx) += 4;
	}else if(v2=='p'){
		//mov 8+4*i2(%ebp),%edx
		unsigned char mov_edx[] = {0x8b, 0x55};
		concat(code,mov_edx,idx,2);
		code[*idx] = (unsigned char) i2*4 + 8;
		(*idx)++;
	}else if(v2=='v'){
		//TODO - -4+(-4)*i2(%ebp),%edx
		unsigned char mov_edx[] = {0x8b, 0x55};
		concat(code,mov_edx,idx,2);
		code[*idx] = (unsigned char) i2*-4 -4;
		(*idx)++;
	}

	//make operations with %edx and %eax
	if(op=='+'){
		unsigned char makeop[] = {0x01, 0xd0};
		concat(code,makeop,idx,2);
	}else if(op=='-'){
		unsigned char makeop[] = {0x29, 0xd0};
		concat(code,makeop,idx,2);
	}else if(op=='*'){
		unsigned char makeop[] = {0x0f, 0xaf, 0xc2};
		concat(code,makeop,idx,3);
	}
}

void cmd_atr(char v0, int i0, unsigned char *code, int * idx){
	if(v0=='p'){
		//mov %eax, 8+4*i0(%ebp)
		unsigned char mov_eax[] = {0x89, 0x45};
		concat(code,mov_eax,idx,2);
		code[*idx] = (unsigned char) i0*4 + 8;
		(*idx)++;
	}else if(v0=='v'){
		//TODO mov %eax, -4+(-4)*i0(%ebp)
		unsigned char mov_eax[] = {0x89, 0x45};
		concat(code,mov_eax,idx,2);
		code[*idx] = (unsigned char) i0*-4 - 4;
		(*idx)++;
	}
}

void cmd_func(unsigned char ** func, int * funcIdx, unsigned char * code, int * idx){
	unsigned char begin[] = {0x55, 0x89, 0xe5};
	(*funcIdx)++;
	func[*funcIdx] = &code[*idx];
	#if (DEBUG == 2)
		printf(">>>>Antes de begin %p:	<<<<<\n",&code[*idx]);
	#endif
	concat(code,begin,idx,3);
	#if (DEBUG == 2)
		printf(">>>>Depois de begin %p:	<<<<<\n",&code[*idx]);
	#endif
}

void cmd_end(unsigned char * code, int * idx){
	unsigned char end[] = {0x89, 0xec, 0x5d, 0xc3};
	concat(code,end,idx,4);
}

void cmd_call(int fc, char v1, int i1,unsigned char ** func, unsigned char *code, int * idx){
	unsigned char add4esp[] = {0x83, 0xc4, 0x04};

	if(v1=='$'){
		code[*idx] = 0x68; // push
		(*idx)++;
		*( (int *) &code[*idx] ) = i1; 
		(*idx) += 4;
	} else if(v1=='p'){
		//push i1*4 + 8(%ebp)
		unsigned char pushl[] = {0xff, 0x75};
		concat(code,pushl,idx,2);
		code[*idx] = (unsigned char) i1*4 + 8;
		(*idx)++;
	} else if(v1=='v'){
		//TODO push i1*-4 - 4(%ebp)
		unsigned char pushl[] = {0xff, 0x75};
		concat(code,pushl,idx,2);
		code[*idx] = (unsigned char) i1*-4 -4;
		(*idx)++;
	}

	code[*idx] = 0xe8; // call
	(*idx)++;
	*( (int *) &code[*idx] ) = func[fc] - &code[(*idx)+4];
	#if (DEBUG==2)
		printf(">>>Deslocamento do call(f%d %p - prox linha %p): %p\n",fc, func[fc], &code[(*idx)+4], (void *)(func[fc] - &code[(*idx)+4]));
	#endif
	(*idx) += 4;
	concat(code,add4esp,idx,3);
}