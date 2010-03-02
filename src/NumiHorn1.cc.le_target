#include "NumiDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Polycone.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "NumiMagneticField.hh"
#include "G4FieldManager.hh"
#include "NumiHornSpiderSupport.hh"

static const G4double in=2.54*cm;

void NumiDetectorConstruction::ConstructHorn1(G4ThreeVector hornpos, G4RotationMatrix hornrot)
{
  G4ThreeVector translation;
  G4RotationMatrix rotation;
  NumiDataInput* ND=NumiDataInput::GetNumiDataInput(); 
  // subtract TargetHallPosition to get origin at the face of Horn1
  G4ThreeVector TargetHallPosition=G4ThreeVector(0,0,NumiData->TargetAreaLength/2.+NumiData->TargetAreaZ0);
  // G4double MHorn1Length=133.966*2.54*cm+3.938*in+1.75*in+2.5*in;
  G4double Horn1Z0=-2.436*in;// HornZ0 in Horn coordinate system
  G4double MVgap=0.01*mm;
  G4double Fgap=0.1*mm; 
  G4double OCZ0=0.*in;
  G4double OCZ1=126.092*in;
  G4double ICZ0=0.*in;
  G4double ICZ1=129.3566*in;
  G4double FZ0=0.*in;
  G4double FZ1=128.1096*in-Fgap;
  G4double frontRmin=2.116*in;
  G4double frontRmax=2.436*in;
  G4double frontRtor=3.763*in;
  G4double epsilon=.0000001*mm;
  G4ThreeVector MHorn1Origin=G4ThreeVector(0,0,0);
  G4double maxDev=.5*mm;
  
  //inner and outer radius of inner and outer conductor evaluated at Npoints points
  // if deviation in between the two points is greater then maxDev then create a polycone plane 
  G4int nPoints=G4int((ND->PHorn1EndZ0[ND->NPHorn1EndN-1]+ND->PHorn1EndLength[ND->NPHorn1EndN-1]+frontRmax)/(.02*mm));
  G4double deltaZ=(ND->PHorn1EndZ0[ND->NPHorn1EndN-1]+frontRmax)/nPoints;
  
  G4int nOut(0),nIn(0),nMV(0),nF(0);
  G4double zPos(0);
  vdouble_t OCzPos,OCRout,OCRin, ICzPos,ICRout,ICRin;
  vdouble_t FzPos, FRin, FRout, MVzPos,MVRout,MVRin;
  
  OCzPos.push_back(OCZ0); OCRin.push_back(PHorn1OCRin(OCzPos[0])); OCRout.push_back(PHorn1OCRout(OCzPos[0]));
  ICzPos.push_back(ICZ0); ICRin.push_back(PHorn1ICRin(ICzPos[0])); ICRout.push_back(PHorn1ICRout(ICzPos[0]));
  FzPos.push_back(FZ0) ; FRin.push_back(PHorn1ICRout(FzPos[0])+Fgap) ; FRout.push_back(PHorn1OCRin(FzPos[0])-Fgap);
  MVzPos.push_back(Horn1Z0-MVgap); MVRin.push_back(PHorn1ICRin(MVzPos[0])-MVgap); MVRout.push_back(PHorn1OCRout(MVzPos[0])+MVgap);

  G4double lastICzPos=ICzPos[0];
  G4double maxR,minR,endZ;
  maxR=ND->PHorn1EndRout[0]; minR=ND->PHorn1EndRin[0]; endZ=ND->PHorn1EndZ0[0]+ND->PHorn1EndLength[0];
  for (G4int ii=1;ii<ND->NPHorn1EndN;ii++){
    if (maxR<ND->PHorn1EndRout[ii]) maxR=ND->PHorn1EndRout[ii];
    if (minR<ND->PHorn1EndRin[ii]) minR=ND->PHorn1EndRin[ii];
    if (endZ<ND->PHorn1EndZ0[ii]+ND->PHorn1EndLength[ii]) endZ=ND->PHorn1EndZ0[ii]+ND->PHorn1EndLength[ii];
  }

  for (G4int ii=1;ii<nPoints;ii++){
    zPos=Horn1Z0+deltaZ*ii;
    if ((fabs(PHorn1OCRout(zPos)-(PHorn1OCRout(zPos-deltaZ)+PHorn1OCRout(zPos+deltaZ))/2.)>maxDev)||
	(fabs(PHorn1OCRin(zPos)-(PHorn1OCRin(zPos-deltaZ)+PHorn1OCRin(zPos+deltaZ))/2.)>maxDev)||
	((fabs(PHorn1OCRout(zPos)-PHorn1OCRout(zPos-deltaZ))<epsilon)&&(fabs(PHorn1OCRout(zPos)-PHorn1OCRout(zPos+deltaZ))>epsilon))||
	((fabs(PHorn1OCRout(zPos)-PHorn1OCRout(zPos-deltaZ))>epsilon)&&(fabs(PHorn1OCRout(zPos)-PHorn1OCRout(zPos+deltaZ))<epsilon)&&(zPos>OCZ0&&zPos<OCZ1))||
	((fabs(PHorn1OCRin(zPos)-PHorn1OCRin(zPos-deltaZ))<epsilon)&&(fabs(PHorn1OCRin(zPos)-PHorn1OCRin(zPos+deltaZ))>epsilon))||
	((fabs(PHorn1OCRin(zPos)-PHorn1OCRin(zPos-deltaZ))>epsilon)&&(fabs(PHorn1OCRin(zPos)-PHorn1OCRin(zPos+deltaZ))<epsilon)&&(zPos>ICZ0)))  
      { 
	if (zPos>OCZ0&&zPos<OCZ1){
	  nOut++;
	  OCzPos.push_back(zPos); OCRout.push_back(PHorn1OCRout(zPos)); OCRin.push_back(PHorn1OCRin(zPos));
	  if (zPos>=FZ0&&zPos<FZ1){
	    nF++;	  
	    FzPos.push_back(zPos); FRin.push_back(PHorn1ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn1OCRin(FzPos[nF])-Fgap);}
	  nIn++;
	  ICzPos.push_back(zPos); ICRout.push_back(PHorn1ICRout(zPos)); ICRin.push_back(PHorn1ICRin(zPos));
	  nMV++;
	  MVzPos.push_back(zPos); MVRout.push_back(PHorn1OCRout(zPos)+MVgap);	MVRin.push_back(PHorn1ICRin(zPos)-MVgap);
	}
	else if (zPos>=OCZ1){
	  if (zPos>=FZ0&&zPos<FZ1){
	    nF++;
	    FzPos.push_back(zPos); FRin.push_back(PHorn1ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn1OCRin(FzPos[nF])-Fgap);}
	  nMV++;	  
	  MVzPos.push_back(zPos); 
	  if (maxR>=PHorn1OCRout(zPos)) MVRout.push_back(maxR+MVgap);
	  else MVRout.push_back(PHorn1OCRout(zPos)+MVgap);
	  if (minR<=PHorn1ICRin(zPos)) MVRin.push_back(minR-MVgap);
	  else 	MVRin.push_back(PHorn1ICRin(zPos)-MVgap);
	}
      }
    if ((fabs(PHorn1ICRout((lastICzPos+zPos)/2.)-(PHorn1ICRout(lastICzPos)+PHorn1ICRout(zPos))/2.)>maxDev)||
	((fabs(PHorn1ICRin((lastICzPos+zPos)/2.)-(PHorn1ICRin(lastICzPos)+PHorn1ICRin(zPos))/2.)>maxDev))||
	((fabs(PHorn1ICRout(zPos)-PHorn1ICRout(zPos-deltaZ))<epsilon)&&(fabs(PHorn1ICRout(zPos)-PHorn1ICRout(zPos+deltaZ))>epsilon))||
	((fabs(PHorn1ICRout(zPos)-PHorn1ICRout(zPos-deltaZ))>epsilon)&&(fabs(PHorn1ICRout(zPos)-PHorn1ICRout(zPos+deltaZ))<epsilon)))
      {
	lastICzPos=zPos;
	if (zPos>ICZ0&&zPos<=ICZ1){
	  nIn++;
	  ICzPos.push_back(zPos); ICRout.push_back(PHorn1ICRout(zPos)); ICRin.push_back(PHorn1ICRin(zPos));
	  if (zPos>=FZ0&&zPos<FZ1){
	    nF++;
	    FzPos.push_back(zPos); FRin.push_back(PHorn1ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn1OCRin(FzPos[nF])-Fgap);}
	  nMV++;
	  MVzPos.push_back(zPos); MVRout.push_back(PHorn1OCRout(zPos)+MVgap); MVRin.push_back(PHorn1ICRin(zPos)-MVgap);
	}
	else {
	  nMV++;
	  MVzPos.push_back(zPos); MVRout.push_back(PHorn1OCRout(zPos)+MVgap); MVRin.push_back(PHorn1ICRin(zPos)-MVgap);
	}
      }
  }
  
  nF++;
  FzPos.push_back(FZ1); FRin.push_back(PHorn1ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn1OCRin(FzPos[nF])-Fgap);
  nIn++;
  ICzPos.push_back(ICZ1); ICRin.push_back(PHorn1ICRin(ICzPos[nIn]));ICRout.push_back(PHorn1ICRout(ICzPos[nIn]));
  nOut++;
  OCzPos.push_back(OCZ1); OCRin.push_back(PHorn1OCRin(OCzPos[nOut]));OCRout.push_back(PHorn1OCRout(OCzPos[nOut]));
  nMV++;
  MVzPos.push_back(endZ+MVgap);

  if (maxR>=PHorn1OCRout(zPos)) MVRout.push_back(maxR+MVgap);
  else MVRout.push_back(PHorn1OCRout(zPos)+MVgap);
  if (minR<=PHorn1ICRin(zPos)) MVRin.push_back(minR-MVgap);
  else MVRin.push_back(PHorn1ICRin(zPos)-MVgap);

  // Create Mother Volume
  G4VSolid* sMHorn1;
  G4Material* material=Vacuum; 
  sMHorn1= new G4Polycone("sMH",0.,360.*deg,nMV+1,&MVzPos[0],&MVRin[0],&MVRout[0]);
  G4LogicalVolume *lvMHorn1 = new G4LogicalVolume(sMHorn1,material,"lvMHorn1",0,0,0);
  ND->ApplyStepLimits(lvMHorn1); // Limit Step Size
  G4VisAttributes* invisible=new G4VisAttributes(false);
  lvMHorn1->SetVisAttributes(invisible);
  rotation=hornrot;
  translation=hornpos-TargetHallPosition;
  G4VPhysicalVolume* pvMHorn1 = new G4PVPlacement(G4Transform3D(rotation,translation),"MHorn1",lvMHorn1,TGAR,false,0);


  /**
   * FLUGG - 
   * Volume added to follow particles by Alex Himmel 3-21-07
   */
  G4double boxX = NumiData->TargetAreaWidth/2.;
  G4double boxY = NumiData->TargetAreaHeight/2.;
  G4double boxZ = 1*cm;
	
  G4Box *sHorn1Box = new G4Box("sHorn1Box", boxX, boxY, boxZ);
  G4Material *boxmat = TGAR->GetLogicalVolume()->GetMaterial();
  G4LogicalVolume *lvHorn1Box = new G4LogicalVolume(sHorn1Box, boxmat, "lvHorn1Box",0,0,0);
  ND->ApplyStepLimits(lvHorn1Box); // Limit Step Size
  translation += G4ThreeVector(0.,0.,(MVzPos[nMV] - MVzPos[0])/2.+.5*cm);
  new G4PVPlacement(G4Transform3D(rotation,translation),"Horn1Box",lvHorn1Box,TGAR,false,0);


  
  //Front part
  G4VSolid* sHorn1Front;
  G4Torus* sFrontTorus=new G4Torus("sFrontTorus",frontRmin,frontRmax,frontRtor,0,360.*deg);
  G4Box* sBox=new G4Box("sBox",frontRtor*2.,frontRtor*2.,frontRmax*2.);
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,frontRmax*2);
  sHorn1Front=new G4SubtractionSolid("sHorn1Front",sFrontTorus,sBox,G4Transform3D(rotation,translation)); //need only half of torus

  G4LogicalVolume* lvHorn1Front=new G4LogicalVolume(sHorn1Front,GetMaterial(ND->hrnmat),"lvHorn1Front",0,0,0);
  ND->ApplyStepLimits(lvHorn1Front); // Limit Step Size
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=-MHorn1Origin+G4ThreeVector(0.,0.,OCZ0);
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1Front",lvHorn1Front,pvMHorn1,false,0);
  
  //Outer Conductor
  G4Polycone* sPHorn1OC=new G4Polycone("sPHorn1OC",0.,360.*deg,nOut+1,&OCzPos[0],&OCRin[0],&OCRout[0]);
  G4LogicalVolume* lvPHorn1OC=new G4LogicalVolume(sPHorn1OC,GetMaterial(ND->hrnmat),"lvPHorn1OC",0,0,0);
  ND->ApplyStepLimits(lvPHorn1OC); // Limit Step Size
  G4FieldManager* FieldMgr2 = new G4FieldManager(numiMagFieldOC); //create a local field
  FieldMgr2->SetDetectorField(numiMagFieldOC); //set the field 
  FieldMgr2->CreateChordFinder(numiMagFieldOC); //create the objects which calculate the trajectory
  lvPHorn1OC->SetFieldManager(FieldMgr2,true); //attach the local field to logical volume
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0.)-MHorn1Origin;
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1OC",lvPHorn1OC,pvMHorn1,false,0);
 
  //Inner Conductor
  G4Polycone* sPHorn1IC=new G4Polycone("sPHorn1IC",0.,360.*deg,nIn+1,&ICzPos[0],&ICRin[0],&ICRout[0]);
  G4LogicalVolume* lvPHorn1IC=new G4LogicalVolume(sPHorn1IC,GetMaterial(ND->hrnmat),"lvPHorn1IC",0,0,0);
  ND->ApplyStepLimits(lvPHorn1IC); // Limit Step Size
  G4FieldManager* FieldMgr = new G4FieldManager(numiMagFieldIC); //create a local field		 
  FieldMgr->SetDetectorField(numiMagFieldIC); //set the field 
  FieldMgr->CreateChordFinder(numiMagFieldIC); //create the objects which calculate the trajectory
  lvPHorn1IC->SetFieldManager(FieldMgr,true); //attach the local field to logical volume
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0)-MHorn1Origin;
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1IC",lvPHorn1IC,pvMHorn1,false,0);
   
  //Field Part
  G4Polycone* sPConeF=new G4Polycone("sPCone1F",0.,360.*deg,nF+1,&FzPos[0],&FRin[0],&FRout[0]);
  G4Torus* sTorusF=new G4Torus("sTorusF",0.,frontRmin-Fgap,frontRtor,0,360.*deg);
  rotation=G4RotationMatrix(0.,0.,0.); translation =G4ThreeVector(0.,0.,Horn1Z0+frontRmax);
  G4UnionSolid *sPHorn1F=new G4UnionSolid("sPHorn1F",sPConeF,sTorusF,G4Transform3D(rotation,translation));
  G4LogicalVolume* lvPHorn1F=new G4LogicalVolume(sPHorn1F,Air,"lvPHorn1F",0,0,0);
  ND->ApplyStepLimits(lvPHorn1F); // Limit Step Size
  lvPHorn1F->SetVisAttributes(invisible);
  //------ Modification by Alex Himmel 3-19-07----------
  lvPHorn1F->SetOptimisation(false);
  //------ End of Modification -------------------------
  
  G4FieldManager* FieldMgr3 = new G4FieldManager(numiMagField); //create a local field      
  FieldMgr3->SetDetectorField(numiMagField); //set the field 
  FieldMgr3->CreateChordFinder(numiMagField); //create the objects which calculate the trajectory 
  //  FieldMgr3->GetChordFinder()->SetDeltaChord(.5*mm);
  lvPHorn1F->SetFieldManager(FieldMgr3,true); //attach the local field to logical volume
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0)-MHorn1Origin;
  G4VPhysicalVolume *pvPHorn1F=new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1F",lvPHorn1F,pvMHorn1,false,0);
  
  //Spider Support
  for (G4int ii=0;ii<G4int(ND->Horn1SS.size());ii++){
    for (G4int jj=0;jj<ND->NHorn1SpidersPerPlaneN;jj++){
      G4double angle=G4double(360.*deg*jj/ND->NHorn1SpidersPerPlaneN);
      G4double halfThick=(&ND->Horn1SS[ii])->bottomL/2.;
      G4double rIn=PHorn1ICRout(ND->Horn1SpiderSupportZ0[ii]+halfThick)+Fgap;//
      G4double rIn2=PHorn1ICRout(ND->Horn1SpiderSupportZ0[ii]-halfThick)+Fgap;
      if (rIn2>rIn) rIn=rIn2;
      G4double rOut=PHorn1OCRin(ND->Horn1SpiderSupportZ0[ii])-Fgap;//In and out radius of mother vol.
      ConstructSpiderSupport(&(ND->Horn1SS[ii]),angle,ND->Horn1SpiderSupportZ0[ii],rIn,rOut,pvPHorn1F,ii+jj);
    }
  }

  //Horn end
  G4VSolid* sPHorn1End;
  for (G4int ii=0;ii<ND->NPHorn1EndN;ii++)
    {
      G4String volName=ND->PHorn1EndVolName[ii];
      sPHorn1End=new G4Tubs(volName.append("s"),ND->PHorn1EndRin[ii],ND->PHorn1EndRout[ii],ND->PHorn1EndLength[ii]/2.,0.,360.*deg);
      G4LogicalVolume* lvPHorn1End=new G4LogicalVolume(sPHorn1End,GetMaterial(ND->PHorn1EndGeantMat[ii]),volName.append("lv"),0,0,0);
      ND->ApplyStepLimits(lvPHorn1End); // Limit Step Size
      rotation=G4RotationMatrix(0.,0.,0.);
      translation=G4ThreeVector(0.,0.,ND->PHorn1EndZ0[ii]+ND->PHorn1EndLength[ii]/2.)-MHorn1Origin;
      new G4PVPlacement(G4Transform3D(rotation,translation),volName,lvPHorn1End,pvMHorn1,false,0);
    }
    
}

