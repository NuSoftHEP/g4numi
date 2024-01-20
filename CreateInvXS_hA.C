#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using std::cout;
using std::string;

// ---------- CONSTANTS ---------- //

const Double_t TARGET_THICK = 0.7;      // target thickness in cm
const Double_t CM2_PER_MB   = 1e-27;    // conversion factor
const Double_t NA           = 6.022e23; // Avogadro constant

// particles

const Int_t    N_PARTICLES = 8;  // number of particles to create histograms for
const string   PARTICLE_NAME [N_PARTICLES]   = // particles names (as in G4)
      {"pi+", "pi-", "kaon+", "kaon-", "kaon0L", "kaon0S", "proton", "neutron"};
const string   PARTICLE_SYMBOL [N_PARTICLES] = // particles abbreviations
      {"pip", "pim", "kap", "kam", "klong", "kshort", "prt", "neu"};
const Int_t    PARTICLE_PDG [N_PARTICLES]    = // particles pdg
      {211, -211, 321, -321, 130, 310, 2212, 2112};
const Double_t PARTICLE_MASS [N_PARTICLES]   = // particles masses [GeV]
{0.13957, 0.13957, 0.493667, 0.493667, 0.497648, 0.497648, 0.938272, 0.939565};

const Double_t NUCLEON_MASS = (0.938272 + 0.939565) / 2.0; // mass of target

// ---------- INLINE FUNCTIONS ---------- // 

// return mass based on particle name
inline Double_t getMass (const string &particle)
{
  for (UInt_t i = 0; i < N_PARTICLES; i++)
    if (particle.compare (PARTICLE_NAME[i]) == 0) return PARTICLE_MASS[i];

  return 0.0;
}

// calculate sqrt(s)
inline Double_t calcS (const string &particle, const Double_t &energy)
{
  const Double_t mass = getMass (particle);
  return sqrt (mass * mass + NUCLEON_MASS * (NUCLEON_MASS  + 2.0 * energy));
}

// calculate normalization factor (mass number, density, nof incident particles)
inline Double_t xsFact (const Double_t &A, const Double_t &rho, const UInt_t &N)
{
  const Double_t massA    = A / NA;                         // in g
  const Double_t density  = rho / massA;                    // in cm-3
  const Double_t factor   = 1.0 / (density * TARGET_THICK); // in cm-2
  return factor / CM2_PER_MB / N;                           // in mb 
}

// ---------- DECLARATIONS ---------- //

struct g4na49Config;  // store for g4na49 simulation setup

// open output root file (name based on simulation config), prepare folders
Bool_t initializeOutputFile (const g4na49Config &conf);

// no of histos per particle in input file
UInt_t getNHistos ();

// load yield histograms from input file
Bool_t loadYields (TH1D **pt, TH2D *xfPt[N_PARTICLES], const UInt_t &nHistos);

// extract xF and xF width from histogram title ("Yield: xFdown < x_{F} < xFup")
void getXf (const char *title, Double_t &value, Double_t &width);

// ---------- MAIN FUNCTION ---------- //

TFile *outputFile      = NULL; // output root files with xsec
const TFile *inputFile = NULL; // input files with Yields
string workingDir      = ".";  // default input/output dir

