#!/bin/bash

#	compilar.sh
#
#	compilar los programas



echo "compilar.sh"
echo " "

echo "COmpilo programa tes_cparNN ... Lexer / Gramatica  ... "
echo " "



flex lexer.l
yacc -d parser.y

#gcc -o tes_cpar03  tes_cpar03.c  lex.yy.c  y.tab.c   -lfl
gcc -o tes_cpar04  tes_cpar04.c  lex.yy.c  y.tab.c   -lfl



cp tes_cpar04.exe ../bin
