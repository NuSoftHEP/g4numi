//
// NumiSteppingAction.hh
//

#ifndef NumiSteppingAction_H
#define NumiSteppingAction_H 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "NumiDataInput.hh"

class G4EventManager;
class NumiEventAction;
class NumiRunManager;

class NumiSteppingAction : public G4UserSteppingAction
{
  
 public:
  NumiSteppingAction();
  virtual ~NumiSteppingAction();
  
  virtual void UserSteppingAction(const G4Step*);
  void CheckInTrackingDetectorH1Plane(const G4Step *theStep);//prachi


private:
   
   NumiDataInput *NDI;
   NumiRunManager *pRunManager;
   G4EventManager *EvtManager;
   NumiEventAction *NumiEvtAct;
   G4LogicalVolume *TrkPlnH1Logical;//prachi


   G4bool fPrintAllSteps;
   G4bool fPrintSplitting;

   G4bool fPrintMuAlcove1;
   G4bool fPrintMuAlcove2;
   G4bool fPrintMuAlcove3;
   G4bool fPrintDeltaAlcove1;
   G4bool fPrintDeltaAlcove2;
   G4bool fPrintDeltaAlcove3;

   G4bool fPrintProcesses;
   G4bool fPrintTouchableHistory;
   mutable std::ofstream fOutStudyGeantino;

   mutable G4String fGeantinoStudyName;
   mutable G4String fKeyVolumeNameFrom;
   mutable G4String fKeyVolumeNameTo;
   //
   // Tentative MIPP renormalization Utility tool.  
   //
     
  //
  // Geantino analysis 
  //
  mutable double fTotalAbsDecayChan;
  mutable double fTotalAbsHorn1Neck;
  mutable double fTotalAbsHorn2Entr;
  mutable double fWaterAbsDecayChan;
  mutable double fWaterAbsHorn1Neck;
  mutable double fWaterAbsHorn2Entr;
  mutable double fAlumAbsHorn2Entr;
  mutable bool fGoneThroughHorn1Neck;
  mutable bool fGoneThroughTarget;
  mutable bool fGoneThroughHorn2Entr;
  mutable int fEvtIdPrevious;
  
  void StudyAbsorption(const G4Step*); 
  void StudyPropagation(const G4Step*); 
  void StudyCheckOverlap(const G4Step*);
  void StudyBFieldWithMuons(const G4Step*);
  void dumpStepCheckVolumeAndFields(const G4Step*);
   
public:

   bool EscapingTarget(const G4String &preVolName, const G4String &postVolName) const;

   inline void SetGeantinoStudyName(G4String s) const { fGeantinoStudyName = s;} // G4 does not seems to allow to modify 
                                                                                 // an Action container.. 
   inline std::string GetGeantinoStudyName() const { return fGeantinoStudyName;}
   
   inline void OpenOutStudyGeantino(const char *fName) const {
      if(fOutStudyGeantino.is_open()) {
#ifdef ANCIENT_G4
        G4Exception("NumiSteppingAction::OpenOutStudyGeantino, Geantino Output file already open"); 
#else
        G4Exception("NumiSteppingAction","NumiSteppingAction",FatalException,"OpenOutStudyGeantino, Geantino Output file already open"); 
#endif
         exit(2); // only under 4.9.2 .. not reachable under 4.9.5 
      }
      fOutStudyGeantino.open(fName);
      if (fGeantinoStudyName.contains("PropCO")) fOutStudyGeantino << " id x y z xo yo zo step matPre matPost " << std::endl;
      if (fGeantinoStudyName.contains("Absorb")) 
        fOutStudyGeantino << " id x y E AbsTDecayChan AbsTHorn1Neck AbsHorn2E AbsWaterDecayChan AbsWHorn1Neck AbsWHorn2E AbsAlumHorn2Entr  " 
	              << std::endl;
       if (fGeantinoStudyName.contains("BField")) fOutStudyGeantino << " id x y z px py pz matPre matPost " << std::endl;
   }
   inline void SetKeyVolumeNameFrom(G4String s) const { fKeyVolumeNameFrom  = s;}
   inline void SetKeyVolumeNameTo(G4String s) const { fKeyVolumeNameTo  = s;}
};

#endif