// calcualte cross section for meson production in hA based on yields
// use current dir for input/output or $G4NA49_ROOTDIR if defined
void CreateInvXS (const string inputFileName)
{
  // use $G4NA49_ROOTDIR as working dir (if defined)
  if (getenv("G4NA49_ROOTDIR")) workingDir = getenv ("G4NA49_ROOTDIR");

  // open input file
  inputFile  = new TFile ((workingDir + "/" + inputFileName).c_str(), "READ");
  if (inputFile->IsZombie()) return; // stop if there is no input file

  // load g4na49 simulation setup from input file tree
  const g4na49Config config ((TTree*) inputFile->Get ("setup"));

  // prepare output file (name is based on g4na49 configuration)
  // stop if outputFile could not be created
  if (!initializeOutputFile (config)) return; 

  const UInt_t nHistos = getNHistos(); // get no of pT histos per particle 
  // stop if there are no histograms or their number is different per particle
  if (nHistos == 0) return; 

  TH1D   *xsPt [nHistos][N_PARTICLES]; // pT histogram for given xF range
  TH2D *xsXfPt [N_PARTICLES];          // xF vs pT histogram

  // fill histograms with yields (convert to XS later)
  loadYields ((TH1D**)xsPt, xsXfPt, nHistos);

  inputFile->Close(); // close input file

  // ----- CONVERT YIELDS TO XSEC ----- //

  // sqrt(s)
  const Double_t sqrtS    = calcS (config.particleName, config.particleEnergy);
  // jacobian for dsigma / d^3p -> dsigma / dpT^2dxF
  const Double_t jacobian = 2.0 / TMath::Pi() / sqrtS;
  // calculate normalization factor 1 / nof targets / nof incident particles
  const Double_t sigmaFactor =
                 xsFact (config.targetA, config.targetDensity, config.nEvents);
  
  // ----- xF vs pT histograms ----- //

  cout << "Converting xF vs pT yields to cross section:\n\n";

  for (UInt_t ip = 0; ip < N_PARTICLES; ip++) // particle loop
  {    
    const Double_t mass  = PARTICLE_MASS[ip]; // particle mass
    const Double_t mass2 = mass * mass;       // particle mass ^2
    
    xsXfPt[ip]->Sumw2(); // it does not hurt, but is it necessary here?

    const Int_t xfBins = xsXfPt[ip]->GetNbinsX();
    
    for (UInt_t ixf = 1; ixf <= xfBins; ixf++) // xF loop
    {
      // get xF and xF bin width
      const Double_t xf      = xsXfPt[ip]->GetXaxis()->GetBinCenter (ixf);
      const Double_t xfWidth = xsXfPt[ip]->GetXaxis()->GetBinWidth (ixf);

      const Double_t pl = xf * sqrtS / 2.0; // longitudinal momentum (in CMS)
      
      for(UInt_t ipt = 1; ipt <= xsXfPt[ip]->GetNbinsY(); ipt++) // pT loop
      {
        // get yield and error
        const Double_t yield      = xsXfPt[ip]->GetBinContent (ixf, ipt);
        const Double_t yieldError = xsXfPt[ip]->GetBinError (ixf, ipt);

        //if (yield + yieldError == 0) continue; // skip zero-value bins
        
        // get transverse momentum and its bin width
        const Double_t pt      = xsXfPt[ip]->GetYaxis()->GetBinCenter(ipt);
        const Double_t ptWidth = xsXfPt[ip]->GetYaxis()->GetBinWidth(ipt);
        // calculate dpT2 = pTup^2 - pTdown^2 =
        // = (pT + width/2)^2 - (pT - width/2)^2 = 2 * pT * width 
        const Double_t pt2Width = 2.0 * pt * ptWidth;

        const Double_t energy = sqrt (pt * pt + pl * pl +  mass2); // in CMS
        
        // dsigma / d^3p = jacobian * dsigma / dpT^2dxF (in CMS)
        // f = E * dsigma / d^3p is lorentz invariant
        const Double_t f = jacobian * energy / xfWidth / pt2Width;

        // total factor (f + normalization)
        const Double_t factor = f * sigmaFactor;

        // fill histogram with xsec = factor * yield
        xsXfPt[ip]->SetBinContent (ixf, ipt, yield * factor);
        xsXfPt[ip]->SetBinError   (ixf, ipt, yieldError * factor);
      } // pT loop

      cout << "\t -> processing " << PARTICLE_NAME[ip] << ": "
           << int(100.0 * ixf / xfBins) << "% \r" << flush;
      
    } // xF loop

    cout << "\t -> processing " << PARTICLE_NAME[ip] << ": done\n\n";
 
  } // particle loop

  // ----- pT histograms ----- //
  // note: one can make the script faster assuming same binning for all pT
  // histograms and changing order of loops so get_xF is less called

  outputFile->cd(0);

  cout << "Converting pT yields to cross section:\n\n";

  for (UInt_t ip = 0; ip < N_PARTICLES; ip++) // loop over particles
  {
    const Double_t mass  = PARTICLE_MASS[ip]; // particle mass
    const Double_t mass2 = mass * mass;       // particle mass ^2
    
    for (UInt_t ih = 0; ih < nHistos; ih++) // loop over histograms (xF ranges)
    {
      Double_t xf, xfWidth;                           // xF value and range 
      getXf (xsPt[ih][ip]->GetTitle(), xf, xfWidth); // extract from title

      // change histogram title to XSEC: xFdown < x_{F} < xFup
      xsPt[ih][ip]->SetTitle (Form ("XSEC: %f < x_{F} < %f",
                                 xf - xfWidth / 2.0, xf + xfWidth / 2.0));

      xsPt[ih][ip]->Sumw2(); // it does not hurt, but is it necessary here?

      const Double_t pl = xf * sqrtS / 2.0; // longitudinal momentum (in CMS)

      for (UInt_t ipt = 0; ipt < xsPt[ih][ip]->GetNbinsX(); ipt++) // pT loop
      {
        // get yield and error
        const Double_t yield      = xsPt[ih][ip]->GetBinContent (ipt);
        const Double_t yieldError = xsPt[ih][ip]->GetBinError (ipt);

        if (yield + yieldError == 0) continue; // skip zero-value bins

         // get transverse momentum and its bin width
        const Double_t pt      = xsPt[ih][ip]->GetXaxis()->GetBinCenter (ipt);
        const Double_t ptWidth = xsPt[ih][ip]->GetXaxis()->GetBinWidth (ipt);
        // calculate dpT2 = pTup^2 - pTdown^2 =
        // = (pT + width/2)^2 - (pT - width/2)^2 = 2 * pT * width 
        const Double_t pt2Width = 2.0 * pt * ptWidth;

        // calculate energy (in CMS)
        const Double_t energy = sqrt (pt * pt + pl * pl +  mass2);

        // dsigma / d^3p = jacobian * dsigma / dpT^2dxF (in CMS)
        // f = E * dsigma / d^3p is lorentz invariant
        const Double_t f = jacobian * energy / xfWidth / pt2Width;

        // total factor (f + normalization)
        const Double_t factor = f * sigmaFactor;

        // fill histogram with xsec = factor * yield
        xsPt[ih][ip]->SetBinContent (ipt, yield * factor);
        xsPt[ih][ip]->SetBinError   (ipt, yieldError * factor);
      } // pT loop

      cout << "\t -> processing " << PARTICLE_NAME[ip] << ": "
           << int(100.0 * ih / nHistos) << "% \r" << flush;

    } // xF loop
    
    cout << "\t -> processing " << PARTICLE_NAME[ip] << ": done\n\n";

  } // particle loop
  
  outputFile->Write(); // save histograms
  outputFile->Close(); // close output file
}

