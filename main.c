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

	gera(f,&code,&pFunc);

	pFunc = (funcp) (code);
	printf("Resultado: %d\n",(*pFunc)(3,5,6));

	return 0;
}