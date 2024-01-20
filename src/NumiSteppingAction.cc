//----------------------------------------------------------------------
// NumiSteppingAction.cc
// $Id: NumiSteppingAction.cc,v 1.16.4.12 2017/11/02 21:47:34 lebrun Exp $
//----------------------------------------------------------------------

//C++
#include <string>

#include "G4ProcessManager.hh"
#include "NumiSteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4UnitsTable.hh"
#include "NumiTrajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "NumiTrackInformation.hh"
#include "NumiDataInput.hh"
#include "NumiAnalysis.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Event.hh"
#include "NumiTrackInformation.hh"
#include "NumiAnalysis.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4EventManager.hh"
#include "NumiEventAction.hh"
#include "NumiRunManager.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"

#include "G4Electron.hh"//prachi

#include "NumiDetectorConstruction.hh"//prachi

NumiSteppingAction::NumiSteppingAction()
   :fPrintAllSteps(false),
    fPrintSplitting(false),
    fPrintMuAlcove1(false),
    fPrintMuAlcove2(false),
    fPrintMuAlcove3(false),
    fPrintDeltaAlcove1(false),
    fPrintDeltaAlcove2(false),
    fPrintDeltaAlcove3(false),
    fPrintProcesses(false),
    fPrintTouchableHistory(false),
    fGeantinoStudyName("none"),
    fKeyVolumeNameFrom("none"),
    fKeyVolumeNameTo("none")

{
  NDI = NumiDataInput::GetNumiDataInput();
  pRunManager=(NumiRunManager*)NumiRunManager::GetRunManager();

  fEvtIdPrevious = -5555;
}

NumiSteppingAction::~NumiSteppingAction()
{
   if(fOutStudyGeantino.is_open())  fOutStudyGeantino.close(); 
}


