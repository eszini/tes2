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
gcc -o tes_tparser01  tes_tparser01.c  lex.yy.c  y.tab.c   -lfl



cp tes_tparser01.exe ../bin
