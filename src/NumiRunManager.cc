//----------------------------------------------------------------------
// NumiRunManager.cc
// $Id: NumiRunManager.cc,v 1.8.4.5 2014/09/16 16:21:41 laliaga Exp $
//----------------------------------------------------------------------

#include "G4RunManager.hh"
#include "NumiRunManager.hh"
#include "NumiDataInput.hh"
#include "TStopwatch.h"
#include "TTime.h"
#include "NumiPrimaryGeneratorAction.hh"

NumiRunManager::NumiRunManager()
  :primaryGeneratorAction(0)
{
   NumiData = NumiDataInput::GetNumiDataInput();
   if(NumiData->GetDebugLevel() > 0)
   {
      std::cout << "NumiRunManager Constructor Called." << std::endl;
   }
}

NumiRunManager::~NumiRunManager()
{
   if(NumiData->GetDebugLevel() > 0)
   {
      std::cout << "NumiRunManager Destructor Called." << std::endl;
   }

}
void NumiRunManager::BeamOn(G4int n_event,const char* macroFile,G4int n_select)
{

   if(NumiData->GetDebugLevel() > 0)
   {
      G4cout << "NumiRunManager::BeamOn() Called." << G4endl;
   }
      
  nEvents = n_event;
  G4bool runOn(true);
  G4bool cond = ConfirmBeamOnCondition();
   if(cond)
     {
       G4cout << "Beam condition on." << G4endl;
       
       //       NumiData->SetRunNumber(macroFile);
       primaryGeneratorAction = (NumiPrimaryGeneratorAction*)(this)->userPrimaryGeneratorAction;
       if(NumiData->useFlukaInput || NumiData->useMarsInput)
       {
	 runOn=primaryGeneratorAction->OpenNtuple(NumiData->GetExtNtupleFileName());
  	 n_event = primaryGeneratorAction->GetNoOfPrimaries();
       }
       else if(NumiData->useMuonBeam && !(NumiData->useMuonInput))
       {   
	 G4cout << "Generating Muon Beam" << G4endl;  	 
	 primaryGeneratorAction->SetMuonBeam();
       }
       else if(NumiData->useMuonBeam && NumiData->useMuonInput)
       {
          G4cout << "Generating Muon Beam from input file" << G4endl;  	 
	 runOn=primaryGeneratorAction->OpenNtuple(NumiData->GetExtNtupleFileName());
  	 n_event = primaryGeneratorAction->GetNoOfPrimaries();
       }
       else if(NumiData->useTestBeam)
       {
	 G4cout << "Test Beam is set" << G4endl;  	 
       }
       else if(NumiData->useMacro)
       {
	 G4cout << "using macro parameters" << G4endl;  	 
       }
       else if(NumiData->GetDetailedProtonBeam())
       {
	 G4cout << "Using the Detailed Proton Beam." << G4endl;
       }
       else
       {
     	 primaryGeneratorAction->SetProtonBeam();
       }

             
       if (runOn)
       {
	 TStopwatch *sWatch=new TStopwatch();
	 sWatch->Start(true);
	 numberOfEventToBeProcessed = n_event;
	 RunInitialization();

         if(NumiData->GetOkToRun())
         {
            if(n_event>0) DoEventLoop(n_event,macroFile,n_select);
         }
	 
	 RunTermination();
	 
	 if(NumiData->useFlukaInput){
	   primaryGeneratorAction->CloseNtuple();
	 }
	 if(NumiData->useMuonInput){
	   primaryGeneratorAction->CloseNtuple();
	 }
	 
	 sWatch->Stop();
	 G4cout <<"Processed "<<n_event<<" particles in ";
	 G4cout.precision(3);
	 G4cout << sWatch->RealTime()
		<< " s" << G4endl;
	 delete sWatch;
       }
     }
}

