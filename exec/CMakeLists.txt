include_directories( ${CMAKE_SOURCE_DIR}/include )

cet_make_exec(
    NAME       g4numi
    SOURCE     g4numi.cc
    LIBRARIES  Geant4::G4interfaces
               Geant4::G4vis_management
               src
)

cet_make_exec(
    NAME       g4na49
    SOURCE     g4na49.cc
    LIBRARIES  Geant4::G4FR
               Geant4::G4GMocren
               Geant4::G4RayTracer
               Geant4::G4Tree
               Geant4::G4vis_management
               Geant4::G4visHepRep
               Geant4::G4VRML
               src
)
