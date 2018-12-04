#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <locale.h>

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

/*
	A alternativa, no arquivo, é uma letra. Mas conceitualmente é um número.
	A função é uma conversão entre as duas.
	Ex de conversão: a=0, b=1, c=2, d=3
	Autor: Luísa;
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
/* Inverso da função de cima. Utilizada na hora da impressão dos arquivos.
Autor: Luísa*/
char altParaChar( int diff )
{
	switch(diff)
	{
		case 0:
			diff = 'a';
			break;
		case 1:
			diff = 'b';
			break;
		case 2:
			diff = 'c';
			break;
		case 3:
			diff = 'd';
			break;
	}
	return diff;
}


/* Lê até que encontre uma nova linha com um dígito (uma nova questão)
 * ou uma letra (uma nova alternativa).
 *
 * Autor: Léo H.
 */
char * ateTerminador(FILE * f, char * terminators)  {
	int strLen = 0; /* Tamanho de str */
	int i = 0;
	char * str = calloc(1, sizeof(char));
	char * linha = calloc( TAM_MAX_LINHA, sizeof(char));
	while( !feof(f) && strchr(terminators,fpeek(f)) == NULL ) {
		fgets(linha, TAM_MAX_LINHA, f);
		strLen += strlen(linha);
		str = realloc(str, 1 + strLen);
		strcat(str, linha);
	}
	/* Elimina caracteres redundantes */
	for(i = strLen - 1; i > 0; i-- ){
		if(!isprint(str[i]))
			str[i] = '\0';
		else break;
	}
	free(linha);
	return str;
}

/* Ler de um arquivo as questões

	Autor: Léo H.;
*/
void doArquivo(struct Questao ** vetQuestoes, int * qtd){
	/* Quantas questões foram alocadas no vetQuestoes. */
	int numQstAlocado = 10;
	FILE * entrada = fopen("entrada.txt", "rt");
	*qtd = 0;
	*vetQuestoes = (struct Questao*) calloc(numQstAlocado,sizeof(struct Questao));;
	if( entrada == NULL || *vetQuestoes == NULL ){
		fprintf(stderr, "Sem memória! Encerrando o programa...\n");
		exit(-1);
	} else {
		while( !feof(entrada) ){
			/* Expandir o vetor se necessário */
			if( (*qtd) >= numQstAlocado -1 ){
				numQstAlocado += 10;
				*vetQuestoes = (struct Questao*) realloc(*vetQuestoes, numQstAlocado * sizeof(struct Questao));
				if(*vetQuestoes == NULL ) {
					printf("Sem memória! Encerrando o programa...\n");
					exit(-2);
				}
			}
			fscanf(entrada, "%*d %c ", &((*vetQuestoes)[*qtd]).dificuldade);
			fflush(entrada);
			/* Quando tivermos menos alternativas do que esperávamos, resulta strings em branco.
			 	Porém, se tivessemos menos que duas alternativas, o programa não tem sentido. */
			(*vetQuestoes)[*qtd].enunciado = ateTerminador(entrada, "a");
				
			(*vetQuestoes)[*qtd].alternativas[0] = ateTerminador(entrada, "b");
			(*vetQuestoes)[*qtd].alternativas[1] = ateTerminador(entrada, "c123456789");
			(*vetQuestoes)[*qtd].alternativas[2] = ateTerminador(entrada, "d123456789");
			(*vetQuestoes)[*qtd].alternativas[3] = ateTerminador(entrada, "123456789");

			(*qtd)++;
		}
		fclose(entrada);
	}
}

/* A ordem das questões e das alternativas. Use como o índice da lista

	Autor: Ana;
*/
int *criarOrdemAleatoria(int numItens, int numMin, int numMax)
{
	srand((unsigned)time(NULL));
	
	int y,x,*usado,*novo_indice;

	usado = (int*)calloc(numItens, sizeof(int));
	novo_indice = (int*)malloc(numItens*sizeof(int));
	if( novo_indice == NULL ){
		exit(4);
	}
    
	for (x=0;x<numItens;x++)
	{
		novo_indice[x] = numMin+ (rand() % abs(numMax-numMin));
	    do
	    {
	        usado[x]=0;
	        for (y=0;y<x;y++)
	        {
	        	if (novo_indice[x]==novo_indice[y]){
	        		usado[x] = 1;
	        		novo_indice[x] = numMin+ (rand() % abs(numMax-numMin));
				//break;
				}
			}            
	    }while(usado[x]==1);
	}	
    return  novo_indice;
}

