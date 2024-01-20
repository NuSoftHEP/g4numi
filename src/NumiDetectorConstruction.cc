//----------------------------------------------------------------------
// $Id: NumiDetectorConstruction.cc,v 1.13.4.11.2.3 2018/07/24 20:26:28 laliaga Exp $
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
#include "NumiDecayPipeMagneticField.hh"
#include "G4Transform3D.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UnitsTable.hh"

#include "G4RegionStore.hh"
#include "G4SolidStore.hh"
#include "G4GeometryManager.hh"
#ifndef FLUGG
#include "G4RunManager.hh"
#endif

NumiDetectorConstruction::NumiDetectorConstruction()
    : fBeamType("Unknown"),
      fDuratekShift(0.0),
      fTHBlockShift(0.0),
      fDeltaOuterThickness(0.0),
      fBaffleOuterRadius(3.0*cm),
      fBaffleInnerRadius(5.5*mm),
      fBaffleLength(1.5*m),
      fForcedOldTarget(false),
      fHorn1ExtraLayerAlum(0.),
      fHorn1FieldZCutUpstream(-32.0),
      fHorn1FieldZCutDwnstream(3277.),
      fHorn1UpstrIOisTorus(true),
      fHorn1CurrentEqualizerLongAbsLength(0.),
      fHorn1CurrentEqualizerQuadAmpl(0.),
      fHorn1CurrentEqualizerOctAmpl(0.),
      fNovaTargetHTilt(0.),
      fNovaTargetVTilt(0.),
      fNovaTargetXOffset(0.),
      fNovaTargetYOffset(0.),
      fNovaTargetExtraFlangeThick(0.),
      fHorn1StripLinesThick(0.)
{
  //Scan the input file     
  NumiData = NumiDataInput::GetNumiDataInput();

  if(NumiData->GetDebugLevel() > 0)
  {
     std::cout << "NumiDetectorConstruction Constructor Called." << std::endl;
  }

  // Pointers for magnetic fields ***    
  numiMagField = new NumiMagneticField();
  numiMagField->SetIgnoreCEQBr(fIgnoreCEQBr);
  numiMagField->SetHorn1FieldZCutUpstream(fHorn1FieldZCutUpstream); 
  numiMagField->SetHorn1FieldZCutDwnstream(fHorn1FieldZCutDwnstream);
  numiMagField->SetHorn1CurrentEqualizerLongAbsLength(fHorn1CurrentEqualizerLongAbsLength);
  numiMagField->SetHorn1CurrentEqualizerQuadAmpl(fHorn1CurrentEqualizerQuadAmpl); 
  numiMagField->SetHorn1CurrentEqualizerOctAmpl(fHorn1CurrentEqualizerOctAmpl); 
  numiMagFieldIC = new NumiMagneticFieldIC();
  numiMagFieldOC = new NumiMagneticFieldOC();
  numiDecayMagField = new NumiDecayPipeMagneticField();

#ifndef FLUGG
  // create commands for interactive definition of the geometry
  detectorMessenger = new NumiDetectorMessenger(this);
#endif
  DefineMaterials();
}

NumiDetectorConstruction::~NumiDetectorConstruction()
{

   if(NumiData->GetDebugLevel() > 0)
   {
      std::cout << "NumiDetectorConstruction Destructor Called." << std::endl;
   }

   delete numiMagField; 
   delete numiMagFieldIC;
   delete numiMagFieldOC; 
   delete numiDecayMagField;
   delete NumiData;
   DestroyMaterials();
#ifndef FLUGG
   delete detectorMessenger;
#endif
}

