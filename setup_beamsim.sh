#!/bin/bash
#-*-Shell-Script-*- #

echo "You should run now setup_beamsim -w 1"
setup_beamsim(){
    #. "/grid/fermiapp/products/minerva/etc/setups.sh"
     TOP=${PWD}
    local SETWDIR=0
    while getopts "w" opt
      do  case $opt in
	  w) SETWDIR=1 ;;    
	  h) dohelp=1 ;;      
	  [?]) echo "bad command line option"
	      exit 1;;
      esac
    done
    shift ${OPTIND-1}
    
    if [ "$HOSTNAME" == "if01.fnal.gov" ]; then
        echo 'Setting up MINERVA batch submission using minerva_jobsub'
        source /grid/fermiapp/minerva/condor/setup.minerva.condor.sh
        echo ''
    fi
        #setup -f Linux+2.6-2.5 gcc v3_4_3
        #setup gcc v3_4_3 -j -f Linux+2.6-2.5 -z /afs/fnal.gov/ups/db
        # Comment OUT the line below if you are using a local geant4 - g4numi_environment.sh will do this correctly for you
      	#setup -q g77-OpenGL -f Linux+2.6 geant4 v4_9_2_p03
        #setup -f Linux+2.6 -q GCC_3_4_6 root v5_22_00j
        #setup -f Linux+2.6-2.5 clhep v2_0_4_5

        source /nusoft/app/externals/setup

        setup geant4 v4_9_6_p01a -q debug:e2
        setup root v5_34_05 -q debug:e2

        export BEAMSIM="${TOP}"
        export G4NUMIVER="v4"
        #setup g4photon
        #export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$G4LIB/plists/Linux-g++"
	#export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$G4LIB/Geant4-9.6.1/Linux-g++"

# people who just want to run g4numi probably do not 
# need to set G4WORKDIR
    if (( SETWDIR==1 )); then
#        export G4WORKDIR="${TOP}"
        echo "G4WORKDIR is ${G4WORKDIR}"
    else
        unset G4WORKDIR
    fi
    
    unset OPTSTRING
    unset OPTIND
}
setup_beamsim -w 1

