#!/bin/bash
#
# Mike Martens 3/16/2010
#
# Run as setup.sh minos or setup.sh nova
# Script to setup environment variables for compiling and running g4numi
#
# Updated to link to code in the /grid/fermiapp/nova/novasrt area
# This script applies only to g4numi (and not g4numi_flugg)
# 
# Uses GEANT4.9.0, CLHEP2.0.3.2, ROOT5.20.0, XERCES2.8.0 
#    

echo ""
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


# Use the following to locate the directory of this file (setup.sh) 
#  and change directory to that location

script_dir="${BASH_SOURCE[0]}"
if([ -h "${script_dir}" ]) then
    while([ -h "${script_dir}" ]) do script_dir=`readlink "${script_dir}"`; done
fi
cd `dirname ${script_dir}`
script_dir=`pwd`

echo "Found script directory: $script_dir"


if [ $TARGET_TYPE == "nova" ] ; then
    echo ""
    cp -v src/NumiDetectorConstruction.cc.me_target src/NumiDetectorConstruction.cc
    cp -v src/NumiTarget.cc.me_target src/NumiTarget.cc
    cp -v src/NumiTargetHall.cc.me_target src/NumiTargetHall.cc
    cp -v src/NumiHorn1.cc.me_target src/NumiHorn1.cc
    cp -v src/NumiDataInput.cc.me_target src/NumiDataInput.cc
    cp -v include/NumiDataInput.hh.me_target include/NumiDataInput.hh
    echo ""
elif [ $TARGET_TYPE == "minos" ] ; then
    echo ""
    cp -v src/NumiDetectorConstruction.cc.le_target src/NumiDetectorConstruction.cc    
    cp -v src/NumiTarget.cc.le_target src/NumiTarget.cc
    cp -v src/NumiTargetHall.cc.le_target src/NumiTargetHall.cc
    cp -v src/NumiHorn1.cc.le_target src/NumiHorn1.cc
    cp -v src/NumiDataInput.cc.le_target src/NumiDataInput.cc
    cp -v include/NumiDataInput.hh.le_target include/NumiDataInput.hh
    echo ""
fi

# I have not got this to compile with GEANT4.9.3
# From the ReleaseNotes for GEANT4.9.3
#   This release of Geant4 has been verified with CLHEP, release 2.0.4.5. 
#   Use of a different CLHEP version may cause incorrect simulation results. 
# export G4INSTALL=/grid/fermiapp/nova/novasrt/geant4/geant4.9.3

export G4INSTALL=/grid/fermiapp/nova/novasrt/geant4/geant4.9.0
export G4INCLUDE=$G4INSTALL/include/
export G4LIB=$G4INSTALL/lib
export G4SYSTEM=Linux-g++
export G4LIB_BUILD_SHARED=1
export G4LEVELGAMMADATA=$G4INSTALL/data/PhotonEvaporation2.0
export G4LEDATA=$G4INSTALL/data/G4EMLOW4.3
export G4NEUTRONHPDATA=$G4INSTALL/data/G4NDL3.11
export G4VMC=$G4INSTALL/geant4_vmc/geant4_vmc.2.3

export CLHEP_DIR=/grid/fermiapp/nova/novasrt/clhep/2.0.3.2
export CLHEP_BASE_DIR=$CLHEP_DIR
export CLHEP_INCLUDE_DIR=$CLHEP_DIR/include
export CLHEP_LIB_DIR=$CLHEP_DIR/lib

export XERCESCROOT=/grid/fermiapp/nova/novasrt/xerces-c/xerces-c-src_2_8_0

export ROOTSYS=/grid/fermiapp/nova/novasrt/root/root.5.20.00

echo "Using GEANT4   version ${G4INSTALL}"
echo "Using CLHELP   version ${CLHEP_DIR}"
echo "Using Xerces   version ${XERCESCROOT}"
echo "Using ROOT     version ${ROOTSYS}"
echo -n "Using gcc      version "
gcc -dumpversion
echo "  (This script has worked with gcc 3.4.6)"
echo ""

# Warn user if adding directories multiple times 
if (echo $LD_LIBRARY_PATH | grep -i geant4) ; then
 echo ""
 echo "WARNING!  GEANT4 directory already exists in LD_LIBRARY_PATH"
 echo ""
fi
if (echo $LD_LIBRARY_PATH | grep -i clhep) ; then
 echo ""
 echo "WARNING!  CLHEP directory already exists in LD_LIBRARY_PATH"
 echo ""
fi
if (echo $LD_LIBRARY_PATH | grep -i xerces) ; then
 echo ""
 echo "WARNING!  Xerces directory already exists in LD_LIBRARY_PATH"
 echo ""
fi
if (echo $LD_LIBRARY_PATH | grep -i root) ; then
 echo ""
 echo "WARNING!  ROOT directory already exists in LD_LIBRARY_PATH"
 echo ""
fi

# set library path
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROOTSYS}/lib:${G4LIB}/Linux-g++:${CLHEP_LIB_DIR}:${XERCESCROOT}/lib"


# set path
if (echo $PATH | grep -i root) ; then
 echo ""
 echo "WARNING!  ROOT directory already exists in PATH"
 echo ""
fi
export PATH="${PATH}:${ROOTSYS}/bin"

# set the g4numi working directory
export G4NUMI_WORK=$PWD
export G4WORKDIR=$G4NUMI_WORK
echo ""
echo "Setting working directory to ${G4WORKDIR}"

echo ""
echo "Files and environment have been setup for ${TARGET_TYPE} geometry"
echo "Now recompile g4numi using gmake"
return




