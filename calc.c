/* esta função calcula os primos e representa o filho(s) da função principal */
/*     http://www.numaboa.com.br/criptologia/matematica/primos.php	     */


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {		// estrutura que vai ter o numero primo e o pid
	pid_t processo;		// numero do processo
	int numero;		// numero primo 
} recebe;

void calcprimos(long int a, long int b)	// a representa o limite inferior, e o b o superior
{
	int i, j, k, z;		// inteiros relativos ao cálculo dos números primos
	extern int pdes[2];	// aproveitar o mesmo pipe criado no ficheiro principal
	FILE *fp;
	recebe envia;
	envia.processo = getpid();	// armazenar o pid deste filho na estrutura

//  Debug para o intervalo de números
//      printf("inferior: %d, superior: %d\n", a, b);
//

	for (i = a; i <= b; i++) {	// percorrer números a testar
		if (i <= 1)	// tem que se ter cuidado porque o 1 não é número primo
			i = 2;
		z = 0;		// voltar a por a variável de contagem de divisores a 0
		for (j = i; j > 0; j--) {	// ciclo para percorrer os divisores
			k = i % j;	// calcular o módulo do possível primo por um divisor
			if (k == 0 && j != 1 && j != i)	// se o número tiver divisor, não for igual a ele próprio
				z++;	// nem estiver a ser dividido pela unidade, incrementar o
		}

		if (z == 0) {	// se a condição de cima nunca aconteceu, então tem que ser primo
			envia.numero = i;
			write(pdes[1], &envia, sizeof(recebe));
		}
	}
	i = -1;			// as próximas linhas fazem com que este processo no fim de por os primos no pipe, o encerre com um -1 para que o pai
	envia.numero = i;	// saiba quando acaba um, e começa outro
	write(pdes[1], &envia, sizeof(recebe));
	fclose(fp);		// necessário para os números aparecerem por ordem
}
