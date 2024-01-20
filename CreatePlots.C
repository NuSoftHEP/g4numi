#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdio.h>

using std::cout;
using std::string;

// ---------- CONSTANTS ---------- //

// particles

const Int_t  N_PARTICLES = 8;  // number of particles to create histograms for
const string PARTICLE_NAME [N_PARTICLES]   = // particles names (as in G4)
      {"pi+", "pi-", "kaon+", "kaon-", "kaon0L", "kaon0S", "proton", "neutron"};
const string PARTICLE_SYMBOL [N_PARTICLES] = // particles abbreviations
      {"pip", "pim", "kap", "kam", "klong", "kshort", "prt", "neu"};

// ---------- INLINE FUNCTIONS ---------- //

// close all input files
inline void closeInputFiles (const TFile** inputFiles, const UInt_t &nFiles)
{
  for (UInt_t i = 0; i < nFiles; i++) (TFile*)(*inputFiles++)->Close();
}

inline void getHistograms (const TFile** inputFiles, const UInt_t &nFiles,
                           TH1D **h, const UInt_t &ip, const UInt_t &ih)
{
  for (UInt_t i = 0; i < nFiles; i++)
    (TH1D*)(*h++) = (TH1D*)(TFile*)(*inputFiles++)->Get (
                                               Form ("%s/pT_for_xF%03d_%s",
                                               PARTICLE_SYMBOL[ip].c_str(), ih,
                                               PARTICLE_SYMBOL[ip].c_str()));
}

inline Double_t calculateAlpha (const Double_t &xf, const Double_t &pt)
{
  static Double_t factors[6] =
                  {0.509855, -0.855034, 0.817537, 0.13593, 0.0474293, 1};

  return ((factors[0]*xf*xf + factors[1]*xf + factors[2]) *
          (factors[3]*pt*pt + factors[4]*pt + factors[5]));
}

// ---------- DECLARATIONS ---------- //

struct g4na49Config;  // store for g4na49 simulation setup

// convert string "file1 file2 ..." to vector of strings and return its size
UInt_t makeList (const string &input, std::vector <string> &listFiles);

// open and check input files
bool openFiles (TFile **inputFiles, std::vector <string> &listFiles);

// open output root file (name based on simulation config), prepare folders
Bool_t initializeOutputFile (const g4na49Config &conf);

// no of histos per particle in input files
UInt_t getNHistos (TFile** inputFiles, const UInt_t &nFiles);

// extract xF from histogram title ("Yield: xFdown < x_{F} < xFup")
Double_t getXf (const char *title);

// ---------- MAIN FUNCTION ---------- //

TFile *outputFile = NULL; // output root file with plots
string workingDir = ".";  // default input/output dir

