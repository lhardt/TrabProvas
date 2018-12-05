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
	/* Título da questão; */
	char * enunciado;
	/* O índice da resposta certa; */
	int respostaCerta;
	/* as quatro alternativas da questão; */
	char * alternativas[4];
	/* dificuldade (F, M ou D); */
	char dificuldade;
};

/* Função auxiliar, como a fpeek do C++.
 * 'Espia' como será o próximo caracter de um arquivo.
 * Autor: Léo H.; */
int fpeek( FILE * stream ) {
    int c;
    c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

/* A alternativa, no arquivo, é uma letra. Mas conceitualmente é um número.
 * A função é uma conversão entre as duas.
 * Ex de conversão: a=0, b=1, c=2, d=3.
 * Utilizada na hora da impressão dos arquivos.
 * Autor: Luísa; */
char altParaChar( int diff ){
	switch(diff){
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
 * Autor: Léo H.; */
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
		if(!isprint(str[i])){
			str[i] = '\0';
		}else break;
	}
	free(linha);
	return str;
}

/* A alternativa sempre começa com 'a ', 'b ', etc. Limpa a antiga string.
 * Autor: Léo H.;*/
char * removerCabecalho(char * str){
	char * str2 = calloc(strlen(str), sizeof(char));
	char dummy;
	sscanf(str, "%c %s", &dummy, str2);
	free(str);
	return str2;
}

/* Ler de um arquivo as questões
 * Autor: Léo H.;*/
void doArquivo(struct Questao ** vetQuestoes, int * qtd){
	/* Quantas questões foram alocadas no vetQuestoes. */
	int numQstAlocado = 10;
	FILE * entrada = fopen("entrada.txt", "rt");
	*qtd = 0; int dummy = 0;
	*vetQuestoes = (struct Questao*) calloc(numQstAlocado,sizeof(struct Questao));;
	if( entrada == NULL || *vetQuestoes == NULL ){
		printf("Sem memória! Encerrando o programa...\n");
		exit(-1);
	}
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
		fscanf(entrada, "%d %c ", &dummy, &((*vetQuestoes)[*qtd]).dificuldade);
		/* Quando tivermos menos alternativas do que esperávamos, resulta strings em branco.
			Porém, se tivessemos menos que duas alternativas, o programa não tem sentido. */
		(*vetQuestoes)[*qtd].enunciado = ateTerminador(entrada, "a");

		(*vetQuestoes)[*qtd].alternativas[0] = removerCabecalho(ateTerminador(entrada, "b"));
		(*vetQuestoes)[*qtd].alternativas[1] = removerCabecalho(ateTerminador(entrada, "c123456789"));
		(*vetQuestoes)[*qtd].alternativas[2] = removerCabecalho(ateTerminador(entrada, "d123456789"));
		(*vetQuestoes)[*qtd].alternativas[3] = removerCabecalho(ateTerminador(entrada, "123456789"));

		(*qtd)++;
	}
	fclose(entrada);
}

/* A ordem das questões e das alternativas. Use como o índice da lista
 * Autor: Ana; */
int *criarOrdemAleatoria(int numItens, int numMin, int numMax){
	srand((unsigned)time(NULL));
	
	int y,x,*usado,*novo_indice;

	usado = (int*)calloc(numItens, sizeof(int));//indice não usado=0, usado=1
	novo_indice = (int*)malloc(numItens*sizeof(int));
	if( novo_indice == NULL ){
		exit(4);
	}
	for (x=0;x<numItens;x++){
		novo_indice[x] = numMin+ (rand() % abs(numMax-numMin));//sorteia
	    do{
	        usado[x]=0;
	        for (y=0;y<x;y++){//percorre vetor novo_indice até a quantidade de indices já sorteados
	        	if (novo_indice[x]==novo_indice[y]){//se o valor do recem sorteado for igual a outro sorteado anteriormente, continua sorteando até que aconteca o contrario
	        		usado[x] = 1;
	        		novo_indice[x] = numMin+ (rand() % abs(numMax-numMin));
			}
		}            
	    }while(usado[x]==1);
	}	
    return  novo_indice;
}

/* Embaralha alternativas de determinada questão de acordo com o índice de *criarOrdemAleatoria()
 * Autor: Ana; */
void embaralharAlternativas(struct Questao *pQuestao){
	int x,y, tam, *indice;
	char *novaOrdem[4];
	indice = criarOrdemAleatoria(4, 0, 4);// quantidade,valor min, valor max
    
	for (x=0; x<4;x++){//copia altermativas para vetor auxiliar
		tam = strlen ((*pQuestao).alternativas[indice[x]]);
		novaOrdem[x] = (char*) malloc (tam*sizeof(char));
		if(novaOrdem[x]==NULL){
			exit(2);
		}		
		strcpy (novaOrdem[x], (*pQuestao).alternativas[indice[x]]);		
				
		if (indice[x]==0){//muda a alternativa que contem a resposta
			(*pQuestao).respostaCerta= x;
		}
	}
	for (y=0;y<4;y++){//copia alternativas pra sua posição final no vetor
		tam = strlen (novaOrdem[y]);
		
		free ((*pQuestao).alternativas[y]);
		(*pQuestao).alternativas[y] = (char*) malloc (tam*sizeof(char));
		
		strcpy ((*pQuestao).alternativas[y],novaOrdem[y]);
	}
}

/* Embaralha as questões de acordo com o índice de *criarOrdemAleatoria()
 * Autor: Ana; */
