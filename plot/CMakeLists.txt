
cet_make_library(
    SOURCE        G4numiNtp.cxx
    LIBRARY_NAME  plot
    LIBRARIES     ROOT::Core
                  Geant4::G4global
)

install_source()
install_headers()

file(GLOB root_macros *.C)
install(
    FILES ${root_macros}
    DESTINATION macros/root
)
