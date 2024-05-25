#!/bin/bash

#	compilar.sh
#
#	compilar los programas



echo "compilar.sh"
echo " "

echo "COmpilo programa tes_cpar03 ... Lexer / Gramatica  ... "
echo " "



flex lexer.l
yacc -d parser.y
gcc -o tes_cpar03  tes_cpar03.c  lex.yy.c  y.tab.c   -lfl


