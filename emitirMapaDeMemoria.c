#include "montador.h"
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

/* 
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
 
/*funcao que verifica se o token eh ou nao valido dada uma string para comparacao, 
 * caso a string corresponda com o token retorna 1, caso nao corresponda retorna 0
 * caso ocorra um erro com o token passado a funcao imprime que ouve erro
*/
int match(char *string, char *pattern){
	regex_t padrao;
	int err;
	
	if((err = regcomp(&padrao, pattern, REG_EXTENDED)) != 0){
		char errorString[512];
		regerror(err, &padrao, errorString, 512);
		fprintf(stderr, "Erro RegEx: %s\n", errorString);
	}
	
	if((regexec(&padrao, string, 0, (regmatch_t *)NULL, 0)) == 0){
		return 1;
	} else{
		return 0;
	}
	
	regfree(&padrao);
}

/* Identifica o tipo rotulo, caso haja um erro lexico retorna -1
 * Essa funcao eh chamada caso um rotulo tenha sido identificado,
 * entao ela verifica se esse eh um rotulo valido (sem erro lexico), 
 * se for um rotulo sem erros lexicos, então ela atribui um token pra 
 * esse rotulo e retorna 1, caso haja um erro lexico, a função retorna -1.
 */
 
int tipo_palavra(char linha[], int l, int n_linha, char *copia){
	
	int j = 0;
	char *palavra;
	Token novoToken;
	
	/* Declaracao do padrao de rotulo
	 * Esse rotulo diz que a na string tem que ter uma palavra que possa ter
	 * infinitos espaços ou zero, seguido de uma palavra que pode comecar com
	 * letras maiusculas ou minusculas, ou underscore, e no restante da palavra
	 * pode conter tambem numeros, terminando a palavra com apenas um caractere
	 * ":"
	 */
	char padrao_rot[] = "(^[ \t]*[a-zA-Z_][a-zA-Z0-9_]*:[^:]*$)";
	
	if(match(copia, padrao_rot) == 1){
		
		palavra = strtok(linha, "\t ");
		
		novoToken.tipo = DefRotulo;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		
		adicionarToken(novoToken);
		
		return 1;
	}
	
	return -1;
}


/*identifica os tipos diretivas a partir dos tokens, caso haja um erro lexico retorna -1
 * Essa fucao eh chamada caso tenha sido identificado uma diretiva na linha
 * então ela verifica caso a caso qual diretiva eh e se contem erros lexicos, 
 * se nao contiver um erro a funcao atribui os tokens necessarios e retorna 1
 */
