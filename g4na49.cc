#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "Randomize.hh"

#include "NA49DetectorConstruction.hh"
#include "NA49PhysicsList.hh"
//#include "QBBC.hh"
//#include "QGSP.hh"
#include "FTFP_BERT.hh"

#include "NA49PrimaryGeneratorAction.hh"

#include "NA49RunAction.hh"
#include "NA49EventAction.hh"
#include "NA49StackingAction.hh"
#include "NA49TrackingAction.hh"
#include "NA49Config.hh"

#include "G4VisExecutive.hh"
#include <cstdlib>
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main (int argc,char** argv)
{
  NA49Config config (argc, argv);

  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());

  //get the pointer to the User Interface manager
  G4UImanager* UI = G4UImanager::GetUIpointer();
        
  //Construct the default run manager
  G4RunManager * runManager = new G4RunManager();

  //set mandatory initialization classes
  runManager->SetUserInitialization(
              new NA49DetectorConstruction(config.getTarget()));

  FTFP_BERT* physicsList = new FTFP_BERT;

  runManager->SetUserInitialization(physicsList);
  //runManager->SetUserInitialization(new QBBC(1,"QBEC_HP"));
  //runManager->SetUserInitialization(new QGSP);

  runManager->SetUserAction(new NA49PrimaryGeneratorAction());

  //set user action classes
  runManager->SetUserAction(new NA49RunAction(config));
  runManager->SetUserAction(new NA49EventAction());
  runManager->SetUserAction(new NA49StackingAction());
  runManager->SetUserAction(new NA49TrackingAction());

  //not sure what those options are (taken from macros/na49_example.mac)
  UI->ApplyCommand("/control/verbose 0");
  UI->ApplyCommand("/run/verbose 0");
  UI->ApplyCommand("/tracking/verbose 0");
  UI->ApplyCommand("/testhadr/Update");
  UI->ApplyCommand("/run/initialize");

  //We use the energy beam as the 1st random seed 
  UI->ApplyCommand("/random/setSeeds " + config.getBeam().energy + " "
                   + config.getRunNumber());
  UI->ApplyCommand("/gun/particle " + config.getBeam().particle);
  UI->ApplyCommand("/gun/energy " + config.getBeam().energy + " GeV");
  UI->ApplyCommand("/run/beamOn " + config.getNevents());

  UI->ApplyCommand("/control/execute");

  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//do we need interactive mode for this?

//~ if (argc==1)   // Define UI terminal for interactive mode
//~ {
    //~ G4VisManager* visManager = 0;
//~ 
    //~ #ifdef G4VIS_USE
	//~ //visualization manager
	//~ visManager = new G4VisExecutive;
	//~ visManager->Initialize();
    //~ #endif
	//~ G4UIsession* session = 0;
    //~ #ifdef G4UI_USE_TCSH
	//~ session = new G4UIterminal(new G4UItcsh);
    //~ #else
	//~ session = new G4UIterminal();
    //~ #endif
	//~ session->SessionStart();
	//~ delete session;
	//~ 
    //~ if(visManager) delete visManager;
//~ 
    //~ return 0;
//~ }
