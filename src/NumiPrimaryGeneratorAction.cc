//----------------------------------------------------------------------
// $Id
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <iomanip>

#include "NumiPrimaryGeneratorAction.hh"

#include <math.h>
#include <map>
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "NumiPrimaryMessenger.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "NumiDataInput.hh"
#include "G4UImanager.hh"
#include "NumiRunManager.hh"
#include "NumiParticleCode.hh"
#include "NumiAnalysis.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

#include "TROOT.h"
#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TSystem.h>

using namespace std;

NumiPrimaryGeneratorAction::NumiPrimaryGeneratorAction()
   :fStart(false),
    fUseGeantino(false),
    fUseMuonGeantino(false),
    fZOriginGeantino(-515.), // Upstream of target.
    fSigmaZOriginGeantino(100.), // Upstream of target.
    fPolarAngleGeantino(.005),
    fPolarAngleGeantinoMin(0.)
{
  beamMessenger = new NumiPrimaryMessenger(this);
  fND=NumiDataInput::GetNumiDataInput();
  G4int n_particle = 1;
  fIsFirst=true;
  fParticleGun = new G4ParticleGun(n_particle);
  fRunManager=(NumiRunManager*)NumiRunManager::GetRunManager();
  tunnelPos = G4ThreeVector(0,0,fND->TunnelLength/2.+fND->TunnelZ0);
  cosx=0;
  cosy=0;

  if(fND->GetDebugLevel() > 0)
   {
      std::cout << "NumiPrimaryGeneratorAction Constructor Called." << std::endl;
   }

}

NumiPrimaryGeneratorAction::~NumiPrimaryGeneratorAction()
{
   if(fND->GetDebugLevel() > 0)
   {
      std::cout << "NumiPrimaryGeneratorAction Destructor Called." << std::endl;
   }

  delete beamMessenger;
  delete fParticleGun;
}

void NumiPrimaryGeneratorAction::SetProtonBeam()
{
   if(fND->GetDebugLevel() > 1) { G4cout << "NumiRunManager::SetProtonBeam() called." << G4endl;}
   
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
 
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("proton"));
 // fParticleGun->SetParticleDefinition(particleTable->FindParticle("electron"));

  fParticleGun->SetParticleEnergy(fND->protonKineticEnergy);
  fParticleGun->SetParticlePosition(fND->beamPosition);
  fParticleGun->SetParticleMomentumDirection(fND->beamDirection);

  fCurrentPrimaryNo=0;
}

// Used as a quicker check of muon response
// past the end of the decay pipe. The values are hard-coded
// for the muon 'beam' as to make it easier to change.

void NumiPrimaryGeneratorAction::SetMuonBeam()
{
   if(fND->GetDebugLevel() > 1) { G4cout << "NumiRunManager::SetMuonBeam() called." << G4endl;}
   
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("mu+"));
  fptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum("mu+"));

  G4double mass = particleTable->FindParticle("mu+")->GetPDGMass();
  G4double mom = fND->GetMuonBeamMomentum();
  fParticleGun->SetParticleEnergy( sqrt(mass*mass+mom*mom)-mass );
  fParticleMomentum=G4ThreeVector(0.0*MeV, 0.0*MeV, mom);
     
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1)); // straight down the pipe

  /*G4float xpos, ypos, zpos;

  xpos = 0;
  ypos = 0;
  zpos = fND->DecayPipeLength + fND->TunnelZ0;;
 
  G4ThreeVector muonBeamPos = G4ThreeVector(xpos, ypos, zpos) - tunnelPos;
  G4ThreeVector muonBeamDirection = G4ThreeVector(0, 0, 1); // straight down the pipe

  fParticleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));
  //  fParticleGun->SetParticleEnergy(80*GeV);// just a guess right now
  fParticleGun->SetParticlePosition(muonBeamPos);
  fParticleGun->SetParticleMomentumDirection(muonBeamDirection);
  */

  fCurrentPrimaryNo=0;
}

