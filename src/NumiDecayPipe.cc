//----------------------------------------------------------------------
// $Id: NumiDecayPipe.cc,v 1.8.4.5 2017/05/11 10:52:32 lcremone Exp $
//----------------------------------------------------------------------

#include "NumiDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4Polycone.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4AssemblyVolume.hh"
#include "NumiDataInput.hh"
#include "NumiDecayPipeMagneticField.hh"
#include "G4FieldManager.hh"

static G4double in=2.54*cm;

void NumiDetectorConstruction::ConstructDecayPipe(bool heInDecayPipe, bool applyDecayPipeMagneticField)
{
  G4ThreeVector translation=G4ThreeVector(0.,0.,0.);
  G4RotationMatrix rotation=G4RotationMatrix(0.,0.,0.);

  // Tunnel
  G4double l=NumiData->TunnelLength/2;
  G4double r=NumiData->TunnelRadius;
  G4ThreeVector tunnelPosition=G4ThreeVector(0,0,l+NumiData->TunnelZ0);
 
  G4Tubs* sTUNE = new G4Tubs("TUNE_S",0.,r,l,0,360.*deg);
  G4LogicalVolume* lvTUNE = new G4LogicalVolume(sTUNE,GetMaterial(NumiData->TunnelGEANTmat),"TUNE_log",0,0,0); 
  lvTUNE->SetVisAttributes(G4VisAttributes::Invisible);
  pvTUNE = new G4PVPlacement(0,tunnelPosition,"TUNE",lvTUNE,ROCK,false,0);

  // Shielding
  l=NumiData->ShieldLength/2.;
  G4double rIn=NumiData->ShieldRin;
  G4double rOut=NumiData->ShieldRout;
  G4ThreeVector sc01Position=G4ThreeVector(NumiData->ShieldX0,NumiData->ShieldY0,NumiData->ShieldZ0+l)-tunnelPosition;

  G4Tubs* sSC01 = new G4Tubs("SC01_solid",rIn,rOut,l,0,360.*deg);
  G4LogicalVolume* lvSC01 = new G4LogicalVolume(sSC01,GetMaterial(NumiData->ShieldGEANTmat),"SC01_log",0,0,0); 
  new G4PVPlacement(0,sc01Position,"SC01",lvSC01,pvTUNE,false,0);

  //****************************************************
  //outer DP tracking volume
  G4Tubs* sDPOuterTrackerTube = new G4Tubs("sDPOuterTrackerTube",rOut,rOut+(0.001*mm),l+(0.001*mm),0,360.*deg);
  G4LogicalVolume* lvDPOuterTrackerTube = new G4LogicalVolume(sDPOuterTrackerTube,DecayPipeVacuum,"lvDPOuterTrackerTube",0,0,0); 

  G4Tubs* sDPOuterTrackerEnd = new G4Tubs("sDPOuterTrackerEnd",0.0,rOut,(0.001*mm),0,360.*deg);
  G4LogicalVolume* lvDPOuterTrackerEnd = new G4LogicalVolume(sDPOuterTrackerEnd,DecayPipeVacuum,"lvDPOuterTrackerEnd",0,0,0); 
  G4ThreeVector DPOuterTrackerEndPosition=G4ThreeVector(NumiData->ShieldX0,NumiData->ShieldY0,NumiData->ShieldZ0+(l*2.0)+(0.001*mm/2.0)*2.0)-tunnelPosition;

    
  //****************************************************


  // Decay Pipe
  l=NumiData->DecayPipeLength/2.;
  r=NumiData->DecayPipeRadius+NumiData->DecayPipeWallThick;
  G4ThreeVector decayPipePosition=G4ThreeVector(0,0,NumiData->DecayPipeZ0+l)-tunnelPosition;

  G4Tubs* sDPIP = new G4Tubs("sDPIP",0.,r,l,0,360.*deg);
  G4LogicalVolume* lvDPIP = new G4LogicalVolume(sDPIP,GetMaterial(NumiData->DecayPipeGEANTmat),"lvDPIP",0,0,0); 
  G4VPhysicalVolume* pvDPIP = new G4PVPlacement(0,decayPipePosition,"DPIP",lvDPIP,pvTUNE,false,0);
 
  // Decay Volume
  l=(NumiData->DecayPipeLength-NumiData->DecayPipeEWinThick)/2.;
  r=NumiData->DecayPipeRadius;
  G4ThreeVector decayVolumePosition=G4ThreeVector(0,0,(NumiData->DecayPipeFWinThick-NumiData->DecayPipeEWinThick)/2.);
  decayVolumePosition=G4ThreeVector(0,0,(-NumiData->DecayPipeEWinThick)/2.);//for non flat window

  G4Tubs* sDVOL = new G4Tubs("DVOL_solid",0.,r,l,0,360.*deg);
  G4Material* decayVolMaterial=DecayPipeVacuum;
  if(heInDecayPipe) decayVolMaterial=DecayPipeHelium;
  G4LogicalVolume* lvDVOL = new G4LogicalVolume(sDVOL,decayVolMaterial,"DVOL_log",0,0,0); 


  G4cout << "Just to be sure the value is " << applyDecayPipeMagneticField  << G4endl;
  // Decay pipe magnetic field
  if (applyDecayPipeMagneticField) {
    G4cout << "And Im in here "<< G4endl;
    G4FieldManager* FieldMngr = new G4FieldManager(numiDecayMagField); //create a local field 
    FieldMngr->SetDetectorField(numiDecayMagField); //set the field 
    FieldMngr->CreateChordFinder(numiDecayMagField); //create the objects which calculate the trajectory
    lvDVOL->SetFieldManager(FieldMngr,true); //attach the local field to logical volume
  }
  
  G4VPhysicalVolume* pvDVOL=new G4PVPlacement(0,decayVolumePosition,"DVOL",lvDVOL,pvDPIP,false,0);

  //****************************************************
  //inner DP tracking volume
  G4Tubs* sDPInnerTrackerTube = new G4Tubs("sDPInnerTrackerTube",r-(0.001*mm),r,l-(0.001*mm*2.0),0,360.*deg);
  G4LogicalVolume* lvDPInnerTrackerTube = new G4LogicalVolume(sDPInnerTrackerTube,decayVolMaterial,"lvDPInnerTrackerTube",0,0,0); 

  G4Tubs* sDPInnerTrackerEnd = new G4Tubs("sDPInnerTrackerEnd",0.0,r-(0.001*mm),(0.001*mm),0,360.*deg);
  G4LogicalVolume* lvDPInnerTrackerEnd = new G4LogicalVolume(sDPInnerTrackerEnd,decayVolMaterial,"lvDPInnerTrackerEnd",0,0,0); 
  G4ThreeVector DPInnerTrackerEndPosition=decayVolumePosition + G4ThreeVector(0,0,l+(0.001*mm/2.0));
  
  //****************************************************

  // Upstream window
  //couple of tubes and spheres and a polycone
  //for tubes we need Rin, Rout, material,volName,Z0,length
  G4int NUpWnTubesN=5;
  //G4double UpWnTubeZ0[]      ={45.28*m  ,45.3054*m , 45.4578*m , 45.4769*m, 45.496*m};
  G4double UpWnTubeZ0[]      ={2.295*in   , 3.295*in  ,  9.295*in  ,  10.045*in ,  10.795*in}; //Distance from beginning of decay pipe
  G4double UpWnTubeLength[]  ={1.*in    ,6.*in     , .75*in    , .75*in   , 6.*in    };
  G4double UpWnTubeRin[]     ={19.5*in  ,20.8125*in,19.5*in    , 19.5*in  , 20.8125*in };
  G4double UpWnTubeRout[]    ={22.*in   ,21.*in    ,22.*in     , 22.*in   , 21.*in};
  G4int UpWnTubeVolMaterial[]={9       , 9       ,  9       ,    10      , 10};
  G4String UpWnTubeVolName[] ={"UpWnAl1","UpWnAl2" ,"UpWnAl3"  ,"UpWnFe1" ,"UpWnFe2" };
  // for (G4int ii=0;ii<NUpWnTubesN;ii++){
  //  UpWnTubeZ0[ii]+=NumiData->DecayPipeZ0;
  // }
  G4VSolid *sUpWnTube;
  G4LogicalVolume *lvUpWnTube;
  for(G4int ii=0;ii<NUpWnTubesN;ii++){
    sUpWnTube=new G4Tubs(UpWnTubeVolName[ii].append("S"),UpWnTubeRin[ii],UpWnTubeRout[ii],UpWnTubeLength[ii]/2.,0.,360.*deg);
    lvUpWnTube=new G4LogicalVolume(sUpWnTube,GetMaterial(UpWnTubeVolMaterial[ii]),UpWnTubeVolName[ii].append("LV"),0,0,0);
    
    G4ThreeVector translation=G4ThreeVector(0.,0.,UpWnTubeZ0[ii]+UpWnTubeLength[ii]/2.)-G4ThreeVector(0,0,(-NumiData->DecayPipeEWinThick+NumiData->DecayPipeLength)/2.);
    G4RotationMatrix rotation=G4RotationMatrix(0.,0.,0.);
    new G4PVPlacement(G4Transform3D(rotation,translation),UpWnTubeVolName[ii],lvUpWnTube,pvDVOL,false,0);
  }

  //Spherical Al part
  l=NumiData->DecayPipeFWinThick/2.;
  r=NumiData->DecayPipeRadius;
  G4double rAlWinCurv=70.*in;  //curvature of Al window
  G4double thickAlWin=0.063*in; //thickness of Al window
  G4double angleAlWin=16.175*deg; 

  G4ThreeVector sphereAlPos=G4ThreeVector(0,0,(NumiData->DecayPipeEWinThick-NumiData->DecayPipeLength)/2.-rAlWinCurv*cos(angleAlWin)+UpWnTubeZ0[0]+UpWnTubeLength[0]);
  G4Sphere *sUpWnSteelSphere1=new G4Sphere("UpWnSph1",
					 rAlWinCurv,rAlWinCurv+thickAlWin,
					 0.*deg,360.*deg,
					 0.*deg,angleAlWin);
  G4LogicalVolume *upwnSteelSphere1=new G4LogicalVolume(sUpWnSteelSphere1,
						       Al,
						       "lvAlUpWn",
						       0,0,0);
  new G4PVPlacement(0,sphereAlPos,"UpWn1",upwnSteelSphere1,pvDVOL,false,0);

  //Spherical Fe part
  G4ThreeVector sphereFePos=G4ThreeVector(0,0,(NumiData->DecayPipeEWinThick-NumiData->DecayPipeLength)/2.-69.3*in+19.303*in-3./8.*in);
  G4Sphere *sUpWnSteelSphere2=new G4Sphere("upwnSph2",
					   69.3*in,69.675*in,
					   0.*deg,360.*deg,
					   17.64*deg,8.748*deg);
  G4LogicalVolume *upwnSteelSphere2=new G4LogicalVolume(sUpWnSteelSphere2,
						       Fe,
						       "lvFeUpWn",
						       0,0,0);
  new G4PVPlacement(0,sphereFePos,"UpWn2",upwnSteelSphere2,pvDVOL,false,0);
  
  G4ThreeVector upwnPos=G4ThreeVector(0,0,(NumiData->DecayPipeEWinThick-NumiData->DecayPipeLength)/2.);
  //G4double polyConeZ0=45.28*m;
  G4double polyConeLength=11.707*in;
  G4double polyConeR0in=13.321*in-3./8.*in;
  G4double polyConeThick=3./8.*in;
  G4int NpolyConeDivN=10;

  G4double z[11],Rin[11],Rout[11];
  for (G4int ii=0;ii<NpolyConeDivN;ii++){
    z[ii]=polyConeLength/(NpolyConeDivN-1)*ii;
    Rin[ii]=25.179*in+sqrt(polyConeR0in*polyConeR0in-z[ii]*z[ii]);
    Rout[ii]=25.179*in+sqrt((polyConeR0in+polyConeThick)*(polyConeR0in+polyConeThick)-z[ii]*z[ii]);    
  }
  G4Polycone *sPolyCone=new G4Polycone("sUpWnPolyCone",
				       0.,360.*deg,
				       NpolyConeDivN,z,Rin,Rout);
  G4LogicalVolume *lvPolyCone=new G4LogicalVolume(sPolyCone,Fe,"lvUpWnPolyCone",0,0,0);
  new G4PVPlacement(0,upwnPos,"UpWnPolyCone",lvPolyCone,pvDVOL,false,0);
   
  // Downstream window
  l=NumiData->DecayPipeEWinThick/2.;
  r=NumiData->DecayPipeRadius;
  G4ThreeVector dnwnPos=G4ThreeVector(0,0,NumiData->DecayPipeLength/2.-l);

  G4Tubs* sDNWN = new G4Tubs("sDNWN",0.,r,l,0,360.*deg);
  G4LogicalVolume* lvDNWN = new G4LogicalVolume(sDNWN,GetMaterial(NumiData->DecayPipeEWinmat),"lvDNWN",0,0,0); 
  new G4PVPlacement(0,dnwnPos,"DNWN",lvDNWN,pvDPIP,false,0);


  //************************************
  //place decay pipe tracking volumes
  G4VPhysicalVolume* pvDPInnerTrackerTube=new G4PVPlacement(0,decayVolumePosition,"pvDPInnerTrackerTube",lvDPInnerTrackerTube,pvDVOL,false,0);
  pvDPInnerTrackerTube->CheckOverlaps();
  
  G4VPhysicalVolume* pvDPInnerTrackerEnd=new G4PVPlacement(0,DPInnerTrackerEndPosition,"pvDPInnerTrackerEnd",lvDPInnerTrackerEnd,pvDVOL,false,0);
  pvDPInnerTrackerEnd->CheckOverlaps();
 
  G4VPhysicalVolume* pvDPOuterTrackerTube = new G4PVPlacement(0,sc01Position,"pvDPOuterTrackerTube",lvDPOuterTrackerTube,pvTUNE,false,0);
  pvDPOuterTrackerTube->CheckOverlaps();
  
  G4VPhysicalVolume* pvDPOuterTrackerEnd = new G4PVPlacement(0,DPOuterTrackerEndPosition,"pvDPOuterTrackerEnd",lvDPOuterTrackerEnd,pvTUNE,false,0);
  pvDPOuterTrackerEnd->CheckOverlaps();
  //
  //************************************


  G4cout << "Decay Pipe Constructed" <<G4endl;
}
