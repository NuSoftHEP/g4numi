//----------------------------------------------------------------------
// NumiAnalysis.cc
//
// $Id: NumiAnalysis.cc,v 1.26.4.8.2.1 2013/08/19 21:37:37 rhatcher Exp $
//----------------------------------------------------------------------

#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <stdlib.h>

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
#include "draytupleMIB_t.hh"
#include "draytupleSPB_t.hh"
#include "absbkgtuple_t.hh"
#include "zptuple_t.hh" // for raytracing
#include "target_exit_t.hh"// for hadron production studies
#include "NumiParticleCode.hh"
#include "NumiAnalysis.hh"
#include "NumiTrackInformation.hh"
#include "NumiPrimaryGeneratorAction.hh"
#include "NumiDataInput.hh"
#include "NumiNuWeight.hh"
#include "NumiTrajectory.hh"

using namespace std;

NumiAnalysis* NumiAnalysis::instance = 0;

//------------------------------------------------------------------------------------
NumiAnalysis::NumiAnalysis()
   :g4hmmdata(0),
    g4draydataMIB(0),
    g4draydataSPB(0),
    g4absbkgdata(0)
{
  NumiData = NumiDataInput::GetNumiDataInput();

  if(NumiData->GetDebugLevel() > 0)
  {
     std::cout << "NumiAnalysis Constructor Called." << std::endl;
  }

#ifdef G4ANALYSIS_USE
#endif

  
   G4cout << "NumiAnalysis" << G4endl;

  g4data = new data_t();
  g4zpdata = new zptuple_t();
  g4tardata = new target_exit_t();
  

  fcount = 0;
  fentry = 0;
  for(int i = 0; i < 3; ++i)
  {
     fAlcEdep_called.push_back(false);
     fMuInAlc.push_back(false);
  }
  
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
//------------------------------------------------------------------------------------
NumiAnalysis::~NumiAnalysis()
{ 
   if(NumiData->GetDebugLevel() > 0)
   {
      std::cout << "NumiAnalysis Destructor Called." << std::endl;
   }

#ifdef G4ANALYSIS_USE
  // delete things
#endif
}
//------------------------------------------------------------------------------------
NumiAnalysis* NumiAnalysis::getInstance()
{
  if (instance == 0) instance = new NumiAnalysis;
  return instance;
}
//------------------------------------------------------------------------------------
void NumiAnalysis::book()
{
   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::book() called." << G4endl;}

  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  if (NumiData->createNuNtuple){
    sprintf(nuNtupleFileName,"%s_%04d%s.root",(NumiData->nuNtupleName).c_str(),pRunManager->GetCurrentRun()->GetRunID(), (NumiData->geometry).c_str());
    nuNtuple = new TFile(nuNtupleFileName,"RECREATE","root ntuple");
    G4cout << "Creating neutrino ntuple: "<<nuNtupleFileName<<G4endl;
    tree = new TTree("nudata","g4numi Neutrino ntuple");
    tree->Branch("data","data_t",&g4data,32000,1);
  }

  if (NumiData->createTarNtuple){
      sprintf(tarNtupleFileName,"%s_%04d%s.root",(NumiData->tarNtupleName).c_str(),pRunManager->GetCurrentRun()->GetRunID(), (NumiData->geometry).c_str());
      tarNtuple = new TFile(tarNtupleFileName,"RECREATE","root ntuple");
    if(tarNtuple){
      G4cout << "Creating target ntuple: "<<tarNtupleFileName<<G4endl;
      tarNtuple->cd();
      tartree = new TTree("tardata","g4numi Neutrino ntuple");
      //tartree->BranchOld("exit_tar","exit_tar_t",&g4tardata,32000,1);
      tartree->Branch("target_exit","target_exit_t",&g4tardata,32000,99);
    }
    else{
      G4Exception("NumiAnalysis::book()",
                  "bad tarNtuple", FatalException,
                  "Something went wrong with tarNtuple");
    }
  }

  if(NumiData->GetSimAbsBkg() && NumiData->GetCreateAbsBkgNtuple())
  {
     std::string FileNameStr;
     if((NumiData->GetAbsBkgNtupleName()).empty())
     {
        G4cout << "NumiAnalysis::book() - Output file name not set" << G4endl;
        FileNameStr = NumiData->GetAbsBkgNtupleDir() + "/AbsBkg.root";
     }
     else FileNameStr = NumiData->GetAbsBkgNtupleDir() + "/" + NumiData->GetAbsBkgNtupleName();
     
     G4cout << "Creating absorber backgrounds ntuple: "<< FileNameStr.c_str()<<G4endl;

     g4absbkgdata = new absbkgtuple_t();
     absbkgNtuple = new TFile(FileNameStr.c_str(), "RECREATE","abs bkg ntuple");
     if(!absbkgNtuple) G4cout << "Failed to create file" << G4endl;
     absbkgtree = new TTree("absbkg","g4numi Absorber Backgrounds ntuple");
     absbkgtree->Branch("AbsBkgData","absbkgtuple_t",&g4absbkgdata,32000,1);
     g4absbkgdata->Clear();
  }
  
//////////////////////////////////////////////////////////////////
  /*
  if (NumiData->createHadmmNtuple && NumiData->useMuonInput)
  {
     std::string hadmmNtupleFileNameStr;
     if((NumiData->GetHadmmNtupleName()).empty())
        hadmmNtupleFileNameStr = GetOFileName(NumiData->GetExtNtupleFileName());
     else hadmmNtupleFileNameStr = NumiData->GetHadmmNtupleDir() + "/" + NumiData->GetHadmmNtupleName();
     
     G4cout << "Creating hadron and muon monitors ntuple: "<<hadmmNtupleFileNameStr.c_str()<<G4endl;
   
     g4hmmdata = new hadmmtuple_t();
     //
     //Create ntuple for mu just getting into mons
     //
     
     hadmmNtuple = new TFile(hadmmNtupleFileNameStr.c_str(), "RECREATE","hadmm ntuple");
     if(!hadmmNtuple) G4cout << "Failed to create file" << G4endl;
     hadmmtree = new TTree("hadmm","g4numi Hadron and muon monitor ntuple");
     hadmmtree->Branch("hadmmdata","hadmmtuple_t",&g4hmmdata,32000,1);
     g4hmmdata->Clear();
        
  
  }
  */
  /////////////////////////////////////////////////
  
  if (NumiData->createHadmmNtuple && NumiData->useMuonInput)
  {
     std::string hadmmNtupleFileNameStr;
     if((NumiData->GetHadmmNtupleName()).empty())
        hadmmNtupleFileNameStr = GetOFileName(NumiData->GetExtNtupleFileName());
     else hadmmNtupleFileNameStr = NumiData->GetHadmmNtupleDir() + "/" + NumiData->GetHadmmNtupleName();
     
     G4cout << "Creating hadron and muon monitors ntuple: "<<hadmmNtupleFileNameStr.c_str()<<G4endl;
     
     if(!NumiData->GetSimDRays())
     {
        g4hmmdata = new hadmmtuple_t();
        //
        //Create ntuple for mu just getting into mons
        //
                
        hadmmNtuple = new TFile(hadmmNtupleFileNameStr.c_str(), "RECREATE","hadmm ntuple");
        if(!hadmmNtuple) G4cout << "Failed to create file" << G4endl;
        hadmmtree = new TTree("hadmm","g4numi Hadron and muon monitor ntuple");
        hadmmtree->Branch("hadmmdata","hadmmtuple_t",&g4hmmdata,32000,1);
        g4hmmdata->Clear();

     }
     else if(NumiData->GetSimDRays())
     {
        g4draydataMIB = new draytupleMIB_t();
        
        hadmmNtuple = new TFile(hadmmNtupleFileNameStr.c_str(), "RECREATE","dray ntuple");
        if(!hadmmNtuple) G4cout << "Failed to create file" << G4endl;
        hadmmtree = new TTree("dRay","g4numi Hadron and muon monitor ntuple");
        hadmmtree->Branch("dRaydata","draytupleMIB_t",&g4draydataMIB,32000,1);
        g4draydataMIB->Clear();
     }
  }
  else if (NumiData->createHadmmNtuple && !(NumiData->useMuonInput))
  {
     std::string hadmmNtupleFileNameStr;
     if((NumiData->GetHadmmNtupleName()).empty())
     {
        stringstream path;
        path << NumiData->GetHadmmNtupleDir()
             << "/hadmmNtuple_P"
             << (int)NumiData->GetMuonBeamMomentum()
             << "_"
             << pRunManager->GetCurrentRun()->GetRunID()
             << ".root";

        hadmmNtupleFileNameStr = path.str();
     }
     else hadmmNtupleFileNameStr = NumiData->GetHadmmNtupleDir() + "/" + NumiData->GetHadmmNtupleName();

     G4cout << "Creating hadron and muon monitors ntuple: "<<hadmmNtupleFileNameStr.c_str()<<G4endl;

     if(NumiData->GetSimDRays())
     {
        g4draydataSPB = new draytupleSPB_t();
        
        hadmmNtuple = new TFile(hadmmNtupleFileNameStr.c_str(), "RECREATE","dray ntuple");
        if(!hadmmNtuple) G4cout << "Failed to create file" << G4endl;
        hadmmtree = new TTree("dRay","g4numi Hadron and muon monitor ntuple");
        hadmmtree->Branch("dRaydata","draytupleSPB_t",&g4draydataSPB,32000,1);
        g4draydataSPB->Clear();
     }
  }
///////////////////////////////////////////////////////

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
      G4Exception("NumiAnalysis::book()",
                  "bad zpNtuple", FatalException,
                  "Something went wrong with zpNtuple");
    }
    G4cout<<" End of if statement"<<G4endl;
  }

  //book histograms
}

