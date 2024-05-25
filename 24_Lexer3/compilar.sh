#!/bin/bash

#	compilar.sh
#
#	compilar los programas



echo "compilar.sh"
echo " "

echo "Compilo programa  de prueba para verificar que conjunto basico de lexer y gramatica anda ... "
echo " "



flex lexer.l
yacc -d parser.y
gcc -o tes_tparser02  tes_tparser02.c  lex.yy.c  y.tab.c   -lfl


# -rw-r--r-- 1 Ernesto None 49883 Sep  2 09:09 lex.yy.c
# -rwxr-xr-x 1 Ernesto None  1310 Sep  2 09:08 tes_tparser01.c
# -rw-r--r-- 1 Ernesto None 47620 Sep  2 09:09 y.tab.c
