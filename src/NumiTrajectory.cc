//
// NumiTrajectory.cc
//

#include "NumiTrajectory.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4Polyline.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"
#include "NumiTrackInformation.hh"
#include "NumiDataInput.hh"

G4Allocator<NumiTrajectory> myTrajectoryAllocator;

NumiTrajectory::NumiTrajectory()
{
   fParticleDefinition = 0;
   fParticleName = "";
   fPDGCharge = 0;
   fPDGEncoding = 0;
   fTrackID = 0;
   fParentID = 0;
   fPositionRecord = 0;
   fMomentum = G4ThreeVector(0.,0.,0.);
   fMomentumRecord = 0;
   fVertexPosition = G4ThreeVector(0.,0.,0.);
   fParticleMass = 0.;
   fDecayCode=0;
   fTgen=0;
   fPreStepVolume=0;
   fStepLength = 0;
   fProcessName = "";
   //for dk2nu:
   fTime = 0.0;
   //
   fND=NumiDataInput::GetNumiDataInput();

}

NumiTrajectory::NumiTrajectory(const G4Track* aTrack)
{
   fParticleDefinition = aTrack->GetDefinition();
   fParticleName = fParticleDefinition->GetParticleName();
   fPDGCharge = fParticleDefinition->GetPDGCharge();
   fPDGEncoding = fParticleDefinition->GetPDGEncoding();
   fTrackID = aTrack->GetTrackID();
   fParentID = aTrack->GetParentID();
   fPositionRecord = new NumiTrajectoryPointContainer();
   fPositionRecord->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));
   fMomentumRecord = new NumiTrajectoryMomentumContainer();
   fMomentumRecord->push_back(aTrack->GetMomentum());
   fPreStepVolume = new NumiTrajectoryVolumeName();  
   fPreStepVolume->push_back(aTrack->GetVolume()->GetName());
   fStepLength = new DVec();  
   fStepLength->push_back(aTrack->GetStepLength());
   fMomentum = aTrack->GetMomentum();
   fVertexPosition = aTrack->GetPosition();
   fParticleMass = aTrack->GetDefinition()->GetPDGMass();

   const G4VProcess* pCreatorProcess = aTrack->GetCreatorProcess();
   if (pCreatorProcess) {
       fProcessName = pCreatorProcess->GetProcessName();
   } else {
       fProcessName = "Primary";
   }
   //for dk2nu:
   fTime = aTrack->GetGlobalTime();
   //
   NumiTrackInformation* info=(NumiTrackInformation*)(aTrack->GetUserInformation());
   if (info!=0) {
     fDecayCode = info->GetDecayCode();
     fTgen = info->GetTgen();
     fNImpWt = info->GetNImpWt();
     fParentMomentumAtThisProduction = info->GetParentMomentumAtThisProduction();
   }
   else { 
     fDecayCode = 0;
     fTgen = 0; 
     fNImpWt = 1.;
   }

   fND=NumiDataInput::GetNumiDataInput();
}

NumiTrajectory::NumiTrajectory(NumiTrajectory & right)
  : G4VTrajectory()
{
  fParticleName = right.fParticleName;
  fParticleDefinition = right.fParticleDefinition;
  fPDGCharge = right.fPDGCharge;
  fPDGEncoding = right.fPDGEncoding;
  fTrackID = right.fTrackID;
  fParentID = right.fParentID;
  fPositionRecord = new NumiTrajectoryPointContainer();
  fMomentumRecord = new NumiTrajectoryMomentumContainer();
  fPreStepVolume  = new NumiTrajectoryVolumeName();  
  fStepLength     = new DVec();  

  for(size_t i=0;i<right.fPositionRecord->size();i++)
    {
      G4TrajectoryPoint* rightPoint = (G4TrajectoryPoint*)((*(right.fPositionRecord))[i]);
      fPositionRecord->push_back(new G4TrajectoryPoint(*rightPoint));
    }
  for(size_t i=0;i<right.fMomentumRecord->size();i++)
    {
      G4ThreeVector rightMomentum = (G4ThreeVector)((*(right.fMomentumRecord))[i]);
      fMomentumRecord->push_back(rightMomentum);
    }
  for(size_t i=0;i<right.fPreStepVolume->size();i++)
   {
     G4String rightPreStepVolume=(G4String)((*(right.fPreStepVolume))[i]);
     fPreStepVolume->push_back(rightPreStepVolume);
   }
  for(size_t i=0;i<right.fStepLength->size();i++)
   {
      G4double rightsteplength =(G4double)((*(right.fStepLength))[i]);
      fStepLength->push_back(rightsteplength);
   }
  fMomentum = right.fMomentum;
  fVertexPosition = right.fVertexPosition;
  fParticleMass = right.fParticleMass;
  fDecayCode = right.fDecayCode;
  fTgen = right.fTgen;
  fNImpWt = right.fNImpWt;
  //for dk2nu:
  fTime = right.fTime;
  //
  fND=NumiDataInput::GetNumiDataInput();
}

