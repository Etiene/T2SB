/** Victor Yves			1212469 */
/** Etiene Dalcol		1211996 */

#include <stdio.h>
#include <stdlib.h>
#include "gerador.h"

#define TAM_COD 2000
#define PRM_MAX 10
#define VAR_MAX 10             
#define MAX_FUNCS 33
#define MAX_VARS 10

/**
	code:			vetor com o código de máquina
	idx:			índice que aponta pra última posição do vetor code
	line:			linha atual do arquivo de entrada
	declaredVars:	vetor que indica se a variável já foi declarada
	func:			vetor que armazena o endereço de cada função em code
	funcIdx:		índice que aponta pra última posição do vetor func
*/


/**concat: adiciona o conteúdo de code2 no final de code1
	n: tamanho do vetor code2
*/
void concat (unsigned char * code1, unsigned char * code2, int * idx, int n);

/**cmd_ret: trata os casos de retorno em SB
	v0: 	indica se o primeiro elemento é um parâmetro ou uma variável
	i0: 	índice do primeiro parâmetro/variável
	v1:		indica se o segundo elemento é um parâmetro ou uma variável
	i1:		índice do segundo parâmetro/variável
*/
void cmd_ret(char v0, int i0, char v1, int i1, unsigned char * code, int * idx, int line);

/**cmd_op: trata os casos de operação aritmética em SB
	v1: 	indica se o primeiro elemento é um parâmetro ou uma variável
	i1: 	índice do primeiro parâmetro/variável
	v2:		indica se o segundo elemento é um parâmetro ou uma variável
	i2:		índice do segundo parâmetro/variável
	op:		identifica a operação aritmética
*/
void cmd_op(char v1, int i1, char op, char v2, int i2, unsigned char * code, int * idx, int declaredVars[MAX_FUNCS][MAX_VARS], int line, int funcIdx);

/**cmd_fun: trata o caso de criação de uma função*/
void cmd_func(unsigned char ** func, int * funcIdx, unsigned char * code, int * idx);

/**cmd_end: trata o caso de finalização de uma função*/
void cmd_end(unsigned char * code, int * idx);

/**cmd_call: trata o caso de chamada de função em SB
	fc: índice que identifica a função a ser chamada em func
*/
void cmd_call(int fc, char v1, int i1,unsigned char ** func, unsigned char *code, int * idx);

/**cmd_atr: trata o caso de atribuição em SB*/
void cmd_atr(char v0, int i0, unsigned char *code, int * idx, int declaredVars[MAX_FUNCS][MAX_VARS], int funcIdx );

/**ret_v1: função auxiliar de cmd_ret, insere o código de máquina em code*/
void ret_v1(char v1, int i1, unsigned char * code, int * idx, int line);