//
// Used to begug geometry, and/or, Radiography of the detector. 
//
void NumiPrimaryGeneratorAction::Geantino(G4Event* anEvent)
{
// If nothing else is set, use a proton beam

    fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("geantino")); // geantino
    
    const double dr = fPolarAngleGeantinoMin + 
                      (fPolarAngleGeantino - fPolarAngleGeantinoMin)*CLHEP::RandFlat::shoot();
// Back door to study tile of Horn1 via magentic effect. Take a point source at = 0.
//     double dPhi  = M_PI/2.;
//     while (((dPhi >  M_PI/4.) && (dPhi <  3.0*M_PI/4.)) ||
//            ((dPhi > (M_PI + M_PI/4.)) && (dPhi < (2.0*M_PI - M_PI/4.))))
//        dPhi = 2.0*M_PI*CLHEP::RandFlat::shoot();
	
//    const double dPhi = 2.0*M_PI*CLHEP::RandFlat::shoot();
//    const double dPhi = -M_PI/2. - 0.05 + 0.1*CLHEP::RandFlat::shoot(); 
      // Shooting down, to study the lower portion of the nova target. 
//
     const double dPhi = 0.; // testing magnetic field rotation..  
    const double dx = dr*std::cos(dPhi);
// Shoot downs to check the bottom plate.. 
//    const double dx = 0.; 
    const double dy = dr*std::sin(dPhi);
    const double dz = sqrt(1.0 - (dx*dx + dy*dy));
    G4ThreeVector direction(dx, dy, dz);
// Shoot downs to check the bottom plate.. 
//    G4ThreeVector direction(dx, -1.0*std::abs(dy), dz);
//
// Store it a proton internally to Numi d/s  ... Messy: 
//    fProtonN = fCurrentPrimaryNo;
    
    fTgen=0;
//    const double x =  G4RandGauss::shoot(0.0, 1.3);  
//    const double y =  G4RandGauss::shoot(0.0, 1.3);  
// Back door to study tile of Horn1 via magentic effect. Take a point source at = 0.
//    const double x =  G4RandGauss::shoot(0.0, 1.3e-10);  
    const double x =  90.0;  
    const double y =  G4RandGauss::shoot(0.0, 1.3e-10);  
// Back door to study effect of overlap
//    const double dPhiPos = 2.0*M_PI*CLHEP::RandFlat::shoot();
//    const double x =  G4RandGauss::shoot(0.0, 0.00001) + 26.0 * std::sin(dPhiPos);  
//    const double y =  G4RandGauss::shoot(0.0, 0.00001) + 26.0 * std::cos(dPhiPos);  
    const double z =  fZOriginGeantino +  G4RandGauss::shoot(0.0, fSigmaZOriginGeantino);  
    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
    fParticleGun->SetParticleMomentumDirection(direction);
//    std::cerr << " Muon Vertex " << G4ThreeVector(x, y, z) << " direction " << direction << std::endl;
    if (fUseMuonGeantino) {
       //
       // Go systematically in polar angle, event by event..  Phi fixed to 45 degrees.
       //

       const G4Run *mRun = G4RunManager::GetRunManager()->GetCurrentRun();
       const double frEvt = (double) anEvent->GetEventID()/((double) mRun->GetNumberOfEventToBeProcessed());
       const double dr = fPolarAngleGeantinoMin + (fPolarAngleGeantino - fPolarAngleGeantinoMin)*frEvt;
//       const double dPhiGG = 0.17 + frEvt*(M_PI/2.)/5.; 
       const double dPhiGG = 0. + frEvt*(M_PI/2.)/5.; 
//       const double dx = std::cos(dPhiGG)*dr;
//       const double dy = std::sin(dPhiGG)*dr;
       const double dy = 0.;
       const double dx = dr;
       const double dz = sqrt(1.0 - (dx*dx + dy*dy));
       G4ThreeVector direction(dx, dy, dz);
//       fParticleGun->SetParticlePosition(G4ThreeVector(0., 0.,  fZOriginGeantino));
       fParticleGun->SetParticlePosition(G4ThreeVector(65., 0.,  fZOriginGeantino)); // Testing horn 1 shifts. 
       fParticleGun->SetParticleMomentumDirection(direction);
//       fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(-13)); // mu + 
       fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("chargedgeantino")); // geantino
       fParticleGun->SetParticleEnergy(8.0*CLHEP::GeV); // explicit, we'll keep re-linking.   
    }
    fParticleGun->GeneratePrimaryVertex(anEvent);
    fCurrentPrimaryNo++;
    
}



