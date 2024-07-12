#!/bin/sh
#!/bin/bash

# Compila main.c, adicionando o diretório libcsv ao caminho de pesquisa de cabeçalhos
gcc -c main.c -o main.o -Ilibcsv

# Compila libcsv.c
gcc -c libcsv/libcsv.c -o libcsv.o

# Linka os objetos e cria o executável
gcc libcsv.o main.o -o app