//------------------------------------------------------------------------------------
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

   string::size_type dot_pos = ifilename.find('.');
   if(dot_pos == string::npos)
   {
      cerr << "MakeMuNtp::GetOFileName() - can't find \".\" " << endl;
      return string("temp.root");
   }

   string filePrefix = "";
   ///////////////////////////////////////////////////////////////////////////////////////
   
   if(NumiData->GetSimDRays()) filePrefix = "dRayNtuple";
   else filePrefix = "hadmmNtuple";
   
///////////////////////////////////////////////////////////////////////////////////////
   /*
   filePrefix = "hadmmNtuple";
   */

   const G4int    NInputPart  = NumiData->GetNInputPart();
        if(NInputPart == 1)   ifilename.insert(dot_pos, "a");
   else if(NInputPart == 2)   ifilename.insert(dot_pos, "b");
   else if(NInputPart == 3)   ifilename.insert(dot_pos, "c");
   else if(NInputPart == 4)   ifilename.insert(dot_pos, "d");
   else if(NInputPart == 5)   ifilename.insert(dot_pos, "e");
   else if(NInputPart == 6)   ifilename.insert(dot_pos, "f");
   else if(NInputPart == 7)   ifilename.insert(dot_pos, "g");
   else if(NInputPart == 8)   ifilename.insert(dot_pos, "h");
   else if(NInputPart == 9)   ifilename.insert(dot_pos, "i");
   else if(NInputPart == 10)  ifilename.insert(dot_pos, "j");
   else if(NInputPart == 11)  ifilename.insert(dot_pos, "k");
   else if(NInputPart == 12)  ifilename.insert(dot_pos, "l");
   else if(NInputPart == 13)  ifilename.insert(dot_pos, "m");
   else if(NInputPart == 14)  ifilename.insert(dot_pos, "n");
   else if(NInputPart == 15)  ifilename.insert(dot_pos, "o");
   else if(NInputPart == 16)  ifilename.insert(dot_pos, "p");
   else if(NInputPart == 17)  ifilename.insert(dot_pos, "q");
   else if(NInputPart == 18)  ifilename.insert(dot_pos, "r");
   else if(NInputPart == 19)  ifilename.insert(dot_pos, "s");
   else if(NInputPart == 20)  ifilename.insert(dot_pos, "t");
        

   stringstream ofilename;
   ofilename << NumiData->GetHadmmNtupleDir() << "/" << filePrefix << ifilename;
      

   return ofilename.str();
}

