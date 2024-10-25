# FOR CLASSES_DEF.XML!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/include )

cet_make_library(
    SOURCE        NA49Analysis.cc
                  NA49Config.cc
                  NA49DetectorConstruction.cc
                  NA49DetectorMessenger.cc
                  NA49EventAction.cc
                  NA49EventActionMessenger.cc
                  NA49PhysicsList.cc
                  NA49PhysicsListMessenger.cc
                  NA49PrimaryGeneratorAction.cc
                  NA49RunAction.cc
                  NA49StackingAction.cc
                  NA49TrackInfo.cc
                  NA49TrackingAction.cc
                  NtpMuon.cc
                  NumiAnalysis.cc
                  NumiBaffle.cc
                  NumiDataInput.cc
                  NumiDecayPipe.cc
                  NumiDecayPipeMagneticField.cc
                  NumiDetectorConstruction.cc
                  NumiDetectorMessenger.cc
                  NumiEventAction.cc
                  NumiHadronAbsorber.cc
                  NumiHorn1.cc
                  NumiHorn2.cc
                  NumiHorns.cc
                  NumiHornSpiderSupport.cc
                  NumiImpWeight.cc
                  NumiMagneticField.cc
                  NumiMaterials.cc
                  NumiNOvATarget.cc
                  NumiNuWeight.cc
                  NumiParticleCode.cc
                  NumiPrimaryGeneratorAction.cc
                  NumiPrimaryMessenger.cc
                  NumiRunAction.cc
                  NumiRunActionMessenger.cc
                  NumiRunManager.cc
                  NumiSecMonitors.cc
                  NumiStackingAction.cc
                  NumiSteppingAction.cc
                  NumiTarget.cc
                  NumiTargetHall.cc
                  NumiTrackInformation.cc
                  NumiTrackingAction.cc
                  NumiTrajectory.cc
                  NumiVisManager.cc
    LIBRARY_NAME  src
    LIBRARIES     dataProducts
                  ROOT::Tree
                  Geant4::G4global
                  Geant4::G4persistency
                  Geant4::G4physicslists
                  Geant4::G4run
                  Geant4::G4track
                  dk2nuTree
                  XercesC::XercesC
)

install_source()
