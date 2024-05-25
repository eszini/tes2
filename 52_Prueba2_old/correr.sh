#!/bin/bash

#	correr.sh
#
#	corre procesos en esta etapa



echo "correr.sh"
echo " "

echo "Log file: z3 "
echo " "

# ./tes_cpar03 a.txt x1.txt -v -m -f  > z2
# ./tes_cpar03 a.txt x1.txt -v  -f  > z2

./tes_cpar03 a_neg.txt x2.txt -niv=n01_t1.niv -dng=2  -v -f > z3