//------------------------------------------------------------------------------------
void NumiAnalysis::finish()
{
   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::finish() called." << G4endl;}
   
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

  if (NumiData->GetCreateAbsBkgNtuple()){
    absbkgNtuple->cd();
    absbkgtree->Write();
    absbkgNtuple->Close();
    delete absbkgNtuple;
  }
  
  if (NumiData->createZpNtuple){
    zpNtuple->cd();
    zptree->Write();
    zpNtuple->Close();
    delete zpNtuple;
  }

  if (NumiData->createTarNtuple){
    tarNtuple->cd();
    tartree->Write();
    tarNtuple->Close();
    delete tarNtuple;
  }
}

//------------------------------------------------------------------------------------
void NumiAnalysis::FillHadmmNtuple(const G4Track& track, Int_t hmm_num, Int_t cellNum)
{
   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::FillHadmmNtuple() called." << G4endl;}
   
   if (!NumiData->createHadmmNtuple) return;
         
   G4ParticleDefinition* particleDefinition = track.GetDefinition();
   
   if ( track.GetTrackID() != 1 ||
        (particleDefinition != G4MuonPlus::MuonPlusDefinition()
         && particleDefinition != G4MuonMinus::MuonMinusDefinition()) )
   {
      G4cout << "******** Entry " << fentry
           << ": Problem in NumiAnalysis::FillHadmmNtuple() - The particle is either"
           << " not a muon or the not the orignial muon. Particle = "
           << particleDefinition -> GetParticleName() << " the track number = "
           << track.GetTrackID() << G4endl;
      return;
   }

   if(g4hmmdata)
   {
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
         if(fMuInAlc[hmm_num] == true) return;

         if( !(NumiData->GetMaterialSigma() < 0) && !(NumiData->GetMaterialSigma() > 0))
         {
            g4hmmdata->mmxpos[hmm_num] = track.GetPosition()[0];
            g4hmmdata->mmypos[hmm_num] = track.GetPosition()[1];
            //g4hmmdata->mmzpos[hmm_num] = track.GetPosition()[2];
         }
         
         g4hmmdata->mmpx[hmm_num]   = track.GetMomentum()[0];
         g4hmmdata->mmpy[hmm_num]   = track.GetMomentum()[1];
         g4hmmdata->mmpz[hmm_num]   = track.GetMomentum()[2]; 
         g4hmmdata->cell[hmm_num]   = cellNum;

         /*if(hmm_num == 0)
           G4cout << GetEntry() << ": mmpz = " << g4hmmdata->mmpz[hmm_num]
           << " mmxpos = " << g4hmmdata->mmxpos[hmm_num]
           << " mmcell = " << g4hmmdata->cell[hmm_num] << G4endl; 
         */
         
         fMuInAlc[hmm_num] = true;
      }     
   }
   else if(g4draydataMIB)
   {
      if(hmm_num != 4)
      {  
         if(fMuInAlc[hmm_num] == true) return;
         
         g4draydataMIB->mmxpos[hmm_num] = track.GetPosition()[0];
         g4draydataMIB->mmpx[hmm_num]   = track.GetMomentum()[0];
         g4draydataMIB->mmypos[hmm_num] = track.GetPosition()[1];
         g4draydataMIB->mmpy[hmm_num]   = track.GetMomentum()[1];
         //g4draydataMIB->mmzpos[hmm_num] = track.GetPosition()[2];
         g4draydataMIB->mmpz[hmm_num]   = track.GetMomentum()[2]; 
         g4draydataMIB->cell[hmm_num]   = cellNum;
         
         fMuInAlc[hmm_num] = true;
      }     
   }
   else if(g4draydataSPB)
   {
      if(hmm_num != 4)
      {  
         if(fMuInAlc[hmm_num] == true) return;
         
         g4draydataSPB->mmxpos[hmm_num] = track.GetPosition()[0];
         g4draydataSPB->mmpx[hmm_num]   = track.GetMomentum()[0];
         g4draydataSPB->mmypos[hmm_num] = track.GetPosition()[1];
         g4draydataSPB->mmpy[hmm_num]   = track.GetMomentum()[1];
         //g4draydataSPB->mmzpos[hmm_num] = track.GetPosition()[2];
         g4draydataSPB->mmpz[hmm_num]   = track.GetMomentum()[2]; 
         g4draydataSPB->cell[hmm_num]   = cellNum;
         
         fMuInAlc[hmm_num] = true;
      }     
   }

}

