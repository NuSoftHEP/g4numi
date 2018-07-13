// NumiDetectorConstruction.hh
//------------------------------------------------------------

#ifndef NumiDetectorConstruction_H
#define NumiDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#ifndef FLUGG
#include "NumiDetectorMessenger.hh"
#endif
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include <vector>

class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class NumiDataInput;
class NumiMagneticField;
class NumiMagneticFieldIC;
class NumiMagneticFieldOC;
class NumiDecayPipeMagneticField;
class NumiHornSpiderSupport;
class G4VisAttributes;
class G4VisAttributes;
class LBNEHornRadialEquation; // LBNE misnomer, these equation are found in the NUMI drawings, 
                              // For AD drawing numbers, see ../src/NumiHorn1.cc 

typedef std::vector<G4double> vdouble_t; 
typedef std::vector<G4int> vint_t;

class NumiDetectorConstruction : public G4VUserDetectorConstruction
{
public:
   
   NumiDetectorConstruction();
   ~NumiDetectorConstruction();
   
   G4VPhysicalVolume* Construct();
   
   void SetTargetZ0(G4double val);
   void SetHornCurrent(G4double val);

   void SetAbsorberConfig(G4String config);
   void SetMonAbsorberMaterial(G4String materialChoice, G4int mon);
   void SetMonAbsorberThickness(G4double val, G4int mon);
   void SetAbsorberDistFromMon(G4double val, G4int mon);
     
#ifndef FLUGG
   void UpdateGeometry();
#endif

    void SetBeamType(const G4String& name);
    void SetDuratekShift(G4double shift);
    void SetBlockShift(G4double shift);
    void SetDeltaOuterThickness(G4double delta);

    void SetBafflePosition(const G4ThreeVector& pos);
    void SetTargetPosition(const G4ThreeVector& pos);
    void SetHorn1Position(const G4ThreeVector& pos);
    void SetHorn2Position(const G4ThreeVector& pos);
    void SetHorn1Rotation(const G4ThreeVector& rot);
    void SetHorn2Rotation(const G4ThreeVector& rot);

    void SetBaffleOuterRadius(G4double Rout);
    void SetBaffleInnerRadius(G4double Rin);
    void SetBaffleLength(G4double length);
    
    void SetForcedOldTarget(G4bool forced);
    //Place a new public interface to check that we are in the field region, in case we have 
    // the Alternate Horn1 
    //
   G4double PHorn1ICRin(G4double z) const ;
    
  //New cards for beam optimization (Leo, July 13, 2018)
  void SetNumberOfMEFins(G4double n_fins_me);
  void SetDistanceBetweenMEFins(G4double dist_fins_me);
  
private:
   NumiDataInput* NumiData;
   NumiMagneticField* numiMagField;
   NumiMagneticFieldIC* numiMagFieldIC;
   NumiMagneticFieldOC* numiMagFieldOC;
   NumiDecayPipeMagneticField* numiDecayMagField;
   
   void ConstructTargetHall();
   void ConstructTarget();
   void ConstructNOvATarget();
   void ConstructBaffle();
   void ConstructDecayPipe(G4bool=true, G4bool=false);
   void ConstructHadronAbsorber();  
   void ConstructHorns();  
   void ConstructHorn1(G4ThreeVector pos, G4RotationMatrix rot);
   void ConstructSpiderSupport(NumiHornSpiderSupport *HSS,
                               G4double angle,
                               G4double zPos,
                               G4double rIn,
                               G4double rOut,
                               G4VPhysicalVolume *motherVolume,
                               G4int copyNo); 
   void ConstructHorn1Alternate(G4ThreeVector pos, G4RotationMatrix rot);
   void CreateAndPlaceFinalIOUpstr(const std::string &aName, G4PVPlacement *mother);
   void Horn1InstallSpiderHanger(const G4String &nameStrH, double zLocTweakedDC, 
                                 double zPosMotherVolume, double outerRad, G4PVPlacement *vMother );
   int GetNumberOfInnerHornSubSections(size_t eqn, double z1, double z2, int nMax) const;

