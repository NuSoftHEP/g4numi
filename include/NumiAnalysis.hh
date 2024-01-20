//
// NumiAnalysis.hh
//
// Modified Jul 2005 by A. MArino to make data_t and hadmmtuple_t classes

#ifndef NUMIANALYSIS_HH
#define NUMIANALYSIS_HH

#include "globals.hh"
#include "NumiTrajectory.hh"
#include "NumiDataInput.hh"

//root
#include "TSystem.h"

//G4
#include "G4ios.hh"
#include "G4TrajectoryContainer.hh"

#include <map>
#include <vector>

class G4ParticleDefinition;
class G4Step;
class TFile;
class TTree;
class G4Track;
class G4VTrajectory;
class data_t;
class hadmmtuple_t;
class draytupleMIB_t;
class draytupleSPB_t;
class absbkgtuple_t;
class zptuple_t;
class h1trackingplane_exit_t;

//New for DK2NU:///
#include "dkmeta.h"
#include "dk2nu.h"
class bsim::Location;
class bsim::DkMeta;
class bsim::NuRay;
class bsim::Decay;
class bsim::Ancestor;
class bsim::TgtExit;
class bsim::Traj;
class bsim::Dk2Nu;
///////////////////

class NumiAnalysis
{
public:

   NumiAnalysis();
   ~NumiAnalysis();
   
   void book();
   void finish();
   void finishPL();
   void FillMeta(); //new function to fill dk2meta
   void FillNeutrinoNtuple(const G4Track& track,
                           const std::vector<G4VTrajectory*>& nuHistory);
   G4int GetNucleus(G4String nucl_name); //new function to get target element
   void FillHadmmNtuple(const G4Track& track,Int_t mm_num,Int_t cellNum);
   void FillHadmmNtuple();
   void FillBXDRAW(const G4Step* aStep);
   void FillAlcEdepInfo(const G4Track& track, G4int alc);
   void FillEdep(G4double edep, const G4ParticleDefinition* particleDefinition,
                 const G4int alc, const G4int IntExt, const G4double zpos,
                 Int_t cellNum, const Int_t trackID, const G4double weight);
   void FillAbsorberBkgrdNtuple(const G4Track& track);
   void WriteHadmmNtuple();
   
   void FillZpNtuple();
   void FillZpNtuple(const G4Track& track, Int_t zpnum);
   void WriteZpNtuple();
   
   void FillH1Ntuple();
   void FillH1Ntuple(const G4Step& track);
   void Writeh1trackingplaneNtuple();

   NumiTrajectory* GetParentTrajectory(G4int parentID);
   static NumiAnalysis* getInstance();
   
   void SetAlcEdepFlag(G4bool AlcEdep);
   void SetMuInAlcFlag(G4bool MuInAlc);
   
   void SetCount(G4int count);
   G4int GetCount();
   void SetEntry(G4int entry);
   G4int GetEntry();
   G4double GetDistanceInVolume(NumiTrajectory* wanted_traj, G4String wanted_vol);
private:
   std::string GetOFileName(std::string ifilename);
   
private:
   static NumiAnalysis* instance;
   
   G4double x;
   G4double y;
   G4double z;

   G4double noProtons;
   char asciiFileName[50], nuNtupleFileName[50], hadmmNtupleFileName[50], h1NtupleFileName[50];
   char zpNtupleFileName[50], bxdrawFileName[50];
   std::map<int, int> code;
   
   NumiDataInput* NumiData;
   
   TFile* hadmmNtuple;
   TFile* absbkgNtuple;
   TFile* nuNtuple;
   TFile* zpNtuple;
   TFile* h1trackingNtuple;
   
   TTree* tree;
   TTree* hadmmtree;
   TTree* absbkgtree;
   TTree* zptree;
   TTree* h1trackingtree;
   
   data_t *g4data;
   hadmmtuple_t *g4hmmdata;
   draytupleMIB_t *g4draydataMIB;
   draytupleSPB_t *g4draydataSPB;
   absbkgtuple_t *g4absbkgdata;
   zptuple_t* g4zpdata; 
   h1trackingplane_exit_t *g4h1trackingdata;
   
   G4int fcount;
   G4int fentry;
   
   std::vector<bool> fAlcEdep_called;
   std::vector<bool> fMuInAlc;
   
   //New fr DK2NU:////////////////////////
  std::vector<bsim::Location> vec_loc;
  bsim::DkMeta*  this_meta;
  TTree* meta;

  bsim::Dk2Nu* this_dk2nu;
  bsim::Decay this_decay; 
  bsim::TgtExit this_tgtexit; 
  std::vector<bsim::Traj> vec_traj;
  std::vector<bsim::NuRay> vec_nuray;   
  std::vector<bsim::Ancestor> vec_ancestor;
  std::vector<int> vec_int;
  std::vector<double> vec_dbl;
  //////////////////////////////////////////
  // September 2017 Paul Lebrun.. 
  // Study of the inclusive  numu spectrum for various magnetic field 
  // To speed the I/O and processing on the farm, we include here a fixed size
  // weighted histogram 
  double energyBinSimpleHistoMinerva; // depend on the LE vs ME configuration.. 
  std::vector<double> MinervaNuMuHisto;
  std::vector<double> MinervaNuMuBarHisto;
  std::vector<double> NovaFarNuMuHisto;
  std::vector<double> NovaNearNuMuHisto;
  std::vector<double> NovaFarNuMuBarHisto;
  std::vector<double> NovaNearNuMuBarHisto;
};

#endif 