G4bool NumiPrimaryGeneratorAction::OpenNtuple(G4String ntupleName)
{

   if(fND->GetDebugLevel() > 0) { G4cout << "NumiRunManager::OpenNtuple() called." << G4endl;}
   
   fCurrentPrimaryNo=0;
   
   G4bool fIsOpen=false;
   fRootFile=new TFile(ntupleName,"READ");
   if (!fRootFile->IsZombie())
   {
      
      if(fND->useMuonInput && fND->useMuonBeam)
      {
         fPrimaryNtuple=(TTree*)(fRootFile->Get("muon"));
         fMuon = new NtpMuon();
         fMuon -> SetTree(fPrimaryNtuple);
      }
      else
      {
         fPrimaryNtuple=(TTree*)(fRootFile->Get("h3"));
      }
      if(!fPrimaryNtuple)
      {
	G4cout<<"NumiPrimaryGeneratorAction: Can't find tree "<< G4endl;
      }
      
      if(fND->useMuonInput && fND->useMuonBeam)
      {
         const G4int entries = fPrimaryNtuple->GetEntries();

         G4cout << "Total Entries in input file = " << entries << G4endl;
         
         if(fND->GetNEvents() > 0 && fND->GetNEvents() < entries)
            fNoOfPrimaries = fND->GetNEvents();
         else   
            fNoOfPrimaries = entries;
         
         fCurrentPrimaryNo=0;
         const G4int NInputParts = fND->GetNInputParts();
         const G4int    NInputPart  = fND->GetNInputPart();
         if(NInputPart > 0 )
         {
            if(NInputParts < NInputPart)
            {
               fNoOfPrimaries = 1;
            }
            else
            {
               const G4int remainder = (fNoOfPrimaries%NInputParts);
               fNoOfPrimaries    = (int)(fNoOfPrimaries/NInputParts);
               
               fCurrentPrimaryNo = fNoOfPrimaries*(NInputPart-1);
               
               if(NInputPart == NInputParts)
                  fNoOfPrimaries += remainder;
               
               fStart = true;

            }
         }
         
      }
      else
      {
         fCurrentPrimaryNo=0;
         G4int entries = fPrimaryNtuple->GetEntries();
         if(fND->GetNEvents() > 0 && fND->GetNEvents() < entries)
            fNoOfPrimaries = fND->GetNEvents();
         else   
            fNoOfPrimaries = entries;
         //fCurrentPrimaryNo=0;
         //fNoOfPrimaries=fPrimaryNtuple->GetEntries();
      }
      
     fIsOpen=true;
   }
   else
   {
      G4cout<<"NumiPrimaryGeneratorAction: Input (fluka/mars) root file doesn't exist"
            <<"   Aborting run"<<G4endl;
      fIsOpen=false;
   }
   
   return fIsOpen;
}

void NumiPrimaryGeneratorAction::CloseNtuple()
{
   if(fND->GetDebugLevel() > 0) { G4cout << "NumiRunManager::CloseNtuple() called." << G4endl;}
      
    fRootFile->Close();
    fCurrentPrimaryNo=0;
}

void NumiPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
   
   
   if(fND->GetDebugLevel() > 1) { G4cout << " NumiPrimaryGeneratorAction::GeneratePrimaries() called." << G4endl;}
   
  //  std::cout<<"*************** anEvent: "<<anEvent->get_eventID()<<" *******************"<<std::endl;
  //G4UImanager* UI=G4UImanager::GetUIpointer();

   
  G4int totNoPrim=fRunManager->GetNumberOfEvents();
  if (totNoPrim>20)
  {
     if(fStart) 
     {
        G4cout<<"Processing " << fNoOfPrimaries << " particles starting with "
              << fCurrentPrimaryNo << " and ending with " << fCurrentPrimaryNo+fNoOfPrimaries-1 << G4endl;
         G4cout<<"Processing particles #: "
                 <<fCurrentPrimaryNo<<" to "<< fCurrentPrimaryNo+(totNoPrim/20) - 1 <<G4endl;
     }
     
     if (fCurrentPrimaryNo%(totNoPrim/20)==0)
           G4cout<<"Processing particles #: "
                 <<fCurrentPrimaryNo<<" to "<< fCurrentPrimaryNo+(totNoPrim/20) - 1 <<G4endl;
     
     fStart = false;
  }
  
  if(fND->useMuonBeam && !(fND->useMuonInput))
  {
     G4double x0 = 0.0;
     G4double y0 = 0.0;
     G4double z0 = fND->DecayPipeLength + fND->TunnelZ0 - 7*mm;// just before the endcap account for DP window
     
     if(fND->GetMuonBeamShape() == "circle")
     {
        x0 = fND->DecayPipeRadius*2.0;
        y0 = fND->DecayPipeRadius*2.0; 
        
        
        // Uniformly distributed circular muon beam 
        while(sqrt(pow(x0,2)+pow(y0,2)) > fND->DecayPipeRadius)
        {
           x0 = 2*(G4UniformRand()-0.5)*fND->DecayPipeRadius;
           y0 = 2*(G4UniformRand()-0.5)*fND->DecayPipeRadius;
        }
     }
     else if(fND->GetMuonBeamShape() == "square")
     {
        //
        //Square beam the fits just inside the decay pipe without
        // producing particles that would immediatly hit rock outside the
        // decay pipe.
        //
        x0 = 2*(G4UniformRand()-0.5)*fND->DecayPipeRadius/sqrt(2.0);
        y0 = 2*(G4UniformRand()-0.5)*fND->DecayPipeRadius/sqrt(2.0);
     }
     else if(fND->GetMuonBeamShape() == "gauss" || fND->GetMuonBeamShape() == "gaussian")
     {
        x0 = fND->DecayPipeRadius*2.0;
        y0 = fND->DecayPipeRadius*2.0;

        G4double xsig = fND->GetGaussBeamXSig();
        G4double ysig = fND->GetGaussBeamYSig();

        //
        //gaussian beam
        //
        while(sqrt(x0*x0+y0*y0) > fND->DecayPipeRadius)
        {
           x0 = G4RandGauss::shoot(0.0, xsig); 
           y0 = G4RandGauss::shoot(0.0, ysig); 
        }
     }

     if(fND->GetMuonBeamZPos() > 0.0)
     {
        z0 = fND->GetMuonBeamZPos();
     }
    //z0 = fND->DecayPipeLength + fND->TunnelZ0 - 1;// just before the endcap
     
     fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
     fParticlePosition=G4ThreeVector(x0, y0, z0);
     
     NumiAnalysis* analysis = NumiAnalysis::getInstance();
     if(!analysis)
     {
        G4cout << "Can't get NumiAnalysis pointer" << G4endl;
     }
     analysis->SetCount(0);
     analysis->SetEntry(fCurrentPrimaryNo);
     analysis->SetMuInAlcFlag(false);
     analysis->FillHadmmNtuple();
     
     fParticleGun->GeneratePrimaryVertex(anEvent);
  }
  else if (fND->useMuonInput && fND->useMuonBeam)
  {
    //
    //Need to create a new Gun each time
    //so Geant v4.9 doesn't complain
    //about momentum not match KE
    //
     if(fParticleGun){ delete fParticleGun; fParticleGun = 0;}
     fParticleGun = new G4ParticleGun(1);
     fParticleGun->SetParticleEnergy(0.0*GeV);

    G4String particleName;

    NumiAnalysis* analysis2 = NumiAnalysis::getInstance();
    analysis2->SetCount(0);
    analysis2->SetEntry(fCurrentPrimaryNo);
    analysis2->SetAlcEdepFlag(false);
    analysis2->SetMuInAlcFlag(false);
    
    fMuon -> Clear();
    fMuon -> GetEntry(fCurrentPrimaryNo);
    G4cout << " " << fCurrentPrimaryNo << ", ";
    
    fevtno = fMuon->evtno;
    fmuweight = fMuon->muweight;

    fMuTParentMomentum = G4ThreeVector(fMuon->tpx*GeV,fMuon->tpy*GeV,fMuon->tpz*GeV);
    fMuTParentPosition = G4ThreeVector(fMuon->tvx*cm,fMuon->tvy*cm,fMuon->tvz*cm);
    fMuParentMomentum = G4ThreeVector(fMuon->pdpx*GeV,fMuon->pdpy*GeV,fMuon->pdpz*GeV);
    fMuParentPosition = G4ThreeVector(fMuon->pdvx*cm,fMuon->pdvy*cm,fMuon->pdvz*cm);
    fMuParentProdPosition = G4ThreeVector(fMuon->ppvx*cm,fMuon->ppvy*cm,fMuon->ppvz*cm);
    ftpptype = fMuon->tptype;
    fnimpwt = fMuon->nimpwt;
    fpptype = fMuon->ptype;
    fppmedium = fMuon->ppmedium;
    fpgen = fMuon->pgen;
        
    for(G4int loc = 0;loc<13;loc++){

    fnuray_px[loc] = fMuon->nuray_px[loc];
    fnuray_py[loc] = fMuon->nuray_py[loc];
    fnuray_pz[loc] = fMuon->nuray_pz[loc];
    fnuray_E[loc] = fMuon->nuray_E[loc];
    fnuray_wgt[loc] = fMuon->nuray_wgt[loc];

    }
    fprotonx = fMuon->protonx;
    fprotony = fMuon->protony;
    fprotonz = fMuon->protonz;

    G4String muon_type;
    G4String muplus = "mu+";
    G4String muminus = "mu-";
    G4String type = NumiParticleCode::AsString(NumiParticleCode::IntToEnum(fpptype));
    if      (type == "kaon+" || type == "pi+") { muon_type = muplus; }
    else if (type == "kaon-" || type == "pi-") { muon_type = muminus; }
    else
    {
       muon_type = muplus;
       G4cout << "Parent is " << type
              << ", not a pi +/- or K +/-. Setting muon to "
              << muon_type << G4endl;
    }

    fptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(muon_type));
    

    //
    //z0 = fND->DecayPipeLength + fND->TunnelZ0 - 1;// just before the endcap this is what Jason uses.
    //z0 =        676681        + 45698.5       - 1 = 722378.5 mm
    //which means the endcap is at 722379.5 mm
    //fMuon->muvz*cm = 722380, so subtract 1mm to get 722379 just to be safe
    //

    //
    //need to account for DP window thinkness so subtract 7mm
    //
    G4double z0 = fMuon->muvz*cm - 7*mm; //this is - 7 mm.
    
    fParticlePosition=G4ThreeVector(fMuon->muvx*cm,fMuon->muvy*cm,z0);
    //fParticlePosition=G4ThreeVector(0.0,0.0,737000.);
    fParticleMomentum=G4ThreeVector(fMuon->mupx*GeV,fMuon->mupy*GeV,fMuon->mupz*GeV);
       
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    fParticleGun->SetParticleDefinition(particleTable->FindParticle(muon_type));
    //G4double mass=particleTable->FindParticle(muon_type)->GetPDGMass();

    NumiAnalysis* analysis = NumiAnalysis::getInstance();
    if(!analysis)
    {
      G4cout << "Can't get NumiAnalysis pointer" << G4endl;
    }
    analysis->FillHadmmNtuple();

    if(fMuon->mupz < 1.0)
    {
      flocalParticleMomentum = G4ThreeVector(0.0,0.0,0.0*GeV);
      //fParticleGun->SetParticleEnergy(sqrt(mass*mass+0.0*0.0*GeV+0.0*0.0*GeV+0.0*GeV*0.0*GeV)-mass);
    }
    else
    {
      flocalParticleMomentum = G4ThreeVector(fMuon->mupx*GeV,fMuon->mupy*GeV,fMuon->mupz*GeV);
      //fParticleGun->SetParticleEnergy(sqrt(mass*mass+fMuon->mupx*GeV*fMuon->mupx*GeV+fMuon->mupy*GeV*fMuon->mupy*GeV+fMuon->mupz*GeV*fMuon->mupz*GeV)-mass);
    }

    fParticleGun->SetParticlePosition(fParticlePosition);
    fParticleGun->SetParticleMomentum(flocalParticleMomentum);
    fParticleGun->GeneratePrimaryVertex(anEvent);
    
  }
  else if (fND->useTestBeam)
  { 
    // Test Beam for comparison with FLUGG
    // Fluka-analagous paramaters defined in input file
    
    G4double x0, y0;
    G4double px, py, pz;
    
    if (outR > 0) {
      G4double r0   = G4UniformRand()*(outR - inR) + inR;
      G4double phi0 = G4UniformRand()*2*M_PI;
      
      x0 = cos(phi0) * r0;
      y0 = sin(phi0) * r0;
    }
    else {
      x0 = 0;
      y0 = 0;
    }
    
    if (spread > 0) {
      p += DoubleRand()*spread/2.;
    }
    
    if (cosx + cosy == 0) {
      G4double ptheta  = sqrt(G4UniformRand())*div/2.;
      G4double pphi    = G4UniformRand()*2*M_PI; 
      px = p * sin(ptheta)*cos(pphi);
      py = p * sin(ptheta)*sin(pphi);
      pz = p * cos(ptheta);
    }
    else {
      px = p * cosx;
      py = p * cosy;
      pz = p * sqrt(1 - cosx*cosx - cosy*cosy);
    }
    
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(0.0); 
    fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
    fParticleGun->SetParticleMomentum(G4ThreeVector(px, py, pz));
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
  else if (fND->useMacro)
  {
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
  else if (fND->useFlukaInput || fND->useMarsInput)
  {
    /*
     Fluka and Mars input variables:
     FLUKA                           MARS                   
     -----------------------------------------------------------------------------------------------
     TYPE                            TYPE                                      - type of particle (see NumiAnalysis::GetParticleName())
     X, Y, Z                         X,Y,Z                                     - particle coordinates
     PX, PY, PZ                      PX, PY, PZ                                - particle momentum
     WEIGHT                          WEIGHT                                    - particle weight
     GENER                           GENER                                     - particle generation
     PROTVX, PROTVY, PROTVZ          PVTXX, PVTXY, PVTXZ                       - proton interaction vertex 
     PROTX, PROTY, PROTZ             PROTX, PROTY, PROTZ                       - proton initial coordinates
     PROTPX, PROTPY, PROTPZ          PROTPX, PROTPY, PROTPZ                    - proton initial momentum
     MOMPX,MOMPY,MOMPZ               PPX, PPY, PPZ                             - ???
     MOMTYPE                         PTYPE                                     - ???
     */

      //
    //Need to create a new Gun each time
    //so Geant v4.9 doesn't complain
    //about momentum not match KE
    //
     if(fParticleGun){ delete fParticleGun; fParticleGun = 0;}
     fParticleGun = new G4ParticleGun(1);
     fParticleGun->SetParticleEnergy(0.0*GeV);
    
    G4double x0,y0,z0,px,py,pz;
    G4String particleName;
    fPrimaryNtuple->GetEntry(fCurrentPrimaryNo);
    
    x0 = fPrimaryNtuple->GetLeaf("x")->GetValue()*cm;
    y0 = fPrimaryNtuple->GetLeaf("y")->GetValue()*cm;
    z0 = fPrimaryNtuple->GetLeaf("z")->GetValue()*cm+fND->TargetZ0+35*cm;
    px = fPrimaryNtuple->GetLeaf("px")->GetValue()*GeV;
    py = fPrimaryNtuple->GetLeaf("py")->GetValue()*GeV;
    pz = fPrimaryNtuple->GetLeaf("pz")->GetValue()*GeV;
    
    fParticlePosition=G4ThreeVector(x0,y0,z0);
    fParticleMomentum=G4ThreeVector(px,py,pz);
    
    fWeight = fPrimaryNtuple->GetLeaf("weight")->GetValue();
    fType = G4int(fPrimaryNtuple->GetLeaf("type")->GetValue());
    fTgen = G4int(fPrimaryNtuple->GetLeaf("gener")->GetValue());
    particleName=NumiParticleCode::AsString(NumiParticleCode::IntToEnum(fType));
    fProtonOrigin   = G4ThreeVector(fPrimaryNtuple->GetLeaf("protx")->GetValue()*cm,
                                    fPrimaryNtuple->GetLeaf("proty")->GetValue()*cm,
                                    fPrimaryNtuple->GetLeaf("protz")->GetValue()*cm);
    fProtonMomentum = G4ThreeVector(fPrimaryNtuple->GetLeaf("protpx")->GetValue()*cm,
                                    fPrimaryNtuple->GetLeaf("protpy")->GetValue()*cm,
                                    fPrimaryNtuple->GetLeaf("protpz")->GetValue()*cm);
    
    if (fND->useMarsInput){
      fProtonIntVertex = G4ThreeVector(fPrimaryNtuple->GetLeaf("pvtxx")->GetValue()*cm,
                                       fPrimaryNtuple->GetLeaf("pvtxy")->GetValue()*cm,
                                       fPrimaryNtuple->GetLeaf("pvtxz")->GetValue()*cm);
    }
    else if (fND->useFlukaInput){
      fProtonIntVertex = G4ThreeVector(fPrimaryNtuple->GetLeaf("protvx")->GetValue()*cm,
                                       fPrimaryNtuple->GetLeaf("protvy")->GetValue()*cm,
                                       fPrimaryNtuple->GetLeaf("protvz")->GetValue()*cm);
    }
    
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    fParticleGun->SetParticleDefinition(particleTable->FindParticle(particleName));
    //G4double mass=particleTable->FindParticle(particleName)->GetPDGMass();
    
    //fParticleGun->SetParticleEnergy(sqrt(mass*mass+px*px+py*py+pz*pz)-mass);
    fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
    fParticleGun->SetParticleMomentum(G4ThreeVector(px,py,pz));
    fParticleGun->GeneratePrimaryVertex(anEvent);
    
  }
  else if (fND->GetDetailedProtonBeam())
  {

     //
     //Need to create a new Gun each time
     //so Geant v4.9 doesn't complain
     //about momentum not match KE
     //
     if(fParticleGun){ delete fParticleGun; fParticleGun = 0;}
     fParticleGun = new G4ParticleGun(1);
     fParticleGun->SetParticleEnergy(0.0*GeV);

     //
     // THIS NEEDS TO BE CHECKED!
     // This is supposed to simulate the detailed proton beam used by NuMI
     // Evidence for the use of this code is in g4numi_flugg/scripts/g4numi_fluka.sh
     // and in fluka_NuMI/target_2_baffle.inp. I don't fully understand what this is doing.
     //The code for this must be in the FLUKA code from their website
     // -Laura
     //
    
     G4double x0, y0;
     G4double z0 = fND->beamPosition[2];
     G4double px, py, pz;

     G4double p      = fND->GetProtonMomentum();
     G4double outR   = fND->GetProton_outR();
     G4double inR    = fND->GetProton_inR();
     G4double div    = fND->GetProtonDivergence();
     G4double spread = fND->GetProtonSpread();
     G4double cosx   = fND->GetProton_cosx();
     G4double cosy   = fND->GetProton_cosy();
    
    if (outR > 0)
    {
      G4double r0   = G4UniformRand()*(outR - inR) + inR;
      G4double phi0 = G4UniformRand()*2*M_PI;
      
      x0 = cos(phi0) * r0;
      y0 = sin(phi0) * r0;
    }
    else
    {
       //
       //I changed this. It doesn't make sense to ever have x=y=0 -Laura
       //
       //x0 = 0;
       //y0 = 0;

       G4double sigmax=fND->beamSigmaX;
       G4double sigmay=fND->beamSigmaY;

       x0 = G4RandGauss::shoot(fND->beamPosition[0],sigmax); // for gaussian beam x
      // x0 = 10*(G4UniformRand() - 0.5)*mm; // for uniform beam x
       y0 = G4RandGauss::shoot(fND->beamPosition[1],sigmay); //for gaussian beam y
       //y0 = 10*(0.35 - G4UniformRand())*mm;// for uniform beam y
       //y0 = 10*(G4UniformRand() - 0.5)*mm;//for uniform beam y
      
 
     }
    
    if (spread > 0)
    {
      p += DoubleRand()*spread/2.;
    }
    
    if (cosx + cosy == 0)
    {
      G4double ptheta  = sqrt(G4UniformRand())*div/2.;
      G4double pphi    = G4UniformRand()*2*M_PI; 
      px = p * sin(ptheta)*cos(pphi);
      py = p * sin(ptheta)*sin(pphi);
      pz = p * cos(ptheta);
    }
    else
    {
      px = p * cosx;
      py = p * cosy;
      pz = p * sqrt(1 - cosx*cosx - cosy*cosy);
    }

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    
    fParticleGun->SetParticleDefinition(particleTable->FindParticle("proton"));
    fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
    fParticleGun->SetParticleMomentum(G4ThreeVector(px, py, pz));
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
  else if (fUseGeantino || fUseMuonGeantino ) {
        this->Geantino(anEvent);
  }
  else
  {
     // If nothing else is set, use a basic proton beam
     G4double x0;
     G4double y0; 
     G4double z0;
     G4double sigmax=fND->beamSigmaX;
     G4double sigmay=fND->beamSigmaY;
    
     //x0 = 10*(G4UniformRand() - 0.5)*mm;//for uniform x
     //y0 = 10*(G4UniformRand() - 0.5)*mm;//for uniform y 
     x0 = G4RandGauss::shoot(fND->beamPosition[0],sigmax);
     y0 = G4RandGauss::shoot(fND->beamPosition[1],sigmay);
     z0 = fND->beamPosition[2];
    
     fProtonOrigin   = G4ThreeVector(x0, y0, z0);
     fProtonMomentum = G4ThreeVector(0, 0, fND->protonMomentum);
     fProtonIntVertex = G4ThreeVector(-9999.,-9999.,-9999.);
     fWeight=1.; //for primary protons set weight and tgen
     fTgen=0;
     
     fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
     fParticleGun->GeneratePrimaryVertex(anEvent);
  }
  
  fCurrentPrimaryNo++;
}