void NumiSteppingAction::UserSteppingAction(const G4Step * theStep)
{
   if(NDI->GetDebugLevel() > 3)
   {
      G4int evtno = pRunManager->GetCurrentEvent()->GetEventID();
      std::cout << "Event " << evtno << ": NumiSteppingAction::UserSteppingAction() Called." << std::endl;
   }     

   NumiAnalysis* analysis = NumiAnalysis::getInstance();
   analysis->FillBXDRAW(theStep);
   
   G4Track * theTrack = theStep->GetTrack();
//
//  Suspect that we can not handle very long track (i.e., loopers) at high magnetic field, near Z= 3150 
//  
   G4StepPoint* prePtr =theStep->GetPreStepPoint(); 
   G4ParticleDefinition * particleDefinition = theTrack->GetDefinition();
   if ((particleDefinition->GetPDGEncoding() != 2212) && 
       (theTrack->GetTrackLength() > 6000.0*CLHEP::mm) && (prePtr != 0) 
         && (prePtr->GetPosition()[2] < 3300.*CLHEP::mm)) {
        G4ThreeVector pMom = prePtr->GetMomentum();
        if (pMom[2] < 500.0) {
           std::cerr << " Suspect looper, long track, L= " << theTrack->GetTrackLength() << " at XYZ = " 
               << prePtr->GetPosition()[0] << " / " << prePtr->GetPosition()[1] 
               << " / " << prePtr->GetPosition()[2] << std::endl
               << " ....... Momentum " << pMom[0] << " / " << pMom[1] << " / " << pMom[2] << " Id " << 
               particleDefinition->GetPDGEncoding() << " ...killing this track " << std::endl;
            theTrack->SetTrackStatus(fStopAndKill);
            return;
     }
  } 
   G4String particleType = particleDefinition -> GetParticleType();
   NumiRunManager* aRunManager = reinterpret_cast<NumiRunManager*>(NumiRunManager::GetRunManager());
   
   
   if  ((fGeantinoStudyName.find("Mag") != std::string::npos) 
       && (aRunManager->GetCurrentEvent()->GetEventID() < 5)) 
        this->dumpStepCheckVolumeAndFields(theStep); 

   
   
   // Check if the Pi+, Pi-, K+, K-, K0L, mu+ or mu- decayed and set Ndecay code:
   // 1  K0L -> nu_e pi- e+
   // 2  K0L -> anti_nu_e pi+ e-
   // 3  K0L -> nu_mu pi- mu+
   // 4  K0L -> anti_nu_mu pi+ mu-
   // 5  K+  -> nu_mu mu+
   // 6  K+  -> nu_e pi0 e+
   // 7  K+  -> nu_mu pi0 mu+
   // 8  K-  -> anti_nu_mu mu-
   // 9  K-  -> anti_nu_e pi0 e-
   // 10 K-  -> anti_nu_mu pi0 mu-
   // 11 mu+ -> anti_nu_mu nu_e e+
   // 12 mu- -> nu_mu anti_nu_e e-
   // 13 pi+ -> nu_mu mu+
   // 14 pi- -> anti_nu_mu mu-
   
   
   
   //*****************************************************************
   //*****************************************************************
   //This is for hadron production simulation
   //
   //
   //
   /*
   if(NDI->createH1TrackingPlane){
      if( (theStep->GetPreStepPoint()->GetPhysicalVolume() != NULL) && (theStep->GetPostStepPoint()->GetPhysicalVolume() != NULL)){
         G4String prevolname = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
         G4String postvolname = theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();	 
         if (this->EscapingTarget(prevolname, postvolname)) {
            NumiAnalysis* analysis=NumiAnalysis::getInstance();
            analysis->FillH1Ntuple(*theTrack);
            theTrack->SetTrackStatus(fStopAndKill);
         }      
      }
   }
*/
  if(NDI->createH1TrackingPlane){
    //---tracking planes
    TrkPlnH1Logical = ((NumiDetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetHorn1PlaneLogical();

    if (TrkPlnH1Logical) {

        G4StepPoint *postStep = theStep->GetPostStepPoint();
        if (postStep->GetPhysicalVolume()) {
            G4LogicalVolume *postStepVolume = postStep->GetPhysicalVolume()->GetLogicalVolume();
            if (postStepVolume) {
                if (postStepVolume == TrkPlnH1Logical) {
                    CheckInTrackingDetectorH1Plane(theStep);
                }
            }
        }
    } else {
        std::cout << "H1 Tracking Plane Logical Volume does not exist" << std::endl; 
    }
} else {
    std::cout << "H1 Tracking Plane output is not enabled" << std::endl; 
}

   //
   //
   //*****************************************************************
   //*****************************************************************




           
  
  if (!(NDI->useMuonBeam) && (NDI->GetKillTracking() && theTrack->GetKineticEnergy() < NDI->GetKillTrackingThreshold() ) &&
      (particleDefinition != G4NeutrinoE::NeutrinoEDefinition())&&
      (particleDefinition != G4NeutrinoMu::NeutrinoMuDefinition()) &&
      (particleDefinition != G4NeutrinoTau::NeutrinoTauDefinition()) &&
      (particleDefinition != G4AntiNeutrinoE::AntiNeutrinoEDefinition()) &&
      (particleDefinition != G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()) &&
      (particleDefinition != G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()))
    {
      theTrack->SetTrackStatus(fStopAndKill);
    }

  
  //*****************************************************************
  //*****************************************************************
  //This is for muon monitor stuff 
  //
  //

   if(fPrintAllSteps)
   {
      if(analysis->GetEntry() == 107 &&
         (theTrack -> GetTrackID() == 1828)
         )
      {
         G4cout << "   Parent muon entry # = " << analysis->GetEntry()
                << "   track ID = " << theTrack->GetTrackID()
                << "   track weight = " << theTrack->GetWeight()
                << "   post vol = " << theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()
                << "   pre vol = " << theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
         
      }
   }
  
   if(NDI->useMuonBeam)
   {
      
      G4double newWeight = 1.0;
      
      G4bool reWeightedDelta = false;
      
      G4EventManager* eventManager    = G4EventManager::GetEventManager();
      G4StackManager* stackManager    = eventManager -> GetStackManager();
      G4TrackingManager* trackManager = eventManager -> GetTrackingManager();
      G4SteppingManager* stepManager  = trackManager -> GetSteppingManager();
      
      NumiEvtAct = (NumiEventAction*)(eventManager -> GetUserEventAction());
      
      if(NDI->GetSimDRays() && NDI->GetReWeightDeltas())
      {
         
         
         G4int nSplitDeltas = NDI->GetNSplitDeltas();
         
         if(nSplitDeltas < 2)
            G4cout << "NumiSteppingAction::UserSteppingAction - PROBLEM: requesting to split deltas but number of spilts is < 2" << G4endl;
         
         if(theStep->GetPreStepPoint()->GetPhysicalVolume() != NULL &&
            theStep->GetPostStepPoint()->GetPhysicalVolume() != NULL)
         {
            
            std::string preStepPointName  = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
            std::string postStepPointName = theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
              if((particleDefinition == G4Electron::ElectronDefinition() || 
                particleDefinition == G4Positron::PositronDefinition() ||
                particleDefinition == G4Gamma::Gamma()) &&
               preStepPointName.find("MuCell") == std::string::npos &&
               (postStepPointName.find("MuMonAlcvShot_") != std::string::npos ||
                postStepPointName.find("MuMonAlcv_") != std::string::npos ||
                postStepPointName.find("MuMon_") != std::string::npos) &&
               theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL &&
               theStep->GetPostStepPoint()->GetProcessDefinedStep() -> GetProcessName() == "Transportation" &&
               !(theTrack->GetWeight() < 1.0))
            {
               if(fPrintSplitting)
               {
                  G4cout << "Spilling " << particleDefinition -> GetParticleName() << " : " 
                         << " parent muon entry # = " << analysis->GetEntry()
                         << " track id = " << theTrack ->GetTrackID()
                         << " parent id = " << theTrack ->GetParentID() 
                         << " post step point = " << postStepPointName << G4endl;
               }
               
               const G4Track * theTempTrack = theStep->GetTrack();
               
               newWeight = newWeight/(double)nSplitDeltas;
               
               reWeightedDelta = true;
               
               for(G4int isplit = 1; isplit < nSplitDeltas; ++isplit)
               {
                  //
                  //create a copy of the dynamic particle
                 //
                 //G4DynamicParticle* newDP = new G4DynamicParticle(theTrack->GetDynamicParticle());
                  
                 //
                 //create new track
                 //
                 //G4Track* newTrack = new G4Track(newDP, theTrack->GetGlobalTime(), theTrack->GetPosition());   
                  G4Track* newTrack = new G4Track(*theTempTrack);
                 //
                 //   Touchable handle is copied to keep the pointer
                 //
                 newTrack->SetTouchableHandle(theTempTrack->GetTouchableHandle());
                 newTrack->SetCreatorProcess(theTempTrack->GetCreatorProcess());
                 
                 //
                 //set track ID to be the SAME!!!
                 //set parent ID to the parent of theTempTrack
              //
                 newTrack->SetParentID( theTempTrack->GetParentID() );
                 //newTrack->SetTrackID( theTempTrack->GetTrackID() );
                 NumiEvtAct -> IncrementDRayTrackID();
                 newTrack->SetTrackID( NumiEvtAct -> GetDRayTrackID() );
                 
                 newTrack->SetWeight(newWeight);
                 
                 
                 if(fPrintSplitting)
                 {
                    G4cout << " created spilt delta ray with trackID = " << newTrack->GetTrackID()
                        << " and weight = " << newTrack->GetWeight() << G4endl;
                 }  
                 //(trackManager -> GetTrack()) -> SetWeight(0.666666665);
                 
                 
                 
                 //
                 //I could probably set the weight of theTempTrack
                 //here because if the postStepPoint = MuMonAlcv then
                 //there is no chance that theTempTrack could make it into the alcove on this loop.
                 //But to be safe flag it to set the weight at the end of the loop.
                 //
              
                 
                 G4int nstep = theTempTrack->GetCurrentStepNumber();
                 for(G4int istep = 0; istep < nstep; ++istep)
                    newTrack->IncrementCurrentStepNumber();
                 
                 
                 //G4Step* newStep = new G4Step(theStep);
                 //newTrack->SetStep(newStep);
                 //G4TrackVector *trackVec = new G4TrackVector();
                 //trackVec -> push_back(newTrack);
                 //eventManager -> StackTracks(trackVec);
                 
                 stackManager -> PushOneTrack(newTrack);
              }
           }
         }
      }//end if sim drays and reweight deltas
      
           
     
     if ( (NDI->GetKillTracking() && theTrack->GetKineticEnergy() < NDI->GetKillTrackingThreshold()) )
     {
	theTrack->SetTrackStatus(fStopAndKill);
     }
     
     if( particleDefinition != G4MuonPlus::MuonPlusDefinition()
         && particleDefinition != G4MuonMinus::MuonMinusDefinition() 
         && particleDefinition != G4Gamma::GammaDefinition() 
         && particleDefinition != G4Electron::ElectronDefinition()
         && particleDefinition != G4Positron::PositronDefinition())      
     {
	//NumiAnalysis* analysis = NumiAnalysis::getInstance();
	//G4cout << "Event " << analysis->GetEntry() << ", Process = "
        //<< theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
	//G4cout<< "Event " << analysis->GetEntry() << ", KE = " << theTrack->GetKineticEnergy()
        //<< ", threshold = " << NDI->GetKillTrackingThreshold()  << ", type = "
        //<< particleDefinition << ", zpos = " << theTrack->GetPosition()[2] << G4endl; 
	
	theTrack->SetTrackStatus(fStopAndKill);
        if( !(NDI->GetSimDRays()) && particleDefinition != G4MuonPlus::MuonPlusDefinition()
            && particleDefinition != G4MuonMinus::MuonMinusDefinition() )      
        {
           theTrack->SetTrackStatus(fStopAndKill);
        }
     }
     else if( !(NDI->GetSimDRays()) && theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL 
              && theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()=="Decay")
     {
        //NumiAnalysis* analysis = NumiAnalysis::getInstance();
	
        //G4cout<< "Event " << analysis->GetEntry() << ": Particle decayed" << ", zpos = "
        //<< theTrack->GetPosition()[2] << G4endl; 
	theTrack->SetTrackStatus(fStopAndKill);
     }
     
     /* if(theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="Transportation" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="muIoni" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="muBrems" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="msc" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="muPairProd")*/
     {
        
     }

     if(fPrintProcesses && particleDefinition == G4Gamma::Gamma())
     {
        G4ProcessManager* pm = particleDefinition -> GetProcessManager();
        G4cout << "particle = " << particleDefinition -> GetParticleName() << G4endl;
        G4ProcessVector* pv = pm -> GetProcessList();
        G4cout << " process list: " ;
        for(int i = 0; i< pv->size(); ++i)
        {
           G4VProcess* proc = (*pv)[i];
           //proc -> SetVerboseLevel(2);
           G4cout <<  proc->GetProcessName() << ", ";
        }
        G4cout << G4endl;
        for(int i = 0; i< pv->size(); ++i)
        {
           G4VProcess* proc = (*pv)[i];
           G4cout <<  proc->GetProcessTypeName(proc->GetProcessType()) << ", ";
        }
        G4cout << G4endl;
        G4ProcessVector* pv_along = pm -> GetAlongStepProcessVector();
        G4cout << " along step process list: " ;
        for(int i = 0; i< pv_along->size(); ++i)
        {
           G4VProcess* proc = (*pv_along)[i];
           G4cout <<  proc->GetProcessName() << ", ";
        }
        G4cout << G4endl;


        if(theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL )
           G4cout << "Process = " << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
     }
     


     
     if( !(NDI->GetSimDRays()) &&  (!(NDI->GetMaterialSigma() < 0) && !(NDI->GetMaterialSigma() > 0)) )
     {
        if(particleDefinition == G4MuonPlus::MuonPlusDefinition()
           || particleDefinition == G4MuonMinus::MuonMinusDefinition())
        {
           //
           //To see if the particle has just past the point that is 0.5m upstream into the rock before
           //monitor. Record the particle momentum in order to apply delta ray corrections. --LL
           //
           //for alc0; info will only be recorded the first time this executes
           //
           if( theTrack->GetPosition()[2] > 729811.) 
           {
              NumiAnalysis* analysis = NumiAnalysis::getInstance();
              analysis->FillAlcEdepInfo(*theTrack, 0);
           }
           //
           //for alc1; info will only be recorded the first time this executes
           //
           if( theTrack->GetPosition()[2] > 750941.) 
           {
              NumiAnalysis* analysis = NumiAnalysis::getInstance();
              analysis->FillAlcEdepInfo(*theTrack, 1);
           }
           //
           //for alc2; info will only be recorded the first time this executes
           //
           if( theTrack->GetPosition()[2] > 772062.) 
           {
              NumiAnalysis* analysis = NumiAnalysis::getInstance();
              analysis->FillAlcEdepInfo(*theTrack, 2);
           }
        }
     }
     // catch the muons at z position
     if (NDI->createHadmmNtuple)
     {
         if(theStep->GetPreStepPoint()->GetPosition()[2]<52000.
           && theStep->GetPostStepPoint()->GetPosition()[2]>52000.
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 4, 0);
        }

         else if(theStep->GetPreStepPoint()->GetPosition()[2]<271528.8
           && theStep->GetPostStepPoint()->GetPosition()[2]>271528.8
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 5, 0);
        }

         else if(theStep->GetPreStepPoint()->GetPosition()[2]<496819.2
           && theStep->GetPostStepPoint()->GetPosition()[2]>496819.2
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 6, 0);
        }
        
        else if(theStep->GetPreStepPoint()->GetPosition()[2]<721000.
           && theStep->GetPostStepPoint()->GetPosition()[2]>721000.
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 7, 0);
        }

        else if(theStep->GetPreStepPoint()->GetPosition()[2]<737906.
           && theStep->GetPostStepPoint()->GetPosition()[2]>737906.
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 8, 0);
        }

         else if(theStep->GetPreStepPoint()->GetPosition()[2]<753000.
           && theStep->GetPostStepPoint()->GetPosition()[2]>753000.
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 9, 0);
        }

         else if(theStep->GetPreStepPoint()->GetPosition()[2]<773400.
           && theStep->GetPostStepPoint()->GetPosition()[2]>773400.
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 10, 0);
        }

     } 
     //
     // Checks to see whether the particle has entered the Hadron
     // or muon monitors, and if so calls the NumiAnalysis class
     // to record the particle properties through the monitors.-DJK
     //
     if (theStep->GetPostStepPoint()->GetPhysicalVolume()==NULL
         && theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() != "ROCK")
     {
        G4cerr << "Problem in NumiSteppingAction::UserSteppingAction - PostStepVolume is Null"
               <<" PreStepVolume is " << theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
     }
     
     if (NDI->createHadmmNtuple && theStep->GetPostStepPoint()->GetPhysicalVolume()!=NULL)
     {
        
        if(theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "PVHadMon"
           && theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "ConcShield"
           && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
               || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
        {
           //NumiAnalysis* analysis = NumiAnalysis::getInstance();
           //analysis->FillHadmmNtuple(*theTrack, 4, 0);
        }
        //else if( theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "MuCell" ||
        //         (theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "MuCell"
        //         && theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "ChamberLayer") )
        else if( theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "MuCell")
        {
           
           
           G4TouchableHistory* theTouchable = (G4TouchableHistory*)(theStep->GetPostStepPoint()->GetTouchable());
           std::string CurrVol = theTouchable->GetVolume(0) -> GetName();

           
           
           int cellNum = -99;
           std::string Alcove = "";

           cellNum = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
           

           Alcove  = theTouchable -> GetVolume(1) -> GetName();
           
           


           //////////////////////////////////////////////////////
           

           if(fPrintTouchableHistory)
           {
              G4int histDepth = theTouchable->GetHistoryDepth();       // depth of enclosion
              G4cout << G4endl;
              
              G4cout << "parent muon entry # = " << analysis->GetEntry()
                     << " particle: " << particleDefinition->GetParticleName()
                     << " track id = " << theTrack ->GetTrackID()
                     << " track weight = " << theTrack ->GetWeight() 
                     << "   alcove = " << Alcove  << "  cellNum = " << cellNum << G4endl;
              G4cout << "   edep = " <<   theStep->GetTotalEnergyDeposit() << G4endl;
              G4cout << "   post vol = " << theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()
                     << "   pre vol = " << theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
              G4cout << "    depth = " << histDepth;
              for(int i= 0; i <= histDepth; ++i)
              {
                 G4cout << " " << theTouchable->GetVolume(i) -> GetName();
              }
              G4cout << G4endl;
              
           }
           
           ////////////////////////////////////////////////////
        


           if(Alcove.find("MuMon") == std::string::npos)
           {
              G4cerr << "Problem in NumiSteppingAction::UserSteppingAction - Haveing a problem"
                     << " getting alcove from Touchable" << G4endl;
           }

           NumiAnalysis* analysis = NumiAnalysis::getInstance();
///muons
           if(particleDefinition == G4MuonPlus::MuonPlusDefinition()
             || particleDefinition == G4MuonMinus::MuonMinusDefinition())
           {
              //
              //The first time through the preStep point has to be MuCellLayer
              //when it is exiting we don't care as far as the mmp and mmpos is
              //concerned because we only fill thes the first time it is in MuCell
              //
              
              if(Alcove.find("_0") != std::string::npos)
              {
                 analysis->FillHadmmNtuple(*theTrack, 
                                           0, 
                                           cellNum);
                 
                 analysis->FillEdep(theStep->GetTotalEnergyDeposit(), particleDefinition, 0, 1, -99999.0,
                                    cellNum, theTrack->GetTrackID(), theTrack->GetWeight());

                 if(fPrintMuAlcove1)
                 {
                    /*
                    G4ProcessManager* pm = particleDefinition -> GetProcessManager();
                    G4cout << "particle = " << particleDefinition -> GetParticleName() << G4endl;
                    G4ProcessVector* pv = pm -> GetProcessList();
                    G4cout << " process list: " ;
                    for(int i = 0; i< pv->size(); ++i)
                    {
                       G4VProcess* proc = (*pv)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    G4ProcessVector* pv_along = pm -> GetAlongStepProcessVector();
                    G4cout << " along step process list: " ;
                    for(int i = 0; i< pv_along->size(); ++i)
                    {
                       G4VProcess* proc = (*pv_along)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    */
                    
                    G4cout << "parent muon entry # = " << analysis->GetEntry()
                           << " Alcove = " << Alcove
                           << " track id = " << theTrack ->GetTrackID()
                           << " track weight = " << theTrack ->GetWeight() << G4endl;
                    G4cout << " post process = " ;
                    if(theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL)
                       G4cout << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " pre process = " ;
                    if(theStep->GetPreStepPoint()->GetProcessDefinedStep() != NULL )   
                       G4cout << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " energy dep = " << theStep->GetTotalEnergyDeposit()
                           << " cell = " << cellNum << G4endl;
                 }
              }
              else if(Alcove.find("_1") != std::string::npos)
              {
                 analysis->FillHadmmNtuple(*theTrack,
                                           1,
                                           cellNum);
                 
                 analysis->FillEdep(theStep->GetTotalEnergyDeposit(), particleDefinition, 1, 1, -99999.0,
                                    cellNum, theTrack->GetTrackID(), theTrack->GetWeight());

                 if(fPrintMuAlcove2)
                 {
                    /*
                    G4ProcessManager* pm = particleDefinition -> GetProcessManager();
                    G4cout << "particle = " << particleDefinition -> GetParticleName() << G4endl;
                    G4ProcessVector* pv = pm -> GetProcessList();
                    G4cout << " process list: " ;
                    for(int i = 0; i< pv->size(); ++i)
                    {
                       G4VProcess* proc = (*pv)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    G4ProcessVector* pv_along = pm -> GetAlongStepProcessVector();
                    G4cout << " along step process list: " ;
                    for(int i = 0; i< pv_along->size(); ++i)
                    {
                       G4VProcess* proc = (*pv_along)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    */
                    G4cout << "parent muon entry # = " << analysis->GetEntry()
                           << " Alcove = " << Alcove
                           << " track id = " << theTrack ->GetTrackID()
                           << " track weight = " << theTrack ->GetWeight() << G4endl;
                    G4cout << " post process = " ;
                    if(theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL)
                       G4cout << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " pre process = " ;
                    if(theStep->GetPreStepPoint()->GetProcessDefinedStep() != NULL )   
                       G4cout << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " energy dep = " << theStep->GetTotalEnergyDeposit()
                           << " cell = " << cellNum << G4endl;
                 }
              }
              else if(Alcove.find("_2") != std::string::npos)
              {
                 analysis->FillHadmmNtuple(*theTrack,
                                           2,
                                           cellNum);

                 analysis->FillEdep(theStep->GetTotalEnergyDeposit(), particleDefinition, 2, 1, -99999.0,
                                    cellNum, theTrack->GetTrackID(), theTrack->GetWeight());

                 if(fPrintMuAlcove3)
                 {
                    /*
                    G4ProcessManager* pm = particleDefinition -> GetProcessManager();
                    G4cout << "particle = " << particleDefinition -> GetParticleName() << G4endl;
                    G4ProcessVector* pv = pm -> GetProcessList();
                    G4cout << " process list: " ;
                    for(int i = 0; i< pv->size(); ++i)
                    {
                       G4VProcess* proc = (*pv)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    G4ProcessVector* pv_along = pm -> GetAlongStepProcessVector();
                    G4cout << " along step process list: " ;
                    for(int i = 0; i< pv_along->size(); ++i)
                    {
                       G4VProcess* proc = (*pv_along)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    */
                    G4cout << "parent muon entry # = " << analysis->GetEntry()
                           << " Alcove = " << Alcove
                           << " track id = " << theTrack ->GetTrackID()
                           << " track weight = " << theTrack ->GetWeight() << G4endl;
                    G4cout << " post process = " ;
                    if(theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL)
                       G4cout << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " pre process = " ;
                    if(theStep->GetPreStepPoint()->GetProcessDefinedStep() != NULL )   
                       G4cout << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " energy dep = " << theStep->GetTotalEnergyDeposit()
                           << " cell = " << cellNum << G4endl;
                 }
              }
              else
              {
                 G4cout << "NumiSteppingAction::UserSteppingAction - PROBLEM: CAN'T GET ALCOVE FROM " << Alcove << G4endl;
              }
           }
///deltas
           if(particleDefinition == G4Gamma::Gamma())
           {
              G4double edep = theStep->GetTotalEnergyDeposit();
              if(edep > 0.0)
                 G4cout << "NumiSteppingAction::UserSteppingAction - Gamma with edep = " << edep
                        << " and weight = " << theTrack->GetWeight() << " in alcove " << Alcove << G4endl;

           }
           
              if(particleDefinition == G4Electron::ElectronDefinition() ||  
                 particleDefinition == G4Positron::PositronDefinition())
           {

              if(reWeightedDelta)
              {
                 G4cout << "NumiSteppingAction::UserSteppingAction - PROBLEM: reweighted this delta on this loop "
                        << " and it is making it into a pixel. This could be a problem. I don't think it should happen." << G4endl;
              }
              
              
              G4int IntExt = NumiEvtAct -> GetIntExt(theTrack);
             
              if(Alcove.find("_0") != std::string::npos)
              {
                 analysis->FillEdep(theStep->GetTotalEnergyDeposit(), particleDefinition, 0, IntExt,
                                    NumiEvtAct->GetZPos(theTrack), cellNum, theTrack->GetTrackID(), theTrack->GetWeight());

                 if(fPrintDeltaAlcove1)
                 {
                    /*
                    G4ProcessManager* pm = particleDefinition -> GetProcessManager();
                    G4cout << "particle = " << particleDefinition -> GetParticleName() << G4endl;
                    G4ProcessVector* pv = pm -> GetProcessList();
                    G4cout << " process list: " ;
                    for(int i = 0; i< pv->size(); ++i)
                    {
                       G4VProcess* proc = (*pv)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    G4ProcessVector* pv_along = pm -> GetAlongStepProcessVector();
                    G4cout << " along step process list: " ;
                    for(int i = 0; i< pv_along->size(); ++i)
                    {
                       G4VProcess* proc = (*pv_along)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    */
                    G4cout << "parent muon entry # = " << analysis->GetEntry()
                           << " Alcove = " << Alcove
                           << " track id = " << theTrack ->GetTrackID()
                           << " parent id = " << theTrack ->GetParentID()
                           << " track weight = " << theTrack ->GetWeight() << G4endl;
                    G4cout << " post process = " ;
                    if(theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL)
                       G4cout << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " pre process = " ;
                    if(theStep->GetPreStepPoint()->GetProcessDefinedStep() != NULL )   
                       G4cout << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " energy dep = " << theStep->GetTotalEnergyDeposit()
                           << " cell = " << cellNum << G4endl;
                         
                 }

              }
              else if(Alcove.find("_1") != std::string::npos)
              {
                 analysis->FillEdep(theStep->GetTotalEnergyDeposit(), particleDefinition, 1, IntExt,
                                    NumiEvtAct->GetZPos(theTrack), cellNum, theTrack->GetTrackID(), theTrack->GetWeight());
                          
                 if(fPrintDeltaAlcove2)
                 {
                    /*
                    G4ProcessManager* pm = particleDefinition -> GetProcessManager();
                    G4cout << "particle = " << particleDefinition -> GetParticleName() << G4endl;
                    G4ProcessVector* pv = pm -> GetProcessList();
                    G4cout << " process list: " ;
                    for(int i = 0; i< pv->size(); ++i)
                    {
                       G4VProcess* proc = (*pv)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    G4ProcessVector* pv_along = pm -> GetAlongStepProcessVector();
                    G4cout << " along step process list: " ;
                    for(int i = 0; i< pv_along->size(); ++i)
                    {
                       G4VProcess* proc = (*pv_along)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    */
                    G4cout << "parent muon entry # = " << analysis->GetEntry()
                           << " Alcove = " << Alcove
                           << " track id = " << theTrack ->GetTrackID()
                           << " parent id = " << theTrack ->GetParentID()
                           << " track weight = " << theTrack ->GetWeight() << G4endl;
                    G4cout << " post process = " ;
                    if(theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL)
                       G4cout << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " pre process = " ;
                    if(theStep->GetPreStepPoint()->GetProcessDefinedStep() != NULL )   
                       G4cout << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " energy dep = " << theStep->GetTotalEnergyDeposit()
                           << " cell = " << cellNum << G4endl;
                 }
              }
              else if(Alcove.find("_2") != std::string::npos)
              {
                 analysis->FillEdep(theStep->GetTotalEnergyDeposit(), particleDefinition, 2, IntExt,
                                    NumiEvtAct->GetZPos(theTrack), cellNum, theTrack->GetTrackID(), theTrack->GetWeight());

                 if(fPrintDeltaAlcove3)
                 {
                    /*
                    G4ProcessManager* pm = particleDefinition -> GetProcessManager();
                    G4cout << "particle = " << particleDefinition -> GetParticleName() << G4endl;
                    G4ProcessVector* pv = pm -> GetProcessList();
                    G4cout << " process list: " ;
                    for(int i = 0; i< pv->size(); ++i)
                    {
                       G4VProcess* proc = (*pv)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    G4ProcessVector* pv_along = pm -> GetAlongStepProcessVector();
                    G4cout << " along step process list: " ;
                    for(int i = 0; i< pv_along->size(); ++i)
                    {
                       G4VProcess* proc = (*pv_along)[i];
                       G4cout <<  proc->GetProcessName() << ", ";
                    }
                    G4cout << G4endl;
                    */
                    G4cout << "parent muon entry # = " << analysis->GetEntry()
                           << " Alcove = " << Alcove
                           << " track id = " << theTrack ->GetTrackID()
                           << " parent id = " << theTrack ->GetParentID()
                           << " track weight = " << theTrack ->GetWeight() << G4endl;
                    G4cout << " post process = " ;
                    if(theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL)
                       G4cout << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " pre process = " ;
                    if(theStep->GetPreStepPoint()->GetProcessDefinedStep() != NULL )   
                       G4cout << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
                    G4cout << " energy dep = " << theStep->GetTotalEnergyDeposit()
                           << " cell = " << cellNum << G4endl;
                 }
              }
              else
              {
                 G4cout << "NumiSteppingAction::UserSteppingAction - PROBLEM: CAN'T GET ALCOVE FROM " << Alcove << G4endl;
              }
           }

        }
        else if((theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "ROCK"
                 && theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "MuMonAlcvShot_2_Down"))
        {
           theTrack->SetTrackStatus(fStopAndKill);   
        }
     }

     //
     //for spiltting deltas. see above
     //
     if(reWeightedDelta) (stepManager -> GetfPostStepPoint()) -> SetWeight(newWeight);
  }//end if(NDI->useMuonBeam)
   //
  //
  //*****************************************************************
  //*****************************************************************

  
  //*****************************************************************
  //*****************************************************************
  //This is for the muon monitor Absorber background simulation
  //
  if(NDI->GetSimAbsBkg())
  {
     if (theStep->GetPostStepPoint()->GetPhysicalVolume() != NULL)
     {
        if(theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "HadronAbsorber" &&
           theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "ConcShield")
        {
           if(particleType != "lepton")
           {
              NumiAnalysis* analysis = NumiAnalysis::getInstance();
              analysis -> FillAbsorberBkgrdNtuple(*theTrack);
           }
           theTrack->SetTrackStatus(fStopAndKill);   
        }
     }
  }
  //
  //
  //*****************************************************************
  //*****************************************************************
  //
  // Geantino Analysis, P.L. July 2014
  if (fOutStudyGeantino.is_open()) {
       if (fGeantinoStudyName.find("Absorb") != std::string::npos) StudyAbsorption(theStep);
       if (fGeantinoStudyName.find("Propa") != std::string::npos) StudyPropagation(theStep);
       if (fGeantinoStudyName.find("PropCO") != std::string::npos) StudyCheckOverlap(theStep);
       if (fGeantinoStudyName.find("BFieldMu") != std::string::npos) StudyBFieldWithMuons(theStep);
  }
  //================================
  //=======for Raytracing===========
  //--------------------------------
  if(NDI->raytracing || NDI->createZpNtuple){
     for(G4int in = 0; in<(NDI->NZpoint);in++)//---Jasmine Added
     {
        if((theStep->GetPostStepPoint()->GetPosition()[2]>=NDI->Zpoint[in]&&
            theStep->GetPreStepPoint()->GetPosition()[2]<NDI->Zpoint[in]))
        {
           NumiAnalysis* analysis=NumiAnalysis::getInstance();
           analysis->FillZpNtuple(*theTrack,in);
        }
     }
  }
  //================================
  //--------------------------------

  if (theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL)
  {
     G4int decay_code=0;
     if (theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()=="Decay")
     {
	G4int nSecAtRest = fpSteppingManager->GetfN2ndariesAtRestDoIt();
	G4int nSecAlong  = fpSteppingManager->GetfN2ndariesAlongStepDoIt();
	G4int nSecPost   = fpSteppingManager->GetfN2ndariesPostStepDoIt();
	G4int nSecTotal  = nSecAtRest+nSecAlong+nSecPost;
	G4TrackVector* secVec = fpSteppingManager->GetfSecondary();
	
	if (particleDefinition==G4PionPlus::PionPlusDefinition())
        {
           for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
           {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
                 decay_code=13;
           }
	}
	if (particleDefinition==G4PionMinus::PionMinusDefinition())
        {
           for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
           {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
                 decay_code=14;
           }
	}

	if (particleDefinition==G4KaonPlus::KaonPlusDefinition())
        {
           for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
           {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
              {if (nSecTotal==2) decay_code=5;
		if (nSecTotal==3) decay_code=7;}
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_e")
                 decay_code=6;
           }
	}
	if (particleDefinition==G4KaonMinus::KaonMinusDefinition())
        {
           for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
           {
              if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
	      {if (nSecTotal==2) decay_code=8;
                 if (nSecTotal==3) decay_code=10;}
              if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_e")
                 decay_code=9;
           }
	}
	if (particleDefinition==G4KaonZeroLong::KaonZeroLongDefinition())
        {
           for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	    {
               if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_e")
                  decay_code=1;
               if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_e")
                  decay_code=2;
               if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
                  decay_code=3;
               if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
                  decay_code=4;	    
	    }
	}
	if (particleDefinition==G4MuonPlus::MuonPlusDefinition())
        {
           for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
           {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
                 decay_code=11;	
           }
	}
	if (particleDefinition==G4MuonMinus::MuonMinusDefinition())
        {
           for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
           {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
                 decay_code=12;	
           }
        }
	
	NumiTrackInformation* oldinfo=(NumiTrackInformation*)(theTrack->GetUserInformation()); 
	if (oldinfo!=0)
        {
           oldinfo->SetDecayCode(decay_code);                                                      
	  theTrack->SetUserInformation(oldinfo); 
	}
	else
        {
           NumiTrackInformation* newinfo=new NumiTrackInformation(); 
	  newinfo->SetDecayCode(decay_code);                                                       
	  theTrack->SetUserInformation(newinfo); 
	}
     }
  }

      // Find the secondaries produced by the current step and store
      // the particle momentum when producing these secondaries.
  G4int nSecAtRest = fpSteppingManager->GetfN2ndariesAtRestDoIt();
  G4int nSecAlong  = fpSteppingManager->GetfN2ndariesAlongStepDoIt();
  G4int nSecPost   = fpSteppingManager->GetfN2ndariesPostStepDoIt();
  G4int nSecTotal  = nSecAtRest+nSecAlong+nSecPost;
  G4TrackVector* secVec = fpSteppingManager->GetfSecondary();

  if(nSecTotal>0) {
      for(size_t lp1=(*secVec).size()-nSecTotal; lp1<(*secVec).size(); ++lp1) {
          G4Track* secTrack = (*secVec)[lp1];
          G4ParticleDefinition* def = secTrack->GetDefinition();
              // Showering particles (e+/-,gamma) are not interesting for neutrino production,
              // skip them
        if (def == G4Electron::Electron() || def == G4Positron::Positron() ||
              def == G4Gamma::Gamma()) continue;
          G4ThreeVector mom = theStep->GetPreStepPoint()->GetMomentum();
          if (!secTrack->GetUserInformation()) {
              NumiTrackInformation* trackInformation = new NumiTrackInformation;
              trackInformation->SetParentMomentumAtThisProduction(mom);
              secTrack->SetUserInformation(trackInformation);
          } else {
              NumiTrackInformation* trackInformation
                  = dynamic_cast<NumiTrackInformation*>(secTrack->GetUserInformation());
              trackInformation->SetParentMomentumAtThisProduction(mom);
          }
          
      }
  }

}

