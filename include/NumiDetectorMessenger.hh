#ifndef NumiDetectorMessenger_H
#define NumiDetectorMessenger_H 1

#include "globals.hh"
#include "G4UImessenger.hh"

class NumiDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithoutParameter;
class G4UIcmdWithABool;
class G4UIcmdWith3VectorAndUnit;

class NumiDetectorMessenger: public G4UImessenger {

public:
   NumiDetectorMessenger(NumiDetectorConstruction* );
   ~NumiDetectorMessenger();

   void SetNewValue(G4UIcommand*, G4String);

private:
   NumiDetectorConstruction* NumiDetector;

   G4UIdirectory*              NumiDir;
   G4UIdirectory*              detDir;
   G4UIcmdWithAString*         TargetGasCmd;
   //G4UIcmdWithADoubleAndUnit*  TargetZ0Cmd;
   //G4UIcmdWithADoubleAndUnit*  HornCurrentCmd;
   G4UIcmdWithABool*           ConstructTarget;
   G4UIcmdWithABool*           ConstructSolidMuMons;
   G4UIcmdWithAnInteger*       RunPeriod;
   G4UIcmdWithAString*         BeamConfig;
   G4UIcmdWithABool*           UseCorrHornCurrent;
   G4UIcmdWithABool*           UseHornMisalign;
   G4UIcmdWithABool*           UseTgtDensity;
   G4UIcmdWithAString*         AbsorberConfig;
   G4UIcmdWithAString*         Mon0AbsMatCmd;
   G4UIcmdWithAString*         Mon1AbsMatCmd;
   G4UIcmdWithAString*         Mon2AbsMatCmd;
   G4UIcmdWithADoubleAndUnit*  Mon0AbsThickCmd;
   G4UIcmdWithADoubleAndUnit*  Mon1AbsThickCmd;
   G4UIcmdWithADoubleAndUnit*  Mon2AbsThickCmd;
   G4UIcmdWithADoubleAndUnit*  Mon0AbsDistCmd;
   G4UIcmdWithADoubleAndUnit*  Mon1AbsDistCmd;
   G4UIcmdWithADoubleAndUnit*  Mon2AbsDistCmd;
   G4UIcmdWithADoubleAndUnit*  LengthOfWaterInTgt;
   G4UIcmdWithABool*           HeInDecayPipe;
   G4UIcmdWithABool*           applyDecayPipeMagneticField;
   //G4UIcmdWithAnInteger*     NbLayersCmd;
   G4UIcmdWithoutParameter*    UpdateCmd;

    G4UIdirectory* fBeamConfigDirectory;
    G4UIcmdWithADoubleAndUnit* fDuratekShiftCmd;
    G4UIcmdWithADoubleAndUnit* fTHBlockShiftCmd;
    G4UIcmdWithADoubleAndUnit* fDeltaOuterThicknessCmd;


    G4UIcmdWith3VectorAndUnit* fBafflePositionCmd;
    G4UIcmdWith3VectorAndUnit* fTargetPositionCmd;
    G4UIcmdWith3VectorAndUnit* fHorn1PositionCmd;
    G4UIcmdWith3VectorAndUnit* fHorn2PositionCmd;

    //Adding horn rotation feature
    G4UIcmdWith3VectorAndUnit* fHorn1RotationCmd;
    G4UIcmdWith3VectorAndUnit* fHorn2RotationCmd;
    //

    G4UIcmdWithADoubleAndUnit* fBaffleOuterRadiusCmd;
    G4UIcmdWithADoubleAndUnit* fBaffleInnerRadiusCmd;
    G4UIcmdWithADoubleAndUnit* fBaffleLengthCmd;

    G4UIcmdWithABool* fForcedOldTargetCmd;

    G4UIcmdWithADoubleAndUnit* fHornWaterLayerThick;
    G4UIcmdWithABool* fHorn1IsAlternate;
    G4UIcmdWithABool* fHorn1IsRefined;
    G4UIcmdWithABool* fHorn1UpstrIOisTorus;
    G4UIcmdWithADoubleAndUnit* fHorn1ExtraLayerAlum;
    G4UIcmdWithABool* fDumpBFieldPlease;
    //
    // New Data card to study sensitivity of the ME wiggle to the
    // field map and target geometry
    //
    G4UIcmdWithABool* fIgnoreCEQBr;
    G4UIcmdWithADoubleAndUnit* fHorn1FieldZCutUpstream;
    G4UIcmdWithADoubleAndUnit* fHorn1FieldZCutDwnstream;
    G4UIcmdWithADoubleAndUnit* fHorn1CurrentEqualizerLongAbsLength;
    G4UIcmdWithADouble* fHorn1CurrentEqualizerQuadAmpl;
    G4UIcmdWithADouble* fHorn1CurrentEqualizerOctAmpl;
    G4UIcmdWithADoubleAndUnit* fHorn2CurrentEqualizerLongAbsLength;
    G4UIcmdWithADouble* fHorn2CurrentEqualizerQuadAmpl;
    G4UIcmdWithADouble* fHorn2CurrentEqualizerOctAmpl;

    G4UIcmdWithADouble* fNovaTargetHTilt;
    G4UIcmdWithADouble* fNovaTargetVTilt;
    G4UIcmdWithADoubleAndUnit* fNovaTargetXOffset;
    G4UIcmdWithADoubleAndUnit* fNovaTargetYOffset;
    G4UIcmdWithADoubleAndUnit* fNovaTargetExtraFlangeThick;
    G4UIcmdWithADoubleAndUnit* fHorn1StripLinesThick;
    G4UIcmdWithABool* fUsePosLocalCoordInMagField;
    G4UIcmdWithABool* fUseRotLocalCoordInMagField;

  //New cards for beam optimization (Leo, July 13, 2018)
    G4UIcmdWithAnInteger*         fNumberOfMEFins;
    G4UIcmdWithADoubleAndUnit*    fDistanceBetweenMEFins;
    G4UIcmdWithADoubleAndUnit*    fBudalMonitorMEPosition;
    G4UIcmdWithADoubleAndUnit*    fWidthMEFin;
    G4UIcmdWithAnInteger*         fWingedFin1;
    G4UIcmdWithAnInteger*         fWingedFin2;
    G4UIcmdWithAnInteger*         fWingedFin3;
    G4UIcmdWithAnInteger*         fWingedFin4;
    G4UIcmdWithADoubleAndUnit*    fWingedFinRadius;

    G4UIcmdWithAString* fGDMLOutputCmd;
    G4UIcmdWithABool*   fGDMLStoreRefCmd;
    G4bool              fGDMLStoreReferences;

};

#endif
