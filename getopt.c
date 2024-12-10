#include <stdio.h>
#include <sys/types.h>		// necessário para fork()
#include <unistd.h>		// necessário para o getopt() e fork()
#include <stdlib.h>		// necessário para o atoi/atol
#include <string.h>
#include <signal.h>		// necessário para o kill()

#include "getopt.h"

typedef struct {		// estrutura que vai ter o numero primo e o pid
	pid_t processo;		// numero do processo
	int numero;		// numero primo 
} recebe;

int pdes[2];			// declaração do pipe

void pai(int pdes[2], int p1, char *lvalor, FILE * fp, int m)	// declaração da função do pai
{				// pipe, numero de primos a calcular, nome do ficheiro, ponteiro para o ficheiro, 
	// número de processos criados
	recebe buf;
	int g = 0;		//a variável g representa o número de read() feitos (tem que coincidir com os processos)
	int l = 0;		// a variável l conta o número de primos lidos
	while (g != m) {
		//while (g != m && p1 != l) {   // enquanto não forem lidas as mesmas vezes que os processos, continuar a ler
		read(pdes[0], &buf, sizeof(recebe));
		if (buf.numero == -1)	// quando o número é -1, significa que o processo filho terminou de encher o pipe
			g++;
		else
			l++;
		if (buf.numero != -1 && p1 >= l) {	//se n estivermos no fim do pipe e o número total de primos ainda n tiver sido alcançado
			if (lvalor == NULL) {	//se o ficheiro n for definido na linha de comandos, imprimir no ecrã
				printf
				    ("[PRIMO: %d]\t [PROCESSO: %d]\n",
				     buf.numero, buf.processo);
			} else {
				fprintf(fp,
					"[PRIMO: %d]\t [PROCESSO %d]\n",
					buf.numero, buf.processo);
			}
		} else {	// se já tivermos atingido o número de primos desejados
			if (buf.numero == -1 && p1 <= l) {	// se estivermos no fim de um processo, e se já tivermos os primos
				if (lvalor == NULL)
					printf
					    ("\nO processo %d não é mais necessário, vou matá-lo..\n",
					     buf.processo);
				kill(buf.processo, SIGKILL);	// matam-se os processos que já não são necessários
			}
		}
	}
}
int main(int argc, char **argv)
{
	extern char *optarg;
	char *pvalor = NULL;	// número de primos a achar 
	char *nvalor = NULL;	// número de processos de procura
	char *tvalor = NULL;	// comprimento do subintervalo
	char *lvalor = NULL;	// nome do ficheiro onde guardar o log
	char *gvalor = NULL;	// gama dos valores
	char *num1, *num2;	// num1 é o limite inferior, e o num2 o superior (-g) 

	int o, p1, n1, t1, i, j, m = 0, na, nb, a, b = 0;
	FILE *fp;

	if (pipe(pdes) == -1) {	// inicialização do pipe, se n for possível, dar erro
		perror("Error");
	}

/* documentação usada para usar o getopt():
http://www.opengroup.org/onlinepubs/009695399/functions/getopt.html
http://www.gnu.org/software/libc/manual/html_node/Getopt.html
*/

	while ((o = getopt(argc, argv, "l:g:t:n:p:h")) != -1)
		switch (o) {
		case 'p':
			pvalor = optarg;
			break;
		case 'n':
			nvalor = optarg;
			break;
		case 't':
			tvalor = optarg;
			break;
		case 'l':
			lvalor = optarg;
			break;
		case 'g':
			gvalor = optarg;
			break;
		case 'h':	// todos os outros case têm breaks para que quando se ponha a flag -h nada mais aconteça
			printf("Ajuda:\n\n");
			printf("-p\tNúmero de primos a calcular\n");
			printf
			    ("-n\tNúmero de processos a criar pelo processo detective\n");
			printf
			    ("-t\tComprimento dos sub-intervalos para atribuir a cada processo-filho\n");
			printf
			    ("-l\tFicheiro que irá conter os vários números primos\n");
			printf("-g\tGama limiteInferior:limiteSuperior\n");
			exit(0);

		}


	if (lvalor != NULL) {	// se o ficheiro for especificado, então abre-se de imediato
		fp = fopen(lvalor, "w");	// parte-se do principio que o utilizador não quer binário, e que não quer acomular resultados (w)
		if (fp == NULL) {	// informa o utilizador se n for possível abrir o ficheiro
			printf
			    ("Houve um erro ao abrir o ficheiro para escrita\n");
			exit(6);
		}
	}

	if ((tvalor == NULL) && (nvalor == NULL)) {
		printf
		    ("Deve especificar um dos valores, ou o do número de processos, ou o tamanho do sub-intervalo\n");
		exit(2);
	}

	if (nvalor != NULL) {	// tratamento para quando n são dados valores
		n1 = atoi(nvalor);
	} else {
		n1 = VALORN;
	}
	if (pvalor != NULL) {
		p1 = atoi(pvalor);
	} else {
		p1 = VALORP;
	}
	if (tvalor != NULL) {
		t1 = atoi(tvalor);
	} else {
		t1 = VALORT;
	}
	if (gvalor != NULL) {
		num1 = (char *) malloc(sizeof(char) * 9);	// alocar memória para o limite superior e inferior, sem isto em algumas ocasiões
		num2 = (char *) malloc(sizeof(char) * 9);	// o programa estoirava. O 9 serve para que o long int possa ser utilizado (9 digitos)
		for (i = 0; gvalor[i] != ':'; i++)	//tirar o valor inferior até aos :
			num1[i] = gvalor[i];
		i++;
		for (j = 0; gvalor[i] != '\0'; i++, j++) {	//tirar o valor superior até ao fim
			num2[j] = gvalor[i];
		}
		nb = atol(num1);	//passar a string para long int (9 digitos)
		na = atol(num2);	//claro k 9 digitos é um bocado abusivo...
	}

	else {			// se o valor da gama não for especificado, usar os valores que estão em getopt.h
		nb = VALORB;
		na = VALORA;
	}

	/*      VERIFICAÇÕES SOBRE A VALIDADE DO INPUT DO UTILIZADOR        */

	if ((nvalor != NULL) && (tvalor != NULL)) {
		printf
		    ("Especifique apenas ou o número de processos, ou o tamanho do subintervalo\n");
		exit(2);
	}

	if ((nb) > (na)) {	//até se podia por para trocar
		printf("A gama foi escrita ao contrário....\n");	//mas só se ia gastar mais uma variável :p
		exit(2);
	}

	if (n1 < 1) {
		printf
		    ("O número de processos deve ser sempre maior que 0\n");
		exit(2);
	}

	if (n1 > ((nb + na) / 2)) {	// não consegui gerar um melhor algoritmo para dividir o tamanho da gama pelos processos
		printf
		    ("O número de processos deve ser igual ou menor que o tamanho da gama\n");
		exit(2);
	}

	if (p1 < 1) {
		printf
		    ("O número de primos a calcular deve ser sempre maior que 0\n");
		exit(2);
	}

	if (t1 < 1) {
		printf
		    ("O comprimento do subintervalo deve ser sempre maior que 0\n");
		exit(2);
	}

	if (t1 > (na - nb)) {
		printf
		    ("O tamanho do subintervalo deve ser inferior à gama de valores\n");
		exit(2);
	}


	/*                      FIM DAS VERIFICAÇÕES                    */


	if ((tvalor == NULL) && (nvalor != NULL)) {
		a = ((na - nb) / n1);	// números por cada intervalo
		b = ((na - nb) % n1);
		na = (a) + nb;
	}

	else {
		a = t1;
		n1 = ((na - nb) / t1);
		b = ((na - nb) % n1);
		na = (a) + nb;
	}

	if (nb == 0)
		nb++;

	while (n1 > 0) {	//ciclo para criar número de forks
		if (fork() == 0) {
			calcprimos(nb, na);	// passar o limite inferior e o limite superior
		} else {
			m++;	// por cada fork incrementar o m para depois se poder ler M vezes o pipe (o read bloqueia com o pipe vazio)
		}
		n1--;		// mais um fork criado, passar ao próximo

		if ((tvalor != NULL) && (nvalor == NULL)) {
			nb = nb + t1;
			na = na + t1;
			if (b != 0 && n1 == 1)
				na = na + b;
		}

		else {

			if (b != 0 && n1 == 1)	// ter algum cuidado se a divisão da gama pelos processos n for certa, e acrescentar o resto ao último processo
				na = na + a + b;
			else
				na = na + a;
			nb = nb + a;	// calcular o próximo valor inferior para o próximo fork
		}
	}

	pai(pdes, p1, lvalor, fp, m);	// invocar a função pai com pipe, numero de primos a calcular, nome do ficheiro, ponteiro para o ficheiro, número de processos criados
	close(pdes[1]);		// fechar o pipe (escrita)
	fcloseall();		// fechar os ficheiros
	return 1;
}
