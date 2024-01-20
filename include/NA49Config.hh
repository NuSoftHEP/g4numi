//NA49Conf handles settings (beam, target, #events) from command line;

#ifndef NA49_CONFIG_HH
#define NA49_CONFIG_HH

#include <getopt.h>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "Randomize.hh"
#include <TTree.h>

using namespace std;

//store information about target
struct Target
{
  G4String name;      //e.g. C, Al
  G4double A;         //mass number
  G4int    Z;         //atomic number
  G4double density;   //in g/cm3
  G4double radius;    //in cm

  Target (G4String name = "", G4double A = 0, G4int Z = 0,
          G4double density = 0, G4double radius = 0.3) :
          name(name), A(A), Z(Z), density(density), radius(radius) {};
  void print () const;
};

//store information about beam
struct Beam
{
  G4String particle;  //e.g. proton
  G4String energy;	  //in GeV

  Beam (G4String particle = "", G4String energy = "") :
        particle(particle), energy(energy) {};
  void print () const;
};

//read and store configuration for g4na49 defined by command line arguments
class NA49Config
{
  public:

    NA49Config (int argc, char **argv);
    inline Target getTarget() const {return target;};
    inline Beam getBeam() const {return beam;};
    inline G4String getNevents() const {return nEvents;};
    inline G4String getRunNumber() const {return runNumber;};
    inline G4String getOutputFile() const {return outputFile;};
    inline G4String getOutputDir() const {return outputDir;};
    TTree* createTree() const; //create ROOT TTree with sim config

  private:

    void usage() const;
    //set predefined target (from NA49Targets.hh) if exists
    void setTarget (const G4String &targetName);
    //display chosen configuration and wait for user confirmation
    void checkConf() const;			

    Target target;
    Beam beam;
    G4String nEvents;
    G4String runNumber;       //used in random seed and auto outputFile
    G4String outputFile;      //output root file
    G4String outputDir;       //output root directory

    bool isConfirmed;         //checkConf will be called if false
};
    
#endif //NA49_CONFIG_HH