//------------------------------------------------------------------------------------
void NumiAnalysis::FillEdep(G4double edep, const G4ParticleDefinition* particleDefinition,
                            const G4int alc, const G4int IntExt, const G4double /*zpos*/, Int_t cellNum,
                            const Int_t trackID, const G4double weight)
{

   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::FillEdep() called." << G4endl;}
   
   edep = edep/eV;

   if(g4draydataMIB)
   {
      if ( particleDefinition == G4MuonPlus::MuonPlusDefinition()
           || particleDefinition == G4MuonMinus::MuonMinusDefinition() )
      {
         g4draydataMIB -> SetMuEdep(alc, cellNum, edep);
      }
      else if ( particleDefinition == G4Electron::ElectronDefinition()
                || particleDefinition == G4Positron::PositronDefinition())
      {
         if(IntExt == 0) // ext
         {
            g4draydataMIB -> SetExtEdep(alc, cellNum, edep, trackID, weight);
            //g4draydataMIB->zpos_edep[alc] = zpos;
         }
         else if(IntExt == 1) // int
         {
            g4draydataMIB -> SetIntEdep(alc, cellNum, edep, trackID, weight);
            //g4draydataMIB->zpos_edep[alc] = zpos;
         }
         else
            G4cerr << "NumiAnalysis::FillEdep - Problem: Invalid IntExt flag " << IntExt << G4endl;
      }
      else
         G4cerr << "NumiAnalysis::FillEdep - Problem: Invalid particle type " << particleDefinition -> GetParticleName() << G4endl;
   }
   else if(g4draydataSPB)
   {
      if ( particleDefinition == G4MuonPlus::MuonPlusDefinition()
           || particleDefinition == G4MuonMinus::MuonMinusDefinition() )
      {
         g4draydataSPB->mu_edep[alc] += edep;
      }
      else if ( particleDefinition == G4Electron::ElectronDefinition()
                || particleDefinition == G4Positron::PositronDefinition() )
      {
         if(IntExt == 0) // ext
         {
            g4draydataSPB->ext_edep[alc] += edep;
         }
         else if(IntExt == 1) // int
         {
            g4draydataSPB->int_edep[alc] += edep;
         }
         else
            G4cerr << "NumiAnalysis::FillEdep - Problem: Invalid IntExt flag " << IntExt << G4endl;
      }
      else
         G4cerr << "NumiAnalysis::FillEdep - Problem: Invalid particle type " << particleDefinition -> GetParticleName() << G4endl;
   }
   
}





