#!/bin/bash
# Mike Martens 9/22/09
# Luke Corwin 11/10/2009
#
# Script to setup environment variables for compiling and running g4numi
# Run as setup.sh minos or setup.sh nova
#
# The correct combination seems to be GEANT4.9.0, CLHEP2.0.3.2, ROOT5.20.0, 
# but this was determined by trial and error.
    

if [ X$1 == X ] ; then
    echo "Usage: setup.sh minos"
    echo "   or: setup.sh nova"
    return
elif [ $1 == "minos" ] ; then
    echo "Setting up for MINOS configuration"
    export TARGET_TYPE="minos"
elif [ $1 == "nova" ] ; then
    echo "Setting up for NOvA configuration"
    export TARGET_TYPE="nova"
else 
    echo "Usage: setup.sh minos"
    echo "   or: setup.sh nova"
    return
fi

if [ $TARGET_TYPE == "nova" ] ; then
    echo ""
    cp -v src/NumiDetectorConstruction.cc.me_target src/NumiDetectorConstruction.cc
    cp -v src/NumiTarget.cc.me_target src/NumiTarget.cc
    cp -v src/NumiTargetHall.cc.me_target src/NumiTargetHall.cc
    cp -v src/NumiHorn1.cc.me_target src/NumiHorn1.cc
    cp -v src/NumiDataInput.cc.me_target src/NumiDataInput.cc
    cp -v src/NumiTargetHall.cc.me_target src/NumiTargetHall.cc
    cp -v include/NumiDataInput.hh.me_target include/NumiDataInput.hh
    cp -v ../g4numi_flugg/for/fluscw.f.me_target ../g4numi_flugg/for/fluscw.f 
    echo ""
elif [ $TARGET_TYPE == "minos" ] ; then
    echo ""
    cp -v src/NumiDetectorConstruction.cc.le_target src/NumiDetectorConstruction.cc    
    cp -v src/NumiTarget.cc.le_target src/NumiTarget.cc
    cp -v src/NumiTargetHall.cc.le_target src/NumiTargetHall.cc
    cp -v src/NumiHorn1.cc.le_target src/NumiHorn1.cc
    cp -v src/NumiDataInput.cc.le_target src/NumiDataInput.cc
    cp -v src/NumiTargetHall.cc.le_target src/NumiTargetHall.cc
    cp -v include/NumiDataInput.hh.le_target include/NumiDataInput.hh
    cp -v ../g4numi_flugg/for/fluscw.f.le_target ../g4numi_flugg/for/fluscw.f
    echo ""
fi


# If this script does not work, you might want to try the one 
# of the novasoft setup scripts
#. /nas-pool/e929/code/novasoft/setup/setup_novasoft_fnal.sh
#. /nas-pool/e929/code/novasoft/setup/setup_novasoft_fnal.sh -r development
#. /nas-pool/e929/code/novasoft/setup/setup_novasoft_fnal.csh -r S09.05.01


export CLHEP_DIR=/nas-pool/e929/code/clhep/2.0.3.2
export CLHEP_BASE_DIR=/nas-pool/e929/code/clhep/2.0.3.2
export CLHEP_INCLUDE_DIR=/nas-pool/e929/code/clhep/2.0.3.2/include
export CLHEP_LIB_DIR=/nas-pool/e929/code/clhep/2.0.3.2/lib
echo "Using CLHELP version ${CLHEP_DIR}"

export G4INSTALL=/nas-pool/e929/code/geant4/geant4.9.0
export G4INCLUDE=/nas-pool/e929/code/geant4/geant4.9.0/include/
export G4LIB=/nas-pool/e929/code/geant4/geant4.9.0/lib
export G4SYSTEM=Linux-g++
export G4LIB_BUILD_SHARED=1
export G4LEVELGAMMADATA=/nas-pool/e929/code/geant4/geant4.9.0/data/PhotonEvaporation2.0
export G4LEDATA=/nas-pool/e929/code/geant4/geant4.9.0/data/G4EMLOW4.3
export G4NEUTRONHPDATA=/nas-pool/e929/code/geant4/geant4.9.0/data/G4NDL3.11
export G4VMC=/nas-pool/e929/code/geant4_vmc/geant4_vmc.2.3
echo "Using GEANT 4 version ${G4INSTALL}"

export ROOTSYS=/nas-pool/e929/code/root/root.5.20.00
echo "Using ROOT version ${ROOTSYS}"

export XERCESCROOT=/nas-pool/e929/code/xerces-c/xerces-c-src_2_8_0
echo "Using Xerces version ${XERCESCROOT}"

#setup gcc
setup gcc v3_4_3
echo "Using gcc version ${GCC_DIR}"

# set library path
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROOTSYS}/lib:${G4LIB}/Linux-g++:${CLHEP_LIB_DIR}:${XERCESCROOT}/lib"


# set path
export PATH="${PATH}:${ROOTSYS}/bin"

# set the g4numi working directory

export G4NUMI_WORK=$PWD
export G4WORKDIR=$G4NUMI_WORK
echo "Setting working directory to ${G4WORKDIR}"

echo ""
echo "Files and environment have been setup for ${TARGET_TYPE} geometry"
echo "Now recompile g4numi using gmake"
return




