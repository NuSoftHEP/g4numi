#!/bin/bash
#-*-Shell-Script-*- #

# $Header: /cvs/projects/numi-beam-sim/numi-beam-sim/g4numi/Attic/setup_beamsim.sh,v 1.1.2.15.2.2 2016/09/13 18:46:16 rschroet Exp $

# This only works on a NOvA virtual machine for now

setup_beamsim(){
    
    source /grid/fermiapp/products/nova/externals/setup

    setup geant4 v4_10_1_p03b -q debug:e10

    #dk2nu sets the matching version of ROOT, no need to setup ROOT seperately
    setup dk2nu v01_04_01 -q debug:e10:r6

    setup cmake v3_0_1
    
    WHICH_G4=`ls -d ${GEANT4_FQ_DIR}/lib*/Geant4-*`
    WHICH_CLHEP=`ls -d ${CLHEP_LIB_DIR}/CLHEP-*`

    local TOP=${PWD}

    #job submission and file transfer
    source /grid/fermiapp/products/common/etc/setups.sh
    setup jobsub_client
    setup ifdhc

    export BEAMSIM="${TOP}"
    export G4NUMIVER="v6-proto3"

    setup g4photon v3_1

    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$G4LIB/plists/Linux-g++"

    export G4WORKDIR="${TOP}"
    echo "G4WORKDIR is ${G4WORKDIR}"

}
setup_beamsim