//
//record mu momentum and position at 0.5m into the rock upstream
//of the monitors to apply delta ray corrections
//
//------------------------------------------------------------------------------------
void NumiAnalysis::FillAlcEdepInfo(const G4Track& track, G4int alc)
{
   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::FillAlcEdepInfo() called." << G4endl;}
   
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
//------------------------------------------------------------------------------------
void NumiAnalysis::FillHadmmNtuple() 
{
   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::FillHadmmNtuple() (with no args) called." << G4endl;}

  if (!NumiData->createHadmmNtuple) return;  

  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  NumiPrimaryGeneratorAction *NPGA = (NumiPrimaryGeneratorAction*)(pRunManager)->GetUserPrimaryGeneratorAction();

  if(g4hmmdata)
  {

     if( !(NumiData->GetMaterialSigma() < 0) && !(NumiData->GetMaterialSigma() > 0))
     {
        G4ThreeVector particlePosition = NPGA->GetParticlePosition();
        g4hmmdata->muvx = particlePosition[0];
        g4hmmdata->muvy = particlePosition[1];
        //g4hmmdata->muvz = particlePosition[2];

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

        g4hmmdata->pptype = NPGA->GetMuParentType(); 
        g4hmmdata->ppmedium = NPGA->GetMuParentProdMedium();
        g4hmmdata->pgen = NPGA->GetMuParentGen(); 
        g4hmmdata->ptype = NPGA->GetMuType();
        
        g4hmmdata->evtno = NPGA->GetEvtno();
     }
     
     G4ThreeVector particleMomentum = NPGA->GetParticleMomentum();
     g4hmmdata->mupx = particleMomentum[0];
     g4hmmdata->mupy = particleMomentum[1];
     g4hmmdata->mupz = particleMomentum[2];
     
     G4ThreeVector tparentMomentum = NPGA->GetMuTParentMomentum();
     g4hmmdata->tpx = tparentMomentum[0];
     g4hmmdata->tpy = tparentMomentum[1];
     g4hmmdata->tpz = tparentMomentum[2];

     g4hmmdata->muweight = NPGA->GetMuWeight(); 
     g4hmmdata->tpptype = NPGA->GetTParentType(); 
     g4hmmdata->nimpwt = NPGA->GetImpWeight();
     
     //g4hmmdata->run = pRunManager->GetCurrentRun()->GetRunID();
     //g4hmmdata->mtgthsig = NumiData->beamSigmaX/cm;
     //g4hmmdata->mtgtvsig = NumiData->beamSigmaY/cm;
     //g4hmmdata->mtgthpos = NumiData->beamPosition[0]/cm;
     //g4hmmdata->mtgtvpos = NumiData->beamPosition[1]/cm;
     //g4hmmdata->evtno = pRunManager->GetCurrentEvent()->GetEventID();
     
     
     
     //g4hmmdata->hmmenergy = -81000;
     //g4hmmdata->hmmxpos = -81000;
     //g4hmmdata->hmmypos = -81000;
     //g4hmmdata->hmmzpos = -81000;
     //g4hmmdata->hmmpx = -81000;
     //g4hmmdata->hmmpy = -81000;
     //g4hmmdata->hmmpz = -81000;
     
     for(Int_t i=0;i<3;++i)
     {
        if( !(NumiData->GetMaterialSigma() < 0) && !(NumiData->GetMaterialSigma() > 0))
        {  
           g4hmmdata->mmxpos[i] = -99999.;        
           g4hmmdata->mmypos[i] = -99999.;
           //g4hmmdata->mmzpos[i] = -99999.;
        }
        
        g4hmmdata->mmpx[i]   = -99999.;
        g4hmmdata->mmpy[i]   = -99999.;
        g4hmmdata->mmpz[i]   = -99999.;
        g4hmmdata->cell[i]   = -999;   
     }
  }
  else if(g4draydataMIB)
  {
     G4ThreeVector particlePosition = NPGA->GetParticlePosition();
     g4draydataMIB->muvx = particlePosition[0];
     g4draydataMIB->muvy = particlePosition[1];
     //g4draydataMIB->muvz = particlePosition[2];
     
     G4ThreeVector particleMomentum = NPGA->GetParticleMomentum();
     g4draydataMIB->mupx = particleMomentum[0];
     g4draydataMIB->mupy = particleMomentum[1];
     g4draydataMIB->mupz = particleMomentum[2];
     
     g4draydataMIB->muweight = NPGA->GetMuWeight(); 
     g4draydataMIB->nimpwt = NPGA->GetImpWeight(); 
     g4draydataMIB->ptype = NPGA->GetMuType(); 
               
  }
  else if(g4draydataSPB)
  {
     G4ThreeVector particlePosition = NPGA->GetParticlePosition();
     g4draydataSPB->muvx = particlePosition[0];
     g4draydataSPB->muvy = particlePosition[1];
     g4draydataSPB->muvz = particlePosition[2];
     
     G4ThreeVector particleMomentum = NPGA->GetParticleMomentum();
     g4draydataSPB->mupx = particleMomentum[0];
     g4draydataSPB->mupy = particleMomentum[1];
     g4draydataSPB->mupz = particleMomentum[2];
     
     g4draydataSPB->ptype = NPGA->GetMuType(); 
               
  }

  
}

//------------------------------------------------------------------------------------
void NumiAnalysis::FillAbsorberBkgrdNtuple(const G4Track& track)
{
   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::FillAbsorberBkgrdNtuple() called." << G4endl;}
   
   G4ParticleDefinition * particleType = track.GetDefinition();
   G4String partType = particleType->GetParticleType();
   g4absbkgdata -> ptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleType->GetParticleName()));

   g4absbkgdata->x    = track.GetPosition()[0]/cm;
   g4absbkgdata->px   = track.GetMomentum()[0]/GeV;
   g4absbkgdata->y    = track.GetPosition()[1]/cm;
   g4absbkgdata->py   = track.GetMomentum()[1]/GeV;
   g4absbkgdata->z    = track.GetPosition()[2]/cm;
   g4absbkgdata->pz   = track.GetMomentum()[2]/GeV; 

   g4absbkgdata->KE = track.GetKineticEnergy()/GeV;

   NumiTrackInformation* info = (NumiTrackInformation*)(track.GetUserInformation());
   g4absbkgdata->impwt = info->GetNImpWt();  // Importance weight
   g4absbkgdata->tgen = info->GetTgen()-1;
   
   int tgtz = -999;
   if(NumiData->TargetZ0/cm == -35.)
      tgtz = 0;
   else if(NumiData->TargetZ0/cm == -45.)
      tgtz = 10;
   else if(NumiData->TargetZ0/cm == -135.)
      tgtz = 100;
   else if(NumiData->TargetZ0/cm == -185.)
      tgtz = 150;
   else if(NumiData->TargetZ0/cm == -285.)
      tgtz = 250;

   g4absbkgdata->tgtz = tgtz;
   g4absbkgdata->ihorn = NumiData->HornCurrent/ampere/1000.;

   absbkgtree->Fill(); 
   g4absbkgdata->Clear();
}