// create plots from CreateInvXS.C output; input = "file1 file2 file3 ..."
// use current dir for input/output or $G4NA49_ROOTDIR if defined
void CreatePlots (const string input)
{
  // use $G4NA49_ROOTDIR as working dir (if defined)
  if (getenv("G4NA49_ROOTDIR")) workingDir = getenv("G4NA49_ROOTDIR");

  std::vector <string> listFiles;                   // list of input files
  const UInt_t nFiles = makeList(input, listFiles); // nof of input files

  const TFile **inputFiles = new TFile*[nFiles];    // array of input TFiles

  // read input files; stop script if could not open files
  if (!openFiles (inputFiles, listFiles)) return;

  // read g4na49 simulation setup; stop if different beam
  g4na49Config config;
  if (!config.initialize (inputFiles, nFiles)) return;

  const UInt_t nHistos = getNHistos (inputFiles, nFiles); // #histos / particle
  // stop if different #histos / particle in files (or there are no histograms)
  if (!nHistos) return;

  // open output root file and prepare folders; stop if file could not be opened
  if (!initializeOutputFile (config)) return;

  // create folders for eps files
  for (UInt_t i = 0; i < N_PARTICLES; i++)
    gROOT->ProcessLine (Form (".! mkdir -p %s/plots/%s", workingDir.c_str(),
                              PARTICLE_NAME[i].c_str()));

  
  gROOT->SetBatch();        // go to batch mode (lots of plots will be created)
  gErrorIgnoreLevel = 2000; // do not print info messages (like eps was created)

  for (UInt_t ip = 0; ip < N_PARTICLES; ip++) // particle loop
  {
    outputFile->cd (PARTICLE_SYMBOL[ip].c_str()); // cd into particle folder
   
    for (UInt_t ih = 0; ih < nHistos; ih++) // histogram loop
    {
      // load histograms for given particle and xF range
      TH1D **histogram = new TH1D*[nFiles];
      getHistograms (inputFiles, nFiles, histogram, ip, ih);

      // create multihistogram
      THStack multiHistogram ("", histogram[0]->GetTitle());
      
      for (UInt_t i = 0; i < nFiles; i++) // loop over files 
      {
        // set different colors for each target
		    histogram[i]->SetMarkerStyle (20 + i);
		    histogram[i]->SetMarkerColor (2 + i);
		    histogram[i]->SetLineColor (2 + i);
        // add histogram to multihistogram
        multiHistogram.Add (histogram[i]);
      }
      
      // create canvas to plot multihistogram
      TCanvas *c = new TCanvas (Form ("pT_for_xF%03d_%s", ih,
                                       PARTICLE_SYMBOL[ip].c_str()));

      // draw multihistogram and set axis titles
      multiHistogram.Draw ("nostack"); 
      multiHistogram.GetXaxis()->SetTitle ("p_{T} [GeV]");
      multiHistogram.GetYaxis()->SetTitle ("Ed^{3}#sigma/dp^{3} [mb/GeV^{2}]");
      multiHistogram.GetXaxis()->SetTitleOffset (1.25);
      multiHistogram.GetYaxis()->SetTitleOffset (1.25);

      // create legend
      legend = new TLegend (0.8,0.6,0.9,0.9);

      // add entry to the legend      
      for (UInt_t i = 0; i < nFiles; i++)
          legend->AddEntry (histogram[i], config.targetName[i].c_str(), "p");

      // get xF value for current bin
      Double_t xf = getXf (histogram[0]->GetTitle());

      // create graph for scaled cross section
      TGraphErrors *scaledHistograms = new TGraphErrors [nFiles-1];

      for (UInt_t i = 1; i < nFiles; i++)
      {        
        for (UInt_t ib = 0; ib < histogram[0]->GetNbinsX(); ib++) // bin loop
        {
            Double_t pt = histogram[0]->GetBinCenter (ib); // transverse mom
            Double_t a  = calculateAlpha (xf, pt);         // xsec -> A^alpha
            Double_t factor = pow (config.targetA[i]/config.targetA[0], a);

            scaledHistograms[i-1].SetPoint (ib, pt,
                                  histogram[0]->GetBinContent(ib) * factor);
            // uncoment this line if you want error bands on the plot
            //~ scaledHistograms[i-1].SetPointError (ib, 0,
                             //~ histogram[0]->GetBinError(ib) * factor);
        }

        // set graph color and add to plot
        scaledHistograms[i-1].SetFillColor (2 + i);
        scaledHistograms[i-1].SetFillStyle (0);
        scaledHistograms[i-1].Draw ("SAME3");
      }

      // draw legend
      legend->Draw();
      // save histogram and create eps file
      c->Write();
      c->SaveAs (Form ("%s/plots/%s/pT_for_xF%03d_%s.eps", workingDir.c_str(),
                       PARTICLE_NAME[ip].c_str(), ih,
                       PARTICLE_SYMBOL[ip].c_str()));
      c->Close();
      
      //for (UInt_t i = 0; i < nFiles; i++) histogram[i]->Delete();

      cout << "Processing " << PARTICLE_NAME[ip] << ": "
           << int(100.0 * ih / nHistos) << "%\r" << flush;

    } // histogram loop

    cout << "Processing " << PARTICLE_NAME[ip] << ": done\n";
    
  } // particle loop


  closeInputFiles (inputFiles, nFiles);
  outputFile->Write(); // save histograms
  outputFile->Close(); // close output file
}

// convert string "file1 file2 ..." to vector of strings and return its size
UInt_t makeList (const string &input, std::vector <string> &listFiles)
{
  std::istringstream issFiles(input); // convert char* to iss

  string temp; // temporary store
  while (issFiles >> temp) listFiles.push_back(temp);

  return listFiles.size();
}

// open and check input files
bool openFiles (TFile **inputFiles, std::vector <string> &listFiles)
{
  cout << "Loading files:\n";
   
  for (auto file = listFiles.begin(); file != listFiles.end(); ++file)
  {
    *inputFiles = new TFile ((workingDir + "/" + *file).c_str(), "READ");

    if ((TFile*)(*inputFiles)->IsZombie())
    {
      cout << "\nERROR: " << workingDir + "/" + *file << " is zombie.\n\n";
      return false;
    }
    else cout << workingDir + "/" + *file << " loaded\n";

    inputFiles++;
  }

  return true;
}

