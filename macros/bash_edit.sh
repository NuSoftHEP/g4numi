#!/bin/bash
clear



for ((j=0; j<3; j++))
do

if [ "$j" = "0" ]; then
NREPEAT=100
CURRENT=185
ZPOS=010
FILENAME="G4Nevents_sample.mac"
fi

if [ "$j" = "1" ]; then
NREPEAT=100
CURRENT=200
ZPOS=250
#FILENAME="G4Nevents_water_sample.mac"
FILENAME="G4Nevents_sample.mac"
fi

if [ "$j" = "2" ]; then
NREPEAT=100
CURRENT=200
ZPOS=100
FILENAME="G4Nevents_sample.mac"
fi

if [ "$j" = "3" ]; then
NREPEAT=100
CURRENT=-185
ZPOS=010
FILENAME="G4Nevents_sample.mac"
fi

if [ "$j" = "4" ]; then
NREPEAT=100
CURRENT=000
ZPOS=010
FILENAME="G4Nevents_sample.mac"
fi

if [ "$j" = "5" ]; then
NREPEAT=100
CURRENT=200
ZPOS=150
FILENAME="G4Nevents_sample.mac"
fi

if [ "$j" = "6" ]; then
NREPEAT=100
CURRENT=200
ZPOS=010
FILENAME="G4Nevents_sample.mac"
fi

FILL=vacuum


for ((i=0; i<NREPEAT; i++))
do

TEMP1=temp1.dat
TEMP2=temp2.dat
TEMP3=temp3.dat
TEMP4=temp4.dat
TEMP5=temp5.dat
TEMP6=temp6.dat


  NEWFILE="G4Nevents_me${ZPOS}z${CURRENT}i_$i.mac"

  sed -e "s/le010z000i/me${ZPOS}z${CURRENT}i/" $FILENAME > $TEMP1
  
  sed -e "s/LE010z000i/LE${ZPOS}z${CURRENT}i/" $TEMP1 > $TEMP2

  sed -e "s/vacuum/${FILL}/" $TEMP2 > $TEMP3

  RNDM=$(($i+9876543))
  sed -e "s/setRndmSeed 999/setRndmSeed ${RNDM}/" $TEMP3 > $TEMP4

  RUNID=$(($i))
  sed -e "s/setRunID 999/setRunID ${RUNID}/" $TEMP4 > $TEMP5

  FLUKA_ID=$(($i+1))
  sed -e "s/fluka05_000/fluka05_00${FLUKA_ID}/" $TEMP5 > $TEMP6

  sed -e "s/beamOn 999/beamOn 500000/" $TEMP6 > $NEWFILE

rm -f temp1.dat
rm -f temp2.dat
rm -f temp3.dat
rm -f temp4.dat
rm -f temp5.dat
rm -f temp6.dat

done

done