int tipo_diretiva(char linha[], int l, int n_linha, char *copia){
	
	Token novoToken;
	char *palavra;
	
	/*declaraco do padrao para todos os tipos de diretivas e seus argumentos, usados para encontrar erros lexicos no programa
	 * Os tipos de tokens dados abaixo sao:
	 * org_hex: uma diretiva .org seguida obrigatoriamente de um numero hexadecimal, entre eles podem haver infinitos espacos (0x seguidos por dez algarismos de 0-9 e a-f (podendo ser maiuscula ou minuscula))
	 * org_dec: a diretiva .org seguida de numeros de 0-9 apenas, entre eles podem haver infinitos espacos
	 * set_hex: a diretiva .set seguida de simbolo (pode comecar com letras ou underscore e o resto da palavra pode conter também numeros) e depois um numero hexadecimal (explicado anteriormente), podendo exixstir infinitos espacos entre eles
	 * set_dec: o que muda do padrao .set_hex, eh que o segundo paramentro eh um numero decimal, podendo exixstir infinitos espacos entre eles
	 * align: a diretiva . align seguida de um numero decimal (contendo de 1 a 4 algarismos), podendo existir infinitos espacos entre eles
	 * wfill_hex: a diretiva .wfill seguida de um numero decimal e depois tem como parametro um hexadecimal, entre a diretiva e parametros podem haver infinitos espacos
	 * wfill_dec: o que muda do padrao wfill_hex, eh que o segundo parametro eh um hexadecimal
	 * wfill: o que muda dos padroes dessa diretiva anteriores eh que o segundo parametro pode ser um simbolo ou um rotulo, ambos tem a mesma estrutura
	 * word: a diretira .word seguida por um simbolo ou rotulo, entre eles podem existir infinitos espacos
	 * word_hex: a diretiva .word seguida por um hexadecimal, podendo existir infinitos espacos entre a diretiva e a o parametro
	 * word_hex: igual o word descrito acima, mas seguida por um numero decimal
	 */
	 
	char org_hex[] = "(.org +0x[0-9a-fA-F]{10,10}[ \t\n]*)";
	char org_dec[] = "(.org +[0-9]+[ \t\n]*)";
	char set_hex[] = "(.set[ \t]+[a-zA-z_][a-zA-Z0-9_]*[ \t]*0x[0-9a-fA-F]{10,10}[ \t\n]*)";
	char set_dec[] = "(.set[ \t]+[a-zA-z_][a-zA-Z0-9_]*[ \t]+[0-9]+[ \n]*)";
	char align[] = "(.align[ \t]+[0-9]{1,4}[ \t\n]*)";
	char wfill_hex[] = "(.wfill[ \t]+[0-9]+[ \t]+0x[0-9a-fA-F]{10,10}[ \t\n]*)";
	char wfill_dec[] = "(.wfill[ \t]+[0-9]+[ \t]+[0-9]+[ \t\n]*)" ;
	char wfill[] = "(.wfill[ \t]+[0-9]+[ \t]+[a-zA-z_][a-zA-Z0-9_]*)";
	char word[] = "(.word[ \t]+[a-zA-z_][a-zA-Z0-9_]*)";
	char word_hex[] = "(.word[ \t]+0x[0-9a-fA-F]{10,10}[ \t\n]*)";
	char word_dec[] = "(.word[ \t]+[0-9]+[ \t\n]*)";
	
	/* Aqui o programa verifica se deu match no tipo org_hex, caso tenha dado ele pega a proxima palavra da string
	 * e atribui um novo token para a diretiva, depois pega a proxima palavra da string (o parametro) e atribui um token pra ele.
	 * caso tenha entrado na funcao retorna 1;
	 */
	if(match(copia, org_hex) == 1){
		
		if(l == 0)	palavra = strtok(linha, " \t");
		else palavra = strtok(NULL, " \t");
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " \t");
		
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	} 
	
	/* A partir daqui os if's funcionam da mesma forma que o anterior
	 * caso a diretiva possua mais de um parametro ele pega vai pegando sempre a proxima palavra da string e cria
	 * um novo token pra ela
	 */
	if(match(copia, org_dec)){
		
		if(l == 0)	palavra = strtok(linha, " \t");
		else palavra = strtok(NULL, " \t");
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);

		palavra = strtok(NULL, " \t");	

		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, align)){

		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}

	if(match(copia, set_hex) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, set_dec) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, word) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, word_hex) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, word_dec) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, wfill) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, wfill_hex) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, wfill_dec) == 1){
		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	return -1;
}

/* identifica o tipo instrucao sem erros lexicos, caso haja um erro lexico retorna -1
 * A funcao funciona do mesmo jeito que as funcoes anteriores, mas essa verifica apenas os
 * tipos instrucoes
 */
 
