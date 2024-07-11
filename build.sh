#!/bin/sh
#!/bin/bash

# Compila libcsv.c
gcc -c libcsv.c -o libcsv.o

# Compila main.c
gcc -c main.c -o main.o

# Linka os objetos e cria o executável
gcc libcsv.o main.o -o app

echo "Build completo. Execute com ./app"