//----------------------------------------------------------------------
//$Id: NumiBaffle.cc,v 1.3.2.2 2009/09/24 16:24:11 martens Exp $
//----------------------------------------------------------------------


#include "NumiDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "NumiDataInput.hh"

void NumiDetectorConstruction::ConstructBaffle()

{ 
  G4ThreeVector targetHallPosition=G4ThreeVector(0,0,NumiData->TargetAreaLength/2.+NumiData->TargetAreaZ0);

 // Baffle 
  G4double HPBRin=NumiData->HPBaffleRin;
  G4double HPBRout=NumiData->HPBaffleRout;
  G4double HPBlength=NumiData->HPBaffleLength;
  G4Tubs* sBaffle=new G4Tubs("sBaffle",HPBRin,HPBRout,HPBlength/2.,0.,360.*deg);
  G4LogicalVolume* lvBaffle=new G4LogicalVolume(sBaffle,Target,"lvBaffle",0,0,0);
  G4ThreeVector bafflePos=G4ThreeVector(NumiData->HPBaffleX0,NumiData->HPBaffleY0,NumiData->HPBaffleZ0+HPBlength/2.)-targetHallPosition;
  new G4PVPlacement(0,bafflePos,"Baffle",lvBaffle,TGAR,false,0);
}