   void ConstructHorn2(G4ThreeVector pos, G4RotationMatrix rot);
   void ConstructSecMonitors();
   void DefineMaterials();
   G4VisAttributes* GetMaterialVisAttrib(G4int matCode);
   G4VisAttributes* GetMaterialVisAttrib(G4String matName);
   void DestroyMaterials();
   
#ifndef FLUGG
   // Messenger
   NumiDetectorMessenger *detectorMessenger;
#endif
   
   G4VPhysicalVolume* GetPhysicalVolume(G4String PVname);
   G4Material* GetMaterial(G4int matcode);
   G4double phornRgivenZ(G4double a, G4double b, G4double c, G4double z);
   G4double PHorn2ICRin(G4double z);
   G4double PHorn2ICRout(G4double z);
   G4double PHorn2OCRin(G4double z);
   G4double PHorn2OCRout(G4double z);
//   G4double PHorn1ICRin(G4double z); // Not removed!  See above.  Needed to support Alternate Horn1 
   G4double PHorn1ICRout(G4double z);
   G4double PHorn1OCRin(G4double z);
   G4double PHorn1OCRout(G4double z);
   
   // Materials
   G4Material* Vacuum;
   G4Material* DecayPipeVacuum;
   G4Material* DecayPipeHelium;
   G4Material* SecMonitorHelium;
   G4Material* TargetHelium;
   G4Material* Air;
   G4Material* Water;

   G4Material* Be;
   G4Material* C;
   G4Material* Al;
   G4Material* Ar;
   G4Material* Pb;
   G4Material* Fe;
   G4Material* CT852;
   G4Material* Concrete;
   G4Material* Shotcrete;
   G4Material* Rebar_Concrete;
   G4Material* Target;
   G4Material* DolomiteRock;
   G4Material* DoloStone;
   G4Material* MaqShale;
   G4Material* Chert;
   G4Material* Pyrite;
   G4Material* MaqSiltstone;
   G4Material* var_Al;
   G4Material* var_Stl;
   G4Material* Slab_Stl;
   G4Material* Blu_Stl;
   G4Material* n1018_Stl;
   G4Material* A500_Stl;
   G4Material* M1018_Stl;
   G4Material* Alumina;
   G4Material* HeGas;
   G4Material* Drywall;
   G4Material* Paraffin;
   G4Material* DefaultMaterial;



  // Logical volumes
  //
  G4LogicalVolume* ROCK_log;
  G4LogicalVolume* TRGT_lv;
  // G4LogicalVolume* lvTUNE;
  G4LogicalVolume* BLK_log[100]; 
  G4LogicalVolume* CShld_log[15];
  G4LogicalVolume* TGAR_log;
  G4LogicalVolume* Horn_PM_lv[8];
  G4LogicalVolume* LVCPipe[20];
  G4LogicalVolume* LVCPipeW[20];
  G4LogicalVolume* HadrBox_log;
  G4LogicalVolume* ShldBox_log;

  // Physical volumes
  //
  G4VPhysicalVolume* ROCK;
  G4VPhysicalVolume* pvTUNE;
  G4VPhysicalVolume* TGAR;
  G4VPhysicalVolume* TRGT;
  G4VPhysicalVolume* PHORN[8];
  G4VPhysicalVolume* PVCPipe[20];
  G4VPhysicalVolume* CNT[20];
  G4VPhysicalVolume* HadrBox;
  G4VPhysicalVolume* ShldBox;
  
  //Solids
  //
  G4VSolid* BLK_solid[100];
  G4VSolid* CShld_solid[15];
  G4VSolid* Horn_PM[8];

    G4String fBeamType;
    G4double fDuratekShift;
    G4double fTHBlockShift;
    G4double fDeltaOuterThickness;

    G4ThreeVector fBafflePosition;
    G4ThreeVector fTargetPosition;
    G4ThreeVector fHorn1Position;
    G4ThreeVector fHorn2Position;
    
    G4RotationMatrix fHorn1Rotation;
    G4RotationMatrix fHorn2Rotation;

        /// Baffle dimensions which can be modified through messenger class
    G4double fBaffleOuterRadius;
    G4double fBaffleInnerRadius;
    G4double fBaffleLength;

