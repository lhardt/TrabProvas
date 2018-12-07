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

/* 'Espia' como será o próximo caracter de um arquivo.
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
	char * linha = calloc( 1 + TAM_MAX_LINHA, sizeof(char));
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

/* A alternativa sempre começa com 'a ', 'b ', etc.
 * Autor: Léo H.;*/
char * removerCabecalho(char * str){
	int i = 0, len = strlen(str);
	for( i = 2; i <= len; i++){
		str[i-2] = str[i];
	}
	return str;
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
		printf("Sem memória ou arquivo não encontrado! Encerrando o programa... Função doArquivo\n");
		exit(-1);
	}
	while( !feof(entrada) ){
		/* Expandir o vetor se necessário */
		if( (*qtd) >= numQstAlocado -1 ){
			numQstAlocado += 10;
			*vetQuestoes = (struct Questao*) realloc(*vetQuestoes, numQstAlocado * sizeof(struct Questao));
			if(*vetQuestoes == NULL ) {
				printf("Sem memória! Encerrando o programa... Função doArquivo\n");
				exit(-2);
			}
		}
		fscanf(entrada, "%d %c ", &dummy, &((*vetQuestoes)[*qtd]).dificuldade);
		/* Quando tivermos menos alternativas do que esperávamos, resulta strings em branco.
		 * Porém, se tivessemos menos que duas alternativas, a prova não teria sentido. */
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

	int y,x,*usado,*novo_indice;

	usado = (int*)calloc(numItens, sizeof(int));//indice não usado=0, usado=1
	novo_indice = (int*)malloc(numItens*sizeof(int));
	if( novo_indice == NULL ){
		printf("Sem memória! Encerrando o programa..."
			   " Função criarOrdemAletoria.\n");
		exit(4);
	}
	for (x=0;x<numItens;x++){
		novo_indice[x] = numMin + (rand() % abs(numMax-numMin));//sorteia
	    do{
	        usado[x]=0;
	        for (y=0;y<x;y++){//percorre vetor novo_indice até a quantidade de indices já sorteados
	        	if (novo_indice[x]==novo_indice[y]){//se o valor do recem sorteado for igual a outro sorteado anteriormente, continua sorteando até que aconteca o contrario
	        		usado[x] = 1;
	        		novo_indice[x] = numMin + (rand() % abs(numMax-numMin));
				}
			}
	    }while(usado[x]==1);
	}
    return  novo_indice;
}

/* Embaralha alternativas de determinada questão de acordo com o índice de *criarOrdemAleatoria()
 * Autor: Ana; */
void embaralharAlternativas(struct Questao * pQuestao){
	int x,y, tam, *indice;
	char *novaOrdem[4];
	indice = criarOrdemAleatoria(4, 0, 4);// quantidade,valor min, valor max

	for (x=0; x<4;x++){//copia altermativas para vetor auxiliar
		tam = strlen((*pQuestao).alternativas[indice[x]]);
		novaOrdem[x] = (char*) malloc ((tam+1)*sizeof(char));
		if(novaOrdem[x]==NULL){
			printf("Sem memória! Encerrando o programa... Função embaralharAlternativas.\n");
			exit(2);
		}
		strcpy (novaOrdem[x], (*pQuestao).alternativas[indice[x]]);

		if (indice[x]==0){ //muda a alternativa que contem a resposta
			(*pQuestao).respostaCerta= x;
		}
	}
	for (y=0;y<4;y++){ //copia alternativas pra sua posição final no vetor
		tam = strlen (novaOrdem[y]);
		free ((*pQuestao).alternativas[y]);
		(*pQuestao).alternativas[y] = novaOrdem[y];
	}
}

/* Calcular o primeiro índice das questões fáceis, médias e difíceis.
 * Retorna para índices por referência.
 * Autor: Léo H.;*/
void calcularPosicoes(struct Questao questoes[], int qtdQuestoes, int indices[]) {
	/* A dificuldade que ele vê atualmente. Começa pelas fáceis */
	int diff = 'F';
	int i = 0;
	/* O número de dificuldades diferentes que já encontrou.
	 * Usado de indice para o vetor indices */
	int diffs = 1;
	/* O primeiro nível é fácil e começa em zero; */
	indices[0] = 0;
	for(; i < qtdQuestoes; i ++ ){
		if(questoes[i].dificuldade != diff){
			diff = questoes[i].dificuldade;
			indices[diffs] = i;
			diffs++;
		}
	}
}

/* Embaralha as questões de acordo com o índice de *criarOrdemAleatoria()
 * Autor: Ana; */
