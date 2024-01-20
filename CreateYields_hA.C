#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

using std::string;
using std::cout;

// ---------- CONSTANTS ---------- //

// pT binning

const UInt_t  PT_BINS  = 80;     // number of pT bins
const Float_t PT_MIN   = 0.0125; // minimum value of pT
const Float_t PT_MAX   = 2.0125; // maximum value of pT

// xF binning

const UInt_t   XF_BINS   = 201;     // number of xF bins
const Float_t  XF_MIN    = -0.1025; // minimum value of xF
const Float_t  XF_MAX    = 0.9025;  // maximum value of xf
const Double_t XF_WIDTH  = (XF_MAX - XF_MIN) / XF_BINS; // xf bin width
const Double_t XF_HWIDH  = XF_WIDTH / 2.0;              // half xf bin width

// xF binning for nucleons xF vs pt 2D histogram 

const UInt_t  XF_BINS_NUCLEON = 351;     // number of xF bins (nucleon binning)
const Float_t XF_MIN_NUCLEON  = -0.8025; // minimum xF value (nucleon binning)
const Float_t XF_MAX_NUCLEON  = 0.9525;  // maximum xF value (nucleon binning)

// xF binning for neutron xF histograms

const UInt_t   XF_BINS_NEUTRON = 100; // number of xF bins (neutron xF histos)
const Double_t XF_MIN_NEUTRON  = 0.0; // minimum value of xF (neutron xF histos)
const Double_t XF_MAX_NEUTRON  = 1.0; // maximum value of xF (neutron xF histos)

// xF cut for avg number of pion (with xf in range) per production event

const Double_t XF_CUT_DOWN = -0.1; // minimum value of xF (avg no. of pion)
const Double_t XF_CUT_UP   = 0.5;  // maximum value of XF (avg no. of pion)

// particles

const Int_t  N_PARTICLES = 8;  // number of particles to create histograms for
const string PARTICLE_SYMBOL [N_PARTICLES] = // particles abbreviations
             {"pip", "pim", "kap", "kam", "klong", "kshort", "prt", "neu"};
const Int_t  PARTICLE_PDG [N_PARTICLES]    = // particles pdg
             {211, -211, 321, -321, 130, 310, 2212, 2112};

// other

const UInt_t MAX_PARTICLES = 150; // maximum number of particle in FS

// ---------- INLINE FUNCTIONS ---------- //

// get number of bin for given xF value
inline Int_t getXfBin (const Double_t &xf)
{
  return (xf < XF_MIN || xf >= XF_MAX) ? -1 : Int_t((xf - XF_MIN) / XF_WIDTH);
}

// get xF value for given bin
inline Double_t getXfValue (const Int_t &bin)
{
  return XF_MIN + XF_WIDTH * (0.5 + bin);
}

// check if particle == meson
inline Bool_t isMeson (const Int_t &pdg)
{
  return pdg == 211 || pdg == -211 || pdg == 111 ||               // pion
         pdg == 321 || pdg == -321 || pdg == 310 || pdg == 130;   // kaon
}

// check if particle == nucleon
inline Bool_t isNucleon (const Int_t &pdg)
{
  return pdg == 2212 || pdg == 2112;
}

// return index of a particle in pdgPart table
inline Int_t getParticleIndex (const Int_t &pdg)
{
  for (Int_t i = 0; i < N_PARTICLES; i++) if (pdg == PARTICLE_PDG[i]) return i;
  return -1;
}

inline Bool_t xfInRange (const Double_t &xf)
{
  return (xf > XF_MIN && xf < XF_MAX);
}

// ---------- DECLARATIONS ---------- // 

struct g4na49Config; // store for g4na49 simulation setup

// open output root file (name based on simulation config), prepare folders
Bool_t initializeOutputFile (const string &inputFiles);

// read g4na49 simulations setups from input files and check their compatibility
Bool_t processConfig (const TChain *const simConfig, g4na49Config &conf);