// ---------- DEFINITIONS ---------- //

// open output root file (name based on simulation config), prepare folders
Bool_t initializeOutputFile (const g4na49Config &conf)
{
  // create output file (name is based on g4na49 configuration)
  outputFile = new TFile (Form ("%s/XSEC_%s_%.2f_%s.root", workingDir.c_str(),
                          conf.particleName.c_str(), conf.particleEnergy,
                          conf.targetName.c_str()), "RECREATE");

  if (!outputFile) // stop if outputFile was not created
  {
    cout << "\nERROR: Output file: ";
    printf ("%s/XS_%s_%.2f_%s.root", workingDir.c_str(),
            conf.particleName.c_str(), conf.particleEnergy,
            conf.targetName.c_str());
    cout << " could not be created.\n\n";
    return false;
  }

  // save g4na49 settings
  const TTree *const confTree = conf.createTree ();
  confTree->Write();
  
  // create folders for each particle
  for(Int_t i = 0; i < N_PARTICLES; i++)
    outputFile->mkdir (PARTICLE_SYMBOL[i].c_str());

  return true;
}

// no of histos per particle in input file
UInt_t getNHistos ()
{
  int tempNHistos = 0;

  for (UInt_t i = 0; i < N_PARTICLES; i++) // loop over particles
  {
    inputFile->cd (PARTICLE_SYMBOL[i].c_str()); // cd into particle directory
  
    // for first entry assign the value; for next: return 0 if not as first one
    if (i == 0) tempNHistos = gDirectory->GetNkeys();
    else if (tempNHistos != gDirectory->GetNkeys())
    {
      cout << "\nERROR: some histogram is missing for " << PARTICLE_SYMBOL[i]
           << ".\n\n";
      return 0;
    }
  }

  if (!tempNHistos)
    cout << "\nERROR: there are no histograms in input file.\n\n";
  
  return tempNHistos;
}