void embaralhar (int quant_vet[],struct Questao questoes[], int numQuestoes){
	int *indice, *indMedio, *indDificil,x,y,tam, tamAlt;
	// Número total de itens que foram embaralhados
	int numItens = quant_vet[0]+quant_vet[1]+quant_vet[2];
	// Indice inicial de cada dificuldade
	int comecaEm[3] = {0,0,0};
	calcularPosicoes(questoes, numQuestoes, comecaEm );
	// A nova ordem de questões (com suas respectivas alternativas).
	char *novaOrdem[numItens], *novaAlternativa[numItens][4];

	// Chama a funcao para criar um indice para cada uma das especificações de quantidade e nivel
	indice = criarOrdemAleatoria (quant_vet[0],comecaEm[0],comecaEm[1]);
	indMedio = criarOrdemAleatoria (quant_vet[1],comecaEm[1],comecaEm[2]);
	indDificil = criarOrdemAleatoria (quant_vet[2],comecaEm[2],numQuestoes);

	//Junta todos os indices em um só, no indice de perguntas faceis
	indice = (int*) realloc (indice, numItens*sizeof(int));
	for (x=0;x<quant_vet[1];x++){
		indice [quant_vet[0]+x] = indMedio [x];
	}
	for (x=0;x<quant_vet[2];x++){
		indice [quant_vet[0]+quant_vet[1]+x] = indDificil [x];
	}
	
	//Copia enunciado e alternativa para variável auxiliar
	for (x=0; x<numItens;x++){
		tam = strlen (questoes[indice[x]].enunciado);
		novaOrdem[x] = (char*) malloc ((tam+1)*sizeof(char));
		if(novaOrdem[x]==NULL){
			printf("Sem memória! Encerrando o programa... Função embaralhar.\n");
			exit(2);
		}
		strcpy (novaOrdem[x], questoes[indice[x]].enunciado);
		for (y=0;y<4;y++){ //parte das alternativas
			tamAlt = strlen (questoes[indice[x]].alternativas[y]);
			novaAlternativa[x][y] = (char*) malloc ((tamAlt+1)*sizeof(char));
			if(novaAlternativa[x]==NULL){
				printf("Sem memória! Encerrando o programa... Função embaralhar.\n");
				exit(2);
			}
			strcpy (novaAlternativa[x][y], questoes[indice[x]].alternativas[y]);
		}
	}
	//Copia enunciado e alternativas (da variavel auxiliar) para o vetor de structs
	for (x=0;x<numItens;x++){
		tam = strlen (novaOrdem[x]);
		free (questoes[x].enunciado);
		questoes[x].enunciado = (char*) malloc ((tam+1)*sizeof(char));
		if(questoes[x].enunciado==NULL){
			printf("Sem memória! Encerrando o programa... Função embaralhar.\n");
			exit(3);
		}
		strcpy (questoes[x].enunciado, novaOrdem[x]);
		free (novaOrdem[x]);
		for (y=0;y<4;y++){ //parte das alternativas
			tamAlt = strlen (novaAlternativa[x][y]);
			free (questoes[x].alternativas[y]);
			questoes[x].alternativas[y] = (char*) malloc ((tamAlt+1)*sizeof(char));
			if(questoes[x].alternativas[y]==NULL){
				printf("Sem memória! Encerrando o programa... Função embaralhar.\n");
				exit(3);
			}
			strcpy (questoes[x].alternativas[y], novaAlternativa[x][y]);
			free (novaAlternativa[x][y]);
		}
		embaralharAlternativas(&questoes[x]);//Chama funcao para embaralhar alternativas dessa questao
	}
}

/* Impressão dos arquivos.
 * Autor: Luísa; */
void imprimir (int quant_vet [], struct Questao questoes []){
	FILE *arquivoFinal, *gabarito;
	int i=0, x, num=0;
	char alt, letras;
	int quantVetTotal = 0;

	arquivoFinal = fopen ("Prova.txt", "wt");
	gabarito = fopen ("Gabarito.txt", "wt");

	fprintf(gabarito, "GABARITO\n\n");

	fprintf(arquivoFinal, "PROVA\n\nNome: ____________________________\tTurma: ___________\tData: ____/____/_______\n");
	
	//de 0 a 2, pelo nível de dificuldade
	for(x=0; x<3; x++){
		quantVetTotal += quant_vet[x];
		/* Percorre as perguntas da dificuldade x */
		while (i < quantVetTotal){
			/* Arquivo do gabarito */
			fprintf(arquivoFinal, "\n%d. %s\n", 1+i, questoes[i].enunciado);
			for(num =0 ; num <4 ; num++){
				letras = altParaChar(num);
				fprintf(arquivoFinal, "%c) %s\n", letras, questoes[i].alternativas[num]);
			}
			/* Arquivo da prova */
			alt = altParaChar(questoes[i].respostaCerta);
			switch(x) {
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
			/* Impressão do gabarito no arquivo */
			fprintf(gabarito, "%d (%c) - %c\n", i+1, letras, alt); 
			i++;
		}
	}
}

/* Free para todos vetores alocados dinamicamente
 * Autor: Luísa; */
void liberar(struct Questao questoes[], int qtdQuestoes){
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
void user (int quant_vet[]){
	printf("Informe a quantidade de questões: \n");
	printf("Nível Fácil: ");
	scanf("%d", &quant_vet[0]);
	printf("Nível Médio: ");
	scanf("%d", &quant_vet[1]);
	printf("Nível Difícil: ");
	scanf("%d", &quant_vet[2]);
}
/* três mosqueteiros. Um por todos e todos por um
* Autores: Ana, Léo Hardt e Luísa; */
int main(){
	/* O vetor de todas as questões que o programa leu do arquivo. */
	struct Questao * questoes;
	/* A quantidade de questões lidas pelo arquivo */
	int qtdQuestoes;
	/* A quantidade de questões solicitadas pelo usuário */
	int quant_vet[3];

	setlocale(LC_ALL, "Portuguese");
	srand(time(NULL));

	doArquivo(&questoes, &qtdQuestoes);

	if( qtdQuestoes != 0){
		user (quant_vet);
		embaralhar (quant_vet,questoes, qtdQuestoes);
		imprimir(quant_vet, questoes);
		liberar(questoes, qtdQuestoes);
		printf("Arquivos prontos!\n\nProcure por Prova.txt e Gabarito.txt em sua pasta de arquivos.\n\n");
	} else{
		printf("Erro no arquivo");
	}
	return 0;
}