        /// Allow to run ME beam simulation with the old target
    G4bool fForcedOldTarget; 
    // 
    // The extra layer of Aluminum on top of Horn1 Conductor 
    // 
    G4double fHorn1ExtraLayerAlum; 
    // 
    // The Horn1 equation, implemented first for LBNE.. 
    //
    std::vector<LBNEHornRadialEquation> fHorn1Equations;
    //
    // September 2017: study of the sensitivity of the ME wiggle 
    // to the field map... 
    //
    bool fIgnoreCEQBr;
    double fHorn1FieldZCutUpstream;
    double fHorn1FieldZCutDwnstream;
    //
    bool fHorn1UpstrIOisTorus;
    double fHorn1CurrentEqualizerLongAbsLength;
    double fHorn1CurrentEqualizerQuadAmpl;
    double fHorn1CurrentEqualizerOctAmpl;
    double fHorn2CurrentEqualizerLongAbsLength;
    double fHorn2CurrentEqualizerQuadAmpl;
    double fHorn2CurrentEqualizerOctAmpl;
    
    double fNovaTargetHTilt;
    double fNovaTargetVTilt;
    double fNovaTargetXOffset;
    double fNovaTargetYOffset;
    double fNovaTargetExtraFlangeThick;
    double fHorn1StripLinesThick;
    //
    // Imperfect Current equalizer section support. 
    // To generate the RZ map, one needs a the accurate position of the IC, + thickness. 
    // Define a simple array.. 
    //
    std::vector<double> fEffectiveRadiiForFieldMap; 
    std::vector<double> fEffectiveRadiiForFieldMapH2; 
    
    G4RotationMatrix fRotationMotherNovaTarget;

  //New cards for beam optimization (Leo, July 13, 2018)
  G4int this_NumberOfMEFins;
  G4double this_DistanceBetweenMEFins;
  
    public:
    
    inline void SetIgnoreCEQBr(bool t) { fIgnoreCEQBr= t; }
    inline void SetHorn1FieldZCutUpstream(double p) { fHorn1FieldZCutUpstream= p; }
    inline void SetHorn1FieldZCutDwnstream(double p) { fHorn1FieldZCutDwnstream= p; }
    inline void SetHorn1CurrentEqualizerLongAbsLength(double p) { fHorn1CurrentEqualizerLongAbsLength= p; }
    inline void SetHorn1CurrentEqualizerQuadAmpl(double p) { fHorn1CurrentEqualizerQuadAmpl= p; }
    inline void SetHorn1CurrentEqualizerOctAmpl(double p) { fHorn1CurrentEqualizerOctAmpl= p; }
    inline void SetHorn2CurrentEqualizerLongAbsLength(double p) { fHorn2CurrentEqualizerLongAbsLength= p; }
    inline void SetHorn2CurrentEqualizerQuadAmpl(double p) { fHorn2CurrentEqualizerQuadAmpl= p; }
    inline void SetHorn2CurrentEqualizerOctAmpl(double p) { fHorn2CurrentEqualizerOctAmpl= p; }
    inline void SetHorn1UpstrIOisTorus(bool isTorus) { fHorn1UpstrIOisTorus = isTorus; }
    
    inline void SetNovaTargetHTilt(double p) { fNovaTargetHTilt = p; }
    inline void SetNovaTargetVTilt(double p) { fNovaTargetVTilt = p; }
    inline void SetNovaTargetXOffset(double p) { fNovaTargetXOffset= p; }
    inline void SetNovaTargetYOffset(double p) { fNovaTargetYOffset= p; }
    inline void SetNovaTargetExtraFlangeThick(double p) 
       { fNovaTargetExtraFlangeThick= p; }
    inline void SetHorn1StripLinesThick(double p) 
       { fHorn1StripLinesThick= p; }
   
    
    
    

};
class LBNEHornRadialEquation  {

  public:
      LBNEHornRadialEquation(); // to be able to store in an stl vector. 
      LBNEHornRadialEquation(double rSqrtCoefficient, double zCoefficient, double rOffset, bool parabolic=true);
      double GetVal(double z) const ; 
      void test1() const; // Cross check for equation 1. Will generate G4Exception 
  
  private:
      static double inchDef;
      bool parabolic;
      double rCoeff;
      double zCoeff;
      double rOff;
      double rResc;
      double zResc;
      
  public:    
    inline void SetLongRescaleFactor(double r) {zResc = r;}   // Applicable only for LBNE, but it does not hurt to leave them here.. 
    inline void SetRadialRescaleFactor(double r) {rResc = r;}

};

#endif

