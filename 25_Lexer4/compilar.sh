#!/bin/bash

#	compilar.sh
#
#	compilar los programas



echo "compilar.sh"
echo " "

echo "Compilo programa tes_cparNN ... Lexer / Gramatica  ... "
echo " "



flex lexer.l
yacc -d parser.y

gcc -o tes_cpar05  tes_cpar05.c  lex.yy.c  y.tab.c   -lfl
gcc -o tes_cpar06  tes_cpar06.c  lex.yy.c  y.tab.c   -lfl


cp tes_cpar05.exe ../bin
cp tes_cpar06.exe ../bin

