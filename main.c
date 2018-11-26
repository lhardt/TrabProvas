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

/* Lê até que encontre uma nova linha cujo 1º caracter é um terminador.
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


/* Ler de um arquivo as questões.

	Autor: Léo H.;
*/
void doArquivo(struct Questao ** vetQuestoes, int * qtd){
	/* Quantas questões foram alocadas no vetQuestoes. */
	int numQstAlocado = 10;
	FILE * entrada = fopen("entrada.txt", "r");
	*qtd = 0;
	*vetQuestoes = calloc(numQstAlocado,sizeof(struct Questao));;
	if( entrada == NULL || *vetQuestoes == NULL ){
		fprintf(stderr, "Sem memória! Encerrando o programa...\n");
		exit(-1);
	} else {
		while( !feof(entrada) ){
			/* Expandir o vetor se necessário */
			if( (*qtd) >= numQstAlocado -1 ){
				numQstAlocado += 10;
				*vetQuestoes = realloc(*vetQuestoes, numQstAlocado * sizeof(struct Questao));
				if(*vetQuestoes == NULL ) {
					printf("Sem memória! Encerrando o programa...\n");
					exit(-1);
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

    usado = calloc(numItens, sizeof(int));
    novo_indice = malloc(numItens*sizeof(int));

	for (x=0;x<numItens;x++)//enquanto x< que a quant de nueros pedidos
	{
		novo_indice[x] = numMin+ (rand() % numMax-numMin);//sorteia numero entre o minimo e maximo
	    do
	    {
	        usado[x]=0;
	        for (y=0;y<x;y++)
	        {
	        	if (novo_indice[x]==novo_indice[y]){//se já tiver numero igual no vetor
	        		usado[x] = 1;//significa tem igual, pra usar no do-while
	        		novo_indice[x] = numMin+ (rand() % numMax-numMin);//sorteia de novo
					//break;???
				}
			}
            
	    }while(usado[x]==1);
	}
	
    return  novo_indice;//retorna o novo indice embaralhado

}
/* Embaralha os structs com alternativas (1 por vez)

	Autor: Ana;
*/
void embaralharAlternativas(struct Questao *pQuestao)
{
	
	int x,y, tam, *indice;
	char *novaOrdem[4];

    	indice = criarOrdemAleatoria(4, 0, 4);// quant,min,max
    
	for (x=0; x<4;x++){
		tam = strlen ((*pQuestao).alternativas[indice[x]]);//Lê tamanho da alternativa que vai ser trocada
		novaOrdem[x] = (char*) malloc (tam*sizeof(char));//cria espaço pra essa alternativa em vetor auxiliar
		if(novaOrdem[x]==NULL){
			exit(2);
		}		
		strcpy (novaOrdem[x], (*pQuestao).alternativas[indice[x]]);//copia alternativa pra vetor auxiliar
		
		novaOrdem[x][0] = 'a' + x;//muda a letra da alternativa
		//printf ("\n");
		//puts (novaOrdem[x]);
		
		if (indice[x]==0){
			(*pQuestao).respostaCerta= x;//muda resposta direto no struct
			
		}
	}
	for (y=0;y<4;y++){
		strcpy ((*pQuestao).alternativas[y],novaOrdem[y]);//copia alternativa do vetor auxiliar para o oficial
	}
}

int main(){
	struct Questao * questoes;
	int qtdQuestoes, quant_vet[3],x, numItens;

	printf("Informe a quantidade de questões: \n");
	printf("Nível Fácil: ");
	scanf("%d", &quant_vet[0]);
	printf("Nível Médio: ");
	scanf("%d", &quant_vet[1]);
	printf("Nível Difícil: ");
	scanf("%d", &quant_vet[2]);
	numItens = quant_vet[0]+quant_vet[1]+quant_vet[2];
	
	
	doArquivo(&questoes, &qtdQuestoes);
		
	//embaralharAlternativas(&questoes[i]);

	if( qtdQuestoes != 0)
	{
		//...
	}
	else
	{
		printf("Erro no arquivo");
	}


	return 0;
}
