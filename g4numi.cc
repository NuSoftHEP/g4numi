
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4DecayTable.hh"
#include "G4KL3DecayChannel.hh"
#include "G4StepLimiter.hh"
#include "G4ParticleTable.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4VEnergyLossProcess.hh"

#include "NumiDetectorConstruction.hh"

// Interaction Physics Lists

//#include "QGSP.hh"
//#include "QGSP_BERT.hh"
//#include "QGSC.hh"
//#include "QBBC.hh"
//#include "FTFC.hh"
#include "FTFP_BERT.hh"
//#include "LHEP.hh"
#ifdef WITH_G4NU
  // non-official factory version in "g4nu" namespace
  #include "G4NuPhysicsLists/G4PhysListFactory.hh"   
#endif
#include "G4VModularPhysicsList.hh"

#include "NumiPrimaryGeneratorAction.hh"
#include "NumiEventAction.hh"
#include "NumiStackingAction.hh"
#include "NumiSteppingAction.hh"
#include "NumiTrackingAction.hh"
#include "NumiRunAction.hh"
#include "NumiRunManager.hh"



#ifdef G4VIS_USE
#include "NumiVisManager.hh"
#endif

#include <cassert>
#include <stdlib.h>

int main(int argc,char** argv)
{
  // Construct the default run manager
  G4RunManager* runManager = new NumiRunManager();

  // set mandatory initialization classes
  runManager->SetUserInitialization(new NumiDetectorConstruction);

  // Initialize Physics Lists
  G4VModularPhysicsList* physicsList = 0;
#ifdef WITH_G4NU
  const char* plname = getenv("PHYSICSLIST");
  if ( ! plname ) {
    G4cout << "g4numi::main() - No PHYSICSLIST env variable set" << G4endl; 
    plname = "FTFP_BERT";  // fall back if user didn't specify
  }
  G4cout << "g4numi::main() - using physics list \"" << plname 
         << "\"" << G4endl;
  g4nu::G4PhysListFactory* factory = new g4nu::G4PhysListFactory();
  physicsList = factory->GetReferencePhysList(plname);
  if ( ! physicsList ) {
    factory->PrintAvailableLists();
    assert(physicsList);
  }
#else
  // old style
  //  QGSP_BERT* physicsList = new QGSP_BERT;
  physicsList = new FTFP_BERT;
#endif 
  runManager->SetUserInitialization(physicsList);

#ifdef G4VIS_USE
  // Visualization, if you choose to have it!
  G4VisManager* visManager = new NumiVisManager;
  visManager->Initialize();
#endif

  // set mandatory user action class
  runManager->SetUserAction(new NumiPrimaryGeneratorAction);

  // set user action classes
  runManager->SetUserAction(new NumiEventAction);
  runManager->SetUserAction(new NumiSteppingAction);
  runManager->SetUserAction(new NumiStackingAction);
  runManager->SetUserAction(new NumiTrackingAction);
  runManager->SetUserAction(new NumiRunAction);

  // Initialize G4 kernel
  runManager->Initialize();

  
  {
     G4ProcessManager * pManager = 0; 
     pManager = G4Electron::Electron()->GetProcessManager();
     if(!pManager) G4cout << "g4numi::main() - Can't get a valid Electron process manager" << G4endl;
     
     //pManager->AddProcess(new NumieIonisation,        -1, 4, -1);
     
  }
  {
     G4ProcessManager * pManager = 0;
     pManager = G4Positron::Positron()->GetProcessManager();
     if(!pManager) G4cout << "g4numi::main() - Can't get a valid Positron process manager" << G4endl;
     
     //pManager->AddProcess(new NumieIonisation,        -1, 4, -1);
  }
  {
     G4ProcessManager * pManager = 0;
     pManager  = G4MuonPlus::MuonPlus()->GetProcessManager();
     if(!pManager) G4cout << "g4numi::main() - Can't get a valid MuPlus process manager" << G4endl;
     
     //pManager->AddProcess(new NumiMuIonisation,       -1, 5, -1);
     
  }
  {
     G4ProcessManager * pManager = 0;
     pManager  = G4MuonMinus::MuonMinus()->GetProcessManager();
     if(!pManager) G4cout << "g4numi::main() - Can't get a valid MuMinus process manager" << G4endl;
     
     //pManager->AddProcess(new NumiMuIonisation,       -1, 5, -1);
    
               
  }

  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();


 // ---> April 2006. D.Jaffe modifications to set K0L,K+,K- branching fractions

  //  K0L branching fractions from Table 3 of hep-ex/0512039
  // "Status of the Cabibbo Angle", E. Blucher et al.,
  // Proceedings of the CKM 2005 Workshop (WG1), UC San Diego, 15-18 March 2005
  // Average takes into account KTeV, KLOE, NA48 results
  // KTeV PRL 93, 181802. PR D70 092006
  // KLOE hep-ex/0408027, PL B632 (2006) 43
  // NA48 PL B602 (2004) 41
  
  // K+ branching fractions from PDG2005 update and section 3.2.1 of  hep-ex/0512039
  // for average Ke3 from BNL E865 (PRL 91, 261802) and NA48.
  // B(Ke3) = 5.14+-0.06 %  hep-ex/0512039
  // B(Ke3) = 4.93+-0.07 % PDG2005 (takes into account BNL and some much older results)

  G4cout<<G4endl;
  G4cout << "Setting correct branching fractions and form factors for "
         << "K0L, K+ and K- decays (MINOS-doc-1786)"<<G4endl;
  
  UI->ApplyCommand("/particle/select kaon0L");           // ----------- K0L
  UI->ApplyCommand("/particle/property/decay/select 0"); //3pi0
  UI->ApplyCommand("/particle/property/decay/br 19.72e-2");
  UI->ApplyCommand("/particle/property/decay/select 1"); //pi-,e+,nue
  UI->ApplyCommand("/particle/property/decay/br 20.20e-2");
  UI->ApplyCommand("/particle/property/decay/select 2"); //pi+,e-,nue
  UI->ApplyCommand("/particle/property/decay/br 20.20e-2");
  UI->ApplyCommand("/particle/property/decay/select 3"); //pi-,mu+,nue
  UI->ApplyCommand("/particle/property/decay/br 13.495e-2");
  UI->ApplyCommand("/particle/property/decay/select 4"); //pi+,mu-,nue
  UI->ApplyCommand("/particle/property/decay/br 13.495e-2");
  UI->ApplyCommand("/particle/property/decay/select 5"); //pi+,pi-,pi0
  UI->ApplyCommand("/particle/property/decay/br 12.53e-2");

  UI->ApplyCommand("/particle/select kaon+");            // ------------- K+
  UI->ApplyCommand("/particle/property/decay/select 0"); //mu,nu
  UI->ApplyCommand("/particle/property/decay/br 63.39e-2");
  UI->ApplyCommand("/particle/property/decay/select 1"); //pi,pi0
  UI->ApplyCommand("/particle/property/decay/br 21.03e-2");
  UI->ApplyCommand("/particle/property/decay/select 2"); //3pi
  UI->ApplyCommand("/particle/property/decay/br 5.59e-2");
  UI->ApplyCommand("/particle/property/decay/select 3"); //pi0,e,nu
  UI->ApplyCommand("/particle/property/decay/br 5.14e-2");
  UI->ApplyCommand("/particle/property/decay/select 4"); //pi0,mu,nu
  UI->ApplyCommand("/particle/property/decay/br 3.30e-2");
  UI->ApplyCommand("/particle/property/decay/select 5"); //pi0,pi0,pi
  UI->ApplyCommand("/particle/property/decay/br 1.757e-2");

  UI->ApplyCommand("/particle/select kaon-");            // ------------- K-
  UI->ApplyCommand("/particle/property/decay/select 0"); //mu,nu
  UI->ApplyCommand("/particle/property/decay/br 63.39e-2");
  UI->ApplyCommand("/particle/property/decay/select 1"); //pi,pi0
  UI->ApplyCommand("/particle/property/decay/br 21.03e-2");
  UI->ApplyCommand("/particle/property/decay/select 2"); //3pi
  UI->ApplyCommand("/particle/property/decay/br 5.59e-2");
  UI->ApplyCommand("/particle/property/decay/select 3"); //pi0,e,nu
  UI->ApplyCommand("/particle/property/decay/br 5.14e-2");
  UI->ApplyCommand("/particle/property/decay/select 4"); //pi0,mu,nu
  UI->ApplyCommand("/particle/property/decay/br 3.30e-2");
  UI->ApplyCommand("/particle/property/decay/select 5"); //pi0,pi0,pi
  UI->ApplyCommand("/particle/property/decay/br 1.757e-2");
  // <--- end of djaffe april06 additions						   

  // ---> April 2006. Adding Davids modifications here instead of changing 
  // G4KL3DecayChannel.cc
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator *ptiter=particleTable->GetIterator();

  G4double lamp = 2.81e-2;
  G4double lam0 = 1.64e-2;
  G4double xi_kp= 12.377 * (lam0 - lamp);
  G4double xi_k0= 11.713 * (lam0 - lamp);
  G4double pLambda=0.;
  G4double pXi0=0.;

  ptiter->reset();
  // G4cout <<G4endl;
  //  G4cout <<" Decay mode \t \t \t \t Lambda \t \t \t Xi0 "<<G4endl;
  while ((*ptiter)()){
    G4ParticleDefinition *particle = ptiter->value();
    if (particle==G4KaonPlus::KaonPlusDefinition()||
	particle==G4KaonMinus::KaonMinusDefinition()){
      G4DecayTable *decayTable=particle->GetDecayTable();
      for (G4int ii=0; ii<decayTable->entries(); ++ii){
	if (decayTable->GetDecayChannel(ii)->GetKinematicsName()=="KL3 Decay"){
	  G4KL3DecayChannel *decayChannel=(G4KL3DecayChannel *)decayTable->GetDecayChannel(ii);

	  //G4cout<<particle->GetParticleName();
	  for (G4int jj=0; jj<decayChannel->GetNumberOfDaughters();++jj){
	    //   if (jj==0) G4cout << " -> ";
	    //   else G4cout << " + ";
	    //   G4cout<<decayChannel->GetDaughter(jj)->GetParticleName();
	    if (decayChannel->GetDaughter(jj)==G4Positron::PositronDefinition()){
	      pLambda=lamp;
	      pXi0=xi_kp;
	    }
	    if (decayChannel->GetDaughter(jj)==G4Electron::ElectronDefinition()){
	      pLambda=lamp;
	      pXi0=xi_kp;
	    }
	    if (decayChannel->GetDaughter(jj)==G4MuonPlus::MuonPlusDefinition()){
	      pLambda=lamp;
	      pXi0=xi_kp;
	    }
	    if (decayChannel->GetDaughter(jj)==G4MuonMinus::MuonMinusDefinition()){
	      pLambda=lamp;
	      pXi0=xi_kp;
	    }
	  }//loop over daughters
	  // G4cout<<"\t \t" << decayChannel->GetDalitzParameterLambda() << " -> " << pLambda<< "  "
	  //	<<"\t"<< decayChannel->GetDalitzParameterXi() << " -> " << pXi0 <<G4endl ;
	  decayChannel->SetDalitzParameter(pLambda,pXi0);
	}//check if its KL3 Decay
      }//loop over decay channels
    }
    if (particle==G4KaonZeroLong::KaonZeroLongDefinition()){
      G4DecayTable *decayTable=particle->GetDecayTable();
      for (G4int ii=0; ii<decayTable->entries(); ++ii){
	if (decayTable->GetDecayChannel(ii)->GetKinematicsName()=="KL3 Decay"){
	  G4KL3DecayChannel *decayChannel=(G4KL3DecayChannel *)decayTable->GetDecayChannel(ii);
	  //	  G4cout<<particle->GetParticleName();
	  for (G4int jj=0; jj<decayChannel->GetNumberOfDaughters();++jj){
	    // if (jj==0) G4cout << " -> ";
	    // else G4cout << " + ";
	    // G4cout<<decayChannel->GetDaughter(jj)->GetParticleName();
	    if (decayChannel->GetDaughter(jj)==G4Positron::PositronDefinition()||
		decayChannel->GetDaughter(jj)==G4Electron::ElectronDefinition()){
	      pLambda=lamp;
	      pXi0=xi_k0;
	    }
	    if (decayChannel->GetDaughter(jj)==G4MuonPlus::MuonPlusDefinition()||
		decayChannel->GetDaughter(jj)==G4MuonMinus::MuonMinusDefinition()){
	      pLambda=lamp;
	      pXi0=xi_k0;
	    }
	  }//loop over daughters
	  //	  G4cout<<"\t \t" << decayChannel->GetDalitzParameterLambda() << " -> " << pLambda<< "  "
	  //	<<"\t"<< decayChannel->GetDalitzParameterXi() << " -> " << pXi0 <<G4endl ;

	  decayChannel->SetDalitzParameter(pLambda,pXi0);

	}//check if its KL3 Decay
      }
    } 
  }//loop over particles
  // done with kaon form factors

 
  // Setting the maximum step size to 1 cm
  G4ParticleTable* ptbl = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator* piter = ptbl->GetIterator();
  G4StepLimiter* slim = new G4StepLimiter("StepLimiter");    
  
  piter->reset();
  while ( (*piter)() ) {
    G4ParticleDefinition* pdef = piter->value();
    G4ProcessManager* pmgr = pdef->GetProcessManager();
    
    // add user limit processes for steps and special cuts
    if ( pmgr ) {      
      pmgr->AddProcess( slim, -1, -1, 3);
    }
  }		
  
  if(argc==1)
  // Define (G)UI terminal for interactive mode  
  { 
    G4UIsession * session = 0;

    // G4UIterminal is a (dumb) terminal.
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);      
#else
      session = new G4UIterminal();
#endif    

    session->SessionStart();
    delete session;
  }
  else  // Batch mode
  { 
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    G4cout << "Executing " << command+fileName << G4endl;
    UI->ApplyCommand(command+fileName);
  }

#ifdef G4VIS_USE
  delete visManager;
#endif

#  // job termination
  G4cout<<"Delete RunManager"<<G4endl;
  delete runManager;
  return 0;
}


