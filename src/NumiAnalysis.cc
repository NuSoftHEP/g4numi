//----------------------------------------------------------------------
// NumiAnalysis.cc
//
// $Id: NumiAnalysis.cc,v 1.26.2.4 2009/09/24 16:43:38 martens Exp $
//----------------------------------------------------------------------

#include <vector>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <map.h>

//Root 
#include <TSystem.h>        // ROOT head file for a generic interface to the OS
#include <TStopwatch.h>     // ROOT head file for stopwatch for real and cpu time
#include <TFile.h>          
#include <TTree.h>

//GEANT4 
#include "globals.hh"
#include "G4ios.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"
#include "G4ThreeVector.hh"
#include "G4TrajectoryContainer.hh"
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4Run.hh"

//g4numi 
#include "data_t.hh"
#include "hadmmtuple_t.hh"
#include "zptuple_t.hh" // for raytracing
#include "NumiParticleCode.hh"
#include "NumiAnalysis.hh"
#include "NumiTrackInformation.hh"
#include "NumiPrimaryGeneratorAction.hh"
#include "NumiDataInput.hh"
#include "NumiNuWeight.hh"

using namespace std;

NumiAnalysis* NumiAnalysis::instance = 0;

NumiAnalysis::NumiAnalysis()
{
  NumiData = NumiDataInput::GetNumiDataInput();
#ifdef G4ANALYSIS_USE
#endif

  g4data = new data_t();
  g4hmmdata = new hadmmtuple_t();
  g4zpdata = new zptuple_t();
  g4hmmdata->Clear();

  fcount = 0;
  fentry = 0;
  fAlcEdep_called.push_back(false);
  fAlcEdep_called.push_back(false);
  fAlcEdep_called.push_back(false);

  code[-13]   = 10;
  code[13]    = 11;
  code[111]   = 23;
  code[211]   = 13;
  code[-211]  = 14;
  code[130]   = 12;
  code[321]   = 15;
  code[-321]  = 16;
  code[2112]  = 8;
  code[2212]  = 1;
  code[-2212] = 2;
  code[310]   = 19;
  code[3122]  = 17;
  code[3222]  = 21;
  code[3212]  = 22;
  code[3112]  = 20;
}

NumiAnalysis::~NumiAnalysis()
{ 
#ifdef G4ANALYSIS_USE
  // delete things
#endif
}

NumiAnalysis* NumiAnalysis::getInstance()
{
  if (instance == 0) instance = new NumiAnalysis;
  return instance;
}

void NumiAnalysis::book()
{

  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  if (NumiData->createNuNtuple){
    sprintf(nuNtupleFileName,"%s_%04d%s.root",(NumiData->nuNtupleName).c_str(),pRunManager->GetCurrentRun()->GetRunID(), (NumiData->geometry).c_str());
    nuNtuple = new TFile(nuNtupleFileName,"RECREATE","root ntuple");
    G4cout << "Creating neutrino ntuple: "<<nuNtupleFileName<<G4endl;
    tree = new TTree("nudata","g4numi Neutrino ntuple");
    tree->Branch("data","data_t",&g4data,32000,1);
  }
 
  if (NumiData->createHadmmNtuple){
    if(NumiData->useMuonInput)
    {
       std::string hadmmNtupleFileNameStr;
       if((NumiData->GetHadmmNtupleName()).empty())
          hadmmNtupleFileNameStr = GetOFileName(NumiData->GetExtNtupleFileName());
       else hadmmNtupleFileNameStr = NumiData->GetHadmmNtupleDir() + "/" + NumiData->GetHadmmNtupleName();
      G4cout << "Creating hadron and muon monitors ntuple: "<<hadmmNtupleFileNameStr.c_str()<<G4endl;
      hadmmNtuple = new TFile(hadmmNtupleFileNameStr.c_str(), "RECREATE","hadmm ntuple");
      hadmmtree = new TTree("hadmm","g4numi Hadron and muon monitor ntuple");
      hadmmtree->Branch("hadmmdata","hadmmtuple_t",&g4hmmdata,32000,1);
    }
    else
    {
      sprintf(hadmmNtupleFileName,"%s_%04d%s.root",(NumiData->hadmmNtupleName).c_str(),pRunManager->GetCurrentRun()->GetRunID(), (NumiData->geometry).c_str());
      G4cout << "Creating hadron and muon monitors ntuple: "<<hadmmNtupleFileName<<G4endl;
      hadmmNtuple = new TFile(hadmmNtupleFileName, "RECREATE","hadmm ntuple");
      hadmmtree = new TTree("hadmm","g4numi Hadron and muon monitor ntuple");
      hadmmtree->Branch("hadmmdata","hadmmtuple_t",&g4hmmdata,32000,1);
    }
  }

  if (NumiData->createASCII) {
    G4RunManager* pRunManager = G4RunManager::GetRunManager();
    sprintf(asciiFileName,"%s%s%04d%s",(NumiData->asciiName).c_str(),"_",pRunManager->GetCurrentRun()->GetRunID(),".txt");
    G4cout << "Creating ASCII output file : "<<asciiFileName<<G4endl;
    std::ofstream asciiFile(asciiFileName);
  }

  if (NumiData->createBXDRAW) {
    sprintf(bxdrawFileName,"%s%s%03d%s",(NumiData->bxdrawName).c_str(),"_",pRunManager->GetCurrentRun()->GetRunID(),".dat");
    G4cout << "Creating BXDRAW output file : "<< bxdrawFileName <<G4endl;
    std::ofstream bxdrawFile(bxdrawFileName);
  }
  if(NumiData->raytracing){
    sprintf(zpNtupleFileName,"%s_%04d%s.root",(NumiData->zpNtupleName).c_str(),pRunManager->GetCurrentRun()->GetRunID(),(NumiData->geometry).c_str());
    G4cout<<"Creating validation and muon tracking ntuple:"<<zpNtupleFileName<<G4endl;
    zpNtuple = new TFile(zpNtupleFileName, "RECREATE","zptrack ntuple");
    if(zpNtuple){
      zpNtuple->cd();
      zptree = new TTree("zp","g4numi Tracking particles through ZPoints");
      zptree->Branch("zpdata", "zptuple_t", &g4zpdata,32000,1);
    }
    else{
      G4Exception("Something went wrong with zpNtuple");
    }
    G4cout<<" End of if statement"<<G4endl;
  }

  //book histograms
}


