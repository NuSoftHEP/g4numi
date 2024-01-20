#include "NA49TrackingAction.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4Track.hh"
#include "NA49TrackInfo.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"

#include <iostream>

NA49TrackingAction::NA49TrackingAction() {}
NA49TrackingAction::~NA49TrackingAction() {}


void NA49TrackingAction::PostUserTrackingAction(const G4Track* aTrack){

  

G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if(secondaries)
  {
    bool incoming_primary=false; // is aTrack a primary?
    NA49TrackInfo* info = 0;
    if(aTrack->GetParentID()==0) { // if this is the primary
      info=new NA49TrackInfo();
      incoming_primary=true;
#ifdef DEBUG
      std::cout<<aTrack->GetDefinition()->GetParticleName()<<" detected as primary"<<std::endl;
#endif      
    }
    else info = dynamic_cast<NA49TrackInfo*>(aTrack->GetUserInformation());
   
    size_t nSeco = secondaries->size();
    if(nSeco>0)
    {
      for(size_t i=0; i < nSeco; i++)
      {
        NA49TrackInfo* infoNew = new NA49TrackInfo();
	G4Track* secondary=(*secondaries)[i];
        secondary->SetUserInformation(infoNew);
#ifdef DEBUG
	std::cout<<aTrack->GetDefinition()->GetParticleName()
		 <<"("<<aTrack->GetTrackID()<<")"
		 <<" --> "<< secondary->GetDefinition()->GetParticleName()
		 <<"("<<secondary->GetTrackID()<<")";
#endif
	// now record if this secondary is quickly decaying
	// quick is defined as <1e-16 sec, so as to catch
	// pizero, eta, eta' and sigma_0 which G4 explicitly produces
	// but no hadron production experiment explicitly measures
	bool quick=false;
	const G4double quick_lifetime=1e-16*second;
	const G4ParticleDefinition* pd = secondary->GetDefinition();	
	G4double lifetime=pd->GetPDGLifeTime();
	if(!pd->GetPDGStable() &&  lifetime<quick_lifetime) {
	  quick=true;
	  // add user info recording this as a quickly decaying particle
	  infoNew->fast_decay=true;	  
#ifdef DEBUG
	  std::cout<<" quick ("<<lifetime<<")";
#endif
	}  
#ifdef DEBUG
	else {
	  std::cout<<" slow ";
	}
#endif
	// now record if this is the progeny of a quicky decaying particle
	// note, a track can have fast_decay and fast_decay_progeny both true
	// eta' decays to f.s. with eta, for example
	if(info->fast_decay) {
	  infoNew->fast_decay_progeny=true;
#ifdef DEBUG
	  std::cout<<"| fast progeny";
#endif
	}
#ifdef DEBUG
	else std::cout<<"| slow progeny";
#endif
	///////// primary or in primary fast decay chain /////////
	// now record if the parent was the primary and this 
	// secondary was created in a proton inelastic interaction
	// OR if the parent was a quickly decaying particle in the 
	// primary chain
	bool inel=secondary->GetCreatorProcess()->GetProcessName()=="ProtonInelastic";
	if( (incoming_primary && inel)  
	    || ( info->primary_chain && info->fast_decay)){
	  infoNew->primary_chain=true;
#ifdef DEBUG
	  std::cout<<" | primary_chain";
#endif
	}
#ifdef DEBUG
	else{

	  std::cout<<" | not primary_chain";
	}
	std::cout<<std::endl;
#endif 
      }
    }
  }

}