NumiTrajectory::~NumiTrajectory()
{
  size_t i;
  for(i=0;i<fPositionRecord->size();i++){
    delete  (*fPositionRecord)[i];
  }
  fPositionRecord->clear();

  delete fPositionRecord;

  fMomentumRecord->clear();
  
  delete fMomentumRecord;
  fPreStepVolume->clear();
  delete fPreStepVolume;

  fStepLength->clear();
  delete fStepLength;
 
}


void NumiTrajectory::ShowTrajectory() const
{
   G4cout << G4endl << "TrackID =" << fTrackID 
        << " : ParentID=" << fParentID << G4endl;
   G4cout << "Particle name : " << fParticleName 
        << "  Charge : " << fPDGCharge << G4endl;
   G4cout << "Original momentum : " <<
G4BestUnit(fMomentum,"Energy") << G4endl;
   G4cout << "Vertex : " << G4BestUnit(fVertexPosition,"Length") << G4endl;
   G4cout << "  Current trajectory has " << fPositionRecord->size() 
        << " points." << G4endl;

   for( size_t i=0 ; i < fPositionRecord->size() ; i++){
       G4TrajectoryPoint* aTrajectoryPoint = (G4TrajectoryPoint*)((*fPositionRecord)[i]);
       G4cout << "Point[" << i << "]" 
            << " Position= " << aTrajectoryPoint->GetPosition() << G4endl;
   }
}

void NumiTrajectory::ShowTrajectory(std::ostream& o) const
{
    G4VTrajectory::ShowTrajectory(o);
}