void embaralhar (int quant_vet[],struct Questao questoes[], int numItens)
{
	int *indMedio, *indDificil, *indice,x,y,tam, tamAlt;
	char *novaOrdem[numItens], *novaAlternativa[numItens][4];
	
	// Chama a funcao para criar um indice para cada uma das especificações de quantidade e nivel
	indice = criarOrdemAleatoria (quant_vet[0],0,40);
	indMedio = criarOrdemAleatoria (quant_vet[1],40,80);
	indDificil = criarOrdemAleatoria (quant_vet[2],80,120);
	
	//Junta todos os indices em um só, no indice de perguntas faceis
	indice = (int*) realloc (indice, numItens*sizeof(int));
	for (x=0;x<quant_vet[1];x++){
		indice [quant_vet[0]+x] = indMedio [x];
	}
	for (x=0;x<quant_vet[2];x++){
		indice [quant_vet[1]+quant_vet[2]+x] = indDificil [x];
	}
	//Copia enunciado e alternativa para variável auxiliar
	for (x=0; x<numItens;x++){

		tam = strlen (questoes[indice[x]].enunciado);
		novaOrdem[x] = (char*) malloc (tam*sizeof(char));
		if(novaOrdem[x]==NULL){
			exit(2);
		}
		strcpy (novaOrdem[x], questoes[indice[x]].enunciado);
		for (y=0;y<4;y++){//parte das alternativas
			tamAlt = strlen (questoes[indice[x]].alternativas[y]);
			novaAlternativa[x][y] = (char*) malloc (tamAlt*sizeof(char));
			strcpy (novaAlternativa[x][y], questoes[indice[x]].alternativas[y]);
		}
	}
	//Copia enunciado e alternativas (da variavel auxiliar) para o vetor de structs
	for (x=0;x<numItens;x++){
		tam = strlen (novaOrdem[x]);
		free (questoes[x].enunciado);
		questoes[x].enunciado = (char*) malloc (tam*sizeof(char));
		if(questoes[x].enunciado==NULL){
			exit(3);
		}
		strcpy (questoes[x].enunciado, novaOrdem[x]);
		for (y=0;y<4;y++){ //parte das alternativas
			tamAlt = strlen (novaAlternativa[x][y]);
			free (questoes[x].alternativas[y]);
			questoes[x].alternativas[y] = (char*) malloc (tamAlt*sizeof(char));
			strcpy (questoes[x].alternativas[y], novaAlternativa[x][y]);
		}
		embaralharAlternativas(&questoes[x]);//Chama funcao para embaralhar alternativas dessa questao
	}
}

/* Impressão dos arquivos.
 * Autor: Luísa; */
void imprimir (int quant_vet [], struct Questao questoes []){
	FILE *arquivo_final, *gabarito;
	int i=0, x, contador=0, numero_questao_gab =0, num=0, valor=0;
	char alt, letras;

	arquivo_final = fopen ("Prova.txt", "wt");
	gabarito = fopen ("Gabarito.txt", "wt");
	
	fprintf(gabarito, "GABARITO\n\n");
	for(x=0; x<3; x++){ //de 0 a 2, pelo nível de dificuldade
		i=0;
		do{
			alt = altParaChar(questoes[i].respostaCerta);
			switch(x)
			{
				case 0:
			    		letras = 'F';
			    		break;
			 	case 1:
			   		letras = 'M';
			    		break;
				case 2:
					letras =  'D';
					break;
			}
			fprintf(gabarito, "%d (%c) - %c\n", numero_questao_gab+1, letras, alt); //impressão do gabarito no arquivo
			i++;
			numero_questao_gab++;
     		} while (i < quant_vet [x]); //percorre a quantidade de perguntas do nível de dificuldade estabelecido conforme indicada no início
	}
	
	fprintf(arquivo_final, "PROVA\n\nNome: ____________________________\tTurma: ___________\tData: ____/____/_______\n");
	for(x=0; x<3; x++){ //de 0 a 2, pelo nível de dificuldade
		i=0;
		do{
			valor++;
			fprintf(arquivo_final, "\n%d. %s\n", valor, questoes[i].enunciado);
			for(num =0 ; num <4 ; num++){
				letras = altParaChar(num);
				fprintf(arquivo_final, "%c) %s\n", letras, questoes[i].alternativas[num]);
			}
			i++;
		} while (i < quant_vet [x]); //percorre a quantidade de perguntas do nível de dificuldade estabelecido conforme indicada no início
	}
}

/* Free para todos vetores alocados dinamicamente
 * Autor: Luísa; */
void liberar(struct Questao *questoes, int qtdQuestoes){
	int i=0, x=0;
	for(; x<qtdQuestoes; x++){
		free(questoes[x].enunciado);
		for (; i<4; i++){
			free(questoes[x].alternativas[i]);
		}
	}
	free(questoes);
}
/* três mosqueteiros. Um por todos e todos por um
* Autores: Ana, Léo Hardt e Luísa; */
int main(){
	setlocale(LC_ALL, "Portuguese");
	
	struct Questao * questoes;
	int qtdQuestoes, quant_vet[3], numItens,x;

	printf("Informe a quantidade de questões: \n");
	printf("Nível Fácil: ");
	scanf("%d", &quant_vet[0]);
	printf("Nível Médio: ");
	scanf("%d", &quant_vet[1]);
	printf("Nível Difícil: ");
	scanf("%d", &quant_vet[2]);
	numItens = quant_vet[0]+quant_vet[1]+quant_vet[2];
		
	doArquivo(&questoes, &qtdQuestoes);	
	embaralhar (quant_vet,questoes,numItens);
	
	if( qtdQuestoes != 0){
		imprimir(quant_vet, questoes);
		liberar(questoes, qtdQuestoes);
		printf("Arquivos prontos!\n\nProcure por Prova.txt e Gabarito.txt em sua pasta de arquivos.");
	}
	else{
		printf("Erro no arquivo");
	}
	return 0;
}
