#include <stdio.h>

struct Questao {
	/* Título da questão */
	char * enunciado;
	/* O índice da resposta certa */
	int respostaCerta;
	/* as quatro alternativas da questão */
	char * alternativas[4];
	char dificuldade;
};
/* 
	A alternativa, no arquivo, é uma letra. Mas conceitualmente é um número.
	A função é uma conversão entre as duas.
	Ex de conversão: a=0, b=1, c=2, d=3

	Autor: Luisa;
*/
int charParaAlternativa( char diff );
/* Ler de um arquivo as questões 
	
	Autor: ;
*/
void doArquivo(struct Questao ** vetQuestoes, int * qtdQuestoes);
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
	// Pegar os dados	
	
	return 0;
}