void NumiTrajectory::DrawTrajectory(G4int /*i_mode*/) const
{


   G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
   G4ThreeVector pos;

   G4Polyline pPolyline;
   for (size_t i = 0; i < fPositionRecord->size() ; i++) {
     G4TrajectoryPoint* aTrajectoryPoint = (G4TrajectoryPoint*)((*fPositionRecord)[i]);
     pos = aTrajectoryPoint->GetPosition();
     pPolyline.push_back( pos );
   }

   G4Colour colour(0.2,0.2,0.2);
   if(fParticleDefinition==G4Gamma::GammaDefinition())
      colour = G4Colour(0.,0.,1.);
   else if(fParticleDefinition==G4Electron::ElectronDefinition()
         ||fParticleDefinition==G4Positron::PositronDefinition())
      colour = G4Colour(1.,1.,0.);
   else if(fParticleDefinition==G4MuonMinus::MuonMinusDefinition()
         ||fParticleDefinition==G4MuonPlus::MuonPlusDefinition())
      colour = G4Colour(0.,1.,0.);
   else if(fParticleDefinition->GetParticleType()=="meson")
   {
      if(fPDGCharge!=0.)
         colour = G4Colour(1.,0.,0.);
      else
         colour = G4Colour(0.5,0.,0.);
   }
   else if(fParticleDefinition->GetParticleType()=="baryon")
   {
      if(fPDGCharge!=0.)
         colour = G4Colour(0.,1.,1.);
      else
         colour = G4Colour(0.,0.5,0.5);
   }

   //G4VisAttributes attribs(colour);

   //draw only protons,pi+ and pi-
   G4VisAttributes attribs;
   if (fParticleDefinition==G4Proton::ProtonDefinition()) {
     colour=G4Colour(0.,0.,1.);
     attribs=G4VisAttributes(colour);
     pPolyline.SetVisAttributes(attribs);
     if(pVVisManager) pVVisManager->Draw(pPolyline);
   }
   if (fParticleDefinition==G4PionMinus::PionMinusDefinition()) {
     colour=G4Colour(1.,0.,0.);
     attribs=G4VisAttributes(colour);
     pPolyline.SetVisAttributes(attribs);
     if(pVVisManager) pVVisManager->Draw(pPolyline);
   }
   if (fParticleDefinition==G4PionPlus::PionPlusDefinition()) {
     colour=G4Colour(0.,1.,0.);
     attribs=G4VisAttributes(colour);
     pPolyline.SetVisAttributes(attribs);
     if(pVVisManager) pVVisManager->Draw(pPolyline);
   }

   if(fND->useMuonBeam)
   {
      if (fParticleDefinition==G4MuonMinus::MuonMinusDefinition()
          || fParticleDefinition==G4MuonPlus::MuonPlusDefinition())
      {
         //colour=G4Colour(0.,1.,0.5);
         colour=G4Colour(1.,1.,1.);
         attribs=G4VisAttributes(colour);
         pPolyline.SetVisAttributes(&attribs);
         if(pVVisManager) pVVisManager->Draw(pPolyline);
      }
      if (fParticleDefinition==G4Gamma::GammaDefinition())
      {
         colour=G4Colour(0.,1.,0.);
         attribs=G4VisAttributes(colour);
         pPolyline.SetVisAttributes(&attribs);
         if(pVVisManager) pVVisManager->Draw(pPolyline);
      }
      if (fParticleDefinition==G4Electron::ElectronDefinition()
          || fParticleDefinition==G4Positron::PositronDefinition())
      {
         colour=G4Colour(1.,0.,1.);
         attribs=G4VisAttributes(colour);
         pPolyline.SetVisAttributes(&attribs);
         if(pVVisManager) pVVisManager->Draw(pPolyline);
      }
   }
   
   //pPolyline.SetVisAttributes(attribs);
   //if(pVVisManager) pVVisManager->Draw(pPolyline);
}

void NumiTrajectory::AppendStep(const G4Step* aStep)
{
  fPositionRecord
    ->push_back(new G4TrajectoryPoint(aStep->GetPostStepPoint()
				      ->GetPosition() ));
   fMomentumRecord->push_back(aStep->GetPostStepPoint()->GetMomentum());

   G4Track* aTrack=aStep->GetTrack();
   NumiTrackInformation* info=(NumiTrackInformation*)(aTrack->GetUserInformation());
   if (info!=0) {
     fDecayCode=info->GetDecayCode();
     fTgen=info->GetTgen();
   }
   else fDecayCode=-1;
   
   G4StepPoint * steppoint=aStep->GetPreStepPoint(); 
   G4String PreVolumeName=steppoint->GetPhysicalVolume()->GetName(); 
   fPreStepVolume->push_back(PreVolumeName); 
   fStepLength->push_back(aStep->GetStepLength());

}
  
G4ParticleDefinition* NumiTrajectory::GetParticleDefinition()
{
   return (G4ParticleTable::GetParticleTable()->FindParticle(fParticleName));
}

void NumiTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory) return;

  NumiTrajectory* seco = (NumiTrajectory*)secondTrajectory;
  G4int ent = seco->GetPointEntries();
  for(int i=1;i<ent;i++) // initial point of the second trajectory should not be merged
  {
    fPositionRecord->push_back((*(seco->fPositionRecord))[i]);
  }
  delete (*seco->fPositionRecord)[0];
  seco->fPositionRecord->clear();

}

