//----------------------------------------------------------------------
//NumiMagneticField 
// $Id
//----------------------------------------------------------------------

#include "NumiMagneticField.hh"
#include "G4ios.hh"
#include "G4TransportationManager.hh"
#include "NumiDataInput.hh" 
#include "G4VPhysicalVolume.hh"

//magnetic field between conductors ====================================================
NumiMagneticField::NumiMagneticField()
{
  NumiData=NumiDataInput::GetNumiDataInput();

}

NumiMagneticField::~NumiMagneticField(){;}

void NumiMagneticField::GetFieldValue(const double Point[3],double *Bfield) const
{
  static bool first = true;
  
  G4double current = NumiData->HornCurrent/ampere/1000.;
  G4double radius = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);    
  G4double B = current / (5.*radius/cm)/10*tesla;  //B(kG)=i(kA)/[5*r(cm)], 1T=10kG
  Bfield[0] = -B*Point[1]/radius;
  Bfield[1] = B*Point[0]/radius;
  Bfield[2] = 0.;

  // Martens -- used to verify the horn current that was used
  static bool first_call = true;
  if (first_call) {
    G4cout << "First call to NumiMagneticField::GetFieldValue" << G4endl;
    G4cout << "  Horn current is set to " << NumiData->HornCurrent/ampere << " Amperes " << G4endl;
    first_call = false;
  }


  if(NumiData->jCompare) {// Make gnumi like horns - this is for validation
    G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
    G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
    G4ThreeVector Position=G4ThreeVector(Point[0],Point[1],Point[2]);
    G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
    G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
    G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position);
    
    delete numinavigator;
    
    
    if(localPosition.z()>3*m || localPosition.z()<0*m)  {
      if (first) {
        G4cout << "Applying jCompare in NumiMagneticField." << G4endl;
        first = false;
      }
      Bfield[0]=0;
      Bfield[1]=0;
      Bfield[2]=0;
    }     
  }
}

//magnetic field in inner conductor ====================================================
NumiMagneticFieldIC::NumiMagneticFieldIC()
{
  NumiData=NumiDataInput::GetNumiDataInput();
}

NumiMagneticFieldIC::~NumiMagneticFieldIC(){;}

void NumiMagneticFieldIC::GetFieldValue(const double Point[3],double *Bfield) const
{
  static bool first = true;

  G4double current = NumiData->HornCurrent/ampere/1000.;
  G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
  G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
  G4ThreeVector Position=G4ThreeVector(Point[0],Point[1],Point[2]); 
  G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
  G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
  G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position);

  delete numinavigator;

  G4VSolid * solid=myVolume->GetLogicalVolume()->GetSolid();

  G4double radius = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);
  G4double dOut=0.;
  G4double dIn=0.;
  G4double magBField = 0.;  

  if (myVolume->GetName().contains("IC")){
    dOut=solid->DistanceToOut(localPosition,G4ThreeVector(Point[0]/radius,Point[1]/radius,0)); //distance to outer boundary
    dIn=solid->DistanceToOut(localPosition,G4ThreeVector(-Point[0]/radius,-Point[1]/radius,0));//distance to inner boundary
    if (dOut<1.*m&&dIn<1.*m&&(dOut!=0.&&dIn!=0.)) 
      {
	magBField = current / (5.*radius/cm)/10*tesla; //B(kG)=i(kA)/[5*r(cm)], 1T=10kG
	magBField=magBField*((radius*radius-(radius-dIn)*(radius-dIn))/((radius+dOut)*(radius+dOut)-(radius-dIn)*(radius-dIn)));// linear distribution of current
      }
  }
  /*
  if (Point[2]>92*cm&&Point[2]<92.1*cm) {
     G4cout<<"ICMag: "<<myVolume->GetName()<<" "
	<<Point[0]<<" "
	<<Point[1]<<" "
	<<Point[2]<<" "
	<<sqrt(Point[0]*Point[0]+Point[1]*Point[1])<<" "
	<<magBField<<G4endl;
  }
  */
  if (radius!=0){
    Bfield[0] = -magBField*Point[1]/radius;
    Bfield[1] = magBField*Point[0]/radius;
    Bfield[2] = 0.;}
  else{
    Bfield[0] = 0.; 
    Bfield[1] = 0.; 
    Bfield[2] = 0.; 
  }

    if(NumiData->jCompare &&(localPosition.z()>3*m || localPosition.z()<0*m)) // Make gnumi like horns - this is for validation
    {  
      if (first) {
        G4cout << "Applying jCompare in NumiMagneticFieldIC." << G4endl;
        first = false;
      }      
      Bfield[0]=0;
      Bfield[1]=0;
      Bfield[2]=0;
    }

}


//magnetic field in outter conductor====================================================
NumiMagneticFieldOC::NumiMagneticFieldOC()
{
  NumiData=NumiDataInput::GetNumiDataInput();
}

NumiMagneticFieldOC::~NumiMagneticFieldOC(){;}

void NumiMagneticFieldOC::GetFieldValue(const double Point[3],double *Bfield) const
{
  static bool first = true;

  G4double current = NumiData->HornCurrent/ampere/1000.;
  G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
  G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
  G4ThreeVector Position=G4ThreeVector(Point[0],Point[1],Point[2]); 
  G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
  G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
  G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position);

  delete numinavigator;

  G4VSolid *solid=myVolume->GetLogicalVolume()->GetSolid();

  G4double radius = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);
  G4double dOut=0.;
  G4double dIn=0.;
  G4double magBField = 0.;

  if (myVolume->GetName().contains("OC")){
  dOut=solid->DistanceToOut(localPosition,G4ThreeVector(Point[0]/radius,Point[1]/radius,0)); //distance to outer boundary
  dIn=solid->DistanceToOut(localPosition,G4ThreeVector(-Point[0]/radius,-Point[1]/radius,0));//distance to inner boundary
  if (dOut<1.*m&&dIn<1.*m&&(dOut!=0.&&dIn!=0.)) 
    {
      magBField = current / (5.*radius/cm)/10*tesla; //B(kG)=i(kA)/[5*r(cm)], 1T=10kG
      G4double rIn=radius-dIn;
      G4double rOut=radius+dOut;
      magBField=magBField*(1-(radius*radius-rIn*rIn)/(rOut*rOut-rIn*rIn)); // linear distribution of current
    }
  }

  if (radius!=0){
  Bfield[0] = -magBField*Point[1]/radius;
  Bfield[1] = magBField*Point[0]/radius;
  Bfield[2] = 0.;
  }
  else{
    Bfield[0] = 0.; 
    Bfield[1] = 0.; 
    Bfield[2] = 0.;
  }
  
  if(NumiData->jCompare &&(localPosition.z()>3*m || localPosition.z()<0*m)) // Make gnumi like horns - this is for validation
    {
      if (first) {
        G4cout << "Applying jCompare in NumiMagneticFieldOC." << G4endl;
        first = false;
      }      
      Bfield[0]=0;
      Bfield[1]=0;
      Bfield[2]=0;
    }

}
