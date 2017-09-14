SOURCE_FILES=emitirMapaDeMemoria.c main.c processarEntrada.c token.c

all:
	gcc $(SOURCE_FILES) -lm -o montador.x -g

clean:
	rm -f montador.x
