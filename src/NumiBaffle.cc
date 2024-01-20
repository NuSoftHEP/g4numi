//----------------------------------------------------------------------
//$Id: NumiBaffle.cc,v 1.3.4.4 2014/08/26 19:01:42 lebrun Exp $
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
  G4PVPlacement* pvbaffle = new G4PVPlacement(0,bafflePos,"pvBaffleMother",lvBaffle,TGAR,false,0);
  pvbaffle -> CheckOverlaps();
 // 
 // Add the windows.  Paul Lebrun, August 26 2014. 
 //
 const double radW = 1.5*NumiData->HPBaffleRin;
 const double windowThickness = 0.5*mm; // August 26 2014, based on e-mail from Bob Zwaska, talk by Mike Kodorski, August 20 2014. 
 G4Tubs* sBaffleWindow=new G4Tubs("sBaffleWindow",0. ,radW, windowThickness/2., 0.,360.*deg);
 G4LogicalVolume* lvBaffleWindow=new G4LogicalVolume(sBaffleWindow, Be,"lvBaffleWindow",0,0,0);
 G4ThreeVector bafflePosUpstr(bafflePos); bafflePosUpstr[2] -=  NumiData->HPBaffleLength/2. + windowThickness/2. + 0.005*mm;
 G4PVPlacement* pvbaffleUpstr = new G4PVPlacement(0,bafflePosUpstr,"pvBaffleMother",lvBaffleWindow,TGAR,false,0);
 pvbaffleUpstr -> CheckOverlaps();
 G4ThreeVector bafflePosDownstr(bafflePos); bafflePosDownstr[2] += NumiData->HPBaffleLength/2. + windowThickness/2. + 0.005*mm;
 G4PVPlacement* pvbaffleDownstr = new G4PVPlacement(0,bafflePosDownstr,"pvBaffleMother",lvBaffleWindow,TGAR,false,0);
 pvbaffleDownstr -> CheckOverlaps();
  if(NumiData->GetPrintGeometry())
  {
     G4cout << "Z0   Position of baffle = " << (bafflePos.z() - (HPBlength/2.) )/m << " m " << G4endl;
     G4cout << "Z    Position of baffle = " << (bafflePos.z())/m << " m " << G4endl;
     G4cout << "ZEnd Position of baffle = " << (bafflePos.z() + (HPBlength/2.) )/m << " m " << G4endl;
     G4cout << "Position of Baffle window, upstr  = " << bafflePosUpstr[2] << " mm " << " with respect to Baffle Mother " << G4endl;
     G4cout << "Position of Baffle window, downstr  = " << bafflePosDownstr[2] << " mm " << G4endl;
  }
}
