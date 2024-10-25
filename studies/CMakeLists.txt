
cet_make_library(
    SOURCE        G4MinervaPionIXS.cc
                  G4MinervaProtonIXS.cc
                  NumiKelvinFunctions.cc
    LIBRARY_NAME  studies
    LIBRARIES     Geant4::G4global
                  Geant4::G4processes
)

install_source()
install_headers()