/* Coloca uma lista de questões no arquivo da prova e do gabarito

	Autor: ;
*/
void embaralharPerguntas (int quant_vet[],struct Questao questoes[], int numItens)
{
	int *indMedio, *indDificil, *indice,x,y,tam;
	char *novaOrdem[numItens];
	
	indice = criarOrdemAleatoria (quant_vet[0],0,40);//40 ou 39?
	indMedio = criarOrdemAleatoria (quant_vet[1],40,80);//80 ou 79?
	indDificil = criarOrdemAleatoria (quant_vet[2],80,120);//120 ou 119?
  	
	printf ("\n\nEMBARALHARPERGUNTAS");
	
	indice = (int*) realloc (indice, numItens*sizeof(int));
	
	for (x=0;x<quant_vet[1];x++){
		indice [quant_vet[0]+x] = indMedio [x];
	}
	for (x=0;x<quant_vet[2];x++){
		indice [quant_vet[1]+quant_vet[2]+x] = indDificil [x];
	}
	
	printf ("\nINDICE: ");
	for (x=0;x<numItens;x++){
		printf ("%d ", indice [x]);
	}
	
	for (x=0; x<numItens;x++){		
		tam = strlen (questoes[indice[x]].enunciado);
    		printf ("\nTam: %d \n", tam);
		novaOrdem[x] = (char*) malloc (tam*sizeof(char));
		
		if(novaOrdem[x]==NULL){
			exit(2);
		}		
    		printf ("oi\n");
		strcpy (novaOrdem[x], questoes[indice[x]].enunciado);		
		puts (novaOrdem[x]);		
   		printf ("\n");
	}	
	for (y=0;y<numItens;y++){
		tam = strlen (novaOrdem[y]);
		
		free (questoes[y].enunciado);
		questoes[y].enunciado = (char*) malloc (tam*sizeof(char));
		
		strcpy (questoes[y].enunciado,novaOrdem[y]);
	}	
}

void embaralharAlternativas(struct Questao *pQuestao)
{
	int x,y, tam, *indice;
	char *novaOrdem[4];

    	indice = criarOrdemAleatoria(4, 0, 4);// quant,min,max
    
	for (x=0; x<4;x++){
		tam = strlen ((*pQuestao).alternativas[indice[x]]);
		novaOrdem[x] = (char*) malloc (tam*sizeof(char));
		if(novaOrdem[x]==NULL){
			exit(2);
		}		
		strcpy (novaOrdem[x], (*pQuestao).alternativas[x]);		
		novaOrdem[x][0] = 'a' + x;
		
		if (indice[x]==0){
			(*pQuestao).respostaCerta= x;
		}
	}
	for (y=0;y<4;y++){
		tam = strlen (novaOrdem[y]);
		
		free ((*pQuestao).alternativas[y]);
		(*pQuestao).alternativas[y] = (char*) malloc (tam*sizeof(char));
		
		strcpy ((*pQuestao).alternativas[y],novaOrdem[y]);
	}
}

/*Impressão dos arquivos.
Autor: Luísa */
void imprimir (int quant_vet [], struct Questao questoes [])
{
		FILE *arquivo_final, *gabarito;
		int i=0, x, contador=0, numero_questao_gab =0;
		char alt, letra;

		arquivo_final = fopen ("Prova.txt", "wt");
		gabarito = fopen ("Gabarito.txt", "wt");

    fprintf(gabarito, "GABARITO\n\n"); //impressão do gabarito no arquivo

  for(x=0; x<3; x++) //de 0 a 2, pelo nível de dificuldade
  {
    i=0;
      do{
        alt = altParaChar(questoes[i].respostaCerta);
        fprintf(gabarito, "%d - %c\n", numero_questao_gab+1, alt);
        i++;
        numero_questao_gab++;
      } while (i < quant_vet [x]); //percorre a quantidade de perguntas do nível de dificuldade estabelecido conforme indicada no início
  }
}

int main(){
  setlocale(LC_ALL, "Portuguese");

	struct Questao * questoes;
	int qtdQuestoes, quant_vet[3],x, numItens,i;

	printf("Informe a quantidade de questões: \n");
	printf("Nível Fácil: ");
	scanf("%d", &quant_vet[0]);
	printf("Nível Médio: ");
	scanf("%d", &quant_vet[1]);
	printf("Nível Difícil: ");
	scanf("%d", &quant_vet[2]);
	numItens = quant_vet[0]+quant_vet[1]+quant_vet[2];
		
	doArquivo(&questoes, &qtdQuestoes);	
	embaralharPerguntas (quant_vet,questoes,numItens);
	
	//embaralharAlternativas(&questoes[1]);

	/*for(i = 0; i < qtdQuestoes; i++ ){
			printf("q%d: %s\n\nR. Certa: %d\n\n[%s]\n\n[%s]\n\n[%s]\n\n[%s]\n\n\n", i+1, questoes[i].enunciado, questoes[i].respostaCerta,questoes[i].alternativas[0], questoes[i].alternativas[1], questoes[i].alternativas[2], questoes[i].alternativas[3]);
    
		}fprintf(stderr, "quantidade: %d\n", qtdQuestoes);
  */
	if( qtdQuestoes != 0)
	{
		imprimir(quant_vet, questoes);
	}
	else
	{
		printf("Erro no arquivo");
	}
	return 0;
}