static void error (const char *msg, int line) {
    fprintf(stderr, "erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}


void gera(FILE *f, void **code, funcp *entry){
    int c, i,j;
    int line = 1, 
        idx = 0, 
        funcIdx = -1;
    unsigned char * func[MAX_FUNCS];
    int declaredVars[MAX_FUNCS][MAX_VARS];
    unsigned char *codigo = (unsigned char *) malloc (TAM_COD);
    *code = codigo;

    for(i=0;i<MAX_FUNCS;i++)
         for(j=0;j<MAX_VARS;j++)
            declaredVars[i][j]=0;

    while ((c = fgetc(f)) != EOF) {
    	switch (c){
        	case 'f': {  /* function */
            	char c0;
                if (fscanf(f, "unction%c", &c0) != 1) 
                        error("comando invalido", line);
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
                    printf("%c%d = call %d %c%d\n", v0, i0, fc, v1, i1);
                    cmd_call(fc,v1,i1,func,codigo,&idx);
                }
                else { /* operacao aritmetica */
              		int i1, i2;
                    char v1 = c0, v2, op;
                    if (fscanf(f, "%d %c %c%d", &i1, &op, &v2, &i2) != 4)
                        error("comando invalido", line);
                    printf("%c%d = %c%d %c %c%d\n", v0, i0, v1, i1, op, v2, i2);
                    cmd_op(v1,i1,op,v2,i2,codigo,&idx,declaredVars,line,funcIdx);
                }
                cmd_atr(v0,i0,codigo,&idx,declaredVars,funcIdx);
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

    (*entry) = (funcp) func[funcIdx];
}

void cmd_ret(char v0, int i0, char v1, int i1, unsigned char * code, int * idx, int line){
   if(v0 == '$') {
        if(!i0){
            ret_v1(v1, i1, code, idx, line);
        }
    }
    else{
        int bytesJump;
        unsigned char * enderecoJump;
        unsigned char compara[]={0x83,0x7d,'?',0x00,0x75};   /*cmpl $0, ? / jne */
        unsigned char end[] = {0x89, 0xec, 0x5d, 0xc3}; /* pop mov e ret*/

        if(v0 == 'p')
            compara[2] = (unsigned char) i0*4 + 8;
        else if(v0 == 'v')
            compara[2] = (unsigned char) i0*-4 -4;

        if(i1 >= PRM_MAX)
            error("numero maximo de parametros excedido. ", line);
        concat(code,compara,idx,5);

        bytesJump = *idx;
        enderecoJump = &code[*idx];
        (*idx)++;   //um espaço onde será colocado posteriormente os bytes de jump

        ret_v1(v1, i1, code, idx, line);        
        concat(code,end,idx,4);
        *enderecoJump = *idx - bytesJump - 1;
    }
}

void ret_v1(char v1, int i1, unsigned char * code, int * idx, int line){
	if(v1 == '$'){	//const
        code[*idx] = 0xb8;
        (*idx)++;
        *( (int *) &code[*idx] ) = i1;  
        (*idx) += 4;                                               
    }
    else{
        unsigned char mov_eax[] = {0x8b, 0x45,'?'};
        if(v1 == 'p')
            mov_eax[2] = (unsigned char) i1*4 + 8;
        else if(v1 == 'v')
            mov_eax[2] = (unsigned char) i1*-4 - 4;
                                            
        if(i1 >= PRM_MAX)
            error("numero maximo de parametros excedido. ", line);                                                
        concat(code,mov_eax,idx,3);
	}
}

void concat (unsigned char * code1, unsigned char * code2, int * idx, int n){
    int i;
    for(i = 0;i < n && (*idx) < TAM_COD; i++) {
        code1[*idx] = code2[i];
        (*idx)++;
    }
}

void cmd_op(char v1, int i1, char op, char v2, int i2, unsigned char * code, int * idx, int declaredVars[MAX_FUNCS][MAX_VARS], int line, int funcIdx){
        
    if(v1=='$'){ //mov $i1, %eax
        code[*idx] = 0xb8;
        (*idx)++;
        *( (int *) &code[*idx] ) = i1;  
        (*idx) += 4;
    }
    else if(v1=='p'){ //mov 8+4*i1(%ebp),%eax
        unsigned char mov_eax[] = {0x8b, 0x45,(unsigned char) i1*4 + 8};
        concat(code,mov_eax,idx,3);
    }
    else if(v1=='v'){ // mov -4+(-4)*i1(%ebp),%eax                
        unsigned char mov_eax[] = {0x8b, 0x45, (unsigned char) i1*-4 - 4};
        if(!declaredVars[funcIdx][i1])
            error("Var nao declarada, usada pela primeira vez aqui", line);
        concat(code,mov_eax,idx,3);
    }

    if(v2=='$'){ //mov $i2, %edx
        code[*idx] = 0xba;
        (*idx)++;
        *( (int *) &code[*idx] ) = i2;  
        (*idx) += 4;
    }else if(v2=='p'){ //mov 8+4*i2(%ebp),%edx
        unsigned char mov_edx[] = {0x8b, 0x55,(unsigned char) i2*4 + 8};
        concat(code,mov_edx,idx,3);
    }
    else if(v2=='v'){// -4+(-4)*i2(%ebp),%edx
        unsigned char mov_edx[] = {0x8b, 0x55,(unsigned char) i2*-4 -4};                
        if(!declaredVars[funcIdx][i2])
            error("Var nao declarada, usada pela primeira vez aqui", line);
        concat(code,mov_edx,idx,3);
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

void cmd_atr(char v0, int i0, unsigned char *code, int * idx, int declaredVars[MAX_FUNCS][MAX_VARS], int funcIdx ){
    if(v0=='p'){ //mov %eax, 8+4*i0(%ebp)
        unsigned char mov_eax[] = {0x89, 0x45,(unsigned char) i0*4 + 8};
        concat(code,mov_eax,idx,3);
    }
    else if(v0=='v'){
        unsigned char sub_esp[] = {0x83, 0xec, (unsigned char) i0*4 + 4};
        unsigned char mov_eax[] = {0x89, 0x45, (unsigned char) i0*-4 - 4};
        if(!declaredVars[funcIdx][i0]){ //se a var não foi declarada ainda, diminuir esp
            concat(code,sub_esp,idx,3);
            declaredVars[funcIdx][i0]=1;
        }
        concat(code,mov_eax,idx,3);
    }
}

void cmd_func(unsigned char ** func, int * funcIdx, unsigned char * code, int * idx){
    unsigned char begin[] = {0x55, 0x89, 0xe5};
    (*funcIdx)++;
    func[*funcIdx] = &code[*idx];
    concat(code,begin,idx,3);
}

void cmd_end(unsigned char * code, int * idx){
    unsigned char end[] = {0x89, 0xec, 0x5d, 0xc3};
    concat(code,end,idx,4);
}

void cmd_call(int fc, char v1, int i1,unsigned char ** func, unsigned char *code, int * idx){
    if(v1=='$'){
        code[*idx] = 0x68; // push
        (*idx)++;
        *( (int *) &code[*idx] ) = i1; 
        (*idx) += 4;
	} else if(v1=='p'){
        //push i1*4 + 8(%ebp)
        unsigned char pushl[] = {0xff, 0x75,(unsigned char) i1*4 + 8};
        concat(code,pushl,idx,3);
    }else if(v1=='v'){
        // push i1*-4 - 4(%ebp)
        unsigned char pushl[] = {0xff, 0x75,(unsigned char) i1*-4 -4};
        concat(code,pushl,idx,3);
    }

    code[*idx] = 0xe8; // call
    (*idx)++;
    *( (int *) &code[*idx] ) = func[fc] - &code[(*idx)+4];
    (*idx) += 4;
}

void libera (void* p){
    free(p);
}