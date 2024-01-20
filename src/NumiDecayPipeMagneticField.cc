//----------------------------------------------------------------------
//DecayPipeMagneticField 
// $Id
//----------------------------------------------------------------------

#include "NumiDecayPipeMagneticField.hh"
#include "G4ios.hh"
#include "G4TransportationManager.hh"
#include "NumiDataInput.hh" 
#include "G4VPhysicalVolume.hh"

//magnetic field inside the decay pipe ====================================================
NumiDecayPipeMagneticField::NumiDecayPipeMagneticField()
{
  NumiData=NumiDataInput::GetNumiDataInput();

}

NumiDecayPipeMagneticField::~NumiDecayPipeMagneticField(){;}

void NumiDecayPipeMagneticField::GetFieldValue(const double Point[3],double *Bfield) const
{
  static bool first = true;
  G4Navigator* numinav=new G4Navigator(); //geometry navigator
  G4Navigator* theNav=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  numinav->SetWorldVolume(theNav->GetWorldVolume());
  G4ThreeVector Pos=G4ThreeVector(Point[0],Point[1],Point[2]); 
  G4VPhysicalVolume* myVol = numinav->LocateGlobalPointAndSetup(Pos);
  G4TouchableHistoryHandle touchable = numinav->CreateTouchableHistoryHandle();
  G4ThreeVector localPos = touchable->GetHistory()->GetTopTransform().TransformPoint(Pos);

  delete numinav;
  
  //Average values measured by J. Hylen 2003: Z=beam dir, X=beam left, Y=Up
 Bfield[0] = 0.1*gauss; 
 Bfield[1] = -0.3*gauss; 
 Bfield[2] = -0.07*gauss;
 
 // Bfield[0] = 0.0*gauss; 
 // Bfield[1] = 0.0*gauss; 
 // Bfield[2] = 0.0*gauss; 

  //Max values measured by J. Hylen 2003: Z=beam dir, X=beam left, Y=Up
  //  Bfield[0] = 0.4*gauss; 
  //  Bfield[1] = -0.7*gauss; 
  //  Bfield[2] = -0.7*gauss; 

  if ( first )  first = false;


}