void NumiDetectorConstruction::ConstructSpiderSupport(NumiHornSpiderSupport *HSS,G4double angle,G4double zPos,G4double rIn,G4double rOut,G4VPhysicalVolume *motherVolume, G4int copyNo)
{   
  NumiDataInput* ND=NumiDataInput::GetNumiDataInput(); 
  
  G4double stripW=HSS->stripW;
  G4double stripH=HSS->stripH;
  G4double stripL=HSS->stripL;
  G4double topW=HSS->topW;
  G4double topH=HSS->topH;
  G4double topL=HSS->topL;
  G4double bottomW=HSS->bottomW;
  G4double bottomH=HSS->bottomH;
  G4double bottomL=HSS->bottomL;
  G4double bottomThickMid=HSS->bottomThickMid;  //Thickness of bottom part in the middle
  G4double bottomR=HSS->bottomR;
  G4double ceramicRodR=HSS->ceramicRodR;

  G4Box *sStrip=new G4Box("sStrip",stripW/2.,stripH/2.+.0*in,stripL/2.);
  G4Box *sTop=new G4Box("sTop",topW/2.,topH/2.,topL/2.);
  G4Box *sBottom=new G4Box("sBottom",bottomW/2.,bottomH/2.,bottomL/2.);
  G4Tubs *sBottomSubtr=new G4Tubs("sBottomSubtr",0.,bottomR,bottomL,0.,360.*deg);
  G4ThreeVector translation=G4ThreeVector(bottomW/2.-stripW/2.,stripH/2.+bottomH/2.,0.);
  G4RotationMatrix rotation=G4RotationMatrix(0.,0.,0.);
  G4VSolid *sSpider;
  sSpider=new G4UnionSolid("sSpider",sBottom,sStrip,G4Transform3D(rotation,translation));
  translation=G4ThreeVector(-bottomW/2.+stripW/2.,stripH/2.+bottomH/2.,0.);
  sSpider=new G4UnionSolid("sSpider",sSpider,sStrip,G4Transform3D(rotation,translation));
  translation=G4ThreeVector(0.,stripH+bottomH/2.+topH/2.,0.);
  sSpider=new G4UnionSolid("sSpider",sSpider,sTop,G4Transform3D(rotation,translation));
  translation=G4ThreeVector(0.,(bottomH/2.-bottomThickMid)-bottomR,0.);
  sSpider=new G4SubtractionSolid("sSpider",sSpider,sBottomSubtr,G4Transform3D(rotation,translation));

  G4LogicalVolume *lvSpider=new G4LogicalVolume(sSpider,GetMaterial(ND->hrnmat),"lvBox",0,0,0);
  ND->ApplyStepLimits(lvSpider); // Limit Step Size
  G4RotationMatrix rotPos=G4RotationMatrix(0.,0.,angle);
  G4ThreeVector transPos=G4ThreeVector((rIn+(bottomThickMid-bottomH/2.))*sin(angle),(rIn+(bottomThickMid-bottomH/2.))*cos(angle),zPos);
  G4Transform3D position3D=G4Transform3D(rotPos,transPos);
  new G4PVPlacement(position3D,"SpiderSupport",lvSpider,motherVolume,false,copyNo);

  G4double ceramicRodL=rOut-rIn-topH-bottomThickMid-stripH-2.*mm;
  G4Tubs *sCeramicRod=new G4Tubs("sCeramicRod",0.,ceramicRodR,ceramicRodL/2.,0.,360.*deg);
  G4LogicalVolume *lvCeramicRod=new G4LogicalVolume(sCeramicRod,GetMaterial(ND->hrnmatcr),"lvCeramicRod",0,0,0);
  ND->ApplyStepLimits(lvCeramicRod); // Limit Step Size
  rotPos=G4RotationMatrix(0.,0.,0.);
  rotPos.rotateX(90.*deg); rotPos.rotateZ(-angle);
  transPos=transPos+G4ThreeVector((bottomH/2.+stripH+topH+ceramicRodL/2.)*sin(angle),(bottomH/2.+stripH+topH+ceramicRodL/2.)*cos(angle),0);
  position3D=G4Transform3D(rotPos,transPos);
  new G4PVPlacement(position3D,"CeramicRod",lvCeramicRod,motherVolume,false,copyNo);
}