int tipo_instrucao(char linha[], int l, int n_linha, char *copia){
	Token novoToken;
	char *palavra;
	
	/* Definicao dos padroes para as instrucoes
	 * elas funcionam da seguinte maneira:
	 * instru_nome: uma instrucao do tipo instrucoes que recebem argumentos seguido de um argumento entre "" sendo um rotulo ou simbolo, podendo haver infinitos espacos entre a instrucao e o argumento e entre a " e o argumento
	 * instru_hex: funciona da mesma forma que a anterior, mas nesse caso o argumento eh um hexadecimal
	 * instru_dec: funciona da mesma forma que as anteriores, mas nesse caso o argumento eh um decimal
	 * instru_none: esse eh o caso das instrucoes que nao possuem argumento, entao verifica se eh essa instrucao, caso seja ela pode ser seguida apenas de um tipo comentario (#), sendo esse comentario opicional
	 */
	char instru_nome[] = "((LOAD|LOAD-|LOAD\\||LOADmq_mx|STOR|JUMP|JMP\\+|ADD|ADD\\||SUB|SUB\\||MUL|DIV|STORA)[ \t]+\"[ \t]*[a-zA-z_][a-zA-Z0-9_]*\")";
	char instru_hex[] = "((LOAD|LOAD-|LOAD\\||LOADmq_mx|STOR|JUMP|JMP\\+|ADD|ADD\\||SUB|SUB\\||MUL|DIV|STORA)[ \t]+\"0x[0-9a-fA-F]{10,10}\")";
	char instru_dec[] = "((LOAD|LOAD-|LOAD\\||LOADmq_mx|STOR|JUMP|JMP\\+|ADD|ADD\\||SUB|SUB\\||MUL|DIV|STORA)[ \t]+\"[0-9]+\")";
	char instru_none[] = "((LOADmq|LSH|RSH)[ \t]*(#.*){0,1}$)";
	
	
	/* Nas condicoes abaixo ele verifica qual o tipo de instrucao eh (um que precisa de argumento ou um que nao precisa), e qual o tipo do argumento dado
	 * Se nao houver nenhum erro lexico, entra no if e adiona a proxima palavra da sting pro token de instrucao e cria esse token, caso precise de um argumento 
	 * pega a proxima palavra e cria um token pro tipo de argumento.
	 * Se criar o token, entao a funcao retorna 1 (nao houve erros lexicos), caso nao tenha entrado em nenhum caso, eh porque houve erro lexico, entao a funcao
	 * retorna -1
	 */
	 
	if(match(copia, instru_nome) == 1){
		
		if(l == 0) palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ")	;
		
		novoToken.tipo = Instrucao;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " \"");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, instru_hex) == 1){
		
		if(l == 0) palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ")	;
		
		novoToken.tipo = Instrucao;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " \"");
		
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	if(match(copia, instru_dec) ==  1){
		if(l == 0) palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ")	;
		
		novoToken.tipo = Instrucao;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		palavra = strtok(NULL, " \"");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
		
	}
	
	if(match(copia, instru_none) == 1){
		if(l == 0) palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ")	;
		
		novoToken.tipo = Instrucao;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
		
	
	return -1;
}