std::string NumiAnalysis::GetOFileName(std::string ifilename)
{
   std::string filestart = ifilename;
   
   string::size_type slash_pos = ifilename.find_last_of('/');
   if(slash_pos == string::npos)
   {
      cerr << "MakeMuNtp::GetOFileName() - bad filename" << endl;
      return string("temp.root");
   }

   ifilename.erase(0, slash_pos+1);
 
   filestart.erase(slash_pos+1);

   string::size_type loc_underscore = ifilename.find("_", 0);
   if(loc_underscore == string::npos)
   {
      cerr << "MakeMuNtp::GetOFileName() - bad filename" << endl;
      return string("temp.root");
   }

   ifilename.erase(0, loc_underscore);
 
   stringstream ofilename;
   ofilename << NumiData->GetHadmmNtupleDir() << "/" << "hadmmNtuple" << ifilename;

   return ofilename.str();
}


void NumiAnalysis::finish()
{  
  if (NumiData->createNuNtuple){
    nuNtuple->cd();
    tree->Write();
    nuNtuple->Close();
    delete nuNtuple;
  }

  if (NumiData->createHadmmNtuple){
    hadmmNtuple->cd();
    hadmmtree->Write();
    hadmmNtuple->Close();
    delete hadmmNtuple;
  }
  
  if (NumiData->createZpNtuple){
    zpNtuple->cd();
    zptree->Write();
    zpNtuple->Close();
    delete zpNtuple;
  }
}


void NumiAnalysis::FillHadmmNtuple(const G4Track& track, Int_t hmm_num, Int_t cellNum)
{
   if (!NumiData->createHadmmNtuple) return;
   
   G4RunManager* pRunManager = G4RunManager::GetRunManager();
   
   if(NumiData->useMuonInput)
   {
      NumiPrimaryGeneratorAction *NPGA = (NumiPrimaryGeneratorAction*)(pRunManager)->GetUserPrimaryGeneratorAction();
      
      G4ThreeVector particlePosition = NPGA->GetParticlePosition();
      g4hmmdata->muvx = particlePosition[0];
      g4hmmdata->muvy = particlePosition[1];
      //g4hmmdata->muvz = particlePosition[2];
      
      G4ThreeVector particleMomentum = NPGA->GetParticleMomentum();
      g4hmmdata->mupx = particleMomentum[0];
      g4hmmdata->mupy = particleMomentum[1];
      g4hmmdata->mupz = particleMomentum[2];
      
      G4ThreeVector tparentMomentum = NPGA->GetMuTParentMomentum();
      g4hmmdata->tpx = tparentMomentum[0];
      g4hmmdata->tpy = tparentMomentum[1];
      g4hmmdata->tpz = tparentMomentum[2];

      G4ThreeVector tparentPosition = NPGA->GetMuTParentPosition();
      g4hmmdata->tvx = tparentPosition[0];
      g4hmmdata->tvy = tparentPosition[1];
      g4hmmdata->tvz = tparentPosition[2];
      
      G4ThreeVector parentMomentum = NPGA->GetMuParentMomentum();
      g4hmmdata->pdpx = parentMomentum[0];
      g4hmmdata->pdpy = parentMomentum[1];
      g4hmmdata->pdpz = parentMomentum[2];

      G4ThreeVector parentPosition = NPGA->GetMuParentPosition();
      g4hmmdata->pdvx = parentPosition[0];
      g4hmmdata->pdvy = parentPosition[1];
      g4hmmdata->pdvz = parentPosition[2];

      G4ThreeVector parentProdPosition = NPGA->GetMuParentProdPosition();
      g4hmmdata->ppvx = parentProdPosition[0];
      g4hmmdata->ppvy = parentProdPosition[1];
      g4hmmdata->ppvz = parentProdPosition[2];
      
      g4hmmdata->muweight = NPGA->GetMuWeight(); 
      g4hmmdata->tpptype = NPGA->GetTParentType(); 
      g4hmmdata->nimpwt = NPGA->GetImpWeight(); 
      g4hmmdata->pptype = NPGA->GetMuParentType();
      g4hmmdata->ppmedium = NPGA->GetMuParentProdMedium();
      g4hmmdata->pgen = NPGA->GetMuParentGen(); 

      g4hmmdata->evtno = NPGA->GetEvtno();      
   
   }
   else
   {  
      /*g4hmmdata->run = pRunManager->GetCurrentRun()->GetRunID();
        g4hmmdata->mtgthsig = NumiData->beamSigmaX/cm;
        g4hmmdata->mtgtvsig = NumiData->beamSigmaY/cm;
        g4hmmdata->mtgthpos = NumiData->beamPosition[0]/cm;
        g4hmmdata->mtgtvpos = NumiData->beamPosition[1]/cm;
        g4hmmdata->evtno = pRunManager->GetCurrentEvent()->GetEventID();
      */
   }
   
   
   
   G4ParticleDefinition* particleDefinition = track.GetDefinition();
////////??????????????   
   g4hmmdata->ptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleDefinition->GetParticleName()));
   if( !(g4hmmdata->ptype == 5) && !(g4hmmdata->ptype == 6) )
   {
      cout << "******** Entry " << fentry
           << ": Problem in NumiAnalysis::FillHadmmNtuple() - The particle is "
           << g4hmmdata->ptype << ", not a muon " << endl;
   }
   
   if(!(NumiData->useMuonInput))
   {
      //g4hmmdata->hmmenergy = track.GetTotalEnergy();
      
      if(hmm_num == 4)
      {
         /*g4hmmdata->hmmxpos = track.GetPosition()[0];
           g4hmmdata->hmmypos = track.GetPosition()[1];
           g4hmmdata->hmmzpos = track.GetPosition()[2];
           g4hmmdata->hmmpx = track.GetMomentum()[0];
           g4hmmdata->hmmpy = track.GetMomentum()[1];
           g4hmmdata->hmmpz = track.GetMomentum()[2]; 
         */
	}
   }
   
   if(hmm_num != 4)
   {
      g4hmmdata->mmxpos[hmm_num] = track.GetPosition()[0];
      g4hmmdata->mmpx[hmm_num]   = track.GetMomentum()[0];
      g4hmmdata->mmypos[hmm_num] = track.GetPosition()[1];
      g4hmmdata->mmpy[hmm_num]   = track.GetMomentum()[1];
      //g4hmmdata->mmzpos[hmm_num] = track.GetPosition()[2];
      g4hmmdata->mmpz[hmm_num]   = track.GetMomentum()[2]; 
      g4hmmdata->cell[hmm_num]   = cellNum;
   } 
   
} 




