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
 
 
int match(char *string, char *pattern){
	regex_t padrao;
	
	if(regcomp(&padrao, pattern, REG_EXTENDED) != 0)
		printf("erro regx\n");
	
	if((regexec(&padrao, string, 0, (regmatch_t *)NULL, 0)) == 0){
		return 1;
	} else{
		return 0;
	}
	
	regfree(&padrao);
}

int tipo_palavra(char linha[], int l, int n_linha){
	int j = 0;
	char *palavra = malloc(sizeof(char)*64);
	char padrao_rot[] = "(^( )*[a-zA-Z_][a-zA-Z0-9_]+[:])";
	
	if(match(linha, padrao_rot) == 1){
		
		while(linha[l] != ':'){
			if(linha[l] == ' ') {
				l+=1;
			}else{
				palavra[j] = linha[l];
				j++;
				l++;
			}
		}
		l++;
		palavra[j] = '\0';
		//printf("%s\n\n", palavra);
		
		Token novoToken;
		novoToken.tipo = DefRotulo;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		
		adicionarToken(novoToken);
	}
	
	else{
		l = -1;
	}

	return l;
}

int tipo_diretiva(char linha[], int l, int n_linha){
	int j = 0;
	char *palavra = malloc(sizeof(char)*64);
	char dec[] = "";
	char hex[] = "0x[0-9a-fA-F]{10,10}[^0-9a-fA-f]";
	char org_hex[] = "(.org *0x[0-9a-fA-F]{10,10}[ \n])";
	char org_dec[] = "(.org *[0-9]{1,}([^x]|[ \n]))";
	char set_hex[] = "(.set *[a-zA-z_][a-zA-Z0-9_]* *0x[0-9a-fA-F]{1,})";
	char set_dec[] = "(.set *[a-zA-z_][a-zA-Z0-9_]* *[0-9]{1,}[^a-z])";
	char align[] = "(.align *[0-9]{1,})";
	char wfill_hex[] = "(.wfill *[0-9]* *0x[0-9a-fA-F]{1,})";
	//char wfill_dec = ;
	char wfill_sym[] = "(.wfill *[0-9]* *[a-zA-z_][a-zA-Z0-9_]*)";
	char word[] = "(.word *[a-zA-z_][a-zA-Z0-9_]*)";
	char word_hex[] = "(.word 0x[0-9a-fA-F]{1,})";
	char word_dec[] = "(.word [0-9]{1,}[^a-zA-z ])";
	Token novoToken;
	
	
	//se for a diretiva .org hex
	if(match(linha, org_hex) == 1){
		
		while(linha[l] == ' '){
			l++;
		}
		for(j=0;j<4; j++){
			palavra[j] = linha[l];
			l++;
		}
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		while(linha[l] == ' '){
			l++;
		}
		palavra = malloc(sizeof(char)*12);
		for(j=0;j<12; j++){
			palavra[j] = linha[l];
			l++;
		}
		palavra[j] = '\0';
		novoToken.tipo = Hexadecimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		return l;
	} 
	//caso seja a diretiva .org dec
	if(match(linha, org_dec)){
		while(linha[l] == ' '){
			l++;
		}
		for(j=0;j<4; j++){
			palavra[j] = linha[l];
			l++;
		}
		
		novoToken.tipo = Diretiva;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		
		while(linha[l] == ' '){
			l++;
		}
		j = 0;
		palavra = malloc(sizeof(char)*12);
		while(linha[l] != ' '){
			palavra[j] = linha[l];
			j++;
			l++;
		}
		l++;
		
		palavra[j] = '\0';
		novoToken.tipo = Decimal;
		novoToken.palavra = palavra;
		novoToken.linha = n_linha;
		adicionarToken(novoToken);
		return l;
		
	}
	
	
	
	

	return -1;
}


int processarEntrada(char* entrada, unsigned tamanho) 
{
	char padrao_coment[] = "[#].*";
	char padrao_rotulo[] = "[A-Za-z0-9_]+[:]";
	char padrao_diretiva[] = "(.org)|(.set)|.(align)|(.wfill)|(.word)";
	int n_linha = 0, i, j = 0, l = 0;
	
	
	char linha[tamanho];
	
	for(i=0; i<tamanho; i++){
		if(entrada[i] == '\n'){
			n_linha++;
			linha[j] = '\0';
			j = 0;
			
			if(match(linha, padrao_rotulo) == 1){
				l = tipo_palavra(linha, l, n_linha);	
				
				if(l == -1){
					printf("ROTULO INVALIDO\n");
					return 1;
				}
			}
			
			if(match(linha, padrao_coment) == 1){
				
			}
			if(match(linha, padrao_diretiva) == 1){
				l = tipo_diretiva(linha, l, n_linha);
				
				if(l == -1){
					printf("DIRETIVA INVALIDA\n");
				}		
			}
			l = 0;
		}else{
			linha[j++] = entrada[i];
		}
		
	}
	imprimeListaTokens();
	
  return 0; 
}