// prepare histograms for yields
void initializeHistograms (TH1D *pt[XF_BINS][N_PARTICLES],
                           TH2D *xfPt[N_PARTICLES]);

// ---------- MAIN FUNCTION ---------- //

TFile *outputFile = NULL; // output root files with yields
string workingDir = ".";  // default input/output dir

// create yields from g4na49 output root files (intput_files)
// include_ff -> count particles from fast decays (e.g. eta)
// use current dir for input/output or $G4NA49_ROOTDIR if defined
void CreateYields (const string inputFiles, const Bool_t includeFF = true)
{
  // use $G4NA49_ROOTDIR as working dir (if defined)
  if (getenv("G4NA49_ROOTDIR")) workingDir = getenv ("G4NA49_ROOTDIR");
  
  // prepare output file (name is based on g4na49 configuration)
  // stop if outputFile was not created
  if (!initializeOutputFile (workingDir + "/" + inputFiles)) return;
  
  // load g4na49 results from input files
  TChain *ntuple = new TChain ("hAinfo");
  ntuple->Add ((workingDir + "/" + inputFiles).c_str()); 
  
  TH1D*   yieldPt [XF_BINS][N_PARTICLES]; // pT histogram for given xF range
  TH2D* yieldXfPt [N_PARTICLES];          // xF vs pT histogram

  initializeHistograms (yieldPt, yieldXfPt);

  // xF histograms for neutron yields
  TH1D* yieldXfNeutron        = new TH1D ("dndxf_neu",
                                "; x_{F}; dn/dx_{F} for neutrons",
                               XF_BINS_NEUTRON, XF_MIN_NEUTRON, XF_MAX_NEUTRON);
  TH1D* yieldXfNeutronCut     = new TH1D ("dndxf_neu_cut",
                                "; x_{F}; dn/dx_{F} for neutrons",
                               XF_BINS_NEUTRON, XF_MIN_NEUTRON, XF_MAX_NEUTRON);
  TH1D* yieldXfNeutronProd    = new TH1D ("dndxf_neu_prod",
                                "; x_{F}; dn/dx_{F} for neutrons",
                               XF_BINS_NEUTRON, XF_MIN_NEUTRON, XF_MAX_NEUTRON);
  TH1D* yieldXfNeutronProdCut = new TH1D ("dndxf_neu_prod_cut",
                                "; x_{F}; dn/dx_{F} for neutrons",
                               XF_BINS_NEUTRON, XF_MIN_NEUTRON, XF_MAX_NEUTRON);

  // variables
  Int_t    nParticles;          // no. of particle in FS
  Int_t    pdg [MAX_PARTICLES]; // final particles PDG
  Double_t  xf [MAX_PARTICLES]; // feynman scaling vaiable
  Double_t  pt [MAX_PARTICLES]; // transverse momentum
  Bool_t    ff [MAX_PARTICLES]; // particles from fast decays

  // link variables to proper branch
  ntuple->SetBranchAddress ("npart", &nParticles);
  ntuple->SetBranchAddress ("pdg", pdg);
  ntuple->SetBranchAddress ("xf", xf);
  ntuple->SetBranchAddress ("pt", pt);   
  ntuple->SetBranchAddress ("ff", ff);   

  // counters
  Int_t nElasticEvents    = 0; // 1 nucleon + 1 fragment + 0 mesons
  Int_t nQElasticEvents   = 0; // 2 nucleons + <= 1 fragment + 0 mesons
  Int_t nProductionEvents = 0; // >0 mesons
  Int_t nFragmentsEvents  = 0; // >0 nucleons + >1 fragments + 0 mesons
                               // or >2 nucleons + >0 frgaments + 0 mesons
       
  Long64_t nPip = 0; // total number of positive pions
  Long64_t nPim = 0; // total number of negative pions

  const ULong_t nEntries = (ULong_t)ntuple->GetEntries(); // no. of entries

  for (ULong_t ie = 0; ie < nEntries; ie++)               // loop over entries
  {
    const Int_t nBytes = ntuple->GetEntry (ie);

    if (nBytes == 0)
    {
      cout << "\nEntry " << ie << " does not exist.\n";
      continue;
    }
    else if (nBytes == -1)
    {
      cout << "\nEntry " << ie << " is corrupted.\n";
      continue;
    }

    Int_t nMesons    = 0; // no. of pions and kaons in FS
    Int_t nNucleons  = 0; // no. of nucleons in FS
    Int_t nFragments = 0; // no. of fragments in FS

    Bool_t isElastic    = false; // flag: elastic
    Bool_t isQElastic   = false; // flag: quasi-elastic
    Bool_t isProduction = false; // flag: production 
    Bool_t isFragments  = false; // flag: fragments

    for (UInt_t ip = 0; ip < nParticles; ip++) // particle loop (count them)
    {      
      if      (isMeson (pdg[ip]))    nMesons++;
      else if (isNucleon (pdg[ip]))  nNucleons++;
      else if (pdg[ip] > 1000000000) nFragments++;
    }
  
    // flag the event    
    if (nMesons > 0) // meson production
    {
      nProductionEvents++;
      isProduction = true;
    }  
    else if (nFragments == 1 && nNucleons == 1) // elastic
    {
      nElasticEvents++;
      isElastic = true;
    }
    else if (nNucleons == 2 && nFragments <= 1) // quasi-elastic
    {
      // sometimes QEL-like events have a nuclear fragment, like C11 in the FS
      // sometimes not
      nQElasticEvents++;
      isQElastic = true;
    }      
    else if ((nFragments > 1 && nNucleons > 0) ||
             (nFragments > 0 && nNucleons > 2))
    {
      // events with multiple nuclear fragments in the FS
      // or events with more than 2 nucleons and one or more fragments
      nFragmentsEvents++; 
      isFragments = true;
    }
    else ntuple->Show (ie); // should never happen
    
    for (UInt_t ip = 0; ip < nParticles; ip++) // particle loop (fill histos)
    {
      // skip particles from fast decays (eta,eta') if includeFF = false
      if (!includeFF && ff[ip] == kTRUE) continue;

      Double_t ptValue = pt[ip] / 1000.; // pT of ip-th particle [in GeV]
      Double_t xfValue = xf[ip];         // xF of ip-th particle

      UInt_t iXf = getXfBin (xfValue);              // xf histo index
      Int_t iParticle = getParticleIndex (pdg[ip]); // particle histo index

      if (iParticle < 0) continue; // skip particle not defined in PARTICLE_PDG
      
      // fill yield histograms with meson production events
      if(isProduction)
      {
        yieldXfPt[iParticle]->Fill (xfValue, ptValue);
        // fill pt histo if xf in range
        if (xfInRange (xfValue)) yieldPt[iXf][iParticle]->Fill (ptValue);
      }

      // count pi+ / pi- for given xF cut
      if(xfValue > XF_CUT_DOWN && xfValue < XF_CUT_DOWN)
      {
        if      (pdg[ip] == 211)  nPip++;
        else if (pdg[ip] == -211) nPim++;
      }

      // fill neutron yields histograms
      if (pdg[ip] == 2112)
      {
        yieldXfNeutron->Fill (xfValue);
        if (isProduction) yieldXfNeutronProd->Fill (xfValue);

        double A = 0.398, B = 4.315; // pt < A+B*xF for NA49 neutron acceptance

        if (ptValue < A + B * xfValue)
        {
          yieldXfNeutronCut->Fill (xfValue);
          if (isProduction) yieldXfNeutronProdCut->Fill (xfValue);
        }
      }
    } // end of particle loop
    
    cout << "Processing " << workingDir + "/" + inputFiles << ": "
         << int (100.0 * ie / nEntries) << "% \r" << flush;

  } // end of event loop

  cout << "Processing " << workingDir + "/" + inputFiles << ": done\n";

  // open a txt tile to save some stats
  ofstream qeInfo ((string (outputFile->GetName()) + ".QEinfo").c_str());

  qeInfo << "Processing " << ntuple->GetNtrees() << " trees in the chain\n\n";

  qeInfo << setw (15) << "#Nentries" << setw (15) << "el_like" << setw (15)
         << "qe_like" << setw (15) << "frag_like" << setw (15)
         << "prod_entries\n";

  qeInfo << setw (15) << nEntries << setw (15) << nElasticEvents << setw (15)
         << nQElasticEvents << setw (15) << nFragmentsEvents << setw (15)
         << nProductionEvents << "\n\n";
         
  qeInfo << "average pi+ multiplicity per production event: "
         << double(nPip) / nProductionEvents << "\n";
  qeInfo << "average pi- multiplicity per production event: "
         << double(nPim) / nProductionEvents << "\n";
  qeInfo.close();
  
  outputFile->Write(); // save histograms
  outputFile->Close(); // close output file
}

