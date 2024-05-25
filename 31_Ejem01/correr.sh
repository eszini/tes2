#!/bin/bash

#	correr.sh
#
#	corre procesos en esta etapa



echo "correr.sh"
echo " "

echo "Log file: z1 y z2 "
echo " "


#./tes_cpar05 a.txt x5.txt -mrk=f05_t1.mrk -dic=diccio01.txt -dng=nga -grm=parser_nga2.txt -v -f > zz

./tes_cpar05 a_neg.txt x5_neg.txt -dng=nga -grm=gram_nga.txt -v -f  -opciones=d4 > z1

./tes_cpar05 a_aff.txt x5_aff.txt -dng=nga -grm=gram_nga.txt -v -f  -opciones=d4 > z2



grep Resultado z1 | sort > z1.srt
grep Resultado z2 | sort > z2.srt


grep ": 00" z1.srt | wc -l
grep ": 00" z2.srt | wc -l

grep ": 01" z1.srt > neg01.txt
grep ": 02" z1.srt > neg02.txt
grep ": 03" z1.srt > neg03.txt
grep ": 04" z1.srt > neg04.txt
grep ": 05" z1.srt > neg05.txt
grep ": 06" z1.srt > neg06.txt
grep ": 07" z1.srt > neg07.txt
grep ": 08" z1.srt > neg08.txt
grep ": 09" z1.srt > neg09.txt
grep ": 10" z1.srt > neg10.txt
grep ": 11" z1.srt > neg11.txt
grep ": 12" z1.srt > neg12.txt
grep ": 13" z1.srt > neg13.txt
grep ": 14" z1.srt > neg14.txt

grep ": 01" z2.srt > aff01.txt
grep ": 02" z2.srt > aff02.txt
grep ": 03" z2.srt > aff03.txt
grep ": 04" z2.srt > aff04.txt
grep ": 05" z2.srt > aff05.txt
grep ": 06" z2.srt > aff06.txt
grep ": 07" z2.srt > aff07.txt
grep ": 08" z2.srt > aff08.txt
grep ": 09" z2.srt > aff09.txt
grep ": 10" z2.srt > aff10.txt
grep ": 11" z2.srt > aff11.txt
grep ": 12" z2.srt > aff12.txt
grep ": 13" z2.srt > aff13.txt
grep ": 14" z2.srt > aff14.txt






echo "Totales: " > totales.txt
echo " " >> totales.txt
echo "Sobre frases 'nagated' " >> totales.txt

wc -l neg??.txt > totales.txt

echo " " >> totales.txt
echo " " >> totales.txt
echo "Sobre frases 'affirmed' " >> totales.txt

wc -l aff??.txt >> totales.txt

echo " " >> totales.txt
echo " " >> totales.txt
echo " " >> totales.txt



cat totales.txt

