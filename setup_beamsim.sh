#!/bin/bash
#-*-Shell-Script-*- #

# $Header: /cvs/projects/numi-beam-sim/numi-beam-sim/g4numi/Attic/setup_beamsim.sh,v 1.1.2.18 2018/03/05 19:39:34 bmesserl Exp $


setup_beamsim(){

    local TOP=${PWD}
    export G4WORKDIR="${TOP}"
    export BEAMSIM="${TOP}"
    export G4NUMIVER="v6"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$G4LIB/plists/Linux-g++:${G4WORKDIR}/tmp/Linux-g++/g4numi"

    echo "G4WORKDIR=${G4WORKDIR}"

    if [ ! -d /cvmfs/minerva.opensciencegrid.org ]; then
        echo “experiment CVMFS repo seems to not be present. Sleeping and then exiting.”
        sleep 1000
        exit 1
    fi

    # geant4
    # We use a modified version "that contains a class to store hadronic 
    #   interaction kinematics. The version is geant4.9.2.p03, which is now
    #   setup with /grid/fermiapp/minerva/beamsim/x86_64/geant4/env.sh
    source /cvmfs/minerva.opensciencegrid.org/minerva/beamsim/x86_64/geant4/env.sh

    #root 
    source /cvmfs/minerva.opensciencegrid.org/minerva/beamsim/x86_64/root/bin/thisroot.sh

    # I don't know what g4photon is for so I'm not going to touch it.
    source /cvmfs/minerva.opensciencegrid.org/minerva/setup/setup_minerva_products.sh
    setup g4photon

    # Not explicitly using any ifdh commands in g4numi_job.sh right now, but
    # leave these in, just in case.
    source /cvmfs/fermilab.opensciencegrid.org/products/common/etc/setups.sh
    setup ifdhc #v2_2_3
    export IFDH_GRIDFTP_EXTRA="-st 10" #set ifdh cp stall timeout to 10 sec
    export IFDH_CP_MAXRETRIES=2
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/libfiles
    setup jobsub_client
    export EXPERIMENT=nova
}
setup_beamsim
