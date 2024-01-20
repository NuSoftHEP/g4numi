#
# source /nusoft/app/externals/setup
# setup cmake
# setup geant4 v4_9_6_p01a -q debug:e2
# setup root   v5_34_05    -q debug:e2
#
# cd /nova/app/users/$USER
#
# mkdir cmakebuild
# cd    cmakebuild
#
# WHICH_G4=`ls -d ${GEANT4_FQ_DIR}/lib64/Geant4-*`
# WHICH_CLHEP=`ls -d ${CLHEP_LIB_DIR}/CLHEP-*`
# 
# cmake -DGeant4_DIR=${WHICH_G4} -DCLHEP_DIR=${WHICH_CLHEP}  
#    [ -DWITH_G4NU=ON ] [ -DWITH_DK2NU=ON ] /path/to/source/g4numi
#
#
# Required environment variables:
#    ROOTSYS    e.g. /nusoft/app/externals/root/v5_34_05/Linux64bit+2.6-2.5-e2-debug
#    XERCESROOT e.g. /nusoft/app/externals/xerces_c/v3_1_1/Linux64bit+2.6-2.5-e2-debug
#
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(g4numi)

option(WITH_MODERN_G4 "not building against old G4 w/ 1-arg exception" ON)
option(WITH_DK2NU     "build against Dk2Nu" ON)
option(WITH_G4NU      "build against G4NuPhysicsLists" OFF)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(CLHEP REQUIRED)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake
                      $ENV{ROOTSYS}/cmake/modules
                      $ENV{ROOTSYS}/../source/root/cmake/modules
                      $ENV{ROOTSYS}/etc/cmake
                      ${CMAKE_MODULE_PATH})

find_package(ROOT REQUIRED)
#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include
                    / ) # add abs path for find_file() in FindROOT.cmake
                        # to avoid linkFile-NOTFOUND issue

# Geant4_INCLUDE_DIR --->  /path/to/version/include/Geant4 
#        ... but also want /path/to/version/include
# INCLUDE_DIRS also includes paths for CLHEP and XERCES_C
include_directories(${Geant4_INCLUDE_DIR}/..)  # #include "Geant4/file.h"
include_directories(${Geant4_INCLUDE_DIRS})    # all the standard paths
include_directories($ENV{ROOTSYS}/include)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(_root_internal_libraries
    Core
    Cint
    RIO
    Net
    Hist
    Graf
    Graf3d
    Gpad
    Tree
    Rint
    Postscript
    Matrix
    Physics
    MathCore
    Thread
    EG
)
set(Root_LIBRARIES ${_root_internal_libraries})

link_directories($ENV{ROOTSYS}/lib)

if(WITH_DK2NU)
  # rwh for Minerva + dk2nu
  set(DK2NU $ENV{DK2NU})
  include_directories(${DK2NU}/include/dk2nu/tree)
  link_directories(${DK2NU}/lib)
  set(DK2NU_LIBRARIES dk2nuTree)
endif()

if(WITH_G4NU)
   set(G4NU $ENV{G4NU})
   include_directories(${G4NU}/include)
   link_directories(${G4NU}/lib)
   set(G4NU_LIBRARIES g4nupl)
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB cintheaders ${PROJECT_SOURCE_DIR}/include/*_t.hh)

# RWH weed out some stuff that really shouldn't be in this branch
file(GLOB rmsources1 ${PROJECT_SOURCE_DIR}/src/NA49*)
file(GLOB rmsources2 ${PROJECT_SOURCE_DIR}/src/dk*)
file(GLOB rmsources3 ${PROJECT_SOURCE_DIR}/src/g4numiCint*)
file(GLOB rmheaders1 ${PROJECT_SOURCE_DIR}/include/NA49*)
file(GLOB rmheaders2 ${PROJECT_SOURCE_DIR}/include/dk*)
file(GLOB rmheaders3 ${PROJECT_SOURCE_DIR}/include/g4numiCint*)

LIST(REMOVE_ITEM sources ${rmsources1} ${rmsources2} ${rmsources3} )
LIST(REMOVE_ITEM headers ${rmheaders1} ${rmheaders2} ${rmheaders3} )

#----------------------------------------------------------------------------
# Create the ROOT dictionary for the data classes
# no more need to do that by hand
#
if(EXISTS ${PROJECT_SOURCE_DIR}/LinkdefModern.h)
  # hack because minerva branch has BadThings in Linkdef.h
  set(g4numi_LINKDEF ${PROJECT_SOURCE_DIR}/LinkdefModern.h)
else()
  set(g4numi_LINKDEF ${PROJECT_SOURCE_DIR}/Linkdef.h)
endif()
# MESSAGE("RWH g4numi_LINKDEF ${g4numi_LINKDEF}")
# MESSAGE("RWH headers ${headers}")
set(g4numi_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/g4numiDict)
# MESSAGE("RWH g4numi_DICTIONARY ${g4numi_DICTIONARY}")
ROOT_GENERATE_DICTIONARY(${g4numi_DICTIONARY} ${cintheaders} LINKDEF ${g4numi_LINKDEF})
# MESSAGE("RWH generated g4numi_DICTIONARY ")
set(sources ${sources} ${g4numi_DICTIONARY})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(g4numi g4numi.cc ${sources} ${headers})
target_link_libraries(g4numi ${Geant4_LIBRARIES} ${Root_LIBRARIES} ${DK2NU_LIBRARIES} ${G4NU_LIBRARIES})

if(WITH_DK2NU)
  SET_TARGET_PROPERTIES(g4numi PROPERTIES COMPILE_FLAGS -DWITH_DK2NU )
endif()

if(WITH_G4NU)
  SET_TARGET_PROPERTIES(g4numi PROPERTIES COMPILE_FLAGS -DWITH_G4NU )
endif()

if(WITH_MODERN_G4)
  SET_TARGET_PROPERTIES(g4numi PROPERTIES COMPILE_FLAGS -DMODERN_G4 )
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLEG4NUMI_SCRIPTS
# me000z200i3.mac
  )

foreach(_script ${EXAMPLEG4NUMI_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(G4NUMI DEPENDS g4numi)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS g4numi DESTINATION bin)