//------------------------------------------------------------------------------------
void NumiAnalysis::SetAlcEdepFlag(G4bool AlcEdep)
{
  fAlcEdep_called[0] = AlcEdep;
  fAlcEdep_called[1] = AlcEdep;
  fAlcEdep_called[2] = AlcEdep;
}
//------------------------------------------------------------------------------------
void NumiAnalysis::SetMuInAlcFlag(G4bool MuInAlc)
{
  fMuInAlc[0] = MuInAlc;
  fMuInAlc[1] = MuInAlc;
  fMuInAlc[2] = MuInAlc;
}


//------------------------------------------------------------------------------------
void NumiAnalysis::SetCount(G4int count)
{
  fcount = count;
}
//------------------------------------------------------------------------------------
G4int NumiAnalysis::GetCount()
{
  return fcount;
}
//------------------------------------------------------------------------------------
void NumiAnalysis::SetEntry(G4int entry)
{
  fentry = entry;
}
//------------------------------------------------------------------------------------
G4int NumiAnalysis::GetEntry()
{
  return fentry;
}

//------------------------------------------------------------------------------------
void NumiAnalysis::WriteHadmmNtuple()
{
   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::WriteHadmmNtuple() called." << G4endl;}
      
   if (!(NumiData->createHadmmNtuple)) return;

   if(g4draydataMIB) g4draydataMIB->ClearTrackIDVectors();

   hadmmtree->Fill();
   
   if(g4hmmdata) g4hmmdata->Clear();
   if(g4draydataMIB) g4draydataMIB->Clear();
   if(g4draydataSPB) g4draydataSPB->Clear();
      
   
}