//
//record mu momentum and position at 0.5m into the rock upstream
//of the monitors to apply delta ray corrections
//
void NumiAnalysis::FillAlcEdepInfo(const G4Track& track, G4int alc)
{
  if(fAlcEdep_called[alc] == true) return;

  //G4cout << "fentry = " << fentry
  //<< " Calling NumiAnalysis::FillAlcEdepInfo() for alc "
  //<< alc << " zpos = " << track.GetPosition()[2] << G4endl; 
  
  //g4hmmdata->mmxpos_Edep[alc] = track.GetPosition()[0];
  g4hmmdata->mmpx_Edep[alc]   = track.GetMomentum()[0];
  //g4hmmdata->mmypos_Edep[alc] = track.GetPosition()[1];
  g4hmmdata->mmpy_Edep[alc]   = track.GetMomentum()[1];
  //g4hmmdata->mmzpos_Edep[alc] = track.GetPosition()[2];
  g4hmmdata->mmpz_Edep[alc]   = track.GetMomentum()[2];
      
  fAlcEdep_called[alc] = true;

}


//
//this function only gets called if useMuonInput = true
//
void NumiAnalysis::FillHadmmNtuple() 
{
  if (!NumiData->createHadmmNtuple) return;  

  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  NumiPrimaryGeneratorAction *NPGA = (NumiPrimaryGeneratorAction*)(pRunManager)->GetUserPrimaryGeneratorAction();

  G4ThreeVector particlePosition = NPGA->GetParticlePosition();
  g4hmmdata->muvx = particlePosition[0];
  g4hmmdata->muvy = particlePosition[1];
  //g4hmmdata->muvz = particlePosition[2];
  
  G4ThreeVector particleMomentum = NPGA->GetParticleMomentum();
  g4hmmdata->mupx = particleMomentum[0];
  g4hmmdata->mupy = particleMomentum[1];
  g4hmmdata->mupz = particleMomentum[2];

  G4ThreeVector tparentMomentum = NPGA->GetMuTParentMomentum();
  g4hmmdata->tpx = tparentMomentum[0];
  g4hmmdata->tpy = tparentMomentum[1];
  g4hmmdata->tpz = tparentMomentum[2];
  
  G4ThreeVector tparentPosition = NPGA->GetMuTParentPosition();
  g4hmmdata->tvx = tparentPosition[0];
  g4hmmdata->tvy = tparentPosition[1];
  g4hmmdata->tvz = tparentPosition[2];
  
  G4ThreeVector parentMomentum = NPGA->GetMuParentMomentum();
  g4hmmdata->pdpx = parentMomentum[0];
  g4hmmdata->pdpy = parentMomentum[1];
  g4hmmdata->pdpz = parentMomentum[2];

  G4ThreeVector parentPosition = NPGA->GetMuParentPosition();
  g4hmmdata->pdvx = parentPosition[0];
  g4hmmdata->pdvy = parentPosition[1];
  g4hmmdata->pdvz = parentPosition[2];

  G4ThreeVector parentProdPosition = NPGA->GetMuParentProdPosition();
  g4hmmdata->ppvx = parentProdPosition[0];
  g4hmmdata->ppvy = parentProdPosition[1];
  g4hmmdata->ppvz = parentProdPosition[2];
  
  g4hmmdata->muweight = NPGA->GetMuWeight(); 
  g4hmmdata->tpptype = NPGA->GetTParentType(); 
  g4hmmdata->nimpwt = NPGA->GetImpWeight(); 
  g4hmmdata->pptype = NPGA->GetMuParentType(); 
  g4hmmdata->ppmedium = NPGA->GetMuParentProdMedium();
  g4hmmdata->pgen = NPGA->GetMuParentGen(); 
  ////////??????????????
  g4hmmdata->ptype = NPGA->GetMuType(); 

  
  //g4hmmdata->run = pRunManager->GetCurrentRun()->GetRunID();
  //g4hmmdata->mtgthsig = NumiData->beamSigmaX/cm;
  //g4hmmdata->mtgtvsig = NumiData->beamSigmaY/cm;
  //g4hmmdata->mtgthpos = NumiData->beamPosition[0]/cm;
  //g4hmmdata->mtgtvpos = NumiData->beamPosition[1]/cm;
  //g4hmmdata->evtno = pRunManager->GetCurrentEvent()->GetEventID();

  g4hmmdata->evtno = NPGA->GetEvtno();

  //g4hmmdata->hmmenergy = -81000;
  //g4hmmdata->hmmxpos = -81000;
  //g4hmmdata->hmmypos = -81000;
  //g4hmmdata->hmmzpos = -81000;
  //g4hmmdata->hmmpx = -81000;
  //g4hmmdata->hmmpy = -81000;
  //g4hmmdata->hmmpz = -81000;
  

  for(Int_t i=0;i<3;++i){
    g4hmmdata->mmxpos[i] = -99999.;
    g4hmmdata->mmpx[i]   = -99999.;
    g4hmmdata->mmypos[i] = -99999.;
    g4hmmdata->mmpy[i]   = -99999.;
    //g4hmmdata->mmzpos[i] = -99999.;
    g4hmmdata->mmpz[i]   = -99999.;
    g4hmmdata->cell[i]   = -999;


  }
}