//prachi
void NumiSteppingAction::CheckInTrackingDetectorH1Plane(const G4Step *theStep)
{
  if ((theStep->GetPreStepPoint() == 0) || (theStep->GetPostStepPoint() == 0)) {
    return;
  }

  if (theStep->GetPreStepPoint()->GetPhysicalVolume() == NULL) {
   // std::cout << "(part 2)PreStepPoint's Physical Volume is null" << std::endl; //ps
    return;
  }

  if (theStep->GetPreStepPoint()->GetPhysicalVolume() &&
      theStep->GetPostStepPoint()->GetPhysicalVolume()) {
   // std::cout << "(part 2)Both Pre and Post Step Points have Physical Volumes" << std::endl; //ps
    G4LogicalVolume *preStepVolume =
      theStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();
    G4LogicalVolume *postStepVolume =
      theStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume();
    if (postStepVolume == TrkPlnH1Logical) {
      //std::cout << "(part 2)PostStepPoint's Logical Volume is the H1 Tracking Plane" << std::endl; // ps
      //std::cout<<"Printing variable= postStepVolume"<<postStepVolume<<std::endl;
      if (preStepVolume != TrkPlnH1Logical) {
        //std::cout << "If PostStepVolume is the H1 Tracking plane and PreStepPoint's Logical Volume is not the H1 Tracking Plane then this is true." << std::endl; // ps
        NumiAnalysis* analysis = NumiAnalysis::getInstance();
        // analysis->FillTrackingPlaneH1Data(*theTrack, newtrajectory);
        //std::cout << "Filling H1 Tracking Plane Ntuples" << std::endl; // ps
        //analysis->FillTrackingPlaneH1Data(*theStep);
         analysis->FillH1Ntuple(*theStep);
      }
    }
  }
}