// open output root file (name based on simulation config), prepare folders
Bool_t initializeOutputFile (const g4na49Config &conf)
{
  // create output file (name is based on g4na49 configuration)
  outputFile = new TFile (Form ("%s/PLOTS_%s_%.2f.root", workingDir.c_str(),
                          conf.particleName.c_str(), conf.particleEnergy),
                          "RECREATE");

  if (!outputFile) // stop if outputFile was not created
  {
    cout << "\nERROR: Output file: ";
    printf ("%s/XS_%s_%.2f.root", workingDir.c_str(),
            conf.particleName.c_str(), conf.particleEnergy);
    cout << " could not be created.\n\n";
    return false;
  }
  
  // create folders for each particle
  for(Int_t i = 0; i < N_PARTICLES; i++)
    outputFile->mkdir (PARTICLE_SYMBOL[i].c_str());

  return true;
}

// no of histos per particle in input files
UInt_t getNHistos (TFile** inputFiles, const UInt_t &nFiles)
{
  int tempNHistos = 0;

  for (UInt_t k = 0; k < nFiles; k++) // loop over files
  {
    for (UInt_t i = 0; i < N_PARTICLES; i++) // loop over particles
    {      
      // cd into particle directory
      (TFile*)(*inputFiles)->cd (PARTICLE_SYMBOL[i].c_str()); 
    
      // for first entry assign the value; for next: return 0 if not as first one
      if (i == 0) tempNHistos = gDirectory->GetNkeys();
      else if (tempNHistos != gDirectory->GetNkeys())
      {
        cout << "\nERROR: some histogram is missing for " << PARTICLE_SYMBOL[i]
             << ".\n\n";
        return 0;
      }
    } // particle loop

    *inputFiles++;
    
  } // files loop

  if (!tempNHistos)
    cout << "\nERROR: there are no histograms in input file.\n\n";

  return tempNHistos;
}

// extract xF from histogram title ("Yield: xFdown < x_{F} < xFup")
Double_t getXf (const char *title)
{
  std::istringstream titleSS (title); // convert char* to ss
  string temp;                        // for "Yield", "<", "x_{F}", "<"
  Double_t xFup, xFdown;              // xF boundaries

  //         Yield:  xFdown    <       x_{F}   <       xFup
  titleSS >> temp >> xFdown >> temp >> temp >> temp >> xFup;

  Double_t width = xFup - xFdown; // bin width
    
  return (xFdown + width / 2.0);  // bin center
}

struct g4na49Config // store for g4na49 simulation setup
{
  string particleName;     // incoming particle, e.g. proton, pion
  Double_t particleEnergy; // incoming hadron energy in GeV
  string *targetName;      // target symbol, e.g. C, Al
  Double_t *targetA;       // target mass number

  // load g4na49 setup from input files
  Bool_t initialize (const TFile **inputFiles, const UInt_t &nFiles);
};

Bool_t g4na49Config :: initialize (const TFile **inputFiles,
                                   const UInt_t &nFiles)
{
  targetName = new string [nFiles];   
  targetA    = new Double_t [nFiles];

  // temporary beam info to compare if the same for all entries
  string tempParticleName;
  Double_t tempParticleEnergy;
  
  // workaround to pass string from struct to SetBranchAddress
  string *particle = &tempParticleName;
  
  for (UInt_t i = 0; i < nFiles; i++)
  {
    // workaround to pass string from struct to SetBranchAddress
    string *target   = &targetName[i];
    
    TTree *setupTree = (TTree*) (TFile*)(*inputFiles++)->Get("setup"));
    setupTree->SetBranchAddress ("particle", &particle);
    setupTree->SetBranchAddress ("energy", &tempParticleEnergy);    
    setupTree->SetBranchAddress ("target", &target);
    setupTree->SetBranchAddress ("A", &targetA[i]);    
    setupTree->GetEntry();

    // assign beam values to first entry values
    // check if beam is the same as for first entry/file
    if (i == 0)
    {
      particleEnergy = tempParticleEnergy;
      particleName   = tempParticleName;
    }
    else if (tempParticleEnergy != particleEnergy ||
             particleName.compare(tempParticleName) != 0)
    {
      cout << "\nERROR: Input files contain simulations for different "
           << "particles and/or energies.\n\n";

      return false;
    }
  }

  return true;
}
