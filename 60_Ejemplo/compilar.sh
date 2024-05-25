#!/bin/bash

# United Nations Development Programme
# ARG-838533/2012
#
# Statistical Cause-Effect Analysis in 
# Medical Records Text: A Natural Language Processing Initiative


#	compilar.sh
#
#	compilar los programas



echo "compilar.sh"
echo " "

echo "Compilo programa tes_cparNN ... Lexer / Gramatica  ... "
echo " "



flex lexer.l
yacc -d parser.y

#gcc -o tes_cpar03  tes_cpar03.c  lex.yy.c  y.tab.c   -lfl
gcc -o tes_cpar04.exe  tes_cpar04.c  lex.yy.c  y.tab.c   -lfl  -Wformat-overflow=0



# cp tes_cpar04.exe ../bin
