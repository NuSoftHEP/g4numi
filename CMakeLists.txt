cmake_minimum_required (VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(g4numi VERSION 24.10.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules
                      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  #WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -Wno-pedantic -Wno-unused-local-typedefs -Wno-narrowing -Wno-deprecated-declarations -Wno-unused-variable -Wno-missing-braces -Wno-undefined-var-template
  EXTRA_CXX_FLAGS
)

cet_enable_asserts()

# issue 5589: if we are not in a debugging build, disable the dispatch of debug messages in messagefacility
string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
if( ${BTYPE_UC} STREQUAL "OPT" OR
    ${BTYPE_UC} STREQUAL "PROF" OR
    ${BTYPE_UC} STREQUAL "RELEASE" OR
    ${BTYPE_UC} STREQUAL "MINSIZEREL" )
  message ( STATUS "Disabling LOG_DEBUG from messagefacility library" )
  add_definitions(-DML_NDEBUG)
endif()

add_definitions( -DCMAKEBUILD )

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
# put products in this list that you want to have set up with
# their environment by ups
find_package( Geant4         v4_10_4  )
find_package( canvas_root_io v1_05_02 )
find_package( dk2nudata      v01_08_00g )
find_package(XercesC)
find_package(ROOT 6.22.08d    REQUIRED COMPONENTS Core EG Eve Geom Gdml Gpad Graf Graf3d Gui Hist Matrix MathCore MathMore Minuit Minuit2 Net Physics RIO Spectrum TMVA Thread Tree TreePlayer X3d XMLIO)

include(ArtDictionary)

add_subdirectory(dataProducts)
add_subdirectory(exec)
add_subdirectory(include)
add_subdirectory(macros)
add_subdirectory(plot)
add_subdirectory(src)
add_subdirectory(studies)

install_scripts()

# install root macros
file(GLOB root_macros *.C)
install(
    FILES ${root_macros}
    DESTINATION macros/root
)

cet_cmake_config()
