#!/bin/bash
#echo $X509_USER_PROXY

umask 002
export DISPLAY=localhost0.0
export CPN=/grid/fermiapp/common/tools/cpn 

source /nusoft/app/externals/setup
setup geant4 v4_9_6_p01a -q debug:e2
setup root v5_34_05 -q debug:e2


#create the macro file
echo "BEAMSIM=$BEAMSIM"
echo "G4NUMIVER=$G4NUMIVER"

echo "BEAMCONFIG=$BEAMCONFIG"
echo "DOWATER=$DOWATER"
echo "WATERCM=$WATERCM"
echo "POT=$POT"
echo "RUN=$RUN"
echo "PROCESS=$PROCESS"
SEED=$((RUN*10000+PROCESS))
echo "SEED=$SEED"

export LOGFILE=g4numi_${G4NUMIVER}_${BEAMCONFIG}_${RUN}_${PROCESS}.log
export MACFILE=g4numi_${G4NUMIVER}_${BEAMCONFIG}_${RUN}_${PROCESS}.mac
#cd $CONDOR_DIR_G4NUMI

#Make scratch input/output directories on grid node
#job is running on, but check if it exists first
if [ -d "${_CONDOR_SCRATCH_DIR}/output" ]
  then
    :
  else  
    mkdir ${_CONDOR_SCRATCH_DIR}/output
fi

cd ${_CONDOR_SCRATCH_DIR}/output


if [ "$DOWATER" = "false" ]; then
    
    $BEAMSIM/makemacro.py -t $BEAMSIM/macros/template.mac -b $BEAMCONFIG -p $POT -r $RUN -n $PROCESS -s $SEED > ${MACFILE}

else 

    $BEAMSIM/makemacro.py -t $BEAMSIM/macros/template.mac -b $BEAMCONFIG -w -L $WATERCM -p $POT -r $RUN -n $PROCESS -s $SEED > ${MACFILE}
	
fi

echo $LOGFILE
cat ${MACFILE}
$BEAMSIM/g4numi ${MACFILE} > ${LOGFILE}
#rm g4numi.mac
${CPN} ${_CONDOR_SCRATCH_DIR}/output/* ${OUTDIR}/.

# Be careful about the cleanup, don't just blindly rm -r some directory
rm -f ${_CONDOR_SCRATCH_DIR}/output/*.root
rm -f ${_CONDOR_SCRATCH_DIR}/output/*.mac
rm -f ${_CONDOR_SCRATCH_DIR}/output/*.log