G4VPhysicalVolume* NumiDetectorConstruction::Construct()
{
   G4cout << G4endl;
   G4cout << G4endl;
   G4cout << "********************************************************************" << G4endl;
   G4cout << "********************************************************************" << G4endl;
   G4cout << "NumiDetectorConstruction::Construct Called. Constructing Detector..." << G4endl;
   G4cout << "********************************************************************" << G4endl;
   
   if(NumiData->GetDebugLevel() > 0)
   {
      G4cout << G4endl;
      G4cout << G4endl;
      NumiData -> Print();
      G4cout << G4endl;
      G4cout << G4endl;
   }
   
   
      
  if (!NumiData) {
      /** FLUGG
       * Constructor is not called in flugg, only Construct
       * So constructor code has been moved here.
       */
      //Scan the input file     
      NumiData = NumiDataInput::GetNumiDataInput();

      // Pointers for magnetic fields ***    
      numiMagField = new NumiMagneticField(); 
      numiMagField->SetIgnoreCEQBr(fIgnoreCEQBr);
      numiMagField->SetHorn1FieldZCutUpstream(fHorn1FieldZCutUpstream); 
      numiMagField->SetHorn1FieldZCutDwnstream(fHorn1FieldZCutDwnstream);
      numiMagField->SetHorn1CurrentEqualizerLongAbsLength(fHorn1CurrentEqualizerLongAbsLength);
      numiMagField->SetHorn1CurrentEqualizerQuadAmpl(fHorn1CurrentEqualizerQuadAmpl); 
      numiMagField->SetHorn1CurrentEqualizerOctAmpl(fHorn1CurrentEqualizerOctAmpl); 
      numiMagFieldIC = new NumiMagneticFieldIC();
      numiMagFieldOC = new NumiMagneticFieldOC();
      numiDecayMagField = new NumiDecayPipeMagneticField();
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
  ConstructDecayPipe(NumiData->HeInDecayPipe, NumiData->applyDecayPipeMagneticField);
  ConstructBaffle();
  // insertion point for horn 1 is @3cm
  // drawings have z=0 at insertion point
  G4ThreeVector horn1pos(NumiData->Horn1X0, NumiData->Horn1Y0, NumiData->Horn1Z0);
  G4RotationMatrix horn1rot(NumiData->Horn1Phi, NumiData->Horn1Theta, NumiData->Horn1Psi);
  ConstructHorn1(horn1pos,horn1rot);
  // Okay this is confusing. move horn 1 back by 3cm to match position in gnumi (and in drawings)
  // but horn 2 stays i the same place. If you want to make a 'gnumi-like' horn1 you need to go (in horn1 coordinates)
  // from -3*cm to 2.97m  (the -3 cm probably doesnt matter since not many particles will make it through there anyway)

//prachi start
  ConstructNumiHorn1TrackingPlane(TGAR);//original
  //ConstructNumiHorn1TrackingPlane(ROCK);//test1

//prachi end


  G4ThreeVector horn2pos(NumiData->Horn2X0, NumiData->Horn2Y0, NumiData->Horn2Z0);
  G4RotationMatrix horn2rot(NumiData->Horn2Phi, NumiData->Horn2Theta, NumiData->Horn2Psi);
  if (NumiData->jCompare)
  {
     horn2pos[0] = 0.0;
     horn2pos[1] = 0.0;
     horn2pos[2] = 10.03*m;

  }

  ConstructHorn2(horn2pos,horn2rot);

  
  if (NumiData->constructTarget)
  {
      if (fBeamType.find("me") != std::string::npos &&
          !fForcedOldTarget) ConstructNOvATarget();
      else ConstructTarget();
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


  G4cout << "********************************************************************" << G4endl;
  G4cout << "...Detector Construction Completed." << G4endl;
  G4cout << "********************************************************************" << G4endl;
  G4cout << "********************************************************************" << G4endl;
  G4cout << G4endl;
  G4cout << G4endl;
  
  

  return ROCK;
}
 

//prachi start
void NumiDetectorConstruction::ConstructNumiHorn1TrackingPlane(G4VPhysicalVolume* mother) {
    // Set the position and dimensions for the tracking plane
    double widthTop = 1.0*m;
    double heightTop = 1.0*m;
    double trackingPlanesHalfThickness = 0.5*mm;
    //double z = -1600*CLHEP::cm;  //calculated as the distance 0 gives virtual detector at z=1950, so for getting the virtual detector at 350 cm I had to do like 350-1950=1600; 350cm, 18cm from h1 end
    // double z = -1568*CLHEP::cm;//382cm, 50cm from h1 end
    //double z = -1518*CLHEP::cm; //432 cm, 100cm from h1 end
    //double z = -1468*CLHEP::cm;//482cm , 150cm from h1 end
    double z = -1418*CLHEP::cm; //532 cm, 200cm from h1 end 
    
    // Create a box representing the tracking plane
    G4Box* topBox = new G4Box("Horn1TrackingPlane", widthTop / 2., heightTop / 2., trackingPlanesHalfThickness);

    // Get the material from the mother volume (I think it is air already)
    G4Material* PMat = mother->GetLogicalVolume()->GetMaterial();

    // Create the logical volume for the tracking plane
    TrackingPlaneH1Logical = new G4LogicalVolume(topBox, PMat, "Horn1TrackingPlaneLogical");

    // Set the position of the tracking plane
     G4ThreeVector posTopLevel(0., 0., z);
    //G4ThreeVector posTopLevel(NumiData->Horn1X0, NumiData->Horn1Y0, 3400*CLHEP::mm -NumiData->Horn1Z0);

    // Place the tracking plane inside the mother volume
    G4PVPlacement* vTopLevel = new G4PVPlacement(0, posTopLevel, TrackingPlaneH1Logical, "Horn1TrackingPlane_P", mother->GetLogicalVolume(), false, 1, true);

    if (vTopLevel == NULL) {
        std::cerr << "Can't place Horn1 tracking plane.. Unexpected.." << std::endl;
    }
    else {
        std::cout << "Horn1 tracking plane placed successfully." << std::endl;
    }
}


//prachi end



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

/*
void NumiDetectorConstruction::SetTargetZ0(G4double val) {

	NumiData->SetTargetZ0(val);
	G4cout << " Geometry changed: target position Z0 = " << NumiData->TargetZ0/cm << " cm"<<G4endl;
	G4cout << " Geometry changed: baffle position Z0 = " << NumiData->HPBaffleZ0/cm << " cm"<<G4endl;
	
}

void NumiDetectorConstruction::SetHornCurrent(G4double val) {

	NumiData->SetHornCurrent(val);
	G4cout << " Geometry changed: Horn current = " << NumiData->HornCurrent/ampere<<" A"<< G4endl;

}
*/

//----------------------------------------------------------------------------------------  
void NumiDetectorConstruction::SetAbsorberConfig(G4String config)
{
   //
   // change Absorber configuration
   //
   NumiData -> SetAbsorberConfig(config);
}
//----------------------------------------------------------------------------------------
void NumiDetectorConstruction::SetMonAbsorberMaterial(G4String materialChoice, G4int mon)
{
  // search the material by its name
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);
  if (!pttoMaterial)
  {
     G4cout << "DetectorConstruction::SetAbsorberMaterial - Invalid Material: Setting Air" << G4endl;
     pttoMaterial = G4Material::GetMaterial("Air");
  }

  NumiData->SetAbsorberMaterial(pttoMaterial, mon);
}
//----------------------------------------------------------------------------------------  
void NumiDetectorConstruction::SetMonAbsorberThickness(G4double val, G4int mon)
{
   // change Absorber thickness
   NumiData->SetAbsorberThickness(val, mon);
}
//----------------------------------------------------------------------------------------
void NumiDetectorConstruction::SetAbsorberDistFromMon(G4double val, G4int mon)
{
   // change Distance betw Mon and Absorber
   NumiData->SetAbsorberMonDist(val, mon);
}

#ifndef FLUGG
void NumiDetectorConstruction::UpdateGeometry()
{

  // Clean old geometry
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  //G4RegionStore::GetInstance()->Clean();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  DefineMaterials();
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());

}
#endif


