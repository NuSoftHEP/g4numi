# FOR CLASSES_DEF.XML!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

cet_make_library(
    SOURCE        absbkgtuple_t.cc
                  data_t.cc
                  draytupleMIB_t.cc
                  draytupleSPB_t.cc
                  Edep_t.cc
                  hadmmtuple_t.cc
                  ProdTuple_t.cc
                  target_exit_t.cc
                  TrackInfo_t.cc
                  zptuple_t.cc
    LIBRARY_NAME  dataProducts
    LIBRARIES     ROOT::Core
                  ROOT::Physics
                  Geant4::G4global
)

build_dictionary(DICTIONARY_LIBRARIES dataProducts)

install_source()
install_headers()