G4double NumiDetectorConstruction::PHorn1OCRout(G4double z)
{
  G4double r=0;
  if (z<0.*in){
    r=3.763*in+2.436*in; // for mother vol.
  }
  
  //OC dimensions from drawings
  else if ((z>=0.*in)&&(z<0.756*in)){
    r=3.763*in+2.436*in;
  }
  else if ((z>=0.756*in)&&(z<1.756*in)){
    r=16.25/2.*in;
  }
  else if ((z>=1.756*in)&&(z<2.756*in)){
    r=15.99/2.*in;
  }
  else if ((z>=2.756*in)&&(z<115.971*in)){
    r=13.750/2.*in;
  }
  else if (z>=115.971*in&&z<117.341*in){
    r=(6.875+(z/in-115.971)/(117.341-115.971)*(8.25-6.875))*in;
  }
  else if ((z>=117.341*in)&&(z<123.311*in)){
    r=16.5/2.*in;
  }
  else if ((z>=123.311*in)&&(z<124.811*in)){
    r=23.5/2.*in;
  }
   else if ((z>=124.811*in)&&(z<=126.092*in)){
     r=15.5/2.*in;
   }
   
   //for mother vol. purposes
   else if ((z>126.092*in)){
     r=15.5/2.*in; // for mother vol.
   }
   return r;
}
G4double NumiDetectorConstruction::PHorn1OCRin(G4double z)
{
  G4double r=0;
  //OC dimensions from drawings
  if ((z>=0.*in)&&(z<1.806*in)){
    r=5.879*in;
  }
  else if ((z>=1.806*in)&&(z<116.551*in)){
    r=11.75/2.*in;
  }
  else if (z>=116.551*in&&z<117.341*in){
    r=(5.875+(z/in-116.551)/(117.341-116.551)*(7.25-5.875))*in;
  }
  else if ((z>=117.341*in)&&(z<122.351*in)){
    r=14.5/2.*in;
  }
  else if ((z>=122.351*in)&&(z<124.051*in)){
    r=(14.5/2.-(z/in-122.351)/(124.051-122.351)*(7.25-6.))*in;
  }
  else if ((z>=124.051*in)&&(z<=126.096*in)){
    r=6.*in;
  }
   
  //for mother vol. purposes
  else if ((z>=126.096*in)&&(z<=130.*in)){
     r=5.815*in;
  }
  
  return r;
}
G4double NumiDetectorConstruction::PHorn1ICRout(G4double z)
{
  G4double r=0.;
  if ((z>=0.*in)&&(z<3.32645*in)){
    r=sqrt(1.975805-(0.05585)*(z/in))*in;
  }
  
  //IC from drawings
  if ((z>=3.32645*in)&&(z<30.3150*in)){
    r=sqrt(1.975805-(0.05585)*(z/in))*in;
  }
  else if ((z>=30.3150*in)&&(z<31.8827*in)){
    r=1.063/2.*in;
  }
  else if ((z>=31.8827*in)&&(z<117.1126*in)){
    r=sqrt(0.180183*z/in-5.462253)*in;
  }
  else if ((z>=117.1126*in)&&(z<=128.1096*in)){
    r=8.5/2.*in;
  }
  else if((z>=128.1096*in)&&(z<=129.3566*in)){
    r=11.623/2.*in;
  }
  //for MV
  else if (z>129.3566*in){
    r=8.5/2.*in;
  }

 
return r;
}
G4double NumiDetectorConstruction::PHorn1ICRin(G4double z)
{
 G4double r=0.;
 
  if (z<0.0*in){
    r=1.327*in;//for MV; added 0.05 because ic was outside of mv
  }

  //IC from drawings
  else if ((z>=0.0*in)&&(z<16.1602*in)){
    r=sqrt(1.975805-(0.055858)*(z/in))*in-0.078740*in;
  }
  else if ((z>=16.1602*in)&&(z<30.3150*in)){
    r=sqrt(1.818869-(0.055858)*(z/in))*in;
  }
  else if ((z>=30.3150*in)&&(z<31.8827*in)){
    r=.709/2.*in;//NECK!
  }   
  else if ((z>=31.8827*in)&&(z<36.2709*in)){
    r=sqrt(0.180183*z/in-5.61919)*in; 
  }  
  else if ((z>=36.2709*in)&&(z<117.1126*in)){
    r=sqrt(0.180183*z/in-5.462253)*in-0.078740*in; 
  }  
  
  else if ((z>=117.1126*in)&&(z<=129.3566*in)){
    r=7.75/2.*in;
  }

  //for mother volume i need r defined up to the end of horn
  else if (z>129.3566*in){
    r=7.75/2.*in;
  }
  
return r;
}
