 //----------------------------------------------------------------------
// NumiAnalysis.cc
//
// $Id: NumiAnalysis.cc,v 1.26.4.26 2017/11/02 22:08:00 lebrun Exp $
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
#include "h1trackingplane_exit_t.hh"// for hadron production studies
#include "NumiParticleCode.hh"
#include "NumiAnalysis.hh"
#include "NumiTrackInformation.hh"
#include "NumiPrimaryGeneratorAction.hh"
#include "NumiDataInput.hh"
#include "NumiNuWeight.hh"
#include "NumiTrajectory.hh"
#include "NumiRunManager.hh"
#include "NumiSteppingAction.hh"
#define USEMODGEANT4
#ifdef USEMODGEANT4 

#include "MinervaElementInter.hh"

#endif

using namespace std;

NumiAnalysis* NumiAnalysis::instance = 0;

//------------------------------------------------------------------------------------
NumiAnalysis::NumiAnalysis()
   :g4hmmdata(0),
    g4draydataMIB(0),
    g4draydataSPB(0),
    g4absbkgdata(0),
    energyBinSimpleHistoMinerva(-1.0e10)
// The above histo must be of the same size..     
{
  // 
  //  Simple histogram for quick study of the flux spectrum, Paul Lebrun, September/October 2017.  
  // To turn on, simply uncomment the bin size. 
  //
//  energyBinSimpleHistoMinerva = 0.1;
  if (energyBinSimpleHistoMinerva > 0.) {  
    MinervaNuMuHisto = std::vector<double>(250,0.);
    MinervaNuMuBarHisto = std::vector<double>(250,0.); 
    NovaNearNuMuHisto = std::vector<double>(100,0.);
    NovaNearNuMuBarHisto = std::vector<double>(100,0.); 
    NovaFarNuMuHisto = std::vector<double>(100,0.);
    NovaFarNuMuBarHisto = std::vector<double>(100,0.);
  }
  
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
  g4h1trackingdata = new h1trackingplane_exit_t();
  
  //New for DK2NU:
  this_meta  = new bsim::DkMeta();
  this_dk2nu = new bsim::Dk2Nu();

  fcount = 0;
  fentry = 0;
  for(int i = 0; i < 3; ++i)
  {
     fAlcEdep_called.push_back(false);
     fMuInAlc.push_back(false);
  }
  code[11]    = 2; //prachi for electron
  code[-11]   = 3; //prachi for positron
  code[22]    = 4; //prachi for gamma

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
  //
  // Set the Minerva flux binning depending on the horn configuration. (le vs me ) 
  //
  if (NumiData->GetHornConfig() == G4String("le")) {
    std::cerr << " Low energy config, Setting energyBinSimpleHistoMinerva to 0.02.. " << std::endl;
    energyBinSimpleHistoMinerva = 0.02;
  }  

   if(NumiData->GetDebugLevel() == 10) { G4cout << "NumiAnalysis::book() called." << G4endl;}

  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  if (NumiData->createNuNtuple){
    sprintf(nuNtupleFileName,"%s_%04d%s.root",(NumiData->nuNtupleName).c_str(),pRunManager->GetCurrentRun()->GetRunID(), (NumiData->geometry).c_str());
    nuNtuple = new TFile(nuNtupleFileName,"RECREATE","root ntuple");
    G4cout << "Creating neutrino ntuple: "<<nuNtupleFileName<<G4endl;
    /*
    tree = new TTree("nudata","g4numi Neutrino ntuple");
    tree->Branch("data","data_t",&g4data,32000,1);
    */
    //NEW DK2NU:
    tree = new TTree("dk2nuTree","g4numi Neutrino ntuple");
    tree->Branch("dk2nu","bsim::Dk2Nu",&this_dk2nu,32000,99);
    meta = new TTree("dkmetaTree","Meta info for neutrino run");
    meta->Branch("dkmeta","bsim::DkMeta",&this_meta,32000,99);

  }
//prachi start
  if (NumiData->createH1TrackingPlane){
      sprintf(h1NtupleFileName,"%s_%04d%s.root",(NumiData->h1trackingNtupleName).c_str(),pRunManager->GetCurrentRun()->GetRunID(), (NumiData->geometry).c_str());
      h1trackingNtuple = new TFile(h1NtupleFileName,"RECREATE","root ntuple");
    if(h1trackingNtuple){
      //G4cout << "Creating target ntuple: "<<h1NtupleFileName<<G4endl;
      h1trackingNtuple->cd();
      h1trackingtree = new TTree("H1TrackingPlaneData","g4numi Neutrino ntuple");
      //h1trackingtree->BranchOld("exit_tar","exit_tar_t",&g4h1trackingdata,32000,1);
      h1trackingtree->Branch("H1TrackingPlane","h1trackingplane_exit_t",&g4h1trackingdata,32000,99);
    }
    else{
#ifndef MODERN_G4
      G4Exception("Something went wrong with h1trackingNtuple");
#else
      G4Exception("NumiAnalysis","NumiAnalysis",FatalException,"Something went wrong with h1trackingNtuple");
#endif
    }
  }
//prachi end

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
  if (NumiData-> else hadmmNtupleFileNameStr = NumiData->GetHadmmNtupleDir() + "/" + NumiData->GetHadmmNtupleName();
     
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
#ifndef MODERN_G4
      G4Exception("Something went wrong with zpNtuple");
#else
      G4Exception("NumiAnalysis","NumiAnalysis",FatalException,"Something went wrong with zpNtuple");
#endif
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
void NumiAnalysis::finishPL()
{
  if (energyBinSimpleHistoMinerva < 0.) return;
   G4cout << "NumiAnalysis::finishPL() called." << G4endl;
   std::cerr << "NumiAnalysis::finishPL() called." << G4endl;
   
  // Study of the inclusive  numu spectrum for various magnetic field 
  // To speed the I/O and processing on the farm, we include here a fixed size
  // weighted histogram 
  std::string aDir("./");
  const char* aDirGG = getenv("_CONDOR_SCRATCH_DIR");
  if (aDirGG != 0) aDir = std::string(aDirGG);
  {
    std::string aNamePL(aDir); aNamePL += std::string("/FluxHistoPL.txt");
    std::ofstream fOutPLHisto(aNamePL.c_str());
    fOutPLHisto << " k e numu numub " << std::endl;
    G4RunManager* pRunManager = G4RunManager::GetRunManager();
     int NPots = pRunManager->GetCurrentRun()->GetNumberOfEventToBeProcessed();
    for (size_t k=0; k != MinervaNuMuHisto.size();  k++) {
       fOutPLHisto << " " << k << " " <<  (energyBinSimpleHistoMinerva/2. + energyBinSimpleHistoMinerva*k) 
                << " " << MinervaNuMuHisto[k]/NPots << " " 
		<< MinervaNuMuBarHisto[k]/NPots << std::endl;
    }
    fOutPLHisto.close();
  }
  {
    std::string aNamePL(aDir); aNamePL += std::string("/FluxHistoNovaPL.txt");
    std::ofstream fOutPLHisto(aNamePL.c_str());
    fOutPLHisto << " k e numuN numubN numuF numubF " << std::endl;
    G4RunManager* pRunManager = G4RunManager::GetRunManager();
    int NPots = pRunManager->GetCurrentRun()->GetNumberOfEventToBeProcessed();
    for (size_t k=0; k != NovaNearNuMuHisto.size();  k++) {
      fOutPLHisto << " " << k << " " <<  (0.05 + 0.1*k) 
                << " " << NovaNearNuMuHisto[k]/NPots << " "  << NovaNearNuMuBarHisto[k]/NPots
		<< " " << NovaFarNuMuHisto[k]/NPots << " " 
		<< NovaFarNuMuBarHisto[k]/NPots << std::endl;
    }
    fOutPLHisto.close();
  }
}
void NumiAnalysis::finish() {

   G4cout << "NumiAnalysis::finish() called." << G4endl;
   std::cerr << "NumiAnalysis::finish() called." << G4endl;
  
  if (NumiData->createNuNtuple){
    nuNtuple->cd();
    meta->Write(); //write dkmeta
    tree->Write();
    nuNtuple->Close();
    delete nuNtuple;
  }

  if (NumiData->createHadmmNtuple){
    std::cout<<"hadmm step 1"<<std::endl;
    hadmmNtuple->cd();
    std::cout<<"hadmm step 2"<<std::endl;
    hadmmtree->Write();
    std::cout<<"hadmm step 3"<<std::endl;
    hadmmNtuple->Close();
    std::cout<<"hadmm step 4"<<std::endl;
    delete hadmmNtuple;
    std::cout<<"hadmm step 5"<<std::endl;

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

  if (NumiData->createH1TrackingPlane){
    h1trackingNtuple->cd();
    h1trackingtree->Write();
    h1trackingNtuple->Close();
    delete h1trackingNtuple;
  }
}

void NumiAnalysis::FillMeta(){


  //This class fills dk2meta:
  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  int NPots = pRunManager->GetCurrentRun()->GetNumberOfEventToBeProcessed();

  G4String namentp = (NumiData->nuNtupleName);
  G4int pos_last = namentp.last('_');
  namentp.remove(0,pos_last+1);
  istringstream buffer(namentp);
  int valjob;
  buffer >> valjob;
  //if we are running interactevely (or more than 1000 jobs), we will write
  //job ID = -1
  if(valjob<0 || valjob>=1000)valjob=-1;

  NumiPrimaryGeneratorAction *NPGA = (NumiPrimaryGeneratorAction*)(pRunManager)->GetUserPrimaryGeneratorAction();
 
  this_meta->job  = valjob;
  this_meta->pots = NPots;

  //We are going to do this manually for now:
  //Perhaps should be part of NumiDataInput??

#ifdef USEMODGEANT4
  this_meta->beamsim = "g4numi_v6_MODGEANT4"; 
#else 
  this_meta->beamsim = "g4numi_v6";   
#endif

  this_meta->physics = "geant4_9_2_p03_FTFP_BERT1.0"; 
  this_meta->physcuts = "nofillyet";

  G4String hornC = NumiData->GetBeamConfig();
  G4String tgtC  = NumiData->GetBeamConfig();
  G4int confsize = (NumiData->GetBeamConfig()).length();
  hornC.remove(0,6);
  tgtC.remove(6,confsize);
  G4String playlist  = NumiData->GetPlaylist();
  
  G4bool isHe = NumiData->HeInDecayPipe;
  std::string decayVolMat = "vacuum";
  if(isHe)decayVolMat = "helium";
  this_meta->tgtcfg = std::string(tgtC+"_"+playlist);  
  this_meta->horncfg = std::string(hornC);  
  this_meta->dkvolcfg = decayVolMat;
  //////////////////////////////////////////
  
  this_meta->beam0x = NumiData->beamPosition[0]/cm;
  this_meta->beam0y = NumiData->beamPosition[1]/cm;
  this_meta->beam0z = NumiData->beamPosition[2]/cm;
  this_meta->beamhwidth = NumiData->beamSigmaX/cm;
  this_meta->beamvwidth = NumiData->beamSigmaY/cm;

  G4ThreeVector protonMomentum = NPGA->GetProtonMomentum();
  this_meta->beamdxdz = protonMomentum[0]/protonMomentum[2];
  this_meta->beamdydz = protonMomentum[1]/protonMomentum[2];

  G4double mm2cm = 0.1;
  vec_loc.push_back(bsim::Location(mm2cm*0.0,mm2cm*0.0,mm2cm*0.0,"random"));
  for (G4int ii=0; ii<NumiData->nNear; ++ii){
    G4double xxdet = mm2cm*(NumiData->xdet_near[ii]);
    G4double yydet = mm2cm*(NumiData->ydet_near[ii]);
    G4double zzdet = mm2cm*(NumiData->zdet_near[ii]);
    vec_loc.push_back(bsim::Location(xxdet,yydet,zzdet,NumiData->det_near_name[ii]));
  }
  for (G4int ii=0; ii<NumiData->nFar; ++ii){ 
    G4double xxdet = mm2cm*(NumiData->xdet_far[ii]);
    G4double yydet = mm2cm*(NumiData->ydet_far[ii]);
    G4double zzdet = mm2cm*(NumiData->zdet_far[ii]);
    vec_loc.push_back(bsim::Location(xxdet,yydet,zzdet,NumiData->det_far_name[ii]));
  }
		    
  this_meta->location = vec_loc;
  
  for(int ii=0;ii<NumiData->nVintTot;ii++){
    (this_meta->vintnames).push_back(NumiData->VolVintName[ii]);
  }
  for(int ii=0;ii<NumiData->nVdblTot;ii++){
    (this_meta->vdblnames).push_back(NumiData->VolVdblName[ii]);
  }
  if (NumiData->createNuNtuple) meta->Fill(); // If we don't have the Ntuple properly open, won't work.
  
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
      
     if(hmm_num == 5)
         {
           g4hmmdata->z2xpos = track.GetPosition()[0];
           g4hmmdata->z2ypos = track.GetPosition()[1];
           g4hmmdata->z2zpos = track.GetPosition()[2];
           g4hmmdata->z2px = track.GetMomentum()[0];
           g4hmmdata->z2py = track.GetMomentum()[1];
           g4hmmdata->z2pz = track.GetMomentum()[2];

      }
     if(hmm_num == 6)
         {
           g4hmmdata->z3xpos = track.GetPosition()[0];
           g4hmmdata->z3ypos = track.GetPosition()[1];
           g4hmmdata->z3zpos = track.GetPosition()[2];
           g4hmmdata->z3px = track.GetMomentum()[0];
           g4hmmdata->z3py = track.GetMomentum()[1];
           g4hmmdata->z3pz = track.GetMomentum()[2];

      }
     if(hmm_num == 7)
         {
           g4hmmdata->z4xpos = track.GetPosition()[0];
           g4hmmdata->z4ypos = track.GetPosition()[1];
           g4hmmdata->z4zpos = track.GetPosition()[2];
           g4hmmdata->z4px = track.GetMomentum()[0];
           g4hmmdata->z4py = track.GetMomentum()[1];
           g4hmmdata->z4pz = track.GetMomentum()[2];

      }

     if(hmm_num == 8)
         {
           g4hmmdata->z5xpos = track.GetPosition()[0];
           g4hmmdata->z5ypos = track.GetPosition()[1];
           g4hmmdata->z5zpos = track.GetPosition()[2];
           g4hmmdata->z5px = track.GetMomentum()[0];
           g4hmmdata->z5py = track.GetMomentum()[1];
           g4hmmdata->z5pz = track.GetMomentum()[2];

      }

     if(hmm_num == 9)
         {
           g4hmmdata->z6xpos = track.GetPosition()[0];
           g4hmmdata->z6ypos = track.GetPosition()[1];
           g4hmmdata->z6zpos = track.GetPosition()[2];
           g4hmmdata->z6px = track.GetMomentum()[0];
           g4hmmdata->z6py = track.GetMomentum()[1];
           g4hmmdata->z6pz = track.GetMomentum()[2];

      }
     if(hmm_num == 10)
         {
           g4hmmdata->z7xpos = track.GetPosition()[0];
           g4hmmdata->z7ypos = track.GetPosition()[1];
           g4hmmdata->z7zpos = track.GetPosition()[2];
           g4hmmdata->z7px = track.GetMomentum()[0];
           g4hmmdata->z7py = track.GetMomentum()[1];
           g4hmmdata->z7pz = track.GetMomentum()[2];

      }

      //}
      if(hmm_num != 4)
      {  
         if(fMuInAlc[hmm_num] == true) return;

         if( !(NumiData->GetMaterialSigma() < 0) && !(NumiData->GetMaterialSigma() > 0))
         {
            g4hmmdata->mmxpos[hmm_num] = track.GetPosition()[0];
            g4hmmdata->mmypos[hmm_num] = track.GetPosition()[1];
            g4hmmdata->mmzpos[hmm_num] = track.GetPosition()[2];
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
        g4hmmdata->muvz = particlePosition[2];

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

     g4hmmdata->protonx = NPGA->Getprotonx();
     g4hmmdata->protony = NPGA->Getprotony();
     g4hmmdata->protonz = NPGA->Getprotonz();

     for(int i = 0;i<13;++i)
       {
        g4hmmdata->nuray_px[i] = NPGA->GetNuray_px(i);
        g4hmmdata->nuray_py[i] = NPGA->GetNuray_py(i);
        g4hmmdata->nuray_pz[i] = NPGA->GetNuray_pz(i);
        g4hmmdata->nuray_E[i] = NPGA->GetNuray_E(i);
        g4hmmdata->nuray_wgt[i] = NPGA->GetNuray_wgt(i);

      } 
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
     //
       g4hmmdata->z1xpos = -99999;
       g4hmmdata->z1ypos = -99999;
       g4hmmdata->z1zpos = -99999;
       g4hmmdata->z1px = -99999;
       g4hmmdata->z1py = -99999;
       g4hmmdata->z1pz = -99999;

       g4hmmdata->z2xpos = -99999;
       g4hmmdata->z2ypos = -99999;
       g4hmmdata->z2zpos = -99999;
       g4hmmdata->z2px = -99999;
       g4hmmdata->z2py = -99999;
       g4hmmdata->z2pz = -99999;

       g4hmmdata->z3xpos = -99999;
       g4hmmdata->z3ypos = -99999;
       g4hmmdata->z3zpos = -99999;
       g4hmmdata->z3px = -99999;
       g4hmmdata->z3py = -99999;
       g4hmmdata->z3pz = -99999;

       g4hmmdata->z4xpos = -99999;
       g4hmmdata->z4ypos = -99999;
       g4hmmdata->z4zpos = -99999;
       g4hmmdata->z4px = -99999;
       g4hmmdata->z4py = -99999;
       g4hmmdata->z4pz = -99999;

       g4hmmdata->z5xpos = -99999;
       g4hmmdata->z5ypos = -99999;
       g4hmmdata->z5zpos = -99999;
       g4hmmdata->z5px = -99999;
       g4hmmdata->z5py = -99999;
       g4hmmdata->z5pz = -99999;

       g4hmmdata->z6xpos = -99999;
       g4hmmdata->z6ypos = -99999;
       g4hmmdata->z6zpos = -99999;
       g4hmmdata->z6px = -99999;
       g4hmmdata->z6py = -99999;
       g4hmmdata->z6pz = -99999;

       g4hmmdata->z7xpos = -99999;
       g4hmmdata->z7ypos = -99999;
       g4hmmdata->z7zpos = -99999;
       g4hmmdata->z7px = -99999;
       g4hmmdata->z7py = -99999;
       g4hmmdata->z7pz = -99999;
     
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
    
  G4String myHorn1Config =  NumiData->GetHornConfig();
  const NumiRunManager* theRunManager = reinterpret_cast<const NumiRunManager*>(NumiRunManager::GetRunManager());
  const NumiSteppingAction* theSteppingAction =
     reinterpret_cast<const NumiSteppingAction*>(theRunManager->GetUserSteppingAction());
  
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
  
  G4int tar_pdg = 0; //auxiliar variable to convert to PDG code
  G4int tar_trackId = -1;
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
		if (theSteppingAction->EscapingTarget(lastVolName, nextVolName)) {
                    // tv_ and tp_ are equal to position and
                    // momentum of the particle exiting the target (actually shell around target)
                    ParticleMomentum = PParentTrack->GetMomentum(ii);              
                    ParticlePosition = PParentTrack->GetPoint(ii)->GetPosition();  
                    tptype = NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(PParentTrack->GetParticleName()));
		    tar_pdg = PParentTrack->GetPDGEncoding();
		    tar_trackId = PParentTrack->GetTrackID();
                    findTarget = true;
//	            std::cerr << " NumiAnalysis::FillNeutrinoNtuple Find Target is true Position " 
//		              << ParticlePosition << " momentum " << ParticleMomentum << std::endl;
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
      if (theSteppingAction->EscapingTarget(prevolname, postvolname)) {
            g4data->trkx[1] = ParentPosition[0]/cm;
            g4data->trky[1] = ParentPosition[1]/cm;
            g4data->trkz[1] = ParentPosition[2]/cm;
            g4data->trkpx[1] = ParentMomentum[0]/GeV;
            g4data->trkpy[1] = ParentMomentum[1]/GeV;
            g4data->trkpz[1] = ParentMomentum[2]/GeV;
//	    std::cerr << " NumiAnalysis::FillNeutrinoNtuple filling point 1 ... " << ParentPosition << std::endl;
	 }
	    
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
  int idx_tar_in_chain = -1;

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

      //Searching for the index of the hadron that the target:
      //This is saved later in dk2nu:
      if(g4data->trackId[index] == tar_trackId){
	idx_tar_in_chain = int(index); 
      }

  }

  //For now, I write it by hand but I need to figure out how to do it automatically 
  //this values were extracted from NumiMaterials and checking the volumes definition 
  //of IC, DPIP and DVOL. the fact_X is mass number over density ( gr/mole / g /cm3 )
  //and looking at parent(0), grand-parent(1), great-gran-parent(2)
  // (Leo Aliaga. Feb18, 2015)

  const int Nanc = NumiData->nGenAbs;
//  std::cerr << " NumiAnalysis fill ntuple,  Nanc " << Nanc << std::endl;

  G4double fact_Al = (26.98) / (2.7); 
  G4double fact_Fe = (55.85) / (7.869990); 
  G4double fact_He = (4.003) / (0.000145); 
  G4double dist_IC1[3];
  G4double dist_IC2[3];
  G4double dist_DPIP[3];
  G4double dist_DVOL[3];
  
  if (Nanc > 3) {
    std::cerr << " NumiAnalysis::FillNeutrinoNtuple, Nanc greter than 3, Overwrite!... Quit here " << std::endl;
    exit(2);
  }
  
  //initializing:
  for(int ii=0; ii<Nanc;ii++){
    dist_IC1[ii]  = -1.0*fact_Al;
    dist_IC2[ii]  = -1.0*fact_Al;
    dist_DPIP[ii] = -1.0*fact_Fe;
    dist_DVOL[ii] = -1.0*fact_He;
  }
  //Getting values:
  NumiTrajectory* tmp_traj; 
  for(int ii=0;ii<Nanc;ii++){
    if(history.size()<=3 && ii==2)continue;
    tmp_traj = dynamic_cast<NumiTrajectory*>(tmpHistory.at(history.size()-(ii+2)));
    //If the Horn1 is alternate (See doc 10573), the name convention of the Horn1 IC volumes changes. 
    //Ths part of the code checks if the horn1 is alternate. If not, it assumes it is the nominal (Horn 1 
    //model we used in g4numi v5). 
    double tmp_dist_h1 = 0;
    if( NumiData->GetHorn1IsAlternate() ){
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1IOTransCont0_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1IOTransCont1_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1IOTransCont2_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1IOTransCont3_P");
      
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1UpstrSubSect0WeldUpstr_P");
      
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1UpstrSubSect0_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1UpstrSubSect1_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1UpstrSubSect2_P");
      
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1UpstrSubSect1Weld0_P");
      
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1ToNeckPartM0SubSect0_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1ToNeckPartM0SubSect1_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1ToNeckPartM0SubSect2_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1ToNeckPartM0SubSect3_P");
      
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1Neck_P");
      
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1DownstrPart1SubSect0_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1DownstrPart1SubSect1_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1DownstrPart1SubSect2_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1DownstrPart1SubSect3_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1DownstrPart1SubSect4_P");
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1DownstrPart1SubSect5_P");
      
      tmp_dist_h1 += GetDistanceInVolume(tmp_traj,"Horn1DownstrPart1Weld1_P");
    }
    else{
      tmp_dist_h1 = GetDistanceInVolume(tmp_traj,"PHorn1IC");
    }
    dist_IC1[ii]  = tmp_dist_h1;
    dist_IC2[ii]  = GetDistanceInVolume(tmp_traj,"PHorn2IC");
    dist_DPIP[ii] = GetDistanceInVolume(tmp_traj,"DPIP");
    dist_DVOL[ii] = GetDistanceInVolume(tmp_traj,"DVOL");
    
 }

  
  /////////////////////////////////////////////////////////////////////////
  //In this approach, I am not deleting any g4data variables.
  //Just copying or accomodating values for dk2nu:
  
  //For conversions:
  G4double mm2cm   = 0.1;
  G4double MeV2GeV = 0.001;

  //cleanning everything:
  vec_traj.clear();
  vec_nuray.clear();
  vec_ancestor.clear();
  vec_int.clear();
 vec_dbl.clear();

  //1) Ancestries:

#ifdef USEMODGEANT4 

  MinervaElementInter*  ei =  MinervaElementInter::getInstance();
  std::map<G4int,G4String> ei_map =  ei->GetInfo();
  std::map<G4int,G4String>::iterator ei_it;
  std::map<G4int,G4ThreeVector> pi_map =  ei->GetP();
  std::map<G4int,G4ThreeVector>::iterator pi_it;
  
#endif

  std::size_t ntrajectory = history.size();
  for (std::size_t index = 0; index < ntrajectory; ++index) {
      NumiTrajectory* traj = dynamic_cast<NumiTrajectory*>(tmpHistory.at(index));

      bsim::Ancestor tmp_ancestor;
      
      G4double startx = mm2cm*(traj->GetPoint(0)->GetPosition().x());
      G4double starty = mm2cm*(traj->GetPoint(0)->GetPosition().y());
      G4double startz = mm2cm*(traj->GetPoint(0)->GetPosition().z());  
      G4double localt = traj->GetTime();
      tmp_ancestor.SetStartXYZT(startx,starty,startz,localt);
      
      G4double startpx = MeV2GeV*(traj->GetMomentum(0).x());
      G4double startpy = MeV2GeV*(traj->GetMomentum(0).y());
      G4double startpz = MeV2GeV*(traj->GetMomentum(0).z());
      tmp_ancestor.SetStartP(startpx,startpy,startpz);
      
      tmp_ancestor.pdg = traj->GetPDGEncoding();

      const int lastPoint = traj->GetPointEntries()-1;
      
      G4double stoppx  = MeV2GeV*(traj->GetMomentum(lastPoint).x());
      G4double stoppy  = MeV2GeV*(traj->GetMomentum(lastPoint).y());
      G4double stoppz  = MeV2GeV*(traj->GetMomentum(lastPoint).z());
      tmp_ancestor.SetStopP(stoppx,stoppy,stoppz);

      G4double pprodpx = MeV2GeV*(traj->GetParentMomentumAtThisProduction().x());
      G4double pprodpy = MeV2GeV*(traj->GetParentMomentumAtThisProduction().y());
      G4double pprodpz = MeV2GeV*(traj->GetParentMomentumAtThisProduction().z());
      
      tmp_ancestor.proc    = traj->GetProcessName();
      tmp_ancestor.ivol    = traj->GetPreStepVolumeName(0);
      tmp_ancestor.imat    = "NotFillYet";      
      G4int elemID = 0;     

      tmp_ancestor.polx = 0.0;
      tmp_ancestor.poly = 0.0;
      tmp_ancestor.polz = 0.0;

#ifdef USEMODGEANT4 
      
      //filling inter. elem. and momuntum info:
      ei_it = ei_map.find(traj->GetTrackID());
      pi_it = pi_map.find(traj->GetTrackID());     
      
      if(ei_it != ei_map.end()){
	elemID = GetNucleus(ei_it->second);
	// if(elemID == 0) G4cout<< "NO FOUND IN THE TABLE "<< ei_it->second << G4endl;
      }
      else{
	//the final neutrino does not interact:
	elemID = 0;
      }
      
      if(pi_it != pi_map.end()){
	G4ThreeVector mom_inter = pi_it->second;
	pprodpx  = mom_inter.x()*MeV2GeV;
	pprodpy  = mom_inter.y()*MeV2GeV;
	pprodpz  = mom_inter.z()*MeV2GeV;
      } 
      
#endif

      tmp_ancestor.nucleus = elemID;
      tmp_ancestor.SetPProdP(pprodpx,pprodpy,pprodpz);

      vec_ancestor.push_back(tmp_ancestor);
  }
  /////

  //2) TGT Exit:
  
  this_tgtexit.tvx = g4data->tvx;
  this_tgtexit.tvy = g4data->tvy;
  this_tgtexit.tvz = g4data->tvz;
  this_tgtexit.tpx = g4data->tpx;
  this_tgtexit.tpy = g4data->tpy;
  this_tgtexit.tpz = g4data->tpz;
  this_tgtexit.tptype = tar_pdg;
  this_tgtexit.tgen   =g4data->tgen;

  //2.1)Storing integer values:

  //-) Storing the index of tar
  vec_int.push_back(idx_tar_in_chain);

  //-) We need to see how to do it better. We already store the playlsit in the meta tree 
  // but GENIE is not propagating dkmeta (just dk2nu) and then this playlist ID is going 
  // to know the playlist used.
  G4String playlist  = NumiData->GetPlaylist();
  int plID = -1;
  if(playlist=="downstream")plID = 0;
  if(playlist=="minerva1")  plID = 1;
  if(playlist=="minerva2")  plID = 2;
  if(playlist=="minerva3")  plID = 3;
  if(playlist=="minerva4")  plID = 4;
  if(playlist=="minerva5")  plID = 5;
  if(playlist=="minerva6")  plID = 6;
  if(playlist=="minerva7")  plID = 7;
  if(playlist=="minerva8")  plID = 8;
  if(playlist=="minerva9")  plID = 9;
  if(playlist=="minerva10") plID = 10;
  if(playlist=="minerva11") plID = 11;
  if(playlist=="minerva12") plID = 12;
  if(playlist=="minerva13") plID = 13;
  vec_int.push_back(plID);
  
  //2.2)Storing the distance by density of the particle in IC:
  for(int ii=0;ii<Nanc;ii++)vec_dbl.push_back(dist_IC1[ii]/fact_Al);
  for(int ii=0;ii<Nanc;ii++)vec_dbl.push_back(dist_IC2[ii]/fact_Al);
  for(int ii=0;ii<Nanc;ii++)vec_dbl.push_back(dist_DPIP[ii]/fact_Fe);
  for(int ii=0;ii<Nanc;ii++)vec_dbl.push_back(dist_DVOL[ii]/fact_He);

  /////
  

  //3) Trajectories (ray tracing):
  for (G4int ii=0; ii<10; ++ii){
    bsim::Traj tmp_traj;
    tmp_traj.trkx  = g4data->trkx[ii];
    tmp_traj.trky  = g4data->trky[ii];
    tmp_traj.trkz  = g4data->trkz[ii];
    tmp_traj.trkpx = g4data->trkpx[ii];
    tmp_traj.trkpy = g4data->trkpy[ii];
    tmp_traj.trkpz = g4data->trkpz[ii];
    vec_traj.push_back(tmp_traj);
  }
  ///////

  //4) Decay:
  this_decay.norig  = Norig;
  this_decay.ndecay = NuParentTrack->GetDecayCode();
  this_decay.ntype = particleType->GetPDGEncoding();
  this_decay.vx = x/cm;
  this_decay.vy = y/cm;
  this_decay.vz = z/cm;
  this_decay.pdpx     = g4data->pdPx;
  this_decay.pdpy     = g4data->pdPy;
  this_decay.pdpz     = g4data->pdPz;
  this_decay.ppdxdz   = ParentMomentumProduction[0]/ParentMomentumProduction[2];
  this_decay.ppdydz   = ParentMomentumProduction[1]/ParentMomentumProduction[2];
  this_decay.pppz     = ParentMomentumProduction[2]/GeV;
  this_decay.ppenergy = g4data->ppenergy;
  this_decay.ppmedium = int(g4data->ppmedium);
  this_decay.ptype    = NuParentTrack->GetPDGEncoding();
  this_decay.muparpx  = g4data->muparpx;  
  this_decay.muparpy  = g4data->muparpy;
  this_decay.muparpz  = g4data->muparpz;
  this_decay.mupare   = g4data->mupare;
  this_decay.necm     = g4data->Necm;
  this_decay.nimpwt   = g4data->Nimpwt;
  //////

  //5) NuRay:

  // Random decay:
  G4double RdecPx = NuMomentum[0]/GeV;
  G4double RdecPy = NuMomentum[1]/GeV;
  G4double RdecPz = NuMomentum[2]/GeV;
  G4double RdecE  = track.GetTotalEnergy()/GeV;
  bsim::NuRay tmp_nuray_random(RdecPx,RdecPy,RdecPz,RdecE,1.0);
  vec_nuray.push_back(tmp_nuray_random);
  
  //calculating again...
  //I will optimize this.

  //for near detectors: 
  for(G4int ii=0; ii<NumiData->nNear; ++ii){
    NumiNuWeight nuwgh;
    G4double nu_wght;
    G4double nu_energy;
    std::vector<double> r_det;
    r_det.push_back(NumiData->xdet_near[ii]/cm);
    r_det.push_back(NumiData->ydet_near[ii]/cm);
    r_det.push_back(NumiData->zdet_near[ii]/cm);
    nuwgh.GetWeight(g4data, r_det,nu_wght,nu_energy);
    G4double mom_nu[3];
    mom_nu[0] = (r_det[0]- g4data->Vx) * nu_energy / rad;
    mom_nu[1] = (r_det[1]- g4data->Vy) * nu_energy / rad;
    mom_nu[2] = (r_det[2]- g4data->Vz) * nu_energy / rad;
    bsim::NuRay tmp_nuray(mom_nu[0],mom_nu[1],mom_nu[2],nu_energy,nu_wght);
    vec_nuray.push_back(tmp_nuray);
    if ((energyBinSimpleHistoMinerva > 0.) && 
        (ii == 1) && (nu_energy > 0.)) { // 2nd detector is centered on axis. We
      size_t iBin = nu_energy/energyBinSimpleHistoMinerva;
      if ((iBin < MinervaNuMuHisto.size()) &&  (particleType->GetPDGEncoding() == 14))
        MinervaNuMuHisto[iBin] += nu_wght*g4data->Nimpwt;
      if ((iBin < MinervaNuMuBarHisto.size()) &&  (particleType->GetPDGEncoding() == -14))
        MinervaNuMuBarHisto[iBin] += nu_wght*g4data->Nimpwt;
    }	 	      
    if ((energyBinSimpleHistoMinerva > 0.) && 
        (ii == 2) && (nu_energy > 0.)) { // Nova Near Detector off axis.. 
      size_t iBin = nu_energy/energyBinSimpleHistoMinerva;
      if ((iBin < NovaNearNuMuHisto.size()) &&  (particleType->GetPDGEncoding() == 14))
        NovaNearNuMuHisto[iBin] += nu_wght*g4data->Nimpwt;
      if ((iBin < NovaNearNuMuBarHisto.size()) &&  (particleType->GetPDGEncoding() == -14))
        NovaNearNuMuBarHisto[iBin] += nu_wght*g4data->Nimpwt;
    }	 	      
  }
  //for far detectors: 
  for(G4int ii=0; ii<NumiData->nFar; ++ii){      
      NumiNuWeight nuwgh;
      G4double nu_wght;
      G4double nu_energy;
      std::vector<double> r_det;
      r_det.push_back(NumiData->xdet_far[ii]/cm);
      r_det.push_back(NumiData->ydet_far[ii]/cm);
      r_det.push_back(NumiData->zdet_far[ii]/cm);
      nuwgh.GetWeight(g4data, r_det,nu_wght,nu_energy);
      G4double mom_nu[3];
      mom_nu[0] = (r_det[0]- g4data->Vx) * nu_energy / rad;
      mom_nu[1] = (r_det[1]- g4data->Vy) * nu_energy / rad;
      mom_nu[2] = (r_det[2]- g4data->Vz) * nu_energy / rad;
      bsim::NuRay tmp_nuray(mom_nu[0],mom_nu[1],mom_nu[2],nu_energy,nu_wght);
      vec_nuray.push_back(tmp_nuray);      
      if ((energyBinSimpleHistoMinerva > 0.) && 
          (ii == 1) && (nu_energy > 0.)) { // Nova Near Detector off axis.. 
        size_t iBin = nu_energy/0.1;
        if ((iBin < NovaFarNuMuHisto.size()) &&  (particleType->GetPDGEncoding() == 14))
          NovaFarNuMuHisto[iBin] += nu_wght*g4data->Nimpwt;
        if ((iBin < NovaFarNuMuBarHisto.size()) &&  (particleType->GetPDGEncoding() == -14))
          NovaFarNuMuBarHisto[iBin] += nu_wght*g4data->Nimpwt;
      }	 	      
  }
  ////
  //6) Others:

  //calculating the job number:
  G4String namentp = (NumiData->nuNtupleName);
  G4int namesize = (NumiData->nuNtupleName).last('_');
  namentp.remove(0,namesize+1);
  istringstream buffer(namentp);
  int valjob;
  buffer >> valjob;
  this_dk2nu->job = valjob;
  
  this_dk2nu->potnum = g4data->evtno;
  this_dk2nu->ppvx = g4data->ppvx;
  this_dk2nu->ppvy = g4data->ppvy;
  this_dk2nu->ppvz = g4data->ppvz;

  //Placing in dk2nu:
  this_dk2nu->ancestor = vec_ancestor;
  this_dk2nu->tgtexit = this_tgtexit;
  this_dk2nu->traj = vec_traj;
  this_dk2nu->decay = this_decay;
  this_dk2nu->nuray = vec_nuray;
  this_dk2nu->vint = vec_int;  //Storing the vint
  this_dk2nu->vdbl = vec_dbl; //Storing the vdbl

  /////////////////////////////////////////////////////////////////////////

  
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
//prachi start
void NumiAnalysis::FillH1Ntuple(const G4Step& step)
{
  if(!NumiData->createH1TrackingPlane) return ;

  G4RunManager* pRunManager = G4RunManager::GetRunManager();
     
     G4Track *track=step.GetTrack();
     
     NumiTrackInformation* info = (NumiTrackInformation*)(track->GetUserInformation());
     g4h1trackingdata->impwt = info->GetNImpWt(); 
     g4h1trackingdata->tvx= track->GetPosition()[0]/cm;
     g4h1trackingdata->tvy= track->GetPosition()[1]/cm;
     g4h1trackingdata->tvz= track->GetPosition()[2]/cm;
     g4h1trackingdata->tpx= track->GetMomentum()[0]/GeV;
     g4h1trackingdata->tpy= track->GetMomentum()[1]/GeV;
     g4h1trackingdata->tpz= track->GetMomentum()[2]/GeV;
        
     g4h1trackingdata->tof = track->GetGlobalTime();
   
    G4ParticleDefinition* particleDefinition = track->GetDefinition(); 
    g4h1trackingdata->tptype= NumiParticleCode::AsInt(NumiParticleCode::StringToEnum(particleDefinition->GetParticleName()));

    g4h1trackingdata->run=pRunManager->GetCurrentRun()->GetRunID();
    g4h1trackingdata->evtno = pRunManager->GetCurrentEvent()->GetEventID();
    
     //calculating the job number:
     G4String namentp = (NumiData->h1trackingNtupleName);
     G4int namesize = (NumiData->h1trackingNtupleName).last('_');
     namentp.remove(0,namesize+1);
     istringstream buffer(namentp);
     int valjob;
     buffer >> valjob;
     g4h1trackingdata->job = valjob;

  
    //parent particle production
     g4h1trackingdata->ppvx = track->GetVertexPosition()[0]/cm;
     g4h1trackingdata->ppvy = track->GetVertexPosition()[1]/cm;
     g4h1trackingdata->ppvz = track->GetVertexPosition()[2]/cm;  

    

    Writeh1trackingplaneNtuple();//store info and initialize
}

//------------------------------------------------------------------------------------
void NumiAnalysis::Writeh1trackingplaneNtuple(){

  h1trackingtree->Fill();
    g4h1trackingdata->impwt=-10000;
    g4h1trackingdata->tvx=-10000;
    g4h1trackingdata->tvy=-10000;
    g4h1trackingdata->tvz=-10000;
    g4h1trackingdata->tpx=-10000;
    g4h1trackingdata->tpy=-10000;
    g4h1trackingdata->tpz=-10000;
    g4h1trackingdata->tptype=-10;
    g4h1trackingdata->run=-100;
    g4h1trackingdata->evtno=-100;  
}

//prachi end 
//----------------------------
G4int NumiAnalysis::GetNucleus(G4String nucl_name){
  
  //The new dk2nu store the nucleus element as integer.
  //We have the element name from our custumized geant4. 
  //Form now, I created this map will all elements in g4numi:
  //I am contructing the pdg code based on the convention:
  //+-10LZZZAAAI, where L is the number of s quarks, 
  //I is the isomer id (both set to zero).
  G4int id = 0;

  if(nucl_name == "Hydrogen")        return  1000010010;
  if(nucl_name == "Helium")          return  1000020040;
  if(nucl_name == "Carbon")          return  1000060120;
  if(nucl_name == "Nitrogen")        return  1000070140;
  if(nucl_name == "Oxygen")          return  1000080160;
  if(nucl_name == "Natrium")         return  1000110230;
  if(nucl_name == "Magnesium")       return  1000120240;
  if(nucl_name == "Aluminum")        return  1000130270;
  if(nucl_name == "Silicon")         return  1000140280;
  if(nucl_name == "Phosphorus")      return  1000150310;
  if(nucl_name == "Sulfur")          return  1000160320;
  if(nucl_name == "Argon")           return  1000180400;
  if(nucl_name == "Potassium")       return  1000190390;
  if(nucl_name == "Calcium")         return  1000200400;
  if(nucl_name == "Titanium")        return  1000220480;
  if(nucl_name == "Chromium")        return  1000240520;
  if(nucl_name == "Manganese")       return  1000250550;
  if(nucl_name == "Iron")            return  1000260560;
  if(nucl_name == "Nickel")          return  1000280590;
  if(nucl_name == "Copper")          return  1000290640;
  if(nucl_name == "Gallium")         return  1000310700;
  if(nucl_name == "Mercury")         return  1000802010; 
  if(nucl_name == "Sodium")          return  1000110230;
  if(nucl_name == "Phospho")         return  1000150310;
  if(nucl_name == "Berylliu")        return  1000040090;
  if(nucl_name == "Berillium")       return  1000040090;
  if(nucl_name == "Target")          return  1000060120;
  if(nucl_name == "Lead")            return  1000822070;
  if(nucl_name == "SecMonitorHelium")return  1000020040;
  if(nucl_name == "TargetHelium")    return  1000020040;
  if(nucl_name == "HeGas")           return  1000020040;

  return id;

}

//----------------------------
G4double NumiAnalysis::GetDistanceInVolume(NumiTrajectory* wanted_traj, G4String wanted_vol){
  double dist_vol = 0;
  if(wanted_traj==0)return -1.;
  
  G4ThreeVector ParticlePos;
  G4int npoints = GetParentTrajectory(wanted_traj->GetTrackID())->GetPointEntries();
  
  G4ThreeVector tmp_ipos,tmp_fpos;
  G4ThreeVector tmp_disp;
  
  G4double tmp_dist = 0.0;
  G4bool enter_vol = false;
  G4bool exit_vol  = false;
  for(G4int ii=0; ii<npoints; ++ii){ 
    ParticlePos = (wanted_traj->GetPoint(ii)->GetPosition()/m)*m;
    G4String postvol = "";
    G4String prevol  = wanted_traj->GetPreStepVolumeName(ii);
    if(ii<npoints-1) postvol = wanted_traj->GetPreStepVolumeName(ii+1);

    G4bool vol_in  = ( (prevol != wanted_vol) && (postvol == wanted_vol) ) || ( ii==0 && prevol== wanted_vol);
    G4bool vol_out = (prevol == wanted_vol) && (postvol != wanted_vol);
    if(vol_in){	
      enter_vol = true;
      exit_vol  = false;
      tmp_ipos = G4ThreeVector(ParticlePos[0]/cm,ParticlePos[1]/cm,ParticlePos[2]/cm);
      tmp_dist = 0.0;
    }
    if(enter_vol && !exit_vol){
      tmp_fpos = G4ThreeVector(ParticlePos[0]/cm,ParticlePos[1]/cm,ParticlePos[2]/cm);
      tmp_disp = tmp_fpos - tmp_ipos;
      tmp_dist += tmp_disp.mag();
      tmp_ipos = tmp_fpos;
    }
    if(enter_vol && vol_out){
      tmp_fpos  = G4ThreeVector(ParticlePos[0]/cm,ParticlePos[1]/cm,ParticlePos[2]/cm);
      tmp_disp  = tmp_fpos - tmp_ipos;
      tmp_dist += tmp_disp.mag();
      tmp_ipos  = tmp_fpos;
      exit_vol  = true;
      enter_vol = false;
      dist_vol += tmp_dist;
    }
  }

  return dist_vol;
  
}