//end prachi



void NumiSteppingAction::StudyAbsorption(const G4Step * theStep) {
//
//make sure we are dealing with a geantino, or a mu, to include lengthening of step due to curling in 
// B Field
//
// July 2014 : P.L. changes his mind, do this for pions. only.. 

   G4Track * theTrack = theStep->GetTrack();
   const double eTrack = theTrack->GetTotalEnergy();
//   if (((theTrack->GetParticleDefinition()->GetParticleName()).find("geantino") == std::string::npos) && (
//        ((theTrack->GetParticleDefinition()->GetParticleName()).find("mu+") == std::string::npos ))) return; //for v4.9.4 and above. 

   if (((theTrack->GetDefinition()->GetPDGEncoding())) != 211) return;
 	
   G4StepPoint* prePtr = theStep->GetPreStepPoint();
   if (prePtr == 0) return;
/*
   if ( theTrack->GetNextVolume() == 0 ) {
       fOutStudyGeantino << " " << pRunManager->GetCurrentEvent()->GetEventID(); 
        for (size_t k=0; k!=3; k++) fOutStudyGeantino << " " << prePtr->GetPosition()[k];
	fOutStudyGeantino << " " << totalAbsDecayChan << " " <<  totalAbsHorn1Neck 
	          << " " << totalAbsHorn2Entr;
	fOutStudyGeantino << " " << waterAbsDecayChan << " " <<  waterAbsHorn1Neck 
	          << " " << waterAbsHorn2Entr << " " <<  alumAbsHorn2Entr << std::endl;
		  return;
   }		  
*/
   //
   // I set the position of the geantino production vertex at Z=0.;
   //
//   G4LogicalVolume *volPre = prePtr->GetPhysicalVolume()->GetLogicalVolume();
   if (fEvtIdPrevious  != pRunManager->GetCurrentEvent()->GetEventID() ) { 
//     std::cerr << " Evt id " << 
//           pRunManager->GetCurrentEvent()->GetEventID() <<
//	      " Starting point, z = " << prePtr->GetPosition()[2] << std::endl;
     fTotalAbsDecayChan= 0.;
     fTotalAbsHorn1Neck=0.;
     fTotalAbsHorn2Entr=0.;
     fWaterAbsDecayChan= 0.;
     fWaterAbsHorn1Neck=0.;
     fWaterAbsHorn2Entr=0.;
     fAlumAbsHorn2Entr=0.;
     fGoneThroughTarget = false;
     fGoneThroughHorn1Neck = false;
     fGoneThroughHorn2Entr = false;
     fEvtIdPrevious = pRunManager->GetCurrentEvent()->GetEventID();
     return;
   } 

   if(theStep->GetPreStepPoint()->GetPhysicalVolume() == NULL) return;
   const double ll = theStep->GetStepLength();
   G4StepPoint* postPtr = theStep->GetPostStepPoint();
/*
   if (postPtr == NULL) {
       fOutStudyGeantino << " " << pRunManager->GetCurrentEvent()->GetEventID(); 
        for (size_t k=0; k!=3; k++) fOutStudyGeantino << " " << prePtr->GetPosition()[k];
	fOutStudyGeantino << " " << totalAbsDecayChan << " " <<  totalAbsHorn1Neck 
	          << " " << totalAbsHorn2Entr;
	fOutStudyGeantino << " " << waterAbsDecayChan << " " <<  waterAbsHorn1Neck 
	          << " " << waterAbsHorn2Entr << " " <<  alumAbsHorn2Entr << std::endl;
		  return;
   } 		  
*/
   if (postPtr == NULL) return;
   G4VPhysicalVolume *physVol = postPtr->GetPhysicalVolume();
   std::string vName(physVol->GetName());
   G4Material *material = theTrack->GetMaterial();
    
   if (pRunManager->GetCurrentEvent()->GetEventID() < 20) {
      const double r = std::sqrt(postPtr->GetPosition()[0]*postPtr->GetPosition()[0] + 
                                 postPtr->GetPosition()[1]*postPtr->GetPosition()[1]); 
      const double t = r/std::abs(postPtr->GetPosition()[2] + 515.25); // ZOrigin = -515.25
      // debugging only valid for Zorigin of -515.
      std::cerr << " r = " << r << " theta " << t <<  " z = " << postPtr->GetPosition()[2] << 
	      " In " << vName << " material " << material->GetName()
	      << " InterLength " << material->GetNuclearInterLength() << std::endl;  
   }
   if (vName.find("AlTube2LV") != std::string::npos) {
       fGoneThroughTarget=true;
       return;
   }
   if (!fGoneThroughTarget) return;
   if (postPtr->GetPosition()[2] > 500.) fGoneThroughHorn1Neck=true; // approximate... 
   if (postPtr->GetPosition()[2] > 6560.) fGoneThroughHorn2Entr=true; //truly approximate. 
   if (ll < 1.0e-10) return; 

   fTotalAbsDecayChan += ll/material->GetNuclearInterLength(); 
   if (vName.find("ICW") != std::string::npos) fWaterAbsDecayChan += ll/material->GetNuclearInterLength(); 
   if (!fGoneThroughHorn1Neck) {
     fTotalAbsHorn1Neck += ll/material->GetNuclearInterLength(); 
     if (vName.find("ICW") != std::string::npos) fWaterAbsHorn1Neck += ll/material->GetNuclearInterLength(); 
   }
   if (!fGoneThroughHorn2Entr) {
     fTotalAbsHorn2Entr += ll/material->GetNuclearInterLength();
//     if (theTrack->GetTrackLength() < (6000.0*mm)) 
//       std::cerr << " trackLength = " << theTrack->GetTrackLength() << " Z = " << postPtr->GetPosition()[2] << 
//         " Abs L " << totalAbsHorn2Entr << std::endl;
     
     if (vName.find("ICW") != std::string::npos) {
        fWaterAbsHorn2Entr += ll/material->GetNuclearInterLength(); 
     } else {
       if ((vName.find("Horn1") != std::string::npos) && (material->GetName().find("Alumin") != std::string::npos))
         fAlumAbsHorn2Entr += ll/material->GetNuclearInterLength(); 
      }
   }
   if (postPtr->GetPosition()[2] > 12100.) {
        fOutStudyGeantino << " " << pRunManager->GetCurrentEvent()->GetEventID(); 
        for (size_t k=0; k!=2; k++) fOutStudyGeantino << " " << postPtr->GetPosition()[k];
	fOutStudyGeantino << " " << eTrack;
	fOutStudyGeantino << " " << fTotalAbsDecayChan << " " <<  fTotalAbsHorn1Neck 
	          << " " << fTotalAbsHorn2Entr;
	fOutStudyGeantino << " " << fWaterAbsDecayChan << " " <<  fWaterAbsHorn1Neck 
	          << " " << fWaterAbsHorn2Entr << " " <<  fAlumAbsHorn2Entr << std::endl;
        theTrack->SetTrackStatus(fStopAndKill);
	return;
   } 
    
}
void NumiSteppingAction::StudyPropagation(const G4Step * theStep) {
//
//make sure we are dealing with a geantino... 
//
   G4Track * theTrack = theStep->GetTrack();
//   if ((theTrack->GetParticleDefinition()->GetParticleName()).find("geantino") == std::string::npos) return; v4.9.4 and beyond
   if ((theTrack->GetDefinition()->GetParticleName()).find("geantino") == std::string::npos) return;
   G4StepPoint* prePtr = theStep->GetPreStepPoint();
   if (prePtr == 0) return;
   G4StepPoint* postPtr = theStep->GetPostStepPoint();
   if (postPtr == 0) return;
   if (theStep->GetStepLength() < 0.1*mm) return;
   G4LogicalVolume *volPost = postPtr->GetPhysicalVolume()->GetLogicalVolume();
   G4LogicalVolume *volPre = prePtr->GetPhysicalVolume()->GetLogicalVolume();
   std::string volNamePost(volPost->GetName());
   std::string volNamePre(volPre->GetName());
   if ((volNamePost.find(fKeyVolumeNameTo.c_str()) != std::string::npos) || 
      (volNamePre.find(fKeyVolumeNameFrom.c_str()) != std::string::npos)) {
       std::cout << " NumiSteppingAction::StudyPropagation, critical volume " << std::endl 
	  << ".. from " << volNamePre << " to " << volNamePost 
	             << " detected at " <<  prePtr->GetPosition() << " going to " << postPtr->GetPosition() << std::endl;
       const G4VTouchable *preHist = prePtr->GetTouchable();
       const G4VTouchable *postHist = postPtr->GetTouchable();
       const G4int nDepthPre = preHist->GetHistoryDepth();
       const G4int nDepthPost = postHist->GetHistoryDepth();
       std::cerr << " ....  History depth for pre point " <<  nDepthPre << " Post " << nDepthPost << std::endl;
       for (int k=0; k!=  std::max(nDepthPre, nDepthPost); k++) {
         if ((k <nDepthPre) && (k <nDepthPost))
	     std::cerr << " ............. Translation at depth, pre ... " << preHist->GetTranslation(k) 
	               << " Post " <<  postHist->GetTranslation(k) <<   std::endl;
	 else if (k <nDepthPre) 	       
	     std::cerr << " ..............Translation at depth, pre ... " << preHist->GetTranslation(k) << std::endl;
	 else  if (k <nDepthPost)     
	     std::cerr << " ..............Translation at depth, post ... " << postHist->GetTranslation(k) << std::endl;
       }
       std::cerr << " ------------------------------------------- " << std::endl;	     
   }   
   if (volPre->GetMaterial()->GetName() == volPost->GetMaterial()->GetName()) return;
   fOutStudyGeantino << " " << pRunManager->GetCurrentEvent()->GetEventID(); 
   for (int k=0; k != 3; k++) fOutStudyGeantino << " " << prePtr->GetPosition()[k];
   for (int k=0; k != 3; k++) fOutStudyGeantino << " " << postPtr->GetPosition()[k];
   fOutStudyGeantino << " " << postPtr->GetPosition()[2];
   fOutStudyGeantino << " " << theStep->GetStepLength();
   fOutStudyGeantino << " " << volPre->GetMaterial()->GetName();
   fOutStudyGeantino << " " << volPost->GetMaterial()->GetName();
   fOutStudyGeantino << std::endl;
   G4String vName(volPre->GetName());
//   if (vName.find("DecayPipe") !=  std::string::npos) {
//        theTrack->SetTrackStatus(fStopAndKill);
//    }
}
void NumiSteppingAction::StudyCheckOverlap(const G4Step * theStep) {
//
//make sure we are dealing with a geantino... 
//
   G4Track * theTrack = theStep->GetTrack();
   // Skip this cut for X-checking the muon-genatinos 
//   if ((theTrack->GetParticleDefinition()->GetParticleName()).find("geantino") == std::string::npos) return;
   if( theTrack->GetNextVolume() == 0 ) {
        if (pRunManager->GetCurrentEvent()->GetEventID() < 3) 
	   std::cout << " Out of world with track length  = " << theTrack->GetTrackLength()  << std::endl; 
       return;
   }
   G4StepPoint* prePtr = theStep->GetPreStepPoint();
   if (prePtr == 0) return;
   G4StepPoint* postPtr = theStep->GetPostStepPoint();
   if (postPtr == 0) {
        if (pRunManager->GetCurrentEvent()->GetEventID() < 3) 
	   std::cout << " No Post Point, Last step at Z = " << prePtr->GetPosition()[2] << " r " <<            
	      std::sqrt(prePtr->GetPosition()[0]*prePtr->GetPosition()[0] +
	         prePtr->GetPosition()[1]*prePtr->GetPosition()[1]) << std::endl; 
 
       return;
   }
   if (postPtr->GetPhysicalVolume() == 0) {
        if (pRunManager->GetCurrentEvent()->GetEventID() < 3) 
	   std::cout << " No Post Point Volume Last step at Z = " << prePtr->GetPosition()[2] << " r " <<            
	      std::sqrt(prePtr->GetPosition()[0]*prePtr->GetPosition()[0] +
	         prePtr->GetPosition()[1]*prePtr->GetPosition()[1]) << std::endl; 
   
    return;
   }
   if (prePtr->GetPhysicalVolume() == 0) return;
   G4LogicalVolume *volPost = postPtr->GetPhysicalVolume()->GetLogicalVolume();
   G4LogicalVolume *volPre = prePtr->GetPhysicalVolume()->GetLogicalVolume();
   if (volPre == 0) return;
   if (volPost == 0) return;
  std::string volNamePost(volPost->GetName());
   std::string volNamePre(volPre->GetName());
   if (pRunManager->GetCurrentEvent()->GetEventID() < 3) 
     std::cout << " at Z = " << prePtr->GetPosition()[2] << " r " << 
           std::sqrt(prePtr->GetPosition()[0]*prePtr->GetPosition()[0] +
	         prePtr->GetPosition()[1]*prePtr->GetPosition()[1]) 
	   << ", " << volNamePre  << " to " << postPtr->GetPosition() << ", " << volNamePost
	    << " Pre Material "  << volPre->GetMaterial()->GetName() << std::endl;  
//   if (((volNamePost.find(fKeyVolumeName.c_str()) != std::string::npos) || 
//      (volNamePre.find(fKeyVolumeName.c_str()) != std::string::npos)) &&
//      ( (volNamePost.find(fKeyVolumeNameTo.c_str()) != std::string::npos) || 
//      (volNamePre.find(fKeyVolumeNameTo.c_str()) != std::string::npos))) {
   if ( (volNamePre.find(fKeyVolumeNameFrom.c_str()) != std::string::npos) &&
        (volNamePost.find(fKeyVolumeNameTo.c_str()) != std::string::npos)) {
     fOutStudyGeantino << " " << pRunManager->GetCurrentEvent()->GetEventID(); 
     for (int k=0; k != 3; k++) fOutStudyGeantino << " " << prePtr->GetPosition()[k];
     for (int k=0; k != 3; k++) fOutStudyGeantino << " " << postPtr->GetPosition()[k];
     fOutStudyGeantino << " " << theStep->GetStepLength();
     fOutStudyGeantino << " " << volPre->GetMaterial()->GetName();
     fOutStudyGeantino << " " << volPost->GetMaterial()->GetName();
     fOutStudyGeantino << std::endl;
  }
}
void NumiSteppingAction::StudyBFieldWithMuons(const G4Step * theStep) {
    const G4Event* evtTmp = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    const int eId = evtTmp->GetEventID();
    G4StepPoint* prePtr = theStep->GetPreStepPoint();
    if (prePtr == 0) return;
    G4StepPoint* postPtr = theStep->GetPostStepPoint();
    if (postPtr == 0) return;
    if (postPtr->GetPosition()[2] > 15000.) return;
    G4LogicalVolume *volPost = postPtr->GetPhysicalVolume()->GetLogicalVolume();
    G4LogicalVolume *volPre = prePtr->GetPhysicalVolume()->GetLogicalVolume();
    fOutStudyGeantino << " " << eId; 
    for (int k=0; k != 3; k++) fOutStudyGeantino << " " << prePtr->GetPosition()[k]; 
    const G4DynamicParticle *tr = theStep->GetTrack()->GetDynamicParticle();
    for (int k=0; k != 3; k++) fOutStudyGeantino << " " << tr->GetMomentum()[k];   
    fOutStudyGeantino << " " << volPre->GetMaterial()->GetName();
    fOutStudyGeantino << " " << volPost->GetMaterial()->GetName();
    fOutStudyGeantino << std::endl;
    // Flag the end of Horn1, so we can document.. 
    if (std::abs(prePtr->GetPosition()[2] - 3340.70) < 1.0) {
      std::cerr << " Exiting Horn1, PreName " << volPre->GetName() 
                << " Material " << volPre->GetMaterial()->GetName() << " Post " 
		<< volPost->GetName() << " Material " << volPost->GetMaterial()->GetName() << std::endl; 
    }
}
//
// To support MIPP data re-weighting..
//
bool NumiSteppingAction::EscapingTarget(const G4String &preVolName, const G4String &postVolName) const {

   NumiDataInput* myNumiData = NumiDataInput::GetNumiDataInput();
   G4String myHorn1Config =  myNumiData->GetHornConfig();

   if (myHorn1Config.contains("le") || myHorn1Config.contains("LE")){
      return (preVolName.contains("TGTExit") && postVolName.contains("TargetMother"));
   } else if (myHorn1Config.contains("me") || myHorn1Config.contains("ME")) {
     // More complicated geometry for the off-axis configuration.. 
      return ((preVolName.contains("InsideLEMod") && postVolName.contains("TargetMother")) || // Top, Left/right side, Downstream end  
              (preVolName.contains("TargetFin") && postVolName.contains("TargetBot")));
   } else {
     G4Exception("NumiSteppingAction::EscapingTarget","NumiSteppingAction::EscapingTarget",
                   RunMustBeAborted, "Unknown target configuration, Ntuple will be incomplete...  "); 
   }
   return false;	         

}
void NumiSteppingAction::dumpStepCheckVolumeAndFields(const G4Step * theStep) {

  const NumiRunManager* aRunManager = (NumiRunManager*) NumiRunManager::GetRunManager();
  int idEvt = aRunManager->GetCurrentEvent()->GetEventID();
  if (fEvtIdPrevious != idEvt) {
      if (fOutStudyGeantino.is_open()) fOutStudyGeantino.close();
      std::ostringstream fNameStrStr; fNameStrStr << "./StepSudiesMagn_Evt" << idEvt << ".txt";
      std::string fNameStr(fNameStrStr.str());
      fOutStudyGeantino.open(fNameStr.c_str());
      fOutStudyGeantino 
       << " x0 y0 z0 bx0 by0 bz0 x1 y1 z1 bx1 by1 bz1 xp0 yp0 xp1 yp1 dpt vName0 vName1 " << std::endl;
      fEvtIdPrevious = idEvt;
  }
  G4StepPoint* prePtr = theStep->GetPreStepPoint();
  if (theStep->GetPreStepPoint()->GetPhysicalVolume() == 0) return;
  if (prePtr == 0) return;
  G4Track * theTrack = theStep->GetTrack();
  if ( theTrack->GetNextVolume() == 0 ) {
    if (fOutStudyGeantino.is_open()) fOutStudyGeantino.close();
    return;
  }
  G4StepPoint* postPtr = theStep->GetPostStepPoint();
  if (postPtr == 0) {
    if (fOutStudyGeantino.is_open()) fOutStudyGeantino.close();
    return;
  } 
  for (size_t k=0; k !=3; k++) fOutStudyGeantino << " " << prePtr->GetPosition()[k];
  G4FieldManager *aFieldMgrPre = prePtr->GetPhysicalVolume()->GetLogicalVolume()->GetFieldManager();
  if (aFieldMgrPre == 0) {
   fOutStudyGeantino << " 0. 0. 0. ";
  } else {
    G4MagneticField *aField = (G4MagneticField *) aFieldMgrPre->GetDetectorField();
    if ( aField == 0) {
       fOutStudyGeantino << " 0. 0. 0. ";
    } else {
       double xxx[3]; for (size_t k=0; k!=3; k++) xxx[k] =  prePtr->GetPosition()[k];
       double bfxxx[3]; 
       aField->GetFieldValue(xxx, bfxxx); 
       fOutStudyGeantino << " " << bfxxx[0]/CLHEP::tesla << " " 
                         << bfxxx[1]/CLHEP::tesla << " " << bfxxx[2]/CLHEP::tesla;
    }
  }
  for (size_t k=0; k !=3; k++) fOutStudyGeantino << " " << postPtr->GetPosition()[k];
  G4FieldManager *aFieldMgrpost = postPtr->GetPhysicalVolume()->GetLogicalVolume()->GetFieldManager();
  if (aFieldMgrpost == 0) {
   fOutStudyGeantino << " 0. 0. 0. ";
  } else {
    G4MagneticField *aField = (G4MagneticField *) aFieldMgrpost->GetDetectorField();
    if ( aField == 0) {
       fOutStudyGeantino << " 0. 0. 0. ";
    } else {
       double xxx[3]; for (size_t k=0; k!=3; k++) xxx[k] =  postPtr->GetPosition()[k];
       double bfxxx[3]; 
       aField->GetFieldValue(xxx, bfxxx); 
       fOutStudyGeantino << " " << bfxxx[0]/CLHEP::tesla << " " 
                         << bfxxx[1]/CLHEP::tesla << " " << bfxxx[2]/CLHEP::tesla;
    }
  }
  G4ThreeVector postVec = postPtr->GetMomentum();
  G4ThreeVector preVec = prePtr->GetMomentum();
  fOutStudyGeantino << " " << preVec[0]/preVec[2];
  fOutStudyGeantino << " " << preVec[1]/preVec[2];
  fOutStudyGeantino << " " << postVec[0]/postVec[2];
  fOutStudyGeantino << " " << postVec[1]/postVec[2];
  const double ptPre = std::sqrt(preVec[0]*preVec[0] + preVec[1]*preVec[1])/CLHEP::GeV;
  const double ptPost = std::sqrt(postVec[0]*postVec[0] + postVec[1]*postVec[1])/CLHEP::GeV;
  fOutStudyGeantino << " " << ptPost - ptPre;
  fOutStudyGeantino << " "  << theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  fOutStudyGeantino << " "  << theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
  fOutStudyGeantino << " " << std::endl;
}