// ---------- DEFINITIONS ---------- //

// open output root file (name based on simulation config), prepare folders
Bool_t initializeOutputFile (const string &inputFiles)
{
  // read g4na49 settings from input files
  TChain *const simConfig = new TChain ("setup");
  simConfig->Add (inputFiles.c_str());

  g4na49Config conf; // store for g4na49 simulation setup

  // determine particle, target and no. of events (based on input files)
  // stop script if input files contain different particle/energy/target
  if (!processConfig (simConfig, conf)) return false;

  // create output file (name is based on g4na49 configuration)
  outputFile = new TFile (Form ("%s/Yields_%s_%.2f_%s.root", workingDir.c_str(),
                          conf.particleName.c_str(), conf.particleEnergy,
                          conf.targetName.c_str()), "RECREATE");

  if (!outputFile) // stop if outputFile was not created
  {
    cout << "\nERROR: Output file: ";
    printf ("%s/Yields_%s_%.2f_%s.root", workingDir.c_str(),
            conf.particleName.c_str(), conf.particleEnergy,
            conf.targetName.c_str());
    cout << " could not be created.\n\n";
    return false;
  }
  
  // save g4na49 settings
  const TTree *const confTree = conf.createTree();
  confTree->Write();

  // create folders for each particle
  for (Int_t i = 0; i < N_PARTICLES; i++) outputFile->mkdir (PARTICLE_SYMBOL[i].c_str());

  return true;
}

