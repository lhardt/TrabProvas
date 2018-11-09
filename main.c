#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Tamanho máximo de uma linha da entrada */
#define TAM_MAX_LINHA 255

struct Questao {
	/* Título da questão */
	char * enunciado;
	/* O índice da resposta certa */
	int respostaCerta;
	/* as quatro alternativas da questão */
	char * alternativas[4];
	char dificuldade;
};

/* Função auxiliar, como a fpeek do C++.
 * 'Espia' como será o próximo caracter de um arquivo.
 * Autor: Léo H.;
 */
int fpeek( FILE * stream ) {
    int c;
    c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

/* Se o caracter é o cabeçalho de uma alternativa (a, b, c, ou d)
 *
 * Autor: Léo H.;
 */
int isAlternativa( char diff ){
	switch( diff ) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
			return 1;
	}
	return 0;
}
/* 
	A alternativa, no arquivo, é uma letra. Mas conceitualmente é um número.
	A função é uma conversão entre as duas.
	Ex de conversão: a=0, b=1, c=2, d=3

	Autor: Luisa;
*/
int charParaAlternativa( char diff )
{
	switch(diff)
	{
		case 'a':
			diff = 0;
			break;
		case 'b':
			diff = 1;
			break;
		case 'c':
			diff = 2;
			break;
		case 'd':
			diff = 3;
			break;
	}	
	return diff;
}

/* Lê até que encontre uma nova linha com um dígito (uma nova questão)
 * ou uma letra (uma nova alternativa).
 *
 * Autor: Léo H.
 */
char * ateNovoToken(FILE * f) {
	int strSz = 1, strLen = 0, i = 0;
	char * str = calloc(strSz, sizeof(char));
	char * linha = calloc( TAM_MAX_LINHA, sizeof(char));
	str[0] = 0;
	do {
		fgets(linha, TAM_MAX_LINHA, f);
		strLen += strlen(linha);
		str = realloc(str, 1 +strLen);
		strcat(str, linha);
	} while( !feof(f) && !isdigit(fpeek(f)) && !isAlternativa(fpeek(f)) );

	for(i = strLen - 1; i > 0; i-- ){
		if(!isprint(str[i]))
			str[i] = '\0';
		else break;
	}
	free(linha);
	return str;
}


/* Ler de um arquivo as questões 
	
	Autor: ;
*/
void doArquivo(struct Questao ** vetQuestoes, int * qtdQuestoes){
	FILE * entrada = fopen("entrada.txt", "rt");
	*qtdQuestoes = 0;
	if( entrada != NULL ){
		/* Só para encaixar no scanf. O número da questão aqui é sem propósito. */
		int dummy = 0;
		/* Em qual questão o programa opera atualmente */
		int itQst = 0;
		/* Quantas questões foram alocadas no vetQuestoes. */
		int numQstAlocado = 10;
		/* Porque vetQuestoes não foi inicializado como vetor, o inicializamos. */
		*vetQuestoes = calloc(numQstAlocado,sizeof(struct Questao));
		while( !feof(entrada) ){
			/* Expandir o vetor se necessário */
			if( itQst >= numQstAlocado -1 ){
				numQstAlocado += 10;
				*vetQuestoes = realloc(*vetQuestoes, numQstAlocado * sizeof(struct Questao));
				if( *vetQuestoes == NULL ) {
					printf("Sem memória! Encerrando o programa...\n");
					exit(-1);
				}
			}
			fscanf(entrada, "%d %c ", &dummy, &((*vetQuestoes)[itQst]).dificuldade);
			fflush(entrada);
			(*vetQuestoes)[itQst].enunciado = ateNovoToken(entrada);
			(*vetQuestoes)[itQst].alternativas[0] = ateNovoToken(entrada);
			(*vetQuestoes)[itQst].alternativas[1] = ateNovoToken(entrada);
			(*vetQuestoes)[itQst].alternativas[2] = ateNovoToken(entrada);
			(*vetQuestoes)[itQst].alternativas[3] = ateNovoToken(entrada);

			itQst++;
		}
		*qtdQuestoes = itQst;
		fclose(entrada);
	} else {
		*vetQuestoes = 0;
	}
}

/* Limpa o vetor de questões do programa.
 *
 *	Autor: ;
 */
void freeQuestoes();

/* Ler a quantidade de questões do usuário
	
	Autor: ;
*/
int lerQuantQuestoes();
/* A ordem das questões. Use como o índice da lista
	
	Autor: ;
*/
int * criarOrdemAleatoria();
/* Coloca uma lista de questões no arquivo da prova e do gabarito
	
	Autor: ;
*/
void criarArquivos( struct Questao * questoes, int noQuestoes, int * embaralhamento );
/* Embaralhar alternativas de uma tal questão 
	
	Autor: ;
*/
void embaralharAlternativas( struct Questao * questoes );


int main(){
	struct Questao * questoes;
	int qtdQuestoes;
	
	doArquivo(&questoes, &qtdQuestoes);
	if( qtdQuestoes != 0 ){
		// ...
	} else {
		// erro no arquivo
	}
	// freeQuestoes( questoes );
	
	return 0;
}