void NumiAnalysis::SetAlcEdepFlag(G4bool AlcEdep)
{
  fAlcEdep_called[0] = AlcEdep;
  fAlcEdep_called[1] = AlcEdep;
  fAlcEdep_called[2] = AlcEdep;
}



void NumiAnalysis::SetCount(G4int count)
{
  fcount = count;
}

G4int NumiAnalysis::GetCount()
{
  return fcount;
}

void NumiAnalysis::SetEntry(G4int entry)
{
  fentry = entry;
}

G4int NumiAnalysis::GetEntry()
{
  return fentry;
}

void NumiAnalysis::WriteHadmmNtuple(const G4Track* aTrack)
{
   if (!(NumiData->createHadmmNtuple)) return;
   
   int type;
   if(aTrack)
   {
      G4ParticleDefinition* particleDefinition = aTrack->GetDefinition();
      type = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleDefinition->GetParticleName()));
      ////////??????????????
      if( !(type == 5) && !(type == 6))
      {
         //
         //check that when type != 6 and type != 5 the variables have non valid values.
         //
         if( !(g4hmmdata->mupz < -90 && g4hmmdata->muweight < -90
               && g4hmmdata->pptype < -90 && g4hmmdata->nimpwt < -90) )
         {
            cout << "******** Entry " << fentry
                 << ": Problem in NumiAnalysis::WriteHadmmNtuple() - The particle is "
                 << type << " but the variables have valid values." << endl;
         }   
      }  
      else //if type == 5 || type == 6
      {
         //
         //I don't think there is even a remote chance of the following
         //if statement ever executing. Sanity check.
         //check that if type == 5(6) then g4hmmdata->ptype == 5(6)
         //
         if( (type == 5 && !(g4hmmdata->ptype == 5)) || (type == 6 && !(g4hmmdata->ptype == 6)) )
         {
            cout << "********Entry " << fentry
                 << ": Problem in NumiAnalysis::WriteHadmmNtuple() - " 
                 << "The particle type from the track does not match the "
                 << "ptype variable. " << endl;
         }
         //
         //check that when type == 5(6)  g4hmmdata->mupz is a valid value
         //
         if(g4hmmdata->mupz < -90)
         {
            cout << "********Entry " << fentry
                 << ": Problem in NumiAnalysis::WriteHadmmNtuple() - "
                 << "The particle type from the track is 5 or 6 but the "
                 << "variable mupz is not a valid value. This should not happen."
                 << endl;
         }
      }
   }
   else
   {
      if( !(g4hmmdata->ptype == 5) && !(g4hmmdata->ptype == 6) )
      {
         cout << "********Entry " << fentry
              << ": Problem in NumiAnalysis::WriteHadmmNtuple() - A track "
              << "doesn't exist, so ptype should already be set to 5 or 6. "
              << "But it's not! " << endl;
      }
      
      type = g4hmmdata->ptype;
   }

   //
   //Note on the above: The code seems to track the two neutrinos from muon decay
   //and calls this function for each neutrino. So if the particle is a neutrino
   //I don't want to write out an entry in the output tree. If it is a neutrino,
   //all of the variables should have nonvalid values(i.e. -81000). I think that
   //it Writes the muon entry first so that means that all of the variables will
   //be cleared right after. So when it goes to fill the neutrino entries all of
   //the varibles are -99999. The above code sanity checks this. So below only
   //muons get written to the output and the intial muon entries (mupz, mupx...etc)
   //should match what is in the muon input files entry for entry.
   //
   
   ++fcount;
   
   if(fcount > 1 && g4hmmdata->mupz > 0)
   {
      G4cout << "entry = "<< fentry << ", count = " << fcount << ", mupz = "
             << g4hmmdata->mupz << ", mm1pz = " << g4hmmdata->mmpz[0]
             <<", mm2pz = " << g4hmmdata->mmpz[1] << ", mm3pz = "
             << g4hmmdata->mmpz[2] << G4endl; 
   }

   
   if(type == 5 || type == 6) hadmmtree->Fill(); 
   g4hmmdata->Clear();
   
}