void NumiDetectorConstruction::SetBeamType(const G4String& beamType) {
    fBeamType = beamType;
}


void NumiDetectorConstruction::SetDuratekShift(G4double shift) {
    fDuratekShift = shift;
}

void NumiDetectorConstruction::SetBlockShift(G4double shift) {
    fTHBlockShift = shift;
}

void NumiDetectorConstruction::SetDeltaOuterThickness(G4double delta) {
    fDeltaOuterThickness = delta;
}


void NumiDetectorConstruction::SetBafflePosition(const G4ThreeVector& pos) {
    NumiData->HPBaffleX0 = pos.x();
    NumiData->HPBaffleY0 = pos.y();
    NumiData->HPBaffleZ0 = pos.z();
    
    fBafflePosition = pos;
}

void NumiDetectorConstruction::SetTargetPosition(const G4ThreeVector& pos) {
    NumiData->TargetX0 = pos.x();
    NumiData->TargetY0 = pos.y();
    NumiData->TargetZ0 = pos.z();

    fTargetPosition = pos;
}


void NumiDetectorConstruction::SetHorn1Position(const G4ThreeVector& pos) {
    NumiData->Horn1X0 = pos.x();
    NumiData->Horn1Y0 = pos.y();
    NumiData->Horn1Z0 = pos.z();
        
    fHorn1Position = pos;
}

void NumiDetectorConstruction::SetHorn2Position(const G4ThreeVector& pos) {
    NumiData->Horn2X0 = pos.x();
    NumiData->Horn2Y0 = pos.y();
    NumiData->Horn2Z0 = pos.z();
    
    fHorn2Position = pos;
}

void NumiDetectorConstruction::SetHorn1Rotation(const G4ThreeVector& rot) {
    NumiData->Horn1Phi   = rot.x();
    NumiData->Horn1Theta = rot.y();
    NumiData->Horn1Psi   = rot.z();

}

void NumiDetectorConstruction::SetHorn2Rotation(const G4ThreeVector& rot) {
    NumiData->Horn2Phi   = rot.x();
    NumiData->Horn2Theta = rot.y();
    NumiData->Horn2Psi   = rot.z();    
}

void NumiDetectorConstruction::SetBaffleOuterRadius(G4double Rout) {
    NumiData->HPBaffleRout = Rout;

    fBaffleOuterRadius = Rout;
}

void NumiDetectorConstruction::SetBaffleInnerRadius(G4double Rin) {
    NumiData->HPBaffleRin = Rin;

    fBaffleInnerRadius = Rin;
}

void NumiDetectorConstruction::SetBaffleLength(G4double length) {
    NumiData->HPBaffleLength = length;
    
    fBaffleLength = length;
}

void NumiDetectorConstruction::SetForcedOldTarget(G4bool forced) {
    fForcedOldTarget = forced;
}

