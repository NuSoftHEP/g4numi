//----------------------------------------------------------------------
// $Id: NumiDetectorConstruction.cc,v 1.13.2.5 2009/10/20 16:13:55 martens Exp $
//----------------------------------------------------------------------

#include "NumiDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4Cons.hh"
#include "G4Torus.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4PVReplica.hh"
#include "G4AssemblyVolume.hh"
#include "NumiMagneticField.hh"
#include "G4Transform3D.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4RegionStore.hh"
#include "G4SolidStore.hh"
#include "G4GeometryManager.hh"
#ifndef FLUGG
#include "G4RunManager.hh"
#endif

NumiDetectorConstruction::NumiDetectorConstruction()
{
  //Scan the input file     
  NumiData = NumiDataInput::GetNumiDataInput();

  // Pointers for magnetic fields ***    
  numiMagField = new NumiMagneticField(); 
  numiMagFieldIC = new NumiMagneticFieldIC();
  numiMagFieldOC = new NumiMagneticFieldOC();
  
#ifndef FLUGG
  // create commands for interactive definition of the geometry
  detectorMessenger = new NumiDetectorMessenger(this);
#endif
  DefineMaterials();
}

NumiDetectorConstruction::~NumiDetectorConstruction()
{
  delete numiMagField; 
  delete numiMagFieldIC;
  delete numiMagFieldOC; 
  delete NumiData;
  DestroyMaterials();
#ifndef FLUGG
  delete detectorMessenger;
#endif
}

G4VPhysicalVolume* NumiDetectorConstruction::Construct()
{
  if (!NumiData) {
      /** FLUGG
       * Constructor is not called in flugg, only Construct
       * So constructor code has been moved here.
       */
      //Scan the input file     
      NumiData = NumiDataInput::GetNumiDataInput();

      // Pointers for magnetic fields ***    
      numiMagField = new NumiMagneticField(); 
      numiMagFieldIC = new NumiMagneticFieldIC();
      numiMagFieldOC = new NumiMagneticFieldOC();

      DefineMaterials();
  }

  /* FLUGG - Fluka wants a rectangular world volume
  //Define world volume 
  G4Tubs* ROCK_solid = new G4Tubs("ROCK_solid",0.,NumiData->RockRadius,NumiData->RockHalfLen,0,360.*deg);
  //  ROCK_log = new G4LogicalVolume(ROCK_solid,DoloStone,"ROCK_log",0,0,0);
  */
  G4Box* ROCK_solid = new G4Box("ROCK_solid",NumiData->RockRadius,
                                NumiData->RockRadius,NumiData->RockHalfLen); 
  ROCK_log = new G4LogicalVolume(ROCK_solid, DoloStone,"ROCK_log",0,0,0); 
  ROCK_log->SetVisAttributes(G4VisAttributes::Invisible);
  ROCK = new G4PVPlacement(0,G4ThreeVector(),ROCK_log,"ROCK",0,false,0);
  
  ConstructTargetHall();
  ConstructDecayPipe();
  ConstructBaffle();
  // insertion point for horn 1 is @3cm
  // drawings have z=0 at insertion point
  G4ThreeVector horn1pos(0.,0.,3.*cm);
  G4RotationMatrix horn1rot(0.,0.,0.);
  ConstructHorn1(horn1pos,horn1rot);
  // Okay this is confusing. move horn 1 back by 3cm to match position in gnumi (and in drawings)
  // but horn 2 stays i the same place. If you want to make a 'gnumi-like' horn1 you need to go (in horn1 coordinates)
  // from -3*cm to 2.97m  (the -3 cm probably doesnt matter since not many particles will make it through there anyway)

  //Placing horn at 19m for the NOvA ME configuration - L. Corwin 2009 July 01

  G4ThreeVector horn2pos(0.,0.,19*m); 
  G4RotationMatrix horn2rot(0.,0.,0.);
  if (NumiData->jCompare) {
    G4ThreeVector horn2pos(0.,0.,19.03*m);        
    ConstructHorn2(horn2pos,horn2rot);
  }
  else {
    G4ThreeVector horn2pos(0.,0.,19.*m);
    ConstructHorn2(horn2pos,horn2rot);    
  }

  if (NumiData->constructTarget){
    ConstructTarget();
  }
    ConstructHadronAbsorber(); 
   ConstructSecMonitors();

#ifndef FLUGG
  //Set Vis Attributes according to solid material (only for volumes not explicitly set)
  G4LogicalVolumeStore* lvStore=G4LogicalVolumeStore::GetInstance();
  lvStore=G4LogicalVolumeStore::GetInstance();
  for (size_t ii=0;ii<(*lvStore).size();ii++){   
    if ((*lvStore)[ii]->GetVisAttributes()==0) {
      G4String matName=(*lvStore)[ii]->GetMaterial()->GetName();
      (*lvStore)[ii]->SetVisAttributes(GetMaterialVisAttrib(matName));
    }
  }
#endif

  return ROCK;
}
 
G4VPhysicalVolume* NumiDetectorConstruction::GetPhysicalVolume(G4String PVname)
{
  G4PhysicalVolumeStore* PVStore=G4PhysicalVolumeStore::GetInstance();
  for (size_t ii=0;ii<(*PVStore).size();ii++){
    if ((*PVStore)[ii]->GetName()==PVname) return (*PVStore)[ii];
  }
  G4cout << "NumiDetectorConstruction: Volume "<<PVname<< " is not in Physical Volume Store"<<G4endl;
  return 0;
}
/*
void NumiDetectorConstruction::SetTargetGas(G4String gasChoice) {

	G4Material* pttoMaterial = G4Material::GetMaterial(gasChoice);
	if (pttoMaterial)
*/

void NumiDetectorConstruction::SetTargetZ0(G4double val) {

	NumiData->SetTargetZ0(val);
	G4cout << " Geometry changed: target position Z0 = " << NumiData->TargetZ0/cm << " cm"<<G4endl;
	G4cout << " Geometry changed: baffle position Z0 = " << NumiData->HPBaffleZ0/cm << " cm"<<G4endl;
	
}

void NumiDetectorConstruction::SetHornCurrent(G4double val) {

	NumiData->SetHornCurrent(val);
	G4cout << " Geometry changed: Horn current = " << NumiData->HornCurrent/ampere<<" A"<< G4endl;

}

#ifndef FLUGG
void NumiDetectorConstruction::UpdateGeometry() {


  // Clean old geometry
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  //G4RegionStore::GetInstance()->Clean();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  
}
#endif