// load yield histograms from input file
Bool_t loadYields (TH1D **pt, TH2D *xfPt[N_PARTICLES], const UInt_t &nHistos)
{  
  for (UInt_t ip = 0; ip < N_PARTICLES; ip++) // loop over particles
  {
    outputFile->cd (0); // cd into main directory
  
    // load xF vs pT yields
    TH2D *tempTH2D = (TH2D*) inputFile->Get(Form ("xFpT_%s",
                                                  PARTICLE_SYMBOL[ip].c_str()));
                                                   
    if (!tempTH2D) // check if histogram exists
    {
      printf ("\nERROR: input file does not contain xFpT_%s histogram.\n\n",
              PARTICLE_SYMBOL[ip].c_str());
      return false;
    }

    xfPt[ip] = new TH2D(*tempTH2D); // save histogram
    
    outputFile->cd (PARTICLE_SYMBOL[ip].c_str()); // cd into particle folder

    for (UInt_t ih = 0; ih < nHistos; ih++) // loop over pT histograms
    {
      // load pT yields
      TH1D *tempTH1D = (TH1D*) inputFile->Get (Form ("%s/pT_for_xF%03d_%s",
                                               PARTICLE_SYMBOL[ip].c_str(), ih,
                                               PARTICLE_SYMBOL[ip].c_str()));
                                 
      if (!tempTH1D) // check if histogram exists
      {
        printf ("\nERROR: input file does not contain %s/pT_for_xF%03d_%s "
                "histogram.\n\n", PARTICLE_SYMBOL[ip].c_str(), ih,
                PARTICLE_SYMBOL[ip].c_str());

        return false;
      }
      tempTH1D->GetYaxis()->SetTitle ("inv xs");
      *(pt + ip * nHistos + ih) = new TH1D (*tempTH1D); // save histogram
    }
  }

  return true;
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

  // load g4na49 setup from ttree
  void g4na49Config (const TTree * const tree);

  TTree* createTree (); // make simple tree with simulation setup
};

void g4na49Config :: g4na49Config (const TTree * const tree)
{
  // workaround to pass string from struct to SetBranchAddress
  string *particle = &particleName;
  string   *target = &targetName;
  
  tree->SetBranchAddress ("particle", &particle);
  tree->SetBranchAddress ("energy", &particleEnergy); 
  tree->SetBranchAddress ("target", &target);
  tree->SetBranchAddress ("A", &targetA);
  tree->SetBranchAddress ("Z", &targetZ);
  tree->SetBranchAddress ("density", &targetDensity);
  tree->SetBranchAddress ("nof_events", &nEvents);
  tree->GetEntry();
}

TTree* g4na49Config :: createTree ()
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

// extract xF and xF width from histogram title ("Yield: xFdown < x_{F} < xFup")
void getXf (const char *title, Double_t &value, Double_t &width)
{
  std::istringstream titleSS (title); // convert char* to ss
  string temp;                        // for "Yield", "<", "x_{F}", "<"
  Double_t xFup, xFdown;              // xF boundaries

  //         Yield:  xFdown    <       x_{F}   <       xFup
  titleSS >> temp >> xFdown >> temp >> temp >> temp >> xFup;

  width = xFup - xFdown;        // bin width
  value = xFdown + width / 2.0; // bin center
}