void NumiAnalysis::FillNeutrinoNtuple(const G4Track& track)
{

 
  if (!NumiData->createNuNtuple) return;
    
  
  //Neutrino vertex position and momentum
  G4ThreeVector pos = track.GetPosition()/mm; 
  x = pos.x();
  y = pos.y();
  z = pos.z();
  G4ThreeVector NuMomentum = track.GetMomentum();
  G4int parentID = track.GetParentID();
  
  if (parentID == 0) return; //I have to make some changes so that neutrinos in fluka/mars ntuples don't crash

  NumiTrajectory* NuParentTrack = GetParentTrajectory(parentID);
  G4int point_no = NuParentTrack->GetPointEntries();
  G4ThreeVector ParentMomentumFinal = NuParentTrack->GetMomentum(point_no-1);
  G4ThreeVector vertex_r = (NuParentTrack->GetPoint(point_no-1)->GetPosition()/m)*m; //Should be the same as Neutrino vertex
  G4String parent_name = NuParentTrack->GetParticleName();
  G4double Parent_mass = NuParentTrack->GetMass();
  G4double gamma = sqrt(ParentMomentumFinal*ParentMomentumFinal+Parent_mass*Parent_mass)/Parent_mass; 
  G4double Parent_energy = gamma*Parent_mass;
  G4ThreeVector beta_vec = ParentMomentumFinal/Parent_energy;
  G4double partial = gamma*(beta_vec*NuMomentum);
 
  G4double enuzr = gamma*(track.GetTotalEnergy())-partial; //neutrino energy in parent rest frame

  //fill histograms, ntuples,...
  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  NumiPrimaryGeneratorAction *NPGA = (NumiPrimaryGeneratorAction*)(pRunManager)->GetUserPrimaryGeneratorAction();
 
  g4data->run = pRunManager->GetCurrentRun()->GetRunID();
  g4data->evtno = pRunManager->GetCurrentEvent()->GetEventID();
  g4data->beamHWidth = NumiData->beamSigmaX/cm;
  g4data->beamVWidth = NumiData->beamSigmaY/cm;
  g4data->beamX = NumiData->beamPosition[0]/cm;
  g4data->beamY = NumiData->beamPosition[1]/cm;
 
  G4int particleID = track.GetParentID();
  G4ThreeVector protonOrigin = NPGA->GetProtonOrigin();
  g4data->protonX = protonOrigin[0];
  g4data->protonY = protonOrigin[1];
  g4data->protonZ = protonOrigin[2];

  G4ThreeVector protonMomentum = NPGA->GetProtonMomentum();
  g4data->protonPx = protonMomentum[0];
  g4data->protonPy = protonMomentum[1];
  g4data->protonPz = protonMomentum[2];

  g4data->nuTarZ = NumiData->TargetZ0;
  g4data->hornCurrent = NumiData->HornCurrent/ampere/1000.;

  // Random decay - these neutrinos rarely hit any of the detectors
  g4data->Ndxdz = NuMomentum[0]/NuMomentum[2];
  g4data->Ndydz = NuMomentum[1]/NuMomentum[2];
  g4data->Npz = NuMomentum[2]/GeV;
  g4data->Nenergy = track.GetTotalEnergy()/GeV;

   //other info
  // Neutrino origin:
  // 3 From muon decay
  // 1 From particle from target
  // 2 From scraping
  //check if nu is from muon decay or from a particle from target, otherwise Norig = 2
  G4int Norig = 2;
  if ((parent_name=="mu+") || (parent_name=="mu-")) Norig = 3;
  G4String firstvolname = NuParentTrack->GetPreStepVolumeName(0);
  if (firstvolname.contains("Baffle") || firstvolname.contains("TGT")) Norig = 1;

  g4data->Norig = Norig;
  g4data->Ndecay = NuParentTrack->GetDecayCode();

  G4ParticleDefinition * particleType = track.GetDefinition();
  G4int ntype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleType->GetParticleName()));
  g4data->Ntype = ntype;
  g4data->Vx = x/cm;
  g4data->Vy = y/cm;
  g4data->Vz = z/cm;
  g4data->pdPx = ParentMomentumFinal[0]/GeV;
  g4data->pdPy = ParentMomentumFinal[1]/GeV;
  g4data->pdPz = ParentMomentumFinal[2]/GeV;

  G4ThreeVector ParentMomentumProduction = NuParentTrack->GetMomentum(0);
  g4data->ppdxdz = ParentMomentumProduction[0]/ParentMomentumProduction[2];
  g4data->ppdydz = ParentMomentumProduction[1]/ParentMomentumProduction[2];
  g4data->pppz = ParentMomentumProduction[2]/GeV; 

  G4double parentp = sqrt(ParentMomentumProduction*ParentMomentumProduction);

  g4data->ppenergy = sqrt((parentp*parentp-Parent_mass*Parent_mass))/GeV;

  g4data->ppmedium = 0.; //this is still empty

  g4data->ptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(parent_name));
 
  G4ThreeVector production_vertex = (NuParentTrack->GetPoint(0)->GetPosition()/m)*m; 
  g4data->ppvx = production_vertex[0]/cm;
  g4data->ppvy = production_vertex[1]/cm;
  g4data->ppvz = production_vertex[2]/cm;
  
  //if nu parent is a muon then find muon parent info
  if ((parent_name=="mu+" || parent_name=="mu-") && NuParentTrack->GetParentID()!=0)
    {
      G4int mupar = NuParentTrack->GetParentID();
      NumiTrajectory* MuParentTrack = GetParentTrajectory(mupar);
      G4int nopoint_mupar = MuParentTrack->GetPointEntries();
      G4ThreeVector muparp = MuParentTrack->GetMomentum(nopoint_mupar-1);
      G4double muparm = MuParentTrack->GetMass();
      g4data->muparpx = muparp[0]/GeV; // vector of hadron parent of muon
      g4data->muparpy = muparp[1]/GeV; // 
      g4data->muparpz = muparp[2]/GeV;
      g4data->mupare = (sqrt(muparp*muparp+muparm*muparm))/GeV;
    }
  else
    {
      g4data->muparpx = -999999.;  
      g4data->muparpy = -999999.;
      g4data->muparpz = -999999.;
      g4data->mupare = -999999.;
    }

  g4data->Necm = enuzr/GeV; // Neutrino energy in parent rest frame
  NumiTrackInformation* info = (NumiTrackInformation*)(track.GetUserInformation());
  g4data->Nimpwt = info->GetNImpWt();  // Importance weight
  g4data->tgen = info->GetTgen()-1;

  g4data->xpoint = 0.;  // x, y, z of parent at user selected vol
  g4data->xpoint = 0.;
  g4data->xpoint = 0.;

  /*    
	tgen is is the "generation" number
	of the particle that makes it out of the target. Beam protons have
	tgen=1, any particle produced by a p-C interaction would have tgen=2,
	particles produced from interactions of those products have tgen=3 etc.
	etc. until the cascade exiting the target core.
  */ 
  
  if(!NumiData->useFlukaInput && !NumiData->useMarsInput) //if not using external ntuple then need to find the particle that exited the target
    {
      G4bool findTarget = false;
      G4ThreeVector ParticleMomentum = G4ThreeVector(-999999,-999999,-999999);
      G4ThreeVector ParticlePosition = G4ThreeVector(-999999,-999999,-999999);
      NumiTrajectory* PParentTrack = GetParentTrajectory(track.GetParentID());
      G4int tptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(PParentTrack->GetParticleName()));
      particleID = PParentTrack->GetTrackID();
      
      while (!findTarget && particleID!=1){
	G4int numberOfPoints = PParentTrack->GetPointEntries();
	for (G4int ii=0;ii<numberOfPoints-1; ++ii){
	  G4String lastVolName = PParentTrack->GetPreStepVolumeName(ii);
	  G4String nextVolName = PParentTrack->GetPreStepVolumeName(ii+1);      
	  if (lastVolName.contains("TGTExit") && nextVolName.contains("TargetMother"))
	    {
	      ParticleMomentum = PParentTrack->GetMomentum(ii);              // tv_ and tp_ are equal to position and  
	      ParticlePosition = PParentTrack->GetPoint(ii)->GetPosition();  // momentum of the particle exiting the target (actually shell around target)
	      tptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(PParentTrack->GetParticleName()));
	      findTarget = true;
	    }
	}
	PParentTrack = GetParentTrajectory(PParentTrack->GetParentID());
	particleID = PParentTrack->GetTrackID();
      }
      g4data->tvx = ParticlePosition[0]/cm;
      g4data->tvy = ParticlePosition[1]/cm;
      g4data->tvz = ParticlePosition[2]/cm;
      g4data->tpx = ParticleMomentum[0]/GeV;
      g4data->tpy = ParticleMomentum[1]/GeV;
      g4data->tpz = ParticleMomentum[2]/GeV;
      g4data->tptype = tptype;
    }
  else          // using external ntuple, so set these to whatever comes from that ntuple
    {
      G4ThreeVector ParticlePosition = NPGA->GetParticlePosition();
      g4data->tvx = ParticlePosition[0]/cm;
      g4data->tvy = ParticlePosition[1]/cm;
      g4data->tvz = ParticlePosition[2]/cm;
      G4ThreeVector ParticleMomentum = NPGA->GetParticleMomentum();
      g4data->tpx = ParticleMomentum[0]/GeV;
      g4data->tpy = ParticleMomentum[1]/GeV;
      g4data->tpz = ParticleMomentum[2]/GeV;
      g4data->tptype = NPGA->GetParticleType();
    }   
  
  //set all trk_ & trkp_ to -999999  
  for (G4int ii=0; ii<10; ++ii){
    g4data->trkx[ii] = -999999.;
    g4data->trky[ii] = -999999.;
    g4data->trkz[ii] = -999999.;
    g4data->trkpx[ii] = -999999.;
    g4data->trkpy[ii] = -999999.;
    g4data->trkpz[ii] = -999999.;
  }
  // Neutrino data at different points
  // need neutrino parent info to be filled in g4data by this point

  for (G4int ii=0; ii<NumiData->nNear; ++ii){          // near detector 
    g4data->NdxdzNear[ii] = (x-NumiData->xdet_near[ii])/(z-NumiData->zdet_near[ii]);
    g4data->NdydzNear[ii] = (y-NumiData->ydet_near[ii])/(z-NumiData->zdet_near[ii]);
    
    NumiNuWeight nuwgh;
    G4double nu_wght;
    G4double nu_energy;
    std::vector<double> r_det;
    r_det.push_back(NumiData->xdet_near[ii]/cm);
    r_det.push_back(NumiData->ydet_near[ii]/cm);
    r_det.push_back(NumiData->zdet_near[ii]/cm);
    nuwgh.GetWeight(g4data, r_det,nu_wght,nu_energy);
    g4data->NenergyN[ii] = nu_energy; //in GeV
    g4data->NWtNear[ii]  = nu_wght;
  }

  for (G4int ii=0; ii<NumiData->nFar; ++ii){         // far detector
    g4data->NdxdzFar[ii] = (x-NumiData->xdet_far[ii])/(z-NumiData->zdet_far[ii]);
    g4data->NdydzFar[ii] = (y-NumiData->ydet_far[ii])/(z-NumiData->zdet_far[ii]);
    
    NumiNuWeight nuwgh;
    G4double nu_wght;
    G4double nu_energy;
    std::vector<double> r_det;
    r_det.push_back(NumiData->xdet_far[ii]/cm);
    r_det.push_back(NumiData->ydet_far[ii]/cm);
    r_det.push_back(NumiData->zdet_far[ii]/cm);
    nuwgh.GetWeight(g4data, r_det,nu_wght,nu_energy);
    g4data->NenergyF[ii] = nu_energy; //in GeV
    g4data->NWtFar[ii]   = nu_wght;
   
  }

  //if(parentID!=0){
  G4ThreeVector ParentMomentum;
  G4ThreeVector ParentPosition;
    
  G4bool wasInHorn1 = false;
  G4bool wasInHorn2 = false;  
  for (G4int ii=0; ii<point_no-1; ++ii){ 
    ParentMomentum = NuParentTrack->GetMomentum(ii);
    ParentPosition = (NuParentTrack->GetPoint(ii)->GetPosition()/m)*m;
    
    G4String postvolname = "";
    G4String prevolname = NuParentTrack->GetPreStepVolumeName(ii);
    if (ii<point_no-2) postvolname = NuParentTrack->GetPreStepVolumeName(ii+1);
      
    // parent created inside target
    if ((prevolname.contains("TGT")||prevolname.contains("Budal")) && ii==0){
      g4data->trkx[0] = ParentPosition[0]/cm;
      g4data->trky[0] = ParentPosition[1]/cm;
      g4data->trkz[0] = ParentPosition[2]/cm;
      g4data->trkpx[0] = ParentMomentum[0]/GeV;
      g4data->trkpy[0] = ParentMomentum[1]/GeV;
      g4data->trkpz[0] = ParentMomentum[2]/GeV;}
    //parent at exits target
    if ((prevolname.contains("TGTExit")) && postvolname.contains("TargetMother")){
      g4data->trkx[1] = ParentPosition[0]/cm;
      g4data->trky[1] = ParentPosition[1]/cm;
      g4data->trkz[1] = ParentPosition[2]/cm;
      g4data->trkpx[1] = ParentMomentum[0]/GeV;
      g4data->trkpy[1] = ParentMomentum[1]/GeV;
      g4data->trkpz[1] = ParentMomentum[2]/GeV;}
    //enter horn1
    if (prevolname.contains("TGAR") && postvolname.contains("Horn1")){
      g4data->trkx[2] = ParentPosition[0]/cm;
      g4data->trky[2] = ParentPosition[1]/cm;
      g4data->trkz[2] = ParentPosition[2]/cm;
      g4data->trkpx[2] = ParentMomentum[0]/GeV;
      g4data->trkpy[2] = ParentMomentum[1]/GeV;
      g4data->trkpz[2] = ParentMomentum[2]/GeV;
      wasInHorn1 = true;
    }
    //exit horn1
    if (prevolname.contains("Horn1") && postvolname.contains("TGAR")){
      g4data->trkx[3] = ParentPosition[0]/cm;
      g4data->trky[3] = ParentPosition[1]/cm;
      g4data->trkz[3] = ParentPosition[2]/cm;
      g4data->trkpx[3] = ParentMomentum[0]/GeV;
      g4data->trkpy[3] = ParentMomentum[1]/GeV;
      g4data->trkpz[3] = ParentMomentum[2]/GeV;
    }
    //enter horn2
    if (prevolname.contains("TGAR") && postvolname.contains("Horn2")){
      g4data->trkx[4] = ParentPosition[0]/cm;
      g4data->trky[4] = ParentPosition[1]/cm;
      g4data->trkz[4] = ParentPosition[2]/cm;
      g4data->trkpx[4] = ParentMomentum[0]/GeV;
      g4data->trkpy[4] = ParentMomentum[1]/GeV;
      g4data->trkpz[4] = ParentMomentum[2]/GeV;
      wasInHorn2 = true;
    }
    //exit horn2
    if (prevolname.contains("Horn2") && postvolname.contains("TGAR")){
      g4data->trkx[5] = ParentPosition[0]/cm;
      g4data->trky[5] = ParentPosition[1]/cm;
      g4data->trkz[5] = ParentPosition[2]/cm;
      g4data->trkpx[5] = ParentMomentum[0]/GeV;
      g4data->trkpy[5] = ParentMomentum[1]/GeV;
      g4data->trkpz[5] = ParentMomentum[2]/GeV;
    }
    //enter decay pipe
    if (prevolname.contains("DVOL") && (postvolname.contains("UpWn"))){
      g4data->trkx[6] = ParentPosition[0]/cm;
      g4data->trky[6] = ParentPosition[1]/cm;
      g4data->trkz[6] = ParentPosition[2]/cm;
      g4data->trkpx[6] = ParentMomentum[0]/GeV;
      g4data->trkpy[6] = ParentMomentum[1]/GeV;
      g4data->trkpz[6] = ParentMomentum[2]/GeV;}


      
    // check if the particle passes through the neck of the horn
    // if yes then set the trk_ to +999999
    // need to make this work for arbitrary horn position!!
    if ((ParentPosition[2]>0.&&ParentPosition[2]<3.*m)&&  // horn 1 position 0-3m
	(sqrt(ParentPosition[0]*ParentPosition[0]+ParentPosition[1]*ParentPosition[1])<5.*cm)&&
	(g4data->trkx[2]==-999999. || g4data->trkx[2]==999999.))
      {
	g4data->trkx[2] = 999999.;
	g4data->trky[2] = 999999.;
	g4data->trkz[2] = 999999.;  
      }
    if ((ParentPosition[2]>10.*m&&ParentPosition[2]<13.*m)&&  //horn 2 position 10-13m
	(sqrt(ParentPosition[0]*ParentPosition[0]+ParentPosition[1]*ParentPosition[1])<5.*cm)&&
	(g4data->trkx[4]==-999999. || g4data->trkx[4]==999999.))
      {
	g4data->trkx[4] = 999999.;
	g4data->trky[4] = 999999.;
	g4data->trkz[4] = 999999.;  
      }
  }
    
  ParentMomentum = NuParentTrack->GetMomentum(point_no-1);
  ParentPosition = (NuParentTrack->GetPoint(point_no-1)->GetPosition()/m)*m;
  g4data->trkx[7] = ParentPosition[0]/cm;
  g4data->trky[7] = ParentPosition[1]/cm;
  g4data->trkz[7] = ParentPosition[2]/cm;
  g4data->trkpx[7] = ParentMomentum[0]/GeV;
  g4data->trkpy[7] = ParentMomentum[1]/GeV;
  g4data->trkpz[7] = ParentMomentum[2]/GeV;
  // }
  
  tree->Fill();  


  // Write to file
  if (NumiData->createASCII) {
    std::ofstream asciiFile(asciiFileName, std::ios::app);
    if(asciiFile.is_open()) {
      asciiFile << g4data->Ntype<< " " << g4data->Nenergy << " " << g4data->NenergyN[0] << " " << g4data->NWtNear[0];
      asciiFile << " " << g4data->NenergyF[0] << " " << g4data->NWtFar[0] <<" "<<g4data->Nimpwt<< G4endl; 
      asciiFile.close();
    }
  }
}

