#!/bin/bash
clear

NREPEAT=100

CURRENT=185
ZPOS=010

for ((i=0; i<NREPEAT; i++))
do

  FILENAME="G4Nevents_sample.mac"
  NEWFILE="G4Nevents_le${ZPOS}z${CURRENT}i_$i.mac"

  cp $FILENAME $NEWFILE

done

NREPEAT=100

CURRENT=200
ZPOS=250

for ((i=0; i<NREPEAT; i++))
do

  #FILENAME="G4Nevents_water_sample.mac"
  FILENAME="G4Nevents_sample.mac"
  NEWFILE="G4Nevents_le${ZPOS}z${CURRENT}i_$i.mac"

  cp $FILENAME $NEWFILE

done


NREPEAT=100

CURRENT=200
ZPOS=100

for ((i=0; i<NREPEAT; i++))
do

  FILENAME="G4Nevents_sample.mac"
  NEWFILE="G4Nevents_le${ZPOS}z${CURRENT}i_$i.mac"

  cp $FILENAME $NEWFILE

done

NREPEAT=100

CURRENT=-185
ZPOS=010

for ((i=0; i<NREPEAT; i++))
do

  FILENAME="G4Nevents_sample.mac"
  NEWFILE="G4Nevents_le${ZPOS}z${CURRENT}i_$i.mac"

  cp $FILENAME $NEWFILE

done

NREPEAT=100

CURRENT=000
ZPOS=010

for ((i=0; i<NREPEAT; i++))
do

  FILENAME="G4Nevents_sample.mac"
  NEWFILE="G4Nevents_le${ZPOS}z${CURRENT}i_$i.mac"

  cp $FILENAME $NEWFILE

done