//------------------------------------------------------------------------------------
void NumiAnalysis::FillNeutrinoNtuple(const G4Track& track, const std::vector<G4VTrajectory*>& history)
{

   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::FillNeutrinoNtuple() called." << G4endl;}
 
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

  g4data->ppenergy = sqrt((parentp*parentp+Parent_mass*Parent_mass))/GeV;

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
  
  //if not using external ntuple then need to find the particle that exited the target
  if(!NumiData->useFlukaInput && !NumiData->useMarsInput) 
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
                    // tv_ and tp_ are equal to position and
                    // momentum of the particle exiting the target (actually shell around target)
                    ParticleMomentum = PParentTrack->GetMomentum(ii);              
                    ParticlePosition = PParentTrack->GetPoint(ii)->GetPosition();  
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

      // Reset the neutrino history ntuple variables
  g4data->ntrajectory = -1;
  g4data->overflow = false;
  for (std::size_t index = 0; index < maxGen; ++index) {
      g4data->pdg[index] = 0;
      g4data->trackId[index] = -1;
      g4data->parentId[index] = -1;
      g4data->startx[index]  = 999.999;
      g4data->starty[index]  = 999.999;
      g4data->startz[index]  = 999.999;
      g4data->startpx[index] = 999.999;
      g4data->startpy[index] = 999.999;
      g4data->startpz[index] = 999.999;

      g4data->stopx[index]  = 999.999;
      g4data->stopy[index]  = 999.999;
      g4data->stopz[index]  = 999.999;
      g4data->stoppx[index] = 999.999;
      g4data->stoppy[index] = 999.999;
      g4data->stoppz[index] = 999.999;

      g4data->pprodpx[index] = 999.999;
      g4data->pprodpy[index] = 999.999;
      g4data->pprodpz[index] = 999.999;

      g4data->proc[index] = "NotDefined";
      g4data->ivol[index] = "NotDefined";
      g4data->fvol[index] = "NotDefined";
  }

  
      // Save neutrino history to the ntuple. If the depth of the history is more than the
      // allocated memory in the arrays, then the variable 'overflow' will be set to 'TRUE'.
      // Entries with overflow set to true should be excluded from analysis. Currently at
      // maxGen = 12, this happens approximately every 10,000 neutrinos. If this is unacceptable,
      // the maxGen value should be increased to 13 or higher.
  g4data->ntrajectory = history.size();
  if (history.size()>maxGen) g4data->overflow = true;

      // Create a temporary vector so that it can be reversed.
  std::vector<G4VTrajectory*> tmpHistory(history);
  std::reverse(tmpHistory.begin(),tmpHistory.end());
  for (std::size_t index = 0; index < std::min(maxGen,history.size()); ++index) {
      NumiTrajectory* traj = dynamic_cast<NumiTrajectory*>(tmpHistory.at(index));
      g4data->pdg[index] = traj->GetPDGEncoding();
      g4data->trackId[index] = traj->GetTrackID();
      g4data->parentId[index] = traj->GetParentID();
      g4data->startx[index]  = traj->GetPoint(0)->GetPosition().x();
      g4data->starty[index]  = traj->GetPoint(0)->GetPosition().y();
      g4data->startz[index]  = traj->GetPoint(0)->GetPosition().z();
      g4data->startpx[index] = traj->GetMomentum(0).x();
      g4data->startpy[index] = traj->GetMomentum(0).y();
      g4data->startpz[index] = traj->GetMomentum(0).z();

      const int lastPoint = traj->GetPointEntries()-1;
      g4data->stopx[index]  = traj->GetPoint(lastPoint)->GetPosition().x();
      g4data->stopy[index]  = traj->GetPoint(lastPoint)->GetPosition().y();
      g4data->stopz[index]  = traj->GetPoint(lastPoint)->GetPosition().z();
      g4data->stoppx[index] = traj->GetMomentum(lastPoint).x();
      g4data->stoppy[index] = traj->GetMomentum(lastPoint).y();
      g4data->stoppz[index] = traj->GetMomentum(lastPoint).z();

      g4data->pprodpx[index] = traj->GetParentMomentumAtThisProduction().x();
      g4data->pprodpy[index] = traj->GetParentMomentumAtThisProduction().y();
      g4data->pprodpz[index] = traj->GetParentMomentumAtThisProduction().z();

      g4data->proc[index] = traj->GetProcessName();
      g4data->ivol[index] = traj->GetPreStepVolumeName(0);
      g4data->fvol[index] = traj->GetPreStepVolumeName(lastPoint);

  }


  
  tree->Fill();  
  
  
      // Write to file
  if (NumiData->createASCII)
  {
      std::ofstream asciiFile(asciiFileName, std::ios::app);
      if(asciiFile.is_open())
      {
          asciiFile << g4data->Ntype<< " " << g4data->Nenergy << " " << g4data->NenergyN[0] << " " << g4data->NWtNear[0];
          asciiFile << " " << g4data->NenergyF[0] << " " << g4data->NWtFar[0] <<" "<<g4data->Nimpwt<< G4endl; 
          asciiFile.close();
      }
  }
  

}