void NumiAnalysis::FillBXDRAW(const G4Step* aStep) {

  if (!NumiData->createBXDRAW) return;
  // Based on Fortran code in mgdraw.f in *_fluka/for/ 
  G4int JTRACK, LTRACK;
  G4int TMREG, TNEWREG;
  G4int NCASE;
  G4double WTRACK;
  G4double ETRACK, AM;
  G4double XSCO, YSCO, RSCO, ZSCO;
  G4double CXTRCK, CYTRCK, CZTRCK;

  G4Track * theTrack = aStep->GetTrack();
  G4StepPoint* startPoint = aStep->GetPreStepPoint();
  G4StepPoint* endPoint = aStep->GetPostStepPoint();
  
  
  G4VPhysicalVolume *Start = startPoint->GetPhysicalVolume();
  G4VPhysicalVolume *End   = endPoint->GetPhysicalVolume();

  if (!End || !Start) // Left simulated region
	return;

  G4String StartName = Start->GetName();
  G4String EndName   = End->GetName();

  TMREG = 0;
  TNEWREG = 0;
  WTRACK = theTrack->GetWeight();
  
  if (StartName.contains("Horn1Box"))
	TMREG = 1;             // Horn 1
  else if (StartName.contains("Horn2Box"))
	TMREG = 2;             // Horn 2
  else if (StartName.contains("DVOL") ) 
	TMREG = 3;             // Decay Pipe - DVOL only
  
  if (EndName.contains("Horn1Box"))
	TNEWREG = 1;             // Horn 1
  else if (EndName.contains("Horn2Box"))
	TNEWREG = 2;             // Horn 2
  else if (EndName.contains("DVOL") ) 
        TNEWREG = 3;             // Decay Pipe - DVOL only

  ZSCO = theTrack->GetPosition().z()/cm;

  if ( TMREG != TNEWREG )  {                 // Change Regions 
	if ( TMREG == 1 || TMREG == 2 ||       // Exit horns
		 (ZSCO > 4569  && ZSCO < 4571) || // Enter Decay Pipe
		 (ZSCO > 72237 && ZSCO < 72238)   // Exit Decay Pipe
         ) {
	  
	  ETRACK = theTrack->GetTotalEnergy()/GeV;
	  G4ParticleDefinition * particleType = theTrack->GetDefinition();
	  JTRACK = code[particleType->GetPDGEncoding()];
	  if (JTRACK == 8 || JTRACK == 9)
		return;
	  if (ETRACK > 0.5 ) { // 0.5 GeV Cut off
		NumiTrackInformation* info=(NumiTrackInformation*)(theTrack->GetUserInformation());
		if (info) {
		  LTRACK = info->GetTgen();
		}
		else { 
		  LTRACK = 0; 
		}
		
		NCASE = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
		
		AM = particleType->GetPDGMass()/GeV;
		
		XSCO = theTrack->GetPosition().x()/cm;
		YSCO = theTrack->GetPosition().y()/cm;
		
		RSCO = sqrt(XSCO*XSCO + YSCO*YSCO);
		
		CXTRCK = theTrack->GetMomentumDirection().x();
		CYTRCK = theTrack->GetMomentumDirection().y();
		CZTRCK = theTrack->GetMomentumDirection().z();


		std::ofstream bxdrawFile(bxdrawFileName, std::ios::app);
		bxdrawFile << JTRACK << "  " << TMREG  << "  " << TNEWREG<< "  "  // Code, regions
				   << NCASE  << "  " << WTRACK << "  "                    // Event, Track Weight
				   << LTRACK << "  " << ETRACK << "  " << AM     << "  "  // Gen, E, M
				   << XSCO   << "  " << YSCO   << "  " << RSCO   << "  " << ZSCO << "  "  // crossing point
				   << CXTRCK << "  " << CYTRCK << "  " << CZTRCK << "  "  // direction cosines
				   << G4endl;
		bxdrawFile.close();
	  }
	}
  }
}


