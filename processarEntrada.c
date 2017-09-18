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
 
//funcao que verifica se o token eh ou não valido dada uma string para comparacao
int match(char *string, char *pattern){
	regex_t padrao;
	int err;
	
	if((err = regcomp(&padrao, pattern, REG_EXTENDED)) != 0){
		char errorString[512];
		regerror(err, &padrao, errorString, 512);
		fprintf(stderr, "Erro RegEx: %s\n", errorString);
	}
	
	//caso seja valido retorna 1, senao retorna 0
	if((regexec(&padrao, string, 0, (regmatch_t *)NULL, 0)) == 0){
		return 1;
	} else{
		return 0;
	}
	
	regfree(&padrao);
}

//identifica o tipo rotulo, caso haja um erro lexico retorna -1
int tipo_palavra(char linha[], int l, int n_linha, char *copia){
	
	//declaracao das variaveis
	int j = 0;
	char *palavra;
	Token novoToken;
	
	//declaracao do padrao de rotulo
	char padrao_rot[] = "(^[ \t]*[a-zA-Z_][a-zA-Z0-9_]*:[^:]*$)";
	
	//se na linha tiver um tipo rotulo da match e retorna 1 
	if(match(copia, padrao_rot) == 1){
		
		//pega a primeira palavra da linha
		palavra = strtok(linha, "\t ");
		
		/atribui os valores pro novo Token e o adiciona
		novoToken.tipo = DefRotulo;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		
		adicionarToken(novoToken);
		
		return 1;
	}
	
	//caso não tenha dado match retorna -1 como erro 
	return -1;
}


//identifica os tipos diretivas, caso haja um erro lexico retorna -1
int tipo_diretiva(char linha[], int l, int n_linha, char *copia){
	
	Token novoToken;
	char *palavra;
	
	//declaraco do padrao para todos os tipos de diretivas e seus argumentos, usados para encontrar erros lexicos no programa
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
	char word_dec[] = "(.word[ \t]+[0-9]+[ \t\n]*)";1
	
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
	
	//caso seja a diretiva .align com todos os campos corretos
	if(match(copia, align)){

		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		//pega essa palavra e cria um novo token para a diretiva atribuindo seus valores		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois da diretiva		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	//caso seja a diretiva .set sym hexadecimal
	if(match(copia, set_hex) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		//pega essa palavra e cria um novo token para a diretiva atribuindo seus valores				
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois da diretiva		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois do simbolo
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	//caso seja a diretiva .set sym decimal
	if(match(copia, set_dec) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		//pega essa palavra e cria um novo token para a diretiva atribuindo seus valores			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois da diretiva		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois do simbolo		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	//caso seja a diretiva .word sym ou rot
	if(match(copia, word) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		//pega essa palavra e cria um novo token para a diretiva atribuindo seus valores			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois da diretiva		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Nome;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	//caso seja a diretiva .word hex
	if(match(copia, word_hex) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
		
		//pega essa palavra e cria um novo token para a diretiva atribuindo seus valores			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois da diretiva		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	//caso seja a diretiva .word dec
	if(match(copia, word_dec) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois da diretiva		
		palavra = strtok(NULL, " ");
		
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return 1;
	}
	
	//caso seja a diretiva .wfill sym ou rot
	if(match(copia, wfill) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
		if(l == 0)	palavra = strtok(linha, " ");
		else palavra = strtok(NULL, " ");
			
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		//pega a proxima palavra da linha depois da diretiva		
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
	
	//caso seja a diretiva .wfill hex
	if(match(copia, wfill_hex) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
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
	
	//caso seja a diretiva .wfill dec
	if(match(copia, wfill_dec) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
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

//identifica o tipo rotulo, caso haja um erro gramatical retorna -1
int tipo_instrucao(char linha[], int l, int n_linha, char *copia){
	Token novoToken;
	char *palavra;
	
	char instru_nome[] = "((LOAD|LOAD-|LOAD\\||LOADmq_mx|STOR|JUMP|JMP\\+|ADD|ADD\\||SUB|SUB\\||MUL|DIV|STORA)[ \t]+\"[ \t]*[a-zA-z_][a-zA-Z0-9_]*\")";
	char instru_hex[] = "((LOAD|LOAD-|LOAD\\||LOADmq_mx|STOR|JUMP|JMP\\+|ADD|ADD\\||SUB|SUB\\||MUL|DIV|STORA)[ \t]+\"0x[0-9a-fA-F]{10,10}\")";
	char instru_dec[] = "((LOAD|LOAD-|LOAD\\||LOADmq_mx|STOR|JUMP|JMP\\+|ADD|ADD\\||SUB|SUB\\||MUL|DIV|STORA)[ \t]+\"[0-9]+\")";
	char instru_none[] = "((LOADmq|LSH|RSH)[ \t]*(#.*){0,1}$)";
	
	if(match(copia, instru_nome) == 1){
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
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
		
		//verifica se a linha ja entrou em strtok, se sim continua lendo a mesma string, senao passa linha como parametro do strtok que pega a proxima palavra da linha		
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
	//declaracao dos padroes para comentarios, rotulos, diretivas e instrucoes
	char padrao_coment[] = "[#].*";
	char padrao_rotulo[] = "[A-Za-z0-9_]+[:]";
	char padrao_diretiva[] = "(.org)|(.set)|.(align)|(.wfill)|(.word)";
	char padrao_instrucao[] = "((LOAD)|(LOAD-)|(LOAD\\|)|(LOADmq)|(LOADmq_mx)|(STOR)|(JUMP)|(JMP\\+)|(ADD)|(ADD\\|)|(SUB)|(SUB\\|)|(MUL)|(DIV)|(LSH)|(RSH)|(STORA))";
	char entrada_dir[] = "(^[ \t]*([a-zA-Z0-9_]+[:]+)?[ \t]*((.set[ \t]+[A-Za-z0-9_]+[ \t]+((0x[0-9A-Fa-f]+)|([0-9]+)))|(.org[ \t]+((0x[0-9A-Fa-f]+)|([0-9]+)))|(.align[ \t]+[0-9]+)|(.wfill[ \t]+[0-9]+[ \t]+[A-Za-z0-9_]+)|(.word[ \t]+[A-Za-z0-9_]+))?[ \t]*(#.*)?[ \t]*$)";
	char entrada_inst[] = "(^[ \t]*([a-zA-Z0-9_]+[:]+)?[ \t]*((RSH)|(LSH)|(LOADmq)|(((LOAD)|(LOAD-)|(LOAD\\|)|(LOADmq_mx)|(STOR)|(JUMP)|(JMP\\+)|(ADD)|(ADD\\|)|(SUB)|(SUB\\|)|(MUL)|(DIV)|(STORA))[ \t]+\"[A-Za-z0-9_]+\"))?[ \t]*(#.*)?[ \t]*$)";

	
	//declaracao das variaveis
	char *linha = malloc(tamanho * sizeof(char));
	char copia[tamanho];
	int n_linha = 0, i, j = 0, l = 0, cont = 0;
	
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
			
			//verifica se a linha contem um rotulo, caso contenha mas retornou erro (l = -1) imprime que ocorreu um erro lexico e encerra o programa, caso contrario adiciona o token
			if(match(copia, padrao_rotulo) == 1){
				l = tipo_palavra(linha, l, n_linha, copia);
				cont = 1;
				
				if(l == -1){
					fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n", n_linha);
					return 1;
				}
			}
			
			//verifica se a linha contem um comentario, caso tenha o contador recebe 1
			if(match(copia, padrao_coment) == 1){
				cont = 1;
				
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
	imprimeListaTokens();

	
  return 0; 
}