//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
NumiTrajectory* NumiAnalysis::GetParentTrajectory(G4int parentID)
{

   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::GetParentTrajectory() called." << G4endl;}
      
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

    g4zpdata->pidtype= NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleDefinition->GetParticleName()));
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
    g4zpdata->pidtype=-10;
    g4zpdata->run=-100;
    g4zpdata->evtno=-100;  
}

//------------------------------------------------------------------------------------
void NumiAnalysis::FillTarNtuple(const G4Track& track)
{//---Melissa added
  if(!NumiData->createTarNtuple) return ;

  G4RunManager* pRunManager = G4RunManager::GetRunManager();
 
    G4ParticleDefinition* particleDefinition = track.GetDefinition();

    NumiTrackInformation* info = (NumiTrackInformation*)(track.GetUserInformation());
    g4tardata->impwt = info->GetNImpWt();  // Importance weight

    g4tardata->tvx= track.GetPosition()[0]/cm;
    g4tardata->tvy= track.GetPosition()[1]/cm;
    g4tardata->tvz= track.GetPosition()[2]/cm;
    g4tardata->tpx= track.GetMomentum()[0]/GeV;
    g4tardata->tpy= track.GetMomentum()[1]/GeV;
    g4tardata->tpz= track.GetMomentum()[2]/GeV;

    g4tardata->tptype= NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleDefinition->GetParticleName()));

    g4tardata->run=pRunManager->GetCurrentRun()->GetRunID();
    g4tardata->evtno = pRunManager->GetCurrentEvent()->GetEventID();
    WriteTarNtuple();//store info and initialize
}

//------------------------------------------------------------------------------------
void NumiAnalysis::WriteTarNtuple(){//Melissa added

  tartree->Fill();
    g4tardata->impwt=-10000;
    g4tardata->tvx=-10000;
    g4tardata->tvy=-10000;
    g4tardata->tvz=-10000;
    g4tardata->tpx=-10000;
    g4tardata->tpy=-10000;
    g4tardata->tpz=-10000;
    g4tardata->tptype=-10;
    g4tardata->run=-100;
    g4tardata->evtno=-100;  
}