// read g4na49 simulations setups from input files and check their compatibility
Bool_t processConfig (const TChain *const simConfig, g4na49Config &conf)
{
  g4na49Config tempConf; // temporary store for g4na49 simulation setup

  // workaround to pass string from struct to SetBranchAddress
  string *particle = &tempConf.particleName;
  string *target   = &tempConf.targetName;
  
  // connect branches with proper variables
  simConfig->SetBranchAddress ("particle", &particle);
  simConfig->SetBranchAddress ("energy",&tempConf.particleEnergy);
  simConfig->SetBranchAddress ("target",&target);
  simConfig->SetBranchAddress ("A",&tempConf.targetA);
  simConfig->SetBranchAddress ("Z",&tempConf.targetZ);
  simConfig->SetBranchAddress ("density",&tempConf.targetDensity);
  simConfig->SetBranchAddress ("nof_events",&tempConf.nEvents);

  // loop over entries (each entry = input file)
  for (UInt_t i = 0; i < simConfig->GetEntries(); i++)
  {
    simConfig->GetEntry (i);
    
    // assign variables values to first entry values
    // check if particle, energy and target are the same as for first entry/file
    // increase number of events or stop
    if (i == 0) conf.set (tempConf); 
    else if (conf.isSame (tempConf)) conf.nEvents += tempConf.nEvents;
    else
    {
      cout << "\nERROR: Input files contain simulations for different "
           << "particles and/or energies and/or targets.\n\n";

      return false;
    }
  }

  particle = NULL;
  target = NULL;

  return true;
}