int processarEntrada(char* entrada, unsigned tamanho) 
{
	/*declaracao dos padroes para comentarios, rotulos, diretivas e instrucoes
	 * Os padroes sao:
	 * padrao_coment: tudo que comeca com uma "#" e o que vem depois disso eh um comentario
	 * padrao_rotulo: tudo que na palavra contem letras (maiusculas ou minusculas), numeros ou underscore e termina com ":"
	 * padrao_diretiva: deve conter uma diretiva, entao verifica se na string tem um .org ou .set ou .align ou .wfill ou .word
	 * padrao_instrucao: verifica se na string contem uma instrucao
	 */
	char padrao_coment[] = "[#].*";
	char padrao_rotulo[] = "[A-Za-z0-9_]+[:]";
	char padrao_diretiva[] = "(.org)|(.set)|.(align)|(.wfill)|(.word)";
	char padrao_instrucao[] = "((LOAD)|(LOAD-)|(LOAD\\|)|(LOADmq)|(LOADmq_mx)|(STOR)|(JUMP)|(JMP\\+)|(ADD)|(ADD\\|)|(SUB)|(SUB\\|)|(MUL)|(DIV)|(LSH)|(RSH)|(STORA))";

	/* Esses padroes sao os usados para verificar se ocorre um erro gramatical
	 * nesse caso ele verifica se a linha inteira esta composta de forma correta, os padroes sao:
	 * entrada_dir: linha composta por um rotulo, seguido de uma diretiva, seguida de um comentario, sendo todos eles opicionais, e ainda verifica se pra diretiva os argumentos estao na ordem correta
	 * entrada_inst:linha composta por um rotulo, seguido uma instrucao, seguida de um comentario, sendo todos eles opicionais, possuindo instrucoes que precisam de argumento e outras que nao precisam
	 */ 
	char entrada_dir[] = "(^[ \t]*([a-zA-Z0-9_]+[:]+)?[ \t]*((.set[ \t]+[A-Za-z0-9_]+[ \t]+((0x[0-9A-Fa-f]+)|([0-9]+)))|(.org[ \t]+((0x[0-9A-Fa-f]+)|([0-9]+)))|(.align[ \t]+[0-9]+)|(.wfill[ \t]+[0-9]+[ \t]+[A-Za-z0-9_]+)|(.word[ \t]+[A-Za-z0-9_]+))?[ \t]*(#.*)?[ \t]*$)";
	char entrada_inst[] = "(^[ \t]*([a-zA-Z0-9_]+[:]+)?[ \t]*((RSH)|(LSH)|(LOADmq)|(((LOAD)|(LOAD-)|(LOAD\\|)|(LOADmq_mx)|(STOR)|(JUMP)|(JMP\\+)|(ADD)|(ADD\\|)|(SUB)|(SUB\\|)|(MUL)|(DIV)|(STORA))[ \t]+\"[A-Za-z0-9_]+\"))?[ \t]*(#.*)?[ \t]*$)";

	
	//declaracao das variaveis
	char *linha = malloc(tamanho * sizeof(char));
	char copia[tamanho];
	int n_linha = 0, i, j = 0, l = 0, cont = 0, k;
	
	//percorre todo o vetor de entrada
	for(i=0; i<tamanho; i++){
		
		//caso encontre uma quebra de linha copia tudo o que foi lido na linha para copoa e acrescenta 1 no numero de linhas
		if(entrada[i] == '\n'){
			n_linha++;
			linha[j] = '\0';
			strcpy(copia, linha);
			
			j = 0;
			
			//verifica se é uma linha valida, caso nao seja imprime que ocorreu erro gramatical e para o programa, caso contratio continua a execucao 
			if((match(copia, entrada_dir)) == 0 && (match(copia, entrada_inst)) == 0){
				fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n", n_linha);
				return 1;
			}
			
			//verifica se a linha contem um comentario, caso tenha o contador recebe 1
			if(match(copia, padrao_coment) == 1){
				cont = 1;
				k = 0;
				while(copia[k] != '#'){
					k++;
				}
				copia[k] = '\0';
			}
			//verifica se a linha contem um rotulo, caso contenha mas retornou erro (l = -1) imprime que ocorreu um erro lexico e encerra o programa, caso contrario adiciona o token
			if(match(copia, padrao_rotulo) == 1){
				l = tipo_palavra(linha, l, n_linha, copia);
				cont = 1;
				
				if(l == -1){
					fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n", n_linha);
					return 1;
				}
			}
			
			
			//verifica se a linha contem uma diretiva, caso contenha mas retornou erro encerra o programa e imprime que houve erro lexico, caso contrario adiciona os tokens
			if(match(copia, padrao_diretiva) == 1){
				l = tipo_diretiva(linha, l, n_linha, copia);
				cont = 1;
				
				if(l == -1){
					fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n", n_linha);
					return 1;
				}		
			}
			
			//verifica se a linha contem uma instrucao, caso tenha mas retornou erro encerra o programa e imprime que houve erro, caso contrario adiciona os tokens
			if(match(copia, padrao_instrucao) == 1){
				l = tipo_instrucao(linha, l, n_linha, copia);
				cont = 1;
				
				if(l == -1){
					fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n", n_linha);
					return 1;
				}
				
			}
			linha = malloc(tamanho * sizeof(char));
			l = 0;
			
		//caso não seja quebra de linha continua lendo e copiando a linha	
		}else{
			linha[j++] = entrada[i];
		}
		
	}
	
	//se o contador ao final for zero significa que as definicoes na linha eram todas invalidas, logo ocorreu erro gramatical
	 if(cont == 0){
		fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n", n_linha);
		return 1;
		 
	 }
	
	//caso o programa nao seja interrompido por erro imprime a lista de tokens
	//imprimeListaTokens();

	
  return 0; 
}
