#include <stdio.h>
#include <stdlib.h>
#include "gerador.h"

int main(void)
{
	FILE * f;
	funcp pFunc;
	void * code;

	f = fopen("entrada.sb","r");
	if(!f){
		printf("Erro: Nao foi possivel abrir arquivo.");
		exit(1);
	}

	gera (f,&code,&pFunc);

	printf("Resultado: %d\n",(*pFunc)(10));

	libera(code);

	return 0;
}