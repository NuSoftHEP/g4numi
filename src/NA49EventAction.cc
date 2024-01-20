#include "NA49EventAction.hh"
#include "G4Event.hh"
#include "NA49EventActionMessenger.hh"
#include "NA49Analysis.hh"
#include "NA49TrackInfo.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4Track.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

NA49EventAction::NA49EventAction():
  printModulo(100),
  nSelected(0),
  drawFlag("all"),
  debugStarted(false)
{
  eventMessenger = new NA49EventActionMessenger(this);
  UI = G4UImanager::GetUIpointer();
  selectedEvents.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

NA49EventAction::~NA49EventAction()
{
  delete eventMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void NA49EventAction::BeginOfEventAction(const G4Event* evt)
{
  // New event
  G4int nEvt = evt->GetEventID();
  if(nEvt%1000==0)G4cout<<"EventID " <<nEvt<<G4endl;
 if(nSelected>0) {
    for(G4int i=0; i<nSelected; i++) {
      if(nEvt == selectedEvents[i]) {
        UI->ApplyCommand("/random/saveThisEvent");
        UI->ApplyCommand("/tracking/verbose  2");
        debugStarted = true;
        break;
      }
    }
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void NA49EventAction::AddTrack(const G4Track* aTrack)
{
  
  TrackInfo_t aTrackInfo;
  aTrackInfo.PDGcode = (aTrack->GetDefinition())->GetPDGEncoding();
  aTrackInfo.Pos.SetX(aTrack->GetPosition().x());
  aTrackInfo.Pos.SetY(aTrack->GetPosition().y());
  aTrackInfo.Pos.SetZ(aTrack->GetPosition().z());
  aTrackInfo.Mom.SetPx(aTrack->GetMomentum().x());
  aTrackInfo.Mom.SetPy(aTrack->GetMomentum().y());
  aTrackInfo.Mom.SetPz(aTrack->GetMomentum().z());
  aTrackInfo.Mom.SetE(aTrack->GetTotalEnergy());
  const NA49TrackInfo* info = dynamic_cast<NA49TrackInfo*>(aTrack->GetUserInformation());
  aTrackInfo.FromFast = info->fast_decay_progeny;

  TrackInfoVec.push_back(aTrackInfo);
 
}


void NA49EventAction::EndOfEventAction(const G4Event* evt)
{

  if(debugStarted) {
    UI->ApplyCommand("/tracking/verbose  0");
    debugStarted = false;
  }

  NA49Analysis* analysis = NA49Analysis::getInstance();
  if(TrackInfoVec.size()<150)analysis->FillNtuple(TrackInfoVec);
  TrackInfoVec.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
