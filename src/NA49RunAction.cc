
#include "NA49RunAction.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "NA49Analysis.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

//~ NA49RunAction::NA49RunAction(const G4String &filename) : output(filename)
//~ {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

NA49RunAction::~NA49RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void NA49RunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4int id = aRun->GetRunID();
  G4cout << "### Run " << id << " start" << G4endl;

  const G4long* table_entry;
  table_entry = CLHEP::HepRandom::getTheSeeds();
  G4long id0 = table_entry[0];
  G4long id1 = table_entry[1];

#ifdef G4VIS_USE
  G4UImanager* UI = G4UImanager::GetUIpointer();

  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();

  if(pVVisManager){
    UI->ApplyCommand("/vis/scene/notifyHandlers");
  }
#endif

NA49Analysis* analysis = NA49Analysis::getInstance();
  analysis->book(config);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void NA49RunAction::EndOfRunAction(const G4Run*)
{
  G4cout << "RunAction: End of run actions are started" << G4endl;

#ifdef G4VIS_USE
  if (G4VVisManager::GetConcreteInstance())
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
#endif

NA49Analysis* analysis = NA49Analysis::getInstance();
  analysis->finish();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
