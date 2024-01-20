
#ifndef NA49RunAction_h
#define NA49RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "NA49Config.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class NA49RunAction : public G4UserRunAction
{    
    public: // Without description

	NA49RunAction(const NA49Config &config_) : config(config_) {};
	~NA49RunAction();

    public: // With description
 
	void BeginOfRunAction(const G4Run*);
	// In this method histogramms are booked

	void EndOfRunAction(const G4Run*);
	// In this method bookHisto method is called in which histogramms are filled

    private:

	NA49Config config;

};

#endif