NumiTrajectory* NumiAnalysis::GetParentTrajectory(G4int parentID)
{
  G4TrajectoryContainer* container = 
    G4RunManager::GetRunManager()->GetCurrentEvent()->GetTrajectoryContainer();
  if(container==0) return 0;

  TrajectoryVector* vect = container->GetVector();
  G4VTrajectory* tr;
  G4int ii = 0; 
  while (ii<G4int(vect->size())){  
    tr = (*vect)[ii]; 
    NumiTrajectory* tr1 = (NumiTrajectory*)(tr);  
    if(tr1->GetTrackID() == parentID) return tr1; 
    ++ii; 
  }

  return 0;
}
//=============================================================================
//-----------------------------------------------------------------------------
void NumiAnalysis::FillZpNtuple(const G4Track& track,Int_t zpnum)
{//---Jasmine added
  if(!NumiData->raytracing) return ;
  G4RunManager* pRunManager = G4RunManager::GetRunManager();

  /*  if(g4zpdata->evtno!=  pRunManager->GetCurrentEvent()->GetEventID())
        {
        g4zpdata->run=pRunManager->GetCurrentRun()->GetRunID();
        g4zpdata->evtno = pRunManager->GetCurrentEvent()->GetEventID();
        WriteZpNtuple();//store info and initialize
        }*/

  if(zpnum<(Int_t)NumiData->Zpoint.size()){

    G4ParticleDefinition* particleDefinition = track.GetDefinition();

    g4zpdata->xposatz= track.GetPosition()[0]/cm;
    g4zpdata->yposatz= track.GetPosition()[1]/cm;
    g4zpdata->zposatz= track.GetPosition()[2]/cm;
    g4zpdata->xmomatz= track.GetMomentum()[0]/GeV;
    g4zpdata->ymomatz= track.GetMomentum()[1]/GeV;
    g4zpdata->zmomatz= track.GetMomentum()[2]/GeV;
    g4zpdata->matilen= track.GetMaterial()->GetNuclearInterLength();
    g4zpdata->field= track.GetStepLength()/cm;
    g4zpdata->pathlength= track.GetTrackLength()/cm;
    g4zpdata->ptypeatz= track.GetParentID();

    //    g4zpdata->pidtype= NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleDefinition->GetParticleName()));
    g4zpdata->zpoint=NumiData->Zpoint[zpnum]/cm ;

    g4zpdata->run=pRunManager->GetCurrentRun()->GetRunID();
    g4zpdata->evtno = pRunManager->GetCurrentEvent()->GetEventID();
    WriteZpNtuple();//store info and initialize

  }
}
void NumiAnalysis::WriteZpNtuple(){//Jasmine added

  zptree->Fill();
    g4zpdata->xposatz=-10000;
    g4zpdata->yposatz=-10000;
    g4zpdata->zposatz=-10000;
    g4zpdata->xmomatz=-10000;
    g4zpdata->ymomatz=-10000;
    g4zpdata->zmomatz=-10000;
    g4zpdata->matilen=-10000;
    g4zpdata->field =-10000;
    g4zpdata->pathlength=-10000;
    g4zpdata->zpoint=-10000;
    g4zpdata->ptypeatz=-10;
    //  g4zpdata->pidtype=-10;
    g4zpdata->run=-100;
    g4zpdata->evtno=-100;  
}
