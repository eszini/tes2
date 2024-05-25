#!/bin/bash

# United Nations Development Programme
# ARG-838533/2012
#
# Statistical Cause-Effect Analysis in 
# Medical Records Text: A Natural Language Processing Initiative


#	correr.sh
#
#	corre procesos de etapa prueba de parser



echo "correr.sh"
echo " "

echo "Log file: z2 "
echo " "

# ./tes_cpar03 a.txt x1.txt -v -m -f  > z2
./tes_cpar04.exe a.txt x1.txt -dic=diccio01.txt -mrk=f05_t1.mrk  -v  -f  > z2



