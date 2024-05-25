#!/bin/bash

#	correr.sh
#
#	corre procesos en esta etapa



echo "correr.sh"
echo " "

echo "Log file: z2 "
echo " "

# ./tes_cpar03 a.txt x1.txt -v -m -f  > z2


# ./tes_cpar04 a.txt x1.txt -dic=diccio01.txt -mrk=f05_t1.mrk  -v  -f  -dng=lex  > z2



#./tes_cpar04 a.txt x1.txt  -v  -f  -dng=lex  > z2

#./tes_cpar04 a.txt x1.txt  -v  -f  -dng=nga  > z3


./tes_cpar05 a.txt x5.txt -dng=nga -grm=parser_nga2.txt -v -f > zz



cat zz | grep Resultado > z2

grep ": 00" z2 > sinr
grep ": 01" z2 > zz01
grep ": 02" z2 > zz02
grep ": 03" z2 > zz03
grep ": 04" z2 > zz04
grep ": 05" z2 > zz05
grep ": 06" z2 > zz06
grep ": 07" z2 > zz07
grep ": 08" z2 > zz08
grep ": 09" z2 > zz09
grep ": 10" z2 > zz10
grep ": 11" z2 > zz11
grep ": 12" z2 > zz12
grep ": 13" z2 > zz13

echo "Sin deterctar por ninguna regla ... "
echo " "

wc -l sinr

echo "Total detectados con cada regla ... "
echo " "

wc -l zz??





