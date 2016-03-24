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
class NumiHornSpiderSupport;
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
#ifndef FLUGG
	void UpdateGeometry();
#endif
  //Place a new public interface to check that we are in the field region, in case we have 
  // the Alternate Horn1 
  //
  G4double PHorn1ICRin(G4double z) const ;
  
  private:
  NumiDataInput* NumiData;
  NumiMagneticField* numiMagField;
  NumiMagneticFieldIC* numiMagFieldIC;
  NumiMagneticFieldOC* numiMagFieldOC;
  
  void ConstructTargetHall();
  void ConstructTarget();
  void ConstructBaffle();
  void ConstructDecayPipe();
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
  //  G4double PHorn1ICRin(G4double z); // Not removed!  See above.  Needed to support Alternate Horn1 
  G4double PHorn1ICRout(G4double z);
  G4double PHorn1OCRin(G4double z);
  G4double PHorn1OCRout(G4double z);

  // Materials
  G4Material* Vacuum;
  G4Material* DecayPipeVacuum;
  G4Material* Air;
  G4Material* Water;
  G4Material* He;
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

  // 
  // The Horn1 equation, implemented first for LBNE.. 
  //
  std::vector<LBNEHornRadialEquation> fHorn1Equations;

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

