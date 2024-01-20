#!/bin/bash

echo
echo "======== cd to CONDOR_DIR_INPUT ========"
cd $CONDOR_DIR_INPUT

echo
echo "======== ls ========"
ls

echo
echo "======== UNTARRING... ========"
tar xvfz local_install.tar.gz -C ./ > /dev/null

echo
echo "======== Done untarring. ls ========"
ls

echo
echo "======== SETUP G4, ROOT, ETC ========"
echo "source setup_beamsim.sh"
source setup_beamsim.sh

echo
echo "======== UPDATE MACRO WITH RUN NUMBER ========"
SEED=$((RUN*10000+PROCESS))
sed -i 's/\${seed}/'$SEED'/g' g4numi.mac
OUTFILE="g4numi${G4NUMIVER}_${PLAYLIST}_${BEAMCONFIG}_${PROCESS}"
sed -i 's/\${outfile}/'$OUTFILE'/g' g4numi.mac
OUTFILE2="h1_trackingplane${G4NUMIVER}_${PLAYLIST}_${BEAMCONFIG}_${PROCESS}"
sed -i 's/\${outfile2}/'$OUTFILE2'/g' g4numi.mac


echo "BEAMCONFIG=${BEAMCONFIG}"
echo "PLAYLIST=${PLAYLIST}"
echo "G4NUMIVER=${G4NUMIVER}"
echo "PROCESS=$PROCESS"
echo "SEED=$SEED"
echo "OUTFILE=$OUTFILE"

echo
echo "======== MACRO CONTENT ========"
cat g4numi.mac

echo
echo "======== EXECUTING g4numi ========"
echo "g4numi_g4numi.mac"
g4numi g4numi.mac

echo
echo "Moving output to CONDOR_DIR_G4NUMI"
mv *.root $CONDOR_DIR_G4NUMI