// prepare histograms for yields
void initializeHistograms (TH1D *pt[XF_BINS][N_PARTICLES],
                           TH2D *xfPt[N_PARTICLES])
{
  // pT distribution
  for(UInt_t ih = 0; ih < XF_BINS; ih++) // loop over xF bins
  {
    Double_t xf = getXfValue (ih); // determine xF value for given bin

    for (UInt_t ip = 0; ip < N_PARTICLES; ip++) // loop over particles
    {
      outputFile->cd (PARTICLE_SYMBOL[ip].c_str()); // cd into particle folder
       
      pt[ih][ip] = new TH1D (Form ("pT_for_xF%03d_%s", ih,
                                   PARTICLE_SYMBOL[ip].c_str()),
                             Form ("Yield: %f < x_{F} < %f; p_{T}; yield",
                                   xf - XF_HWIDH, xf + XF_HWIDH),
                                   PT_BINS, PT_MIN, PT_MAX);
    }
  }

  outputFile->cd (0); // cd into main folder

  // xF vs pT distribution
  for (UInt_t i = 0; i < N_PARTICLES; i++) // loop over particles
  {
    // protons and neutrons need a different binning in xF
    if (i < 6) xfPt[i] = new TH2D (Form ("xFpT_%s", PARTICLE_SYMBOL[i].c_str()),
                                   "; x_{F}; p_{T} (GeV/c)", XF_BINS, XF_MIN,
                                   XF_MAX, PT_BINS, PT_MIN, PT_MAX);
    else xfPt[i] = new TH2D (Form ("xFpT_%s", PARTICLE_SYMBOL[i].c_str()),
                             "; x_{F}; p_{T} (GeV/c)", XF_BINS_NUCLEON,
                             XF_MIN_NUCLEON, XF_MAX_NUCLEON,
                             PT_BINS, PT_MIN, PT_MAX);
  }
}

struct g4na49Config // store for g4na49 simulation setup
{
  string particleName;     // incoming particle, e.g. proton, pion
  Double_t particleEnergy; // incoming hadron energy in GeV
  string targetName;       // target symbol, e.g. C, Al
  Double_t targetA;        // target mass number
  Int_t targetZ;           // target atomic number
  Double_t targetDensity;  // target density in g/cm3
  Int_t nEvents;           // number of incident hadrons

  // workaround to set string value (simply auto '=' does not work)
  inline void set (const g4na49Config &conf)
  {
    particleName    = conf.particleName;
    particleEnergy  = conf.particleEnergy;
    targetName      = conf.targetName;
    targetA         = conf.targetA;
    targetZ         = conf.targetZ;
    targetDensity   = conf.targetDensity;
    nEvents         = conf.nEvents;
  }

  // check if all (but nEvents) are the same
  inline Bool_t isSame (const g4na49Config &conf)
  {
    return (targetA == conf.targetA && targetZ == conf.targetZ &&
            targetDensity == conf.targetDensity &&
            particleEnergy == conf.particleEnergy &&
            !particleName.compare(conf.particleName) &&
            !targetName.compare(conf.targetName));
  }

  TTree* createTree(); // make simple tree with simulation setup
};

TTree* g4na49Config :: createTree()
{
  TTree *tree = new TTree ("setup", "Simulation setup");
  tree->Branch ("particle", &particleName);
  tree->Branch ("energy", &particleEnergy); 
  tree->Branch ("target", &targetName);
  tree->Branch ("A", &targetA);
  tree->Branch ("Z", &targetZ);
  tree->Branch ("density", &targetDensity);
  tree->Branch ("nof_events", &nEvents);
  tree->Fill();
  return tree;
}
