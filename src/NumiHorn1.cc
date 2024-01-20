#include "NumiDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Cons.hh"
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
  if (ND->GetDumpBFieldPlease()) {
    std::cerr << "NumiDetectorConstruction::ConstructHorn1 DumpBFieldPlease is On ... " << std::endl;
  } else {
    std::cerr << "NumiDetectorConstruction::ConstructHorn1 DumpBFieldPlease is Off ... " << std::endl;
  }
  const bool isAlternate = ND->GetHorn1IsAlternate();
  const bool isRefined = ND->GetHorn1IsRefined();
  G4cout << "NumiDetectorConstruction::ConstructHorn1 horn 1 is alternate: " << isAlternate << G4endl;
  G4cout << "NumiDetectorConstruction::ConstructHorn1 horn 1 is refined: " << isRefined << G4endl;
  if (isAlternate) {
    G4cout << "Entering ConstructHorn1Alternate" << G4endl;
    this->ConstructHorn1Alternate(hornpos, hornrot); 
    return;
  }
    std::cerr << "NumiDetectorConstruction::ConstructHorn1 Default construction is Done Now ... " << std::endl;
  const double hornWaterLayerThick = ND->GetHornWaterLayerThick(); // July 16-19 2014, P.L.
     if ( hornWaterLayerThick > 10.) {
     std::ostringstream messageOStr; messageOStr << " Unreasonable amount of water " << hornWaterLayerThick;
     std::string message(messageOStr.str());
#ifndef MODERN_G4
     G4Exception("NumiDetectorConstruction::ConstructHorn1");
#else
     G4Exception("numiHorn1","NumiHorn1",FatalException,"NumiDetectorConstruction::ConstructHorn1");
#endif
     exit(2); // only under 4.9.2 .. not reachable under 4.9.5 
   }
   const bool placeWaterLayer = (hornWaterLayerThick > 0.001*mm);
   fHorn1ExtraLayerAlum = ND->GetHorn1ExtraLayerAlum();
   
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
  if (isRefined) maxDev=0.010*mm; // This will increase the number of segment for the Polycones.. but might be needed 
  // Dec 1 2014.. 
  
  //inner and outer radius of inner and outer conductor evaluated at Npoints points
  // if deviation in between the two points is greater then maxDev then create a polycone plane 
  G4int nPoints=G4int((ND->PHorn1EndZ0[ND->NPHorn1EndN-1]+ND->PHorn1EndLength[ND->NPHorn1EndN-1]+frontRmax)/(.02*mm));
  G4double deltaZ=(ND->PHorn1EndZ0[ND->NPHorn1EndN-1]+frontRmax)/nPoints;
//  std::cerr << " NumiHorn1 .... nPoints " << nPoints << "  delta Z "  << deltaZ << " and quit ... " << std::endl; exit(2);
  
  G4int nOut(0),nIn(0),nMV(0),nF(0);
  G4double zPos(0);
  vdouble_t OCzPos,OCRout,OCRin, ICzPos,ICRout,ICRin;
  vdouble_t FzPos, FRin, FRout, MVzPos,MVRout,MVRin;
  vdouble_t FRinW, FRoutW; // for Water layer...July 16 2014
  
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
  
  if (placeWaterLayer) { // July 
    FRinW.clear(); FRoutW.clear();
    for (size_t i=0; i != FRin.size(); i++) {
      FRinW.push_back(FRin[i]);
      FRoutW.push_back(FRin[i] + hornWaterLayerThick);
      FRin[i] += hornWaterLayerThick + epsilon;
//      std::cerr << " Radial setting for section " << i << " FRinW " << FRinW[i] << " FRoutW "
//                << FRoutW[i] << " FRin " << FRin[i] << " FRout " << FRout[i] << " at Z " << ICzPos[i] << std::endl;
    }
  }
//  std::cerr << " FRIn size " << FRin.size() <<  " nIn + 1 = " << nIn +1 << std::endl;
  if (maxR>=PHorn1OCRout(zPos)) MVRout.push_back(maxR+MVgap);
  else MVRout.push_back(PHorn1OCRout(zPos)+MVgap);
  if (minR<=PHorn1ICRin(zPos)) MVRin.push_back(minR-MVgap);
  else MVRin.push_back(PHorn1ICRin(zPos)-MVgap);

  // Create Mother Volume
  G4VSolid* sMHorn1;
//  G4Material* material=Vacuum;
  G4Material* material=Air; // Air inside Air allows for cracks without changing material budget. 
  // Check size .. P.L. July 2014.. 
  if ((MVzPos.size()  != MVRin.size()) || (MVzPos.size()  != MVRout.size())) {
    std::cerr << " Inconsistent of number of section for lvMHorn1, Z /R size  " << MVzPos.size()
              << " / " <<  MVRin.size() << " / " << MVRout.size() << std::endl; exit(2);
  }
  sMHorn1= new G4Polycone("sMH",0.,360.*deg, static_cast<int>(MVzPos.size()), &MVzPos[0],&MVRin[0],&MVRout[0]);
//  std::cerr << " Horn1 Polycone Mother Default Parameters  " << std::endl; 
//  for (size_t k=0; k != MVzPos.size(); k++) {
//    std::cerr << " ZPos " << MVzPos[k] << " Rin " << MVRin[k] << " ROut " << MVRout[k] << std::endl;
//  }
  G4LogicalVolume *lvMHorn1 = new G4LogicalVolume(sMHorn1,material,"lvMHorn1",0,0,0);
  ND->ApplyStepLimits(lvMHorn1); // Limit Step Size
  ND->ApplyTimeLimits(lvMHorn1);//limit TOF for step
  G4VisAttributes* invisible=new G4VisAttributes(false);
  lvMHorn1->SetVisAttributes(invisible);
  rotation=hornrot;
  translation=hornpos-TargetHallPosition;
  G4VPhysicalVolume* pvMHorn1 = new G4PVPlacement(G4Transform3D(rotation,translation),"pvMHorn1Mother",lvMHorn1,TGAR,false,0, true);
  std::cerr << " Position for Horn1 Mother, Default version " << translation << std::endl;
//  std::cerr << " And quit now ... " << std::endl; exit(2);
  pvMHorn1 -> CheckOverlaps();  
  /**
   * FLUGG - 
   * Volume added to follow particles by Alex Himmel 3-21-07
   */
  /*  G4double boxX = NumiData->TargetAreaWidth/2.;
  G4double boxY = NumiData->TargetAreaHeight/2.;
  G4double boxZ = 1*cm;
	
  G4Box *sHorn1Box = new G4Box("sHorn1Box", boxX, boxY, boxZ);
  G4Material *boxmat = TGAR->GetLogicalVolume()->GetMaterial();
  G4LogicalVolume *lvHorn1Box = new G4LogicalVolume(sHorn1Box, boxmat, "lvHorn1Box",0,0,0);
  ND->ApplyStepLimits(lvHorn1Box); // Limit Step Size
  ND->ApplyTimeLimits(lvHorn1Box);//limit TOF for step
  translation += G4ThreeVector(0.,0.,(MVzPos[nMV] - MVzPos[0])/2.+.5*cm);
  new G4PVPlacement(G4Transform3D(rotation,translation),"Horn1Box",lvHorn1Box,TGAR,false,0);
  */

  
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
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1Front",lvHorn1Front,pvMHorn1,false,0, true);
  
  //Outer Conductor
  if ((OCzPos.size()  != OCRin.size()) || (OCzPos.size()  != OCRout.size()) || ((nOut+1) != static_cast<int>(OCRout.size()))) {
    std::cerr << " Inconsistent of number of section for sPHorn1OC, Z /R size  " << MVzPos.size()
              << " / " <<  MVRin.size() << " / " << MVRout.size() << " (nOut+1) " << (nOut+1) << std::endl;
    exit(2);	      
  }
  G4Polycone* sPHorn1OC=new G4Polycone("sPHorn1OC",0.,360.*deg,nOut+1,&OCzPos[0],&OCRin[0],&OCRout[0]);
  G4LogicalVolume* lvPHorn1OC=new G4LogicalVolume(sPHorn1OC,GetMaterial(ND->hrnmat),"lvPHorn1OC",0,0,0);
  ND->ApplyStepLimits(lvPHorn1OC); // Limit Step Size
  G4FieldManager* FieldMgr2 = new G4FieldManager(numiMagFieldOC); //create a local field
  FieldMgr2->SetDetectorField(numiMagFieldOC); //set the field 
  FieldMgr2->CreateChordFinder(numiMagFieldOC); //create the objects which calculate the trajectory
  lvPHorn1OC->SetFieldManager(FieldMgr2,true); //attach the local field to logical volume
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0.)-MHorn1Origin;
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1OC",lvPHorn1OC,pvMHorn1,false,0, true);
 
  //Inner Conductor
  if ((ICzPos.size()  != ICRin.size()) || (ICzPos.size()  != ICRout.size()) || ((nIn+1) != static_cast<int>(ICRout.size()))) {
    std::cerr << " Inconsistent of number of section fors PHorn1IC , Z /R size  " << ICzPos.size()
              << " / " <<  ICRin.size() << " / " << ICRout.size() << " (nIn+1) " << (nIn+1) << std::endl;
	      exit(2);
  }
  G4Polycone* sPHorn1IC=new G4Polycone("sPHorn1IC",0.,360.*deg,nIn+1,&ICzPos[0],&ICRin[0],&ICRout[0]);
  G4LogicalVolume* lvPHorn1IC=new G4LogicalVolume(sPHorn1IC,GetMaterial(ND->hrnmat),"lvPHorn1IC",0,0,0);
  ND->ApplyStepLimits(lvPHorn1IC); // Limit Step Size
  G4FieldManager* FieldMgr = new G4FieldManager(numiMagFieldIC); //create a local field		 
  FieldMgr->SetDetectorField(numiMagFieldIC); //set the field 
  FieldMgr->CreateChordFinder(numiMagFieldIC); //create the objects which calculate the trajectory
  lvPHorn1IC->SetFieldManager(FieldMgr,true); //attach the local field to logical volume
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0)-MHorn1Origin;
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1IC",lvPHorn1IC,pvMHorn1,false,0, true);
   
  //Field Part
  // July 2014: Split this in 2 parts. 
  // Water and Air...
  //
  G4LogicalVolume* lvPHorn1ICW=0;
  if (placeWaterLayer) {
//    std::cerr << " Reaching water layer ( " << hornWaterLayerThick << "mm ), placement in Horn1, with nIn = " 
//              << nIn <<  " and quit " << std::endl; exit(2);
    if ((ICzPos.size()  != FRinW.size()) || (ICzPos.size()  != FRoutW.size()) || ((nIn) != static_cast<int>(FRoutW.size()))) {
      std::cerr << " Inconsistent of number of section fors PHorn1ICWater , Z /R size  " << ICzPos.size()
              << " / " <<  FRinW.size() << " / " << FRoutW.size() << " (nIn) " << (nIn) << std::endl;
      std::cerr << " Probably O.K., take the minimum of these " << std::endl;
    }
    int numSectW = std::min(static_cast<int>(FRinW.size()), static_cast<int>(ICzPos.size()));
    // Take the last one out.. Inconsistent inner radiis.. 
    numSectW--;
    G4Polycone* sPHorn1ICW=new G4Polycone("sPHorn1ICWater",0.,360.*deg, numSectW,&ICzPos[0],&FRinW[0],&FRoutW[0]);
    lvPHorn1ICW=new G4LogicalVolume(sPHorn1ICW, Water, "lvPHorn1ICWater", 0,0,0);
    ND->ApplyStepLimits(lvPHorn1ICW); // Limit Step Size
//    lvPHorn1ICW->SetFieldManager(FieldMgr,true); //attach the local field to logical volume
//    Not that one! P.L., Oct 17 2014.. 
    rotation=G4RotationMatrix(0.,0.,0.);
    translation=G4ThreeVector(0.,0.,0)-MHorn1Origin;
    new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1ICWater",lvPHorn1ICW,pvMHorn1,false,0, true);
//    std::cerr << " Water volume placed...num subsection " << (nIn) << std::endl; 
  }
  G4Polycone* sPConeF=new G4Polycone("sPCone1F",0.,360.*deg,nF+1,&FzPos[0],&FRin[0],&FRout[0]);
  G4Torus* sTorusF=new G4Torus("sTorusF",0.,frontRmin-Fgap,frontRtor,0,360.*deg);
  rotation=G4RotationMatrix(0.,0.,0.); translation =G4ThreeVector(0.,0.,Horn1Z0+frontRmax);
  G4UnionSolid *sPHorn1F=new G4UnionSolid("sPHorn1F",sPConeF,sTorusF,G4Transform3D(rotation,translation));
//  G4LogicalVolume* lvPHorn1F=new G4LogicalVolume(sPHorn1F,Air,"lvPHorn1F",0,0,0);
// Per directive from Jim Hylen, July 31 2014. 
//
  G4LogicalVolume* lvPHorn1F=new G4LogicalVolume(sPHorn1F,Ar,"lvPHorn1F",0,0,0);
  ND->ApplyStepLimits(lvPHorn1F); // Limit Step Size
  lvPHorn1F->SetVisAttributes(invisible);
  //------ Modification by Alex Himmel 3-19-07----------
  lvPHorn1F->SetOptimisation(false);
  //------ End of Modification -------------------------
  
  numiMagField->SetIgnoreCEQBr(fIgnoreCEQBr);;
  numiMagField->SetHorn1FieldZCutUpstream(fHorn1FieldZCutUpstream);
  numiMagField->SetHorn1FieldZCutDwnstream(fHorn1FieldZCutDwnstream);
  numiMagField->SetHorn1CurrentEqualizerLongAbsLength(fHorn1CurrentEqualizerLongAbsLength);
  numiMagField->SetHorn1CurrentEqualizerQuadAmpl(fHorn1CurrentEqualizerQuadAmpl); 
  numiMagField->SetHorn1CurrentEqualizerOctAmpl(fHorn1CurrentEqualizerOctAmpl); 
     
  G4FieldManager* FieldMgr3 = new G4FieldManager(numiMagField); //create a local field 
  FieldMgr3->SetDetectorField(numiMagField); //set the field 
  FieldMgr3->CreateChordFinder(numiMagField); //create the objects which calculate the trajectory 
  //  FieldMgr3->GetChordFinder()->SetDeltaChord(.5*mm);
  lvPHorn1F->SetFieldManager(FieldMgr3,true); //attach the local field to logical volume
  if (placeWaterLayer) lvPHorn1ICW->SetFieldManager(FieldMgr3,true); //same inside field for water as for the one in 
  // between the IC and the OC 
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0)-MHorn1Origin;
  G4VPhysicalVolume *pvPHorn1F=new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1F",lvPHorn1F,pvMHorn1,false,0, true);
  
  //Spider Support
  for (G4int ii=0;ii<G4int(ND->Horn1SS.size());ii++){
    for (G4int jj=0;jj<ND->NHorn1SpidersPerPlaneN;jj++){
      G4double angle=G4double(360.*deg*jj/ND->NHorn1SpidersPerPlaneN);
      G4double halfThick=(&ND->Horn1SS[ii])->bottomL/2.;
      G4double rIn=PHorn1ICRout(ND->Horn1SpiderSupportZ0[ii]+halfThick)+Fgap;//
      G4double rIn2=PHorn1ICRout(ND->Horn1SpiderSupportZ0[ii]-halfThick)+Fgap;
      if (rIn2>rIn) rIn=rIn2;
      if (placeWaterLayer) rIn += hornWaterLayerThick + 3.0*epsilon;
//      std::cerr << " Inner radius for spider support at Z = " << ND->Horn1SpiderSupportZ0[ii] << "  is " << rIn << std::endl; 
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
      new G4PVPlacement(G4Transform3D(rotation,translation),volName,lvPHorn1End,pvMHorn1,false,0, true);
    }


  G4cout << "Horn 1 Constructed" << G4endl;
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
  new G4PVPlacement(position3D,"SpiderSupport",lvSpider,motherVolume,false,copyNo, true);

  G4double ceramicRodL=rOut-rIn-topH-bottomThickMid-stripH-2.*mm;
  G4Tubs *sCeramicRod=new G4Tubs("sCeramicRod",0.,ceramicRodR,ceramicRodL/2.,0.,360.*deg);
  G4LogicalVolume *lvCeramicRod=new G4LogicalVolume(sCeramicRod,GetMaterial(ND->hrnmatcr),"lvCeramicRod",0,0,0);
  ND->ApplyStepLimits(lvCeramicRod); // Limit Step Size
  rotPos=G4RotationMatrix(0.,0.,0.);
  rotPos.rotateX(90.*deg); rotPos.rotateZ(-angle);
  transPos=transPos+G4ThreeVector((bottomH/2.+stripH+topH+ceramicRodL/2.)*sin(angle),(bottomH/2.+stripH+topH+ceramicRodL/2.)*cos(angle),0);
  position3D=G4Transform3D(rotPos,transPos);
  new G4PVPlacement(position3D,"CeramicRod",lvCeramicRod,motherVolume,false,copyNo, true);
}

G4double NumiDetectorConstruction::PHorn1OCRout(G4double z)
{ // Martens 3/26/10
  // The Horn 1 outer conductor is thinner for NOvA than for MINOS.
  // For MINOS the Horn 1 outer conductor is 1 inch thick.
  // For NOvA the Horn 1 outer conductor is 5/8 inch thick.
  // Thus subtract 3/8 inch from the Horn 1 outer conductor outer radius
  // but don't subract the 3/8 inch from the end flanges.
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
    r -= fDeltaOuterThickness;
  }
  else if (z>=115.971*in&&z<117.341*in){
    r=(6.875+(z/in-115.971)/(117.341-115.971)*(8.25-6.875))*in;
    r -= fDeltaOuterThickness;
  }
  else if ((z>=117.341*in)&&(z<123.311*in)){
    r=16.5/2.*in;
    r -= fDeltaOuterThickness;
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
  r += fHorn1ExtraLayerAlum;
  if (r < 0.100*mm) r = 0.1*mm;

 
return r;
}
G4double NumiDetectorConstruction::PHorn1ICRin(G4double z) const
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

void NumiDetectorConstruction::ConstructHorn1Alternate(G4ThreeVector hornpos, G4RotationMatrix hornrot)
{
  G4ThreeVector translation;
  G4RotationMatrix rotation;
  NumiDataInput* ND=NumiDataInput::GetNumiDataInput();
  // 
  // Preparation phase.  Extracted from LBNEVolumePlacements::DeclareHorn1Dims  Make all variable local. Renamed, to avoid 
  // the confusion with the old target region (split geometry) in LBNE 
  //
  std::vector<double> horn1IOInnerRadsUpstr(5, 0.);
  std::vector<double> horn1IOInnerRadsDownstr(5, 0.);
  std::vector<double> horn1IOTransThick(5, 0.);
  std::vector<double> horn1IOUpstrLengths(5, 0.);
  std::vector<double> horn1IOUpstrZPositions(5, 0.);
  horn1IOInnerRadsUpstr[0] = 1.88*in;
//  horn1IOInnerRadsUpstr[1] = 2.28*in;
  horn1IOInnerRadsUpstr[1] = 2.58*in; // Readjusting.. Study of G4Tubs vs Polycones.. IOTrans2
  horn1IOInnerRadsUpstr[2] = 2.78*in; // It will a G4tubs, not G4Cons
  horn1IOInnerRadsUpstr[3] = 4.59*in;// Readjusting.. Study of G4Tubs vs Polycones.. IOTrans2
  horn1IOInnerRadsUpstr[4] = 5.10*in;
  horn1IOInnerRadsDownstr[0] = 1.573*in;
  horn1IOInnerRadsDownstr[1] = 2.08*in;
  horn1IOInnerRadsDownstr[2] = 4.49*in; // It will a G4tubs, not G4Cons This is the outer radius 
  horn1IOInnerRadsDownstr[3] = 5.50*in;
  horn1IOInnerRadsDownstr[4] = 5.53*in;
//  horn1IOTransThick[0] = (0.25*in)/std::cos(M_PI*45./180.);
//  horn1IOTransThick[4] = (0.25*in)/std::cos(M_PI*45./180.);
//  horn1IOTransThick[1] = (0.25*in)/std::cos(M_PI*60./180.);
//  horn1IOTransThick[3] = (0.25*in)/std::cos(M_PI*60./180.);
// Readjusting.. Study of G4Tubs vs Polycones.. November 2014 

  horn1IOTransThick[0] = (0.32*in)/std::cos(M_PI*45./180.);
  horn1IOTransThick[4] = (0.32*in)/std::cos(M_PI*45./180.);
  horn1IOTransThick[1] = (0.32*in)/std::cos(M_PI*60./180.);
  horn1IOTransThick[3] = (0.32*in)/std::cos(M_PI*60./180.);
  horn1IOTransThick[2] = horn1IOInnerRadsDownstr[2] - horn1IOInnerRadsUpstr[2];
  //
  // September 2017.  The Torus solution is better, more exact.  The solution of fined grained sectioning, 
  // done in LBNF is also acceptable. However, this crude splitting has volume overlaps. 
  // We want to do a material budget study in this region, so we keep this crude segmentation, but we correct the geometry and 
  // make sure we do not have overlaps. 
  //
  const double theta2to3 = std::acos(2.226/2.436);
  const double theta0to1 = 0.5*(M_PI/2.0 - theta2to3); // we split in only two pieces..
  //
  // October 2017: 
  //
  // Imperfect Current equalizer section support. 
  // To generate the RZ map, one needs a the accurate position of the IC, + thickness. 
  // Define a simple array.. 
  fEffectiveRadiiForFieldMap.clear();
  // fixed step size of 1 mm, over the entire length of the Horn1.
  // Coordinate system is global., Z0 = Iz0 = 0. 
  for (size_t k=0; k!= 3250; k++) fEffectiveRadiiForFieldMap.push_back(-1.);
  
  
  horn1IOUpstrLengths[0] = 2.436*in*std::sin(theta0to1) - 0.050*CLHEP::mm;
  horn1IOUpstrLengths[4] = horn1IOUpstrLengths[0];
//  horn1IOUpstrLengths[1] = 0.410*in;
// Readjusting.. Study of G4Tubs vs Polycones.. November 2014 
  horn1IOUpstrLengths[1] = 2.436*in - 2.436*in*std::sin(theta0to1) - 0.32*in - 0.050*CLHEP::mm;
  horn1IOUpstrLengths[2] = 0.32*in - 0.050*CLHEP::mm;
  horn1IOUpstrLengths[3] = horn1IOUpstrLengths[1];
  
  horn1IOInnerRadsDownstr[0] = (3.763 - 2.436)*in;
  horn1IOInnerRadsUpstr[0] = (3.763 - 2.436*std::cos(theta0to1))*in;
  horn1IOInnerRadsDownstr[1] = horn1IOInnerRadsUpstr[0];
  horn1IOInnerRadsUpstr[1] = (3.763 - 2.436*std::cos(2.0*theta0to1))*in;

  horn1IOInnerRadsUpstr[2] = 3.763*in;
  horn1IOInnerRadsDownstr[2] = 3.763*in; // Irrelevant, will use the tube. 
  horn1IOInnerRadsDownstr[4] = (3.763 + 2.436)*in;
  horn1IOInnerRadsUpstr[4] = (3.763 + 2.436*std::cos(theta0to1))*in;
  horn1IOInnerRadsDownstr[3] = horn1IOInnerRadsUpstr[4];
  horn1IOInnerRadsUpstr[3] = (3.763 + 2.436*std::cos(2.0*theta0to1))*in;
  
  
//  horn1IOUpstrLengths[2] = 0.370*in; // Increase thickness 
// Too much.. Study of G4Tubs vs Polycones.. November 2014, September 2017 
  horn1IOUpstrLengths[2] = 0.32*in - 0.05*CLHEP::mm; // correct thickness., minus 50 microns  
  const double aHorn1IOTransLength = 2.436*in + 3.316*in; // tight
// With respecto the center of the container volume. (August 2014.. ), holding the 5 volunes (4 cons, 1 tube. )  
                          // Drawing 8875.112 -MD-363097 The 3 cm is the MCZERO offset, per verbal discussion 
// Corrected again, September 2017. for ME wiggle study. 
// with respect to the center of the I/O cont. volume.  
  horn1IOUpstrZPositions[0] = -aHorn1IOTransLength/2. + 2.436*in - horn1IOUpstrLengths[0]/2. ;
  horn1IOUpstrZPositions[4] = horn1IOUpstrZPositions[0];
  horn1IOUpstrZPositions[1] = horn1IOUpstrZPositions[0] - horn1IOUpstrLengths[0]/2. - horn1IOUpstrLengths[1]/2. - 0.005*CLHEP::mm;
  horn1IOUpstrZPositions[3] = horn1IOUpstrZPositions[1];
  horn1IOUpstrZPositions[2] = -aHorn1IOTransLength/2. + 0.32*in/2. + 0.005*CLHEP::mm; // 
  
			      
//  aHorn1RadialSafetyMargin = 2.9*mm; // per agreement between Jim H. and Alberto M., Aug. 22 2013. 
  const double aHorn1RadialSafetyMargin = 2.5*mm; // per agreement between Jim H. and Alberto M., Oct 4 2013 
  
  const double aHorn1IOTransInnerRad = 2.520*in/2. - aHorn1RadialSafetyMargin/2. ; // last term is the 
//  const double aHorn1IOTransOuterRad = 16.250*in/2.; No longer needed.. 
  
  std::vector<double> aHorn1UpstrInnerRadsUpstr(4, 0.);
  std::vector<double> aHorn1UpstrInnerRadsDownstr(4, 0.);
  std::vector<double> aHorn1UpstrInnerRadsOuterUpstr(4, 0.);
  std::vector<double> aHorn1UpstrInnerRadsOuterDownstr(4, 0.);
  std::vector<double> aHorn1UpstrLengths(4, 0.);
  std::vector<double> aHorn1UpstrZPositions(4, 0.);
  
  aHorn1UpstrInnerRadsUpstr[0] = 1.572*in; 
  aHorn1UpstrInnerRadsOuterUpstr[0] = aHorn1UpstrInnerRadsUpstr[0] + 0.32*in; 
  aHorn1UpstrInnerRadsDownstr[0] = 1.41*in; 
  aHorn1UpstrInnerRadsOuterDownstr[0] = aHorn1UpstrInnerRadsDownstr[0] + 0.32*in; 
  aHorn1UpstrLengths[0] = 0.508*in - 0.100*mm;
  aHorn1UpstrZPositions[0] = aHorn1UpstrLengths[0]/2. + 0.025; // With respect to the beginning of mother volume.  
  
  aHorn1UpstrInnerRadsUpstr[1] = aHorn1UpstrInnerRadsDownstr[0]; 
  aHorn1UpstrInnerRadsOuterUpstr[1] = aHorn1UpstrInnerRadsUpstr[1] + 0.32*in; 
  aHorn1UpstrInnerRadsDownstr[1] = 1.288*in; 
  aHorn1UpstrInnerRadsOuterDownstr[1] = aHorn1UpstrInnerRadsDownstr[1] + 0.3*in; 
  aHorn1UpstrLengths[1] = 0.639*in  - 0.100*mm;
  aHorn1UpstrZPositions[1] = aHorn1UpstrZPositions[0] + 0.025*mm +
                                  aHorn1UpstrLengths[0]/2 + aHorn1UpstrLengths[1]/2.;
  
  aHorn1UpstrInnerRadsUpstr[2] = aHorn1UpstrInnerRadsDownstr[1]; 
  aHorn1UpstrInnerRadsOuterUpstr[2] = aHorn1UpstrInnerRadsUpstr[2] + 0.28*in; 
  aHorn1UpstrInnerRadsDownstr[2] = 1.268*in; 
  aHorn1UpstrInnerRadsOuterDownstr[2] = aHorn1UpstrInnerRadsDownstr[2] + 0.118*in; 
  aHorn1UpstrLengths[2] = 2.835*in  - 0.100*mm; // Reduce a bit, too tight..
  aHorn1UpstrZPositions[2] = aHorn1UpstrZPositions[1] + 0.025*mm + aHorn1UpstrLengths[1]/2 + aHorn1UpstrLengths[2]/2.;
  
  aHorn1UpstrInnerRadsUpstr[3] = aHorn1UpstrInnerRadsDownstr[2]; 
  aHorn1UpstrInnerRadsOuterUpstr[3] = aHorn1UpstrInnerRadsUpstr[2] + 0.118*in; 
  aHorn1UpstrInnerRadsDownstr[3] =  aHorn1UpstrInnerRadsUpstr[3]; 
  aHorn1UpstrInnerRadsOuterDownstr[3] = aHorn1UpstrInnerRadsDownstr[3] + 0.20*in; 
  aHorn1UpstrLengths[3] = 0.479*in - 0.100*mm; // Nov 19 2013 : change from 0.40 inches to 0.479, per more 
                                               // accurate reading of drawing 363097
  aHorn1UpstrZPositions[3] = aHorn1UpstrZPositions[2] + 0.025*mm + aHorn1UpstrLengths[2]/2 + aHorn1UpstrLengths[3]/2.;
 //
 // These are elements, approximated as tubes, of the Inner Outer transition piece of Horn1 
 // 
  std::vector<double> aHorn1UpstrOuterIOTransInnerRads(4, 0.);
  std::vector<double> aHorn1UpstrOuterIOTransThicks(4, 0.);
  std::vector<double> aHorn1UpstrOuterIOTransLengths(4, 0.);
  std::vector<double> aHorn1UpstrOuterIOTransPositions(4, 0.);

  aHorn1UpstrOuterIOTransInnerRads[0] = 5.625*in;
  aHorn1UpstrOuterIOTransThicks[0] = 0.30*in;
  aHorn1UpstrOuterIOTransLengths[0] = 0.421*in;
  aHorn1UpstrOuterIOTransPositions[0] = aHorn1UpstrOuterIOTransLengths[0]/2. + 0.005*mm;

  aHorn1UpstrOuterIOTransInnerRads[1] = 5.763*in;
  aHorn1UpstrOuterIOTransThicks[1] = 0.32*in;
  aHorn1UpstrOuterIOTransLengths[1] = 0.421*in;
  aHorn1UpstrOuterIOTransPositions[1] = 
  aHorn1UpstrOuterIOTransLengths[0]+ aHorn1UpstrOuterIOTransLengths[1]/2. + 0.015*mm;
  
  aHorn1UpstrOuterIOTransInnerRads[2] = 5.763*in;
  aHorn1UpstrOuterIOTransThicks[2] = 0.4*in;
  aHorn1UpstrOuterIOTransLengths[2] = 0.75*in;
  aHorn1UpstrOuterIOTransPositions[2] = aHorn1UpstrOuterIOTransLengths[0] +
  aHorn1UpstrOuterIOTransLengths[1] + aHorn1UpstrOuterIOTransLengths[2]/2. + 0.050*mm;

  aHorn1UpstrOuterIOTransInnerRads[3] = 5.763*in;
  aHorn1UpstrOuterIOTransThicks[3] = 2.26*in;
  aHorn1UpstrOuterIOTransLengths[3] = aHorn1IOTransLength - 0.250*mm - 
  aHorn1UpstrOuterIOTransLengths[0] - aHorn1UpstrOuterIOTransLengths[1] - aHorn1UpstrOuterIOTransLengths[2];
    
  aHorn1UpstrOuterIOTransPositions[3] = aHorn1IOTransLength - aHorn1UpstrOuterIOTransLengths[3]/2. - 0.01*mm;;

// Outer dimension of the big tube that contains almost everything.. 
// Except the big connedtors rings downsream (max dim are 23.5 in 
     
  const double aHorn1OuterTubeOuterRad =  13.750*in/2.0;
  const double aHorn1OuterTubeInnerRad =  11.750*in/2.0; // checked with drawing 364094
 // 
//      
// Load the equations Drawing 8875.112-MD 363104, 363105
//
  fHorn1Equations.clear();
  LBNEHornRadialEquation e1(1.975805, -0.055858, -0.078740); fHorn1Equations.push_back(e1);
  LBNEHornRadialEquation e2(1.818869, -0.055858, 0.); fHorn1Equations.push_back(e2);
  LBNEHornRadialEquation eBlank(0., 0.,  0.); fHorn1Equations.push_back(eBlank); // equation 3 not found on drawing. 
  LBNEHornRadialEquation e4(-5.619190, 0.180183, 0.); fHorn1Equations.push_back(e4); 
  LBNEHornRadialEquation e5(-5.462253, 0.180183, -0.078740); fHorn1Equations.push_back(e5); 
  LBNEHornRadialEquation e6(1.97805, -0.055858, 0.); fHorn1Equations.push_back(e6); 
  fHorn1Equations.push_back(eBlank); // equation 7 not found on drawing as well 
  LBNEHornRadialEquation e8(-5.462253, 0.180183, 0.); fHorn1Equations.push_back(e8); 
  std::cerr << " Start testing PlaceFinalHorn1, mother Logical volume name " << TGAR->GetLogicalVolume()->GetName() << std::endl;
  fHorn1Equations[0].test1(); // this supposed to work.  But not if we rescale the Horn1, 
//   fHorn1Equations[3].test1(); // This supposed to fail... 
 
   const double hornWaterLayerThick = ND->GetHornWaterLayerThick(); // July 16-19 2014, P.L.
   if ( hornWaterLayerThick > 10.) {
     std::ostringstream messageOStr; messageOStr << " Unreasonable amount of water " << hornWaterLayerThick;
     std::string message(messageOStr.str());
     G4Exception("NumiDetectorConstruction::ConstructHorn1");
     exit(2); // only under 4.9.2 .. not reachable under 4.9.5 
   }
   //
   // Place the Mother volume, a G4 Polycone. Define first the boundary of Horn1. 
   //
   G4String aHorn1InnerCondMat("Aluminum");  
   std::vector<double> aMotherHorn1AllLengths;
   std::vector<double> aMotherHorn1AllRads;
   const double epsilR = 0.010*mm;
   double zCurrent = 0.;
   aMotherHorn1AllRads.push_back(aHorn1IOTransInnerRad - epsilR);
   aMotherHorn1AllLengths.push_back(zCurrent);
   aMotherHorn1AllRads.push_back(aHorn1IOTransInnerRad - epsilR);
//  zCurrent += fHorn1IOTransLength; // Oversize ? ! Drawing 363092, 363097 says (2.436" +  3.2852"  =  5.752 ) 
   zCurrent += 5.752*in; ; // Correct...
   aMotherHorn1AllLengths.push_back(zCurrent);
  // Up to the neck, equation 
   const int numSectEq1 = 5;
   const double lengthEq1 = 12.896*in/numSectEq1; // Drawin 363104 
   double zz1 = 3.316*in;
   for(int iSect1 = 0; iSect1 != numSectEq1; iSect1++) {
     zz1 += lengthEq1;
     const double rr1 = fHorn1Equations[0].GetVal(zz1) - epsilR - 0.300*mm -0.050*mm*iSect1;// needs a bit more room here, these subsection are too straight..
     zCurrent += lengthEq1;
     aMotherHorn1AllRads.push_back(rr1); 
     aMotherHorn1AllLengths.push_back(zCurrent);
//    std::cerr << " Polycone Mother, subsection 1 , zz1 " << zz1 << " zCurrent " << zCurrent << " rr1 " << rr1 << std::endl;
   }
   const int numSectEq2 = 8;
   const double lengthEq2 = (4.9799*in + 9.2262*in)/numSectEq2; // Drawing 363104, 363105 
   for(int iSect2 = 0; iSect2 != numSectEq2; iSect2++) {
     zz1 += lengthEq2;
     const double rr2 = fHorn1Equations[1].GetVal(zz1)  - epsilR - 0.8*mm - 0.1*mm*iSect2;// empirical, but it will need to be improved on for 
                                                          // misalignment studies. 
     zCurrent += lengthEq2;
     aMotherHorn1AllRads.push_back(rr2); 
     aMotherHorn1AllLengths.push_back(zCurrent);
//    std::cerr << " Polycone Mother, subsection 2 , zz1 " << zz1 << " zCurrent " << zCurrent << " rr2 " << rr2 << std::endl;
   }
  // The neck + all the other sections, set to the neck. 
   const double neckRadius = 0.709*in/2.; // Drawing 363105 
   aMotherHorn1AllRads.push_back(neckRadius - epsilR);
   aMotherHorn1AllLengths.push_back(zCurrent);
   zCurrent += 1.568*in; zz1 += 1.568*in;
   aMotherHorn1AllRads.push_back(neckRadius - epsilR);
   aMotherHorn1AllLengths.push_back(zCurrent);
//  std::cerr << " ... Just downstream of the neck, zz1 = " << zz1 << " zCurrent " << zCurrent << std::endl;
  // Downstream of the neck.. Equation 4 
   const int numSectEq4 = 4;
   const double lengthEq4 = (36.2709 - 31.8827)*in/numSectEq4; // Drawing 363105 
   for(int iSect4 = 0; iSect4 != numSectEq4; iSect4++) {
     zz1 += lengthEq4;
     double rr4 = fHorn1Equations[3].GetVal(zz1)  - epsilR - 0.2*mm - 0.1*mm*iSect4;// empirical, but it will need to be improved on for 
                                                          // misalignment studies. 
     if (iSect4 == 0) rr4 = (rr4 + neckRadius)/2.0; // take the average... Not really important, a realistic plug should nevr fit tightly. 
     zCurrent += lengthEq4;
     aMotherHorn1AllRads.push_back(rr4); 
     aMotherHorn1AllLengths.push_back(zCurrent);
//    std::cerr << " Polycone Mother, subsection 4 , zz1 " << zz1 << " zCurrent " << zCurrent << " rr4 " << rr4 << std::endl;
   }
   // Downstream of the neck.. Equation 5 Up to the downstream flange, drawing 363108, 363096
   const int numSectEq5 = 20;
   const double lengthEq5 = (117.126 - 36.27097)*in/numSectEq5; // Drawing 363105, 363108 
   double rLastParabolic = 0.;
   for(int iSect5 = 0; iSect5 != numSectEq5; iSect5++) {
     zz1 += lengthEq5;
     const double rr5 = fHorn1Equations[4].GetVal(zz1)  - epsilR - 1.0*mm - 0.1*mm*iSect5;// empirical, but it will need to be improved on for 
                                                          // misalignment studies. 
     zCurrent += lengthEq5;
     aMotherHorn1AllRads.push_back(rr5); 
     aMotherHorn1AllLengths.push_back(zCurrent);
//    std::cerr << " Polycone Mother, subsection 5 , zz1 " << zz1 << " zCurrent " << zCurrent << " rr5 " << rr5 << std::endl;
     rLastParabolic = rr5;
   }  
   zCurrent += 14.244*in + 3.0*cm; // xx cm safe for misalignment Drawing 363093 and 363096  
   aMotherHorn1AllRads.push_back(rLastParabolic - epsilR);
   aMotherHorn1AllLengths.push_back(zCurrent);
  //
  // Now the outer...
  // 
   const double rOut = 16.25*in + 7.50*in + 2.5*cm; // 7.5 inches is an overestimate of the max. outer dimension of exhaust piping, downstream. 
    // 2.5 in is a guess the thickness of the outer tube. 
   aMotherHorn1AllRads.push_back(rOut);
   aMotherHorn1AllLengths.push_back(zCurrent);
   aMotherHorn1AllRads.push_back(rOut);
   aMotherHorn1AllLengths.push_back(0.);
  //
  // dump this curve, so we can draw it.
//  std::ofstream fileOutTmp("./RZCurve_1.txt");
//  fileOutTmp << " Z R " << std::endl;
//  for (size_t ii=0 ; ii != aMotherHorn1AllLengths.size(); ii++) {
//     std::cerr << " Radius At at  Z(StartHorn1) "<< fMotherHorn1AllLengths[ii]  
//	       << "  is " << fMotherHorn1AllRads[ii] << std::endl;
//     fileOutTmp <<  " " << fMotherHorn1AllLengths[ii] << " " << fMotherHorn1AllRads[ii] << std::endl;     
//  }
//  fileOutTmp.close();
//  std::cerr << " And quit !!! ... " << std::endl; exit(2);
//
// We prepare the field Maps.  Three of them, one for the mother volume
//    (with check that we don't go below the inner radius of the inner conductor)
  numiMagField->rotateHorns(); 
  numiMagFieldIC->rotateHorns(); 
  numiMagFieldOC->rotateHorns(); 
  numiMagField->SetIgnoreCEQBr(fIgnoreCEQBr);;
  numiMagField->SetHorn1FieldZCutUpstream(fHorn1FieldZCutUpstream);
  numiMagField->SetHorn1FieldZCutDwnstream(fHorn1FieldZCutDwnstream);
  numiMagField->SetHorn1CurrentEqualizerLongAbsLength(fHorn1CurrentEqualizerLongAbsLength);
  numiMagField->SetHorn1CurrentEqualizerQuadAmpl(fHorn1CurrentEqualizerQuadAmpl); 
  numiMagField->SetHorn1CurrentEqualizerOctAmpl(fHorn1CurrentEqualizerOctAmpl); 
     
  G4FieldManager* FieldMgrMother = new G4FieldManager(numiMagField); //create a local field      
  FieldMgrMother->SetDetectorField(numiMagField); //set the field 
  FieldMgrMother->CreateChordFinder(numiMagField); //create the objects which calculate the trajectory 

  G4FieldManager* FieldMgrIC = new G4FieldManager(numiMagFieldIC); //create a local field      
  FieldMgrIC->SetDetectorField(numiMagFieldIC); //set the field 
  FieldMgrIC->CreateChordFinder(numiMagFieldIC); //create the objects which calculate the trajectory 
  //  FieldMgr3->GetChordFinder()->SetDeltaChord(.5*mm);
  G4FieldManager* FieldMgrOC = new G4FieldManager(numiMagFieldOC); //create a local field      
  FieldMgrOC->SetDetectorField(numiMagFieldOC); //set the field 
  FieldMgrOC->CreateChordFinder(numiMagFieldOC); //create the objects which calculate the trajectory 
//
// Compute now the maximum length of the beast. 
//
   double maxLengthHorn1 = -100000.*m;  
   for (std::vector<double>::const_iterator il = aMotherHorn1AllLengths.begin(); il != aMotherHorn1AllLengths.end(); il++) 
	    if (*il > maxLengthHorn1) maxLengthHorn1 = *il;
	    //
	// We shift the Z position by half the length, such the relative definition is the same as for the other volumes. 
   std::vector<double> zz(aMotherHorn1AllLengths);
   for (std::vector<double>::iterator il = zz.begin(); il != zz.end(); il++) *il -= maxLengthHorn1/2.;       
   G4Polycone* aPConMother = new G4Polycone(std::string("MHorn1"), 0.*deg, 360.0*deg, 
                                            static_cast<int>(aMotherHorn1AllLengths.size()), &aMotherHorn1AllRads[0], &zz[0]);
  std::cerr << " Horn1 Polycone Mother Alternate Parameters  " << std::endl; 
  for (size_t k=0; k != zz.size(); k++) {
    std::cerr << " ZPos " << zz[k] << " Radius in RZ " << aMotherHorn1AllRads[k] << std::endl;
  }
   G4LogicalVolume* aLogVolMother = new G4LogicalVolume(aPConMother, G4Material::GetMaterial(std::string("Air")), 
                                                         std::string("lvMHorn1")); 
   aLogVolMother->SetFieldManager(FieldMgrMother,true); //attach the local field to logical volume
   const bool aCheckVolumeOverLapWC=true;
   G4ThreeVector TargetHallPosition=G4ThreeVector(0,0,ND->TargetAreaLength/2.+ ND->TargetAreaZ0);
//   G4ThreeVector hornTranslation(hornpos-TargetHallPosition); 
//   hornTranslation[2] += (-1.0*zz[0]) - 30.*mm; // Guess.. to be check with Geantinos
//   G4PVPlacement* motherTop = new G4PVPlacement(&hornrot, hornTranslation, aLogVolMother, 
//	                             std::string("pvMHorn1Mother"), TGAR->GetLogicalVolume(), false, 0, aCheckVolumeOverLapWC);
   rotation=hornrot;
   translation=hornpos-TargetHallPosition; // Guess.. to be check with Geantinos
   translation[2] += maxLengthHorn1/2 - 61.84; // The way the G4Polycon is defined.. And the infamous MCZero with respect to the start of the Horn1.
   const double theEndOfHorn1wrtTGAR = translation[2] + maxLengthHorn1/2;
   G4PVPlacement* motherTop = new G4PVPlacement(G4Transform3D(rotation,translation),"pvMHorn1Mother", aLogVolMother,TGAR,false,0, true);
   std::cerr << " Position for Horn1 Mother, Alternate version " << translation << std::endl;
   std::cerr << "  ......... hornPos " << hornpos[2] << " Tgt Hall Position " <<  TargetHallPosition[2] << std::endl;
   				     
// 
// Start with upstream Inner to Out transition. Usptream means at Z MC (Z Geant4 world coord. ) Negative
//
  const bool withTorus = fHorn1UpstrIOisTorus;
//  const bool withTorus = false;
  if (withTorus) { // Fancier CGS construct, but geant4 seems to do this correctly. 
                   // Code stolen from old version, checked. 
	// From Z drawing -2.436 inches to Z = 0, a Torus shell. Drawing 8875.112-MD-363097
	
     G4VSolid* sHorn1Front;
     const double frontRmin = 2.116*in;
     const double frontRmax = 2.436*in;
     const double frontRtor = 3.763*in;
     G4Torus* sFrontTorus=new G4Torus("sFrontTorus", frontRmin, frontRmax, frontRtor,0,360.*deg);
     G4Box* sBox=new G4Box("sBox",frontRtor*2.,frontRtor*2.,frontRmax*2.);
     rotation=G4RotationMatrix(0.,0.,0.);
     translation=G4ThreeVector(0.,0.,frontRmax*2);
     sHorn1Front=new G4SubtractionSolid("sHorn1Front",sFrontTorus,sBox,G4Transform3D(rotation,translation)); //need only half of torus

     G4LogicalVolume* lvHorn1Front=new G4LogicalVolume(sHorn1Front,GetMaterial(ND->hrnmat),"lvHorn1FrontTorus",0,0,0);
     ND->ApplyStepLimits(lvHorn1Front); // Limit Step Size
     rotation=G4RotationMatrix(0.,0.,0.);
     translation=G4ThreeVector(0.,0., -maxLengthHorn1/2. + frontRmax);
     new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn1FrontTorus",lvHorn1Front, motherTop, false,0, true);
     // Inner conductor, up to Z= 3.3157. In one section..The magnetic field matters less here, as we are at large radius...
    
     const double r0 = fHorn1Equations[0].GetVal(0.);
     const double r1 = r0 + 0.32*in;
     const double r2 = fHorn1Equations[0].GetVal(3.3157*in);
     const double r3 = r2 + 0.118*in;
     const double zL = 3.3157*in - 0.002*mm;
     G4String nameStr("Horn1FrontInnerConnector");
     G4Cons* aCons = new G4Cons(nameStr, r0, r1, r2, r3, zL/2., 0., 360.0*deg);
     G4LogicalVolume* vlPartIO = new G4LogicalVolume(aCons, G4Material::GetMaterial(std::string("Aluminum")), nameStr);
     translation=G4ThreeVector(0.,0., -maxLengthHorn1/2. + frontRmax + 0.001*mm + zL/2.);
     new G4PVPlacement(G4Transform3D(rotation,translation),"PHornIOInnerZ0",vlPartIO, motherTop, false,0, true);
     // 
     // Connection to Outer Conductor flange.
     //
     const double rMinOuter = (11.75/2.)*in; 
     const double rMaxOuterCon = rMinOuter + 0.32*in;
     const double zLOuterCon = (3.316 - 1.510 - 0.079 - 1.0)*in - 0.002*mm;
     
     nameStr = G4String("Horn1FrontOuterConnector");
     G4Tubs* aTube = new G4Tubs(nameStr, rMinOuter, rMaxOuterCon, zLOuterCon/2., 0., 360.*deg);
     G4LogicalVolume *aLVol= new G4LogicalVolume(aTube, G4Material::GetMaterial(std::string("Aluminum")), nameStr); 
     translation=G4ThreeVector(0.,0., -maxLengthHorn1/2. + frontRmax + 0.001*mm + zLOuterCon/2.);
     new G4PVPlacement(G4Transform3D(rotation,translation),"PHornIOOuterConn",aLVol, motherTop, false,0, true);
     //
     // Upstream Outer Conductor flange. 
     //
     const double rMaxOuterFlange = (16.0/2.)*in;
     const double zLOuterFlange = 1.0*in - 0.002*mm ;     
     nameStr = G4String("Horn1FrontOuterFlange");
     G4Tubs* aFlange = new G4Tubs(nameStr, rMinOuter, rMaxOuterFlange, zLOuterFlange/2., 0., 360.*deg); 
     G4LogicalVolume *aLVolFl= new G4LogicalVolume(aFlange, G4Material::GetMaterial(std::string("Aluminum")), nameStr);
     translation=G4ThreeVector(0.,0., -maxLengthHorn1/2. + frontRmax + 0.001*mm + zLOuterCon + zLOuterFlange/2.);
     new G4PVPlacement(G4Transform3D(rotation,translation),"PHornIOOuterFlange",aLVolFl, motherTop, false,0, true);
     
//  } else if (hornpos[2] < -1.e20) { // skip the I/O transition entirely, to check the geometry. 
  } else { 
      G4String nameHorn1IO("UpstrHorn1TransInnerOuter");
      G4String nameHorn1IOCont(nameHorn1IO + G4String("Cont")); // Cont stand for container. 
//      Create(nameHorn1IOCont);
      const double aRmin = 1.26*in - 0.250*CLHEP::mm;
      const double aRmax = horn1IOInnerRadsDownstr[4] + horn1IOTransThick[4] + 1.*mm; 
      const double aLength = aHorn1IOTransLength; 
      std::cerr << " Params for  UpstrHorn1TransInnerOuterCont rMin " << aRmin 
                << " rmax " << aRmax << " aLength " << aLength << std::endl << " Length of pieces... " << std::endl;
      for (size_t k=0; k != horn1IOUpstrLengths.size(); k++) {
        std::cerr << "  .......... " << k << " " <<  horn1IOUpstrLengths[k] << std::endl;
      }
      G4Tubs* aTubs = new G4Tubs(nameHorn1IO, aRmin, aRmax, aLength/2.,
	                           0., 360.0*deg);
      G4LogicalVolume* aLVol = new G4LogicalVolume(aTubs, G4Material::GetMaterial(std::string("Air")), nameHorn1IO);
      G4ThreeVector aPosition(0., 0., 0.);
      aPosition[2]=  -maxLengthHorn1/2. + aLength/2. + 0.005*CLHEP::mm; 
       std::cerr << " Setting position for UpstrHorn1TransInnerOuterCont, length " << aLength  
	              << " length mother " << aLength << " zRel " << aPosition << std::endl;
      
      G4PVPlacement *vHorn1IOCont = new G4PVPlacement((G4RotationMatrix *) 0, aPosition , aLVol, 
	                             nameHorn1IO, aLogVolMother , false, 0, aCheckVolumeOverLapWC);
      for (size_t iPartIO = 0; iPartIO != aHorn1UpstrInnerRadsUpstr.size(); iPartIO++) {
        std::ostringstream nameStrStr; nameStrStr << nameHorn1IO << "Part" << iPartIO;
        G4String nameStr(nameStrStr.str());
	G4LogicalVolume *vlPartIO;
	if (iPartIO != 2) {
           const double r0 = horn1IOInnerRadsUpstr[iPartIO];
           const double r2 = horn1IOInnerRadsDownstr[iPartIO]; 
           const double r1 = horn1IOInnerRadsUpstr[iPartIO] + horn1IOTransThick[iPartIO];
           const double r3 = horn1IOInnerRadsDownstr[iPartIO] + horn1IOTransThick[iPartIO];
	   const double zL = horn1IOUpstrLengths[iPartIO];
           std::cerr << " Params for  UpstrHorn1TransInnerOuterPart " << iPartIO << " r0 " << r0 
	             << " r1 " << r1 << " r2 " << r2 << " r3 " << r3  << " zl " << zL << std::endl;
	   
           G4Cons* aCons = new G4Cons(nameStr, r0, r1, r2, r3, zL/2., 0., 360.0*deg);
           vlPartIO = new G4LogicalVolume(aCons, G4Material::GetMaterial(std::string("Aluminum")), nameStr);
	} else { 
           const double rMin = horn1IOInnerRadsUpstr[1] + 0.16*in;
           const double rMax = horn1IOInnerRadsUpstr[3] + 0.16*in;
	   const double zL= 0.32*in - 0.050*CLHEP::mm;
           G4Tubs* aTubs = new G4Tubs(nameStr, rMin, rMax, zL/2., 0., 360.0*deg);
           vlPartIO = new G4LogicalVolume(aTubs, G4Material::GetMaterial(std::string("Aluminum")), nameStr);
	}
        G4ThreeVector aPosition(0., 0., 0.);
	aPosition[2] = horn1IOUpstrZPositions[iPartIO];
        std::cerr << " Z Position  UpstrHorn1TransInnerOuterPart " << iPartIO << " = " << aPosition[2] << std::endl;
        new G4PVPlacement((G4RotationMatrix *) 0, aPosition ,vlPartIO, 
	                             std::string("pv")+nameStr, vHorn1IOCont->GetLogicalVolume(), 
				     false, 0, aCheckVolumeOverLapWC);;	
      }
     // Now build the downstream part of the Inner Outer transition conductor, by downstream, we mean 
     // Z G4, world (MC on enginering drawing) > 0. Drawing  8875-112-MD-363097
        //
       // September 2017... This seesm to be duplicated code !...
       //
      /*
     {
       // Bad choice of variable names here, at the prefix "Upstr" appears.. Mean here upstream of the inner conductor 
       // per-se. We keep these variable names, for ease of maintenance of both g4lbne and g4numi. 
       std::string nameStrIO("Horn1IOTransCont");
       G4Tubs* aTube = new G4Tubs(nameStrIO, aHorn1IOTransInnerRad, 
                                  aHorn1IOTransOuterRad, aHorn1IOTransLength/2., 0., 360.*deg);
       G4LogicalVolume *aLVol= new G4LogicalVolume(aTube, G4Material::GetMaterial(std::string("Air")), nameStrIO); 
       G4ThreeVector aPosition(0., 0., 0.);
       aPosition[2] = -maxLengthHorn1/2. + (2.436*in - 30*mm) +  aHorn1IOTransLength/2.;
       G4PVPlacement *vTrUpst = new G4PVPlacement((G4RotationMatrix *) 0, aPosition , aLVol, 
	                             std::string("pv")+nameStrIO, aLogVolMother, false, 0, aCheckVolumeOverLapWC);
       for (size_t k=0; k!=aHorn1UpstrLengths.size(); k++) {
         std::ostringstream nameStrStr; nameStrStr << "Horn1IOTransInnerPart" << k;
         G4String nameStr(nameStrStr.str());
         G4Cons *aCons = new G4Cons(nameStr, aHorn1UpstrInnerRadsUpstr[k],aHorn1UpstrInnerRadsOuterUpstr[k],
                                         aHorn1UpstrInnerRadsDownstr[k],aHorn1UpstrInnerRadsOuterDownstr[k],
	                              aHorn1UpstrLengths[k]/2., 0., 360.0*deg);
         G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
         pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
         G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;  
         posTmp[2] = -1.0*aHorn1IOTransLength/2. + aHorn1UpstrZPositions[k];			      
         new G4PVPlacement((G4RotationMatrix *) 0,	posTmp, 
	                                                    pCurrent, nameStrIO + std::string("_P"), 
                                                            vTrUpst->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);
       }
   
       for (size_t k=0; k!= aHorn1UpstrOuterIOTransInnerRads.size(); k++) {
         std::ostringstream nameStrStr; nameStrStr << "Horn1IOTransOuterPart" << k;
         G4String nameStr(nameStrStr.str());
         G4Tubs *aTubs = new G4Tubs(nameStr, aHorn1UpstrOuterIOTransInnerRads[k],
                                          aHorn1UpstrOuterIOTransInnerRads[k] + aHorn1UpstrOuterIOTransThicks[k],
	                              aHorn1UpstrOuterIOTransLengths[k]/2., 0., 360.0*deg);
         G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;  
         posTmp[2] = -1.0*(aHorn1IOTransLength)/2. + aHorn1UpstrOuterIOTransPositions[k];			      
         G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(std::string("Aluminum")), nameStr);
         pCurrent->SetFieldManager(FieldMgrOC,true); //attach the local field to logical volume
         new G4PVPlacement((G4RotationMatrix *) 0,	posTmp, pCurrent, nameStr + std::string("_P"), 
                        vTrUpst->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	      
       }
     }
*/
     // Install the section of the IC that connect the I/O/ transition to the IC itself  A straight CONS. 
     //
     G4String nameStrIOtoIC("UpstrHorn1TransInnerToIC");
     const double r0tmp = fHorn1Equations[0].GetVal(0.);
     const double r2tmp = (2.520/2.)*in; 
     const double r1tmp = r0tmp + 0.32*in;
     const double r3tmp = r2tmp + 0.25*in; // Approximation of the weld. !!!
     // bug corrected on October 3.. No impact, we use the torus section.. 
     const double zLtmp = 3.316*in - 0.050*CLHEP::mm;
     G4Cons* aConsIOtoIC = new G4Cons(nameStrIOtoIC, r0tmp, r1tmp, r2tmp, r3tmp, zLtmp/2., 0., 360.0*deg);
     G4LogicalVolume *vlPartIOtoIC = new G4LogicalVolume(aConsIOtoIC, G4Material::GetMaterial(std::string("Aluminum")), nameStrIOtoIC);
     G4ThreeVector aPositionTmp(0., 0., 0.);
     aPositionTmp[2] = -aHorn1IOTransLength/2. + 2.436*in + zLtmp/2. + 0.0125*CLHEP::mm;
     std::cerr << " Z Position UpstrHorn1TransInnerToIC  = " << aPositionTmp[2] << std::endl;
     new G4PVPlacement((G4RotationMatrix *) 0, aPositionTmp ,vlPartIOtoIC, 
	                             std::string("pv")+nameStrIOtoIC, vHorn1IOCont->GetLogicalVolume(), 
				     false, 0, aCheckVolumeOverLapWC);
      //finally, completing the I/O section at large radius. 
     G4String nameStrIOtoOC("UpstrHorn1TransInnerToOC");
     const double rMinICtoOC = (2.520/2.)*in + 2.0*2.436*in;
     const double rMaxICtoOC = rMinICtoOC + 0.32*in - 0.05*CLHEP::mm; 
     const double zLICtoOC = 3.316*in - 1.510*in - 0.250*CLHEP::mm; // Approximation of notches, silver plating etc...  
     G4Tubs* aTubsIOtoOC = new G4Tubs(nameStrIOtoOC, rMinICtoOC, rMaxICtoOC,  zLICtoOC/2., 0., 360.0*deg);
     G4LogicalVolume *vlPartIOtoOC = new G4LogicalVolume(aTubsIOtoOC, G4Material::GetMaterial(std::string("Aluminum")), nameStrIOtoOC);
     aPositionTmp[2] = -aHorn1IOTransLength/2. + 2.436*in + zLICtoOC/2. + 0.0125*CLHEP::mm;
     std::cerr << " Z Position UpstrHorn1TransInnerToIC  = " << aPositionTmp[2] << std::endl;
     new G4PVPlacement((G4RotationMatrix *) 0, aPositionTmp ,vlPartIOtoOC, 
	                             std::string("pv")+nameStrIOtoOC, vHorn1IOCont->GetLogicalVolume(), 
				     false, 0, aCheckVolumeOverLapWC);;	
     
     
    
   } // multi cone decomposition, up to Z Drawing of z = 3.3157 inches. 
    // RZ position for the Current Equalizer section map. 
     // For the torus section, we cheat and set to the radius at Z = start of this section.. 
     // Messy, we repeat the code above. 
     const double r0tmpxx = fHorn1Equations[0].GetVal(0.);
     const double r2tmpxx = (2.520/2.)*in; 
     const double r1tmpxx = r0tmpxx + 0.32*in;
     const double r3tmpxx = r2tmpxx + 0.125*in; // Approximation of the weld. 
     for (size_t izz = 0; izz != 30; izz++) fEffectiveRadiiForFieldMap[izz] = r1tmpxx;
     for (size_t izz = 30; izz != 30+84; izz++) fEffectiveRadiiForFieldMap[izz] = r1tmpxx + (izz-30)*(r3tmpxx - r1tmpxx)/84.;
     std::cerr << " Store fEffectiveRadiiForFieldMap, radius at index 84 " << fEffectiveRadiiForFieldMap[84]
               	<< " At 113 " << fEffectiveRadiiForFieldMap[113] << std::endl;	     	
   //
   // We place more volumes in the mother volume.. All individuals cones. 
   //
   // Temporary change to study the effect of the segmentation., parabolic vs polygonal approx 
   // November 30 2014. 
   const bool crudeSegmentation = false;
   const bool refinedSegmentation = ND->GetHorn1IsRefined();
   G4cout << "NumiDetectorConstruction::ConstructHorn1Alternate: refined segmentation is " << refinedSegmentation << G4endl;
   
   const double z21p088 = 21.088*in; //Equation change.  
   const double zOffsetDrawingUpstrEdge = 5.752*in; // On the inner conductor, excluding I/O trans. Drawing 363097 
     
   const double lengthInnerConductUpstr = z21p088 - (3.316*in) - 0.010*mm; // Up to Z = 21.088, Drawing coordinate system.  
  
   int numSubSect = GetNumberOfInnerHornSubSections(0, 0., lengthInnerConductUpstr, 10);
   // Fill with it one or more inner conductor conical section
   // We require a precision of 5 microns in the radius.
   if (crudeSegmentation) numSubSect = 1; // There!  Ultimate simplification. 
   if (refinedSegmentation) numSubSect *= 2; // refine it.. 
   std::cerr << " Number of segments from Z = 0 to 21.088 inches " << numSubSect << std::endl;
   const double deltaZ = lengthInnerConductUpstr/numSubSect;
   for (int iSub=0; iSub != numSubSect; iSub++) { 
     //
      const double aZHorn1ACRNT1Shift =  3.316*in;  // Drawing MD-363097 
      const double zzBegin = aZHorn1ACRNT1Shift + (iSub*deltaZ); // from the 
      const double zzEnd = zzBegin + deltaZ;
     std::ostringstream nameStrStr; nameStrStr << "Horn1UpstrSubSect" << iSub;
     G4String nameStr(nameStrStr.str());
// Smooth transition between equation 1 and 2 
     const double rMin1Eqn1 = fHorn1Equations[0].GetVal(zzBegin); // Equation 1 or 0
     const double rMin2Eqn1 = fHorn1Equations[0].GetVal(zzEnd);
     const double rMin1Eqn2 = fHorn1Equations[1].GetVal(zzBegin); // Equation 1 or 0
     const double rMin2Eqn2 = fHorn1Equations[1].GetVal(zzEnd);
     const double ratio10vs1 = std::min(1.0, (zzBegin/(21.0888*in)));
     double rMin1 = rMin1Eqn2*ratio10vs1 + (1.0-ratio10vs1)*rMin1Eqn1;
     const double ratio20vs1 = std::min(1.0, (zzEnd/(21.0888*in)));
     double rMin2 = rMin2Eqn2*ratio20vs1 + (1.0-ratio20vs1)*rMin2Eqn1;
     double rMax1 = fHorn1Equations[5].GetVal(zzBegin) + hornWaterLayerThick + 0.0025*mm; 
       // Equation 6 (Drawing 8875.112-MD 363104)
     double rMax2 = fHorn1Equations[5].GetVal(zzEnd) + hornWaterLayerThick + 0.0025*mm;     
     std::cerr << " Inner radius for section " << nameStr << " At zzBegin " << zzBegin << " to " << zzEnd 
               << " rMin1 " << rMin1 << " rMin2 " << rMin2 
	       << " rMax1 " << rMax1 << " rMax2 " << rMax2 << std::endl;
     G4Cons *aCons = new G4Cons(nameStr, rMin1, rMax1,rMin2, rMax2,
	                              (deltaZ - 0.005*mm)/2., 0., 360.0*deg);
     G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
     pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
     G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
     // plHUpst constain part of the Inner Outer Transition. Shift downtream by it's length 
     posTmp[2] = -maxLengthHorn1/2. + zOffsetDrawingUpstrEdge + 0.005*mm + deltaZ/2. + (iSub*deltaZ);
     std::cerr << " Placing section " << nameStr << " at z = " << posTmp[2] << std::endl;			      
//	       
     G4PVPlacement *vSub = new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	

     size_t izzBegin = static_cast<size_t>(aZHorn1ACRNT1Shift + 30 + (iSub*deltaZ));			
     fEffectiveRadiiForFieldMap[izzBegin] = rMax1;
     size_t izzEnd = static_cast<size_t>(aZHorn1ACRNT1Shift + 30 + ((iSub+1)*deltaZ));			
     fEffectiveRadiiForFieldMap[izzEnd] = rMax2;
     std::cerr << " fEffectiveRadiiForFieldMap saga, indices " << izzBegin << " / " << izzEnd 
               << " radii " << fEffectiveRadiiForFieldMap[izzBegin] << " / " << fEffectiveRadiiForFieldMap[izzEnd] << std::endl;
    if (hornWaterLayerThick	> 0.002*mm) {
     std::ostringstream nameStrStr; nameStrStr << "Horn1UpstrSubSect" << iSub << "Water";
     G4String nameStr(nameStrStr.str());
     G4Cons *aCons = new G4Cons(nameStr, rMax1 - hornWaterLayerThick, rMax1-0.001*mm,
                                         rMax2 - hornWaterLayerThick, rMax2-0.001*mm,
	                              (deltaZ - 0.0075*mm)/2., 0., 360.0*deg);
     G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(std::string("Water")), nameStr);
     pCurrent->SetFieldManager(FieldMgrMother,true); //attach the local field to logical volume
     G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.; posTmp[2] =0.;			      
     new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        vSub->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
    }
   }
   const bool placeWelds = true;
   const bool placeHangers =  true;
//   const bool placeWelds = false;
//   const bool placeHangers =  false;
   
    // on the number of subsections for the inner conductor, for the upstream part of Horn1 
   // Now add the welding joint between the most upstream part of the inner conductor and the Inner Outer transition section
   // Drawing Drawing 8875.112-MD 363104
   if (placeWelds) {
     G4String nameStr("Horn1UpstrSubSect0WeldUpstr");
     const double length = 12.0*mm; // Make it a bit shorter, it is rounded... 
     double rTmp1 = fHorn1Equations[5].GetVal(3.2645*in) 
                              + 0.02*mm + hornWaterLayerThick;
			      
        // place it a little more detached..Also, the weld is on top of the layer of water.. Oh well.. 
      const double rTmp2 = rTmp1 + 1.8*mm; // 
     G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, 
	                           length/2.   , 0., 360.0*deg);
     G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial("Aluminum"), nameStr);
     pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
     G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
     posTmp[2] = -maxLengthHorn1/2.  + zOffsetDrawingUpstrEdge + length/2. + 1.0*mm; // Drawing MD 363104
     
     std::cerr << " Placing section " << nameStr << " at z = " << posTmp[2] << " radii " 
               << rTmp1 << " , " <<  rTmp2  << " length " << length << std::endl;			      
     new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
   
   }
   // Now place the first support ring, and the spider hang
   const double lengthHangerRing = 0.750*in;
   std::cerr << " First Spider Hanger is in Horn1Upstr section " << std::endl;
   
   G4String nameStrFirstHanger("Horn1UpstrSpiderHanger");
   const double zPosCenterMotherVolume = -1.0*maxLengthHorn1/2.  + 2.436*in + 0.5*mm + 
                                         19.347*in;  // Drawing 363093 and  363097			   
   const double zPosUpstrDrawingCoord = 19.347*in; 			   
   
   if (placeHangers) this->Horn1InstallSpiderHanger(nameStrFirstHanger, zPosUpstrDrawingCoord, 
	                                    zPosCenterMotherVolume, aHorn1OuterTubeInnerRad, motherTop );			       
   // Outer tube 
   const double zShiftDrawingDownstr = 2.436*in; 
      // from now, a better orgin is the upstram point on mother Horn1PolyM1
   
   G4String nameStr("Horn1OutrTube");
   const double lengthOutT = (127.550 - 1.510)*in - 1.0*mm;
   // displace 100 microns to avoid clash with spider hanger.  
   G4Tubs *aTubs = new G4Tubs(nameStr, aHorn1OuterTubeInnerRad + 0.1*mm, 
                                        aHorn1OuterTubeOuterRad + 0.1*mm, lengthOutT/2.   , 0., 360.0*deg);
   G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(std::string("Aluminum")), nameStr);
   pCurrent->SetFieldManager(FieldMgrOC,true); //attach the local field to logical volume
   G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
   const double zOffOut = 3.316*in;
   posTmp[2] = -1.0*(maxLengthHorn1)/2. + zShiftDrawingDownstr + zOffOut + lengthOutT/2. + 0.25*mm;			      
   std::cerr << " Installing Outer tube sptream at " << posTmp[2] << " lengthOutT " << lengthOutT << std::endl;
   new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);
				
   // Outer Tube Upstream flange. See drawing  363094
   G4String nameStrOutFlUpstr("Horn1UpstrOutrTubeFlange");
   const double lengthOutFlUpstr = 1.0*in; //Not cleanly shown on drawing 363094 
   const double rTmpOutFlUpstrInner = aHorn1OuterTubeOuterRad + 0.1*mm;
   const double rTmpOutFlUpstrOuter = rTmpOutFlUpstrInner + 2.5*in; // Still a guess.. Probably a bit oversized.  
   aTubs = new G4Tubs(nameStrOutFlUpstr,  rTmpOutFlUpstrInner, rTmpOutFlUpstrOuter, lengthOutFlUpstr/2.0, 0., 360.0*deg);
   pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(std::string("Aluminum")), nameStrOutFlUpstr);
   posTmp[0] = 0.; posTmp[1] = 0.;
   posTmp[2] = -1.0*(maxLengthHorn1)/2. + zShiftDrawingDownstr + zOffOut + lengthOutFlUpstr + 0.055*mm;
   new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStrOutFlUpstr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);
			//
   double lengthToTheNeck = (30.3150 - 21.088)*in; // Drawing 363105 
   
   // Downstream of the neck, we install everything in Horn1PolyM1.. Simpler.. 
   
   // Let us call this the mid-section.. Historical notation.. It will install in the upstream section.. 
  //
  // Install the length of inner conductor, from downstream end of the target to Zdrawing 21.0888 inches 
  //
//  std::cerr << " ... Length to the neck " << lengthToTheNeck << std::endl;
  {
     int numSubSect = 4; 
     if (crudeSegmentation) numSubSect = 1; // There!  Ultimate simplification. 
     if (refinedSegmentation) numSubSect *= 2; // refine it.. 
     std::cerr << " Number of segments from Z = 21.088 to the Neck " << numSubSect << std::endl;
     const double deltaZ = lengthToTheNeck/numSubSect;
     const double zOffStart = 21.088*in + 0.025*mm;
//     std::cerr << " ...... delta z to 21.0888 " << deltaZ << std::endl;
     for (int iSub = 0; iSub != numSubSect; iSub++) {					      
       const double zzBegin = z21p088 + iSub*deltaZ;
       const double zzEnd = zzBegin + deltaZ;
       std::ostringstream nameStrStr; nameStrStr << "Horn1ToNeckPartM0SubSect" << iSub;
       G4String nameStr(nameStrStr.str());
       const double rMin1Eqn1 = fHorn1Equations[0].GetVal(zzBegin); // Equation 1 or 0
       const double rMin2Eqn1 = fHorn1Equations[0].GetVal(zzEnd);
       const double rMin1Eqn2 = fHorn1Equations[1].GetVal(zzBegin); // Equation 1 or 0
       const double rMin2Eqn2 = fHorn1Equations[1].GetVal(zzEnd);
       const double rMin1 = ((numSubSect - iSub -1)*rMin1Eqn1 + ((iSub+1)*rMin1Eqn2))/numSubSect;
       const double rMin2 = ((numSubSect - iSub -1)*rMin2Eqn1 + ((iSub+1)*rMin2Eqn2))/numSubSect;
       const double rMax1 = fHorn1Equations[5].GetVal(zzBegin) + hornWaterLayerThick + 0.0025; 
       // Equation 6 (Drawing 8875.112-MD 363104)
       const double rMax2 = fHorn1Equations[5].GetVal(zzEnd) + hornWaterLayerThick + 0.0025;
       const double lengthTmp = deltaZ - 0.050*mm;    
       G4Cons *aCons = new G4Cons(nameStr, rMin1, rMax1,rMin2, rMax2, lengthTmp/2., 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
       pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
       posTmp[2] = -1.0*(maxLengthHorn1)/2. + zShiftDrawingDownstr + zOffStart + deltaZ/2. + iSub*deltaZ;
       std::cerr << " Installing mid section Horn1, length " << lengthTmp << " at Z = " << posTmp[2] 
                 << " Rads " << rMin1 << " , " << rMin2 << " max " << rMax1 << " " << rMax2 << std::endl;
       G4PVPlacement *vSub = new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, (aCheckVolumeOverLapWC));	
       size_t izzBegin = static_cast<size_t>(z21p088 + 30 + (iSub*deltaZ));			
       fEffectiveRadiiForFieldMap[izzBegin] = rMax1;
       size_t izzEnd = static_cast<size_t>(z21p088 + 30 + ((iSub+1)*deltaZ));			
       fEffectiveRadiiForFieldMap[izzEnd] = rMax2;
       std::cerr << " fEffectiveRadiiForFieldMap, lengthToNeck, indices " << izzBegin << " / " << izzEnd 
               << " radii " << fEffectiveRadiiForFieldMap[izzBegin] << " / " << fEffectiveRadiiForFieldMap[izzEnd] << std::endl;
			
      if (hornWaterLayerThick	> 0.002*mm) {
       std::ostringstream nameStrStr; nameStrStr << "Horn1ToNeckPartM0SubSect" << iSub << "Water";
       G4String nameStr(nameStrStr.str());
       G4Cons *aCons = new G4Cons(nameStr, rMax1 - hornWaterLayerThick, rMax1-0.001*mm,
                                         rMax2 - hornWaterLayerThick, rMax2-0.001*mm,
	                              (lengthTmp - 0.0075*mm)/2., 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(std::string("Water")), nameStr);
       pCurrent->SetFieldManager(FieldMgrMother,true); //attach the local field to logical volume
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.; posTmp[2] =0.;			      
       new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        vSub->GetLogicalVolume(), false, 1, (aCheckVolumeOverLapWC));	
      }
    } // of the number of subsections in the upstream part of the neck region (zDrawing = 21.0888 inches) 
  }
    // The first weld for this section. 
   if (placeWelds) {
     G4String nameStr("Horn1UpstrSubSect1Weld0");
     G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
     double length = 24.0*mm; //Cover two real sections... 
     posTmp[2] = -1.0*(maxLengthHorn1)/2. + z21p088 + length/2 + zShiftDrawingDownstr; // with respecto the upstr edge of Horn1TopLevelDownstr
     double rTmp1 = fHorn1Equations[5].GetVal(z21p088 - length - 1.0*mm) 
                         + 0.015*mm + hornWaterLayerThick;
        // place it a little more detached..The radius is estimated on the upstream side, biggest radius.
     double rTmp2 = rTmp1 + 1.8*mm; //
     G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, length/2.   , 0., 360.0*deg);
     std::cerr << " Installing the weld   " << nameStr << " at Z " << posTmp[2] << std::endl;
     G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
     pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
     new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
   }
   // 
   // Now the dowstream section. We no longer create the mother volume for it. We will install it directly in 
   // the mother volume in this method.  				
   //
   // Now, the neck. Just a tube 
   //
   {
     G4String nameStr("Horn1Neck");
     const double zNeckDrawing = (30.3150)*in; //start of the neck.. 
     double rTmp1 = (0.709*in/2.); // Drawing 8875.112-MD 363105
     double rTmp2 = (1.063*in/2.) +  + 0.025*mm; 
      // Drawing 8875.112-MD 363105
//     aHorn1NeckOuterRadius = rTmp2; // For use in computing the magnetic field 
// Bug fix, September 2014 : there are no skin depth effect in water!... 
     const double length = 1.5680*in - 0.050*mm; // last term to absord 
        // small shifts in the upstream part.. 
     G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, 
	                           length/2.   , 0., 360.0*deg);
     G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
     pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
     G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
     posTmp[2] = -1.0*(maxLengthHorn1)/2. + zNeckDrawing  + zShiftDrawingDownstr + length/2. + 0.025*mm;
     
     G4PVPlacement* vSub = new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);
     
      size_t izzBegin = static_cast<size_t>(zNeckDrawing + 30.);			
      size_t izzEnd = 839; // to smooth out..			
      for (size_t izzz = izzBegin-1; izzz != izzEnd; izzz++) fEffectiveRadiiForFieldMap[izzz] = rTmp2; // to smooth out.. 
      // There is gap, and the smoothing algoirthm fails otherwise. 		
      std::cerr << " fEffectiveRadiiForFieldMap, Neck, indices " << izzBegin << " / " << izzEnd 
               << " radii " << fEffectiveRadiiForFieldMap[izzBegin] << " / " << fEffectiveRadiiForFieldMap[izzEnd] << std::endl;
     
      		
     if (hornWaterLayerThick > 0.002*mm) {
       G4String nameStrW(nameStr); nameStrW += G4String("Water");
       G4Tubs *aTubsW = new G4Tubs(nameStrW, rTmp2-hornWaterLayerThick-0.012*mm, rTmp2-0.012*mm, 
	                           length/2.   , 0., 360.0*deg);
       G4LogicalVolume *pCurrentW = new G4LogicalVolume(aTubsW, 
                             G4Material::GetMaterial(std::string("Water")), nameStrW);
       pCurrentW->SetFieldManager(FieldMgrMother,true); //attach the local field to logical volume
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.; posTmp[2] =0.;			      
       new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrentW, nameStrW + std::string("_P"), 
                        vSub->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
      }
   }				
    // The downstream part of the real section that has the neck.  
   {
     const double zStartDrawing =  31.8827*in;
     const double zEndDrawing = (41.0776)*in;
     int numSubSect = GetNumberOfInnerHornSubSections(3, zStartDrawing, 
                                                      zEndDrawing, 10); // These Z position are from the start of the inner conductor.   
     if (refinedSegmentation) numSubSect *= 2; // refine it.. 
     if (crudeSegmentation) numSubSect = 1; // There!  Ultimate simplification. 
     std::cerr << " Number of segments from the neck to 41.0776 inches " << numSubSect << std::endl;
     const double deltaZ = (zEndDrawing - zStartDrawing)/numSubSect;
     for (int iSub = 0; iSub != numSubSect; iSub++) {					      
       const double zzBegin = zStartDrawing + iSub*deltaZ;
       const double zzEnd = zzBegin + deltaZ;
       std::ostringstream nameStrStr; nameStrStr << "Horn1DownstrPart1SubSect" << iSub;
       G4String nameStr(nameStrStr.str());
       double rMin1 = fHorn1Equations[3].GetVal(zzBegin); 
       double rMin2 = fHorn1Equations[3].GetVal(zzEnd);
       double rMax1 = fHorn1Equations[7].GetVal(zzBegin) + hornWaterLayerThick + 0.0025; 
                 // Equation 6 (Drawing 8875.112-MD 363104)
       double rMax2 = fHorn1Equations[7].GetVal(zzEnd) + hornWaterLayerThick + 0.0025;     
       G4Cons *aCons = new G4Cons(nameStr, rMin1, rMax1,rMin2, rMax2,
	                              (deltaZ - 0.005*mm)/2., 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
       pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
       posTmp[2] = -1.0*maxLengthHorn1/2. + zzBegin   + zShiftDrawingDownstr + deltaZ/2.;
       std::cerr << " Placing " << nameStr << " at z " << posTmp[2] << std::endl;			      
       G4PVPlacement *vSub = new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, (aCheckVolumeOverLapWC));	
			
       size_t izzBegin = static_cast<size_t>(zStartDrawing + 30 + (iSub*deltaZ));			
       fEffectiveRadiiForFieldMap[izzBegin] = rMax1;
       size_t izzEnd = static_cast<size_t>(zStartDrawing + 30 + ((iSub+1)*deltaZ));			
       fEffectiveRadiiForFieldMap[izzEnd] = rMax2;
       std::cerr << " fEffectiveRadiiForFieldMap, DwnToNeck, indices " << izzBegin << " / " << izzEnd 
               << " radii " << fEffectiveRadiiForFieldMap[izzBegin] << " / " << fEffectiveRadiiForFieldMap[izzEnd] << std::endl;
			
      if (hornWaterLayerThick > 0.002*mm) {
       std::ostringstream nameStrStr; nameStrStr << "Horn1DownstrPart1SubSect" << iSub << "Water";
       G4String nameStr(nameStrStr.str());
       G4Cons *aCons = new G4Cons(nameStr, rMax1 - hornWaterLayerThick, rMax1-0.001*mm,
                                         rMax2 - hornWaterLayerThick, rMax2-0.001*mm,
	                              (deltaZ - 0.0075*mm)/2., 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(std::string("Water")), nameStr);
       pCurrent->SetFieldManager(FieldMgrMother,true); //attach the local field to logical volume
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.; posTmp[2] =0.;			      
       new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        vSub->GetLogicalVolume(), false, 1, (aCheckVolumeOverLapWC));	
      }
    } // of the number of subsection to the neck
    // The weld at the end  
   if (placeWelds) {
     G4String nameStr("Horn1DownstrPart1Weld1");
     const double zWW = (41.0776)*in;; 
      const double length = 24.0*mm; //Cover two real sections... 
     const double rTmp1 = fHorn1Equations[7].GetVal(zWW + length) + 0.150*mm + hornWaterLayerThick;
        // place it a little more detached..Also, the weld is on top of the layer of water.. Oh well.. 
      const double rTmp2 = rTmp1 + 1.8*mm; // 
     G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, 
	                           length/2.   , 0., 360.0*deg);
     G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
     pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
     G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
     posTmp[2] =  -1.0*(maxLengthHorn1)/2. + zWW  + zShiftDrawingDownstr + length/2.;			      
     new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
   }
  } // The downstream part horn1, starting downstream of the real section that has the neck
  // More Inner conductors, covering the drawings 8875.112-MD 363105 through 363109 included.
  // Radial equation 5 and 8 (indices 4 and 7 in our arrays) 
  // From ZDrawing 41.0576 to 117.126
  {
     const double zStartDrawing =  41.108*in;
     const double zEndDrawing = 117.126*in;
     int numSubSect = GetNumberOfInnerHornSubSections(4, zStartDrawing, 
                                                      zEndDrawing, 10); // These Z position are from the start of the inner conductor.   
     if (crudeSegmentation) numSubSect = 1; // There!  Ultimate simplification. 
     if (refinedSegmentation) numSubSect *= 2; // refine it.. 
     const double deltaZ = (zEndDrawing - zStartDrawing)/numSubSect;
//     std::cerr << " Number of subsection for the downstream half of Horn1 " << numSubSect 
//                 << " deltaz " << deltaZ << std::endl;
     std::cerr << " Number of segments from 41.0776 inches to the end " << numSubSect << std::endl;
     for (int iSub = 0; iSub != numSubSect; iSub++) {					      
       const double zzBegin = zStartDrawing + iSub*deltaZ;
       const double zzEnd = zzBegin + deltaZ;
       std::ostringstream nameStrStr; nameStrStr << "Horn1DownstrPart1SubSect" << iSub;
       G4String nameStr(nameStrStr.str());
       const double rMin1 = fHorn1Equations[4].GetVal(zzBegin); // Equation 1
       const double rMin2 = fHorn1Equations[4].GetVal(zzEnd);
       const double rMax1 = fHorn1Equations[7].GetVal(zzBegin) + hornWaterLayerThick + 0.0025; 
          // Equation 6 (Drawing 8875.112-MD 363104)
       const double rMax2 = fHorn1Equations[7].GetVal(zzEnd) + hornWaterLayerThick + 0.0025;     
       G4Cons *aCons = new G4Cons(nameStr, rMin1, rMax1,rMin2, rMax2,
	                              (deltaZ - 0.005*mm)/2., 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
       pCurrent->SetFieldManager(FieldMgrIC,true); //attach the local field to logical volume
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
       posTmp[2] = -1.0*(maxLengthHorn1)/2. + zzBegin  + zShiftDrawingDownstr + deltaZ/2.;			      
       G4PVPlacement *vSub = new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, (aCheckVolumeOverLapWC));	
			
       size_t izzBegin = static_cast<size_t>(zStartDrawing + 30 + (iSub*deltaZ));			
       fEffectiveRadiiForFieldMap[izzBegin] = rMax1;
       size_t izzEnd = static_cast<size_t>(zStartDrawing + 30 + ((iSub+1)*deltaZ));			
       fEffectiveRadiiForFieldMap[izzEnd] = rMax2;
       std::cerr << " fEffectiveRadiiForFieldMap Dwnstr.., indices " << izzBegin << " / " << izzEnd 
               << " radii " << fEffectiveRadiiForFieldMap[izzBegin] << " / " << fEffectiveRadiiForFieldMap[izzEnd] << std::endl;
      if (hornWaterLayerThick > 0.002*mm) {
       std::ostringstream nameStrStr; nameStrStr << "Horn1DownstrPart1SubSect" << iSub << "Water";
       G4String nameStr(nameStrStr.str());
       G4Cons *aCons = new G4Cons(nameStr, rMax1 - hornWaterLayerThick, rMax1-0.001*mm,
                                         rMax2 - hornWaterLayerThick, rMax2-0.001*mm,
	                              (deltaZ - 0.0075*mm)/2., 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aCons, G4Material::GetMaterial(std::string("Water")), nameStr);
       pCurrent->SetFieldManager(FieldMgrMother,true); //attach the local field to logical volume
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.; posTmp[2] =0.;			      
       new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        vSub->GetLogicalVolume(), false, 1, (aCheckVolumeOverLapWC));	
      }
    } // of the number of subsection to the neck 
   {
     // Now the Hangers. (two of them.. ) 
     if (placeHangers) {
       G4String nameStrFirstHanger("Horn1DownstrSecondSpiderHanger");
       double zLocDrawing = zStartDrawing + 1.416*in;
       double zLocPosM =  -1.0*(maxLengthHorn1)/2. + zLocDrawing + zShiftDrawingDownstr + 0.375*in; // with respect to the center of 
       // of the mother volume. 
       this->Horn1InstallSpiderHanger( nameStrFirstHanger, zLocDrawing, zLocPosM, aHorn1OuterTubeInnerRad, motherTop); 
       
       G4String nameStrSecondHanger("Horn1DownstrThirdSpiderHanger");
       zLocDrawing = (80.9951 + 1.791)*in;
       zLocPosM =   -1.0*(maxLengthHorn1)/2. + zLocDrawing + zShiftDrawingDownstr + 0.375*in;
       this->Horn1InstallSpiderHanger( nameStrSecondHanger, zLocDrawing, zLocPosM, aHorn1OuterTubeInnerRad, motherTop); 
     }
     // now a few welds.. 
     std::vector<double> zLocWelds(4,0.); // Drawing coordinate system
     zLocWelds[0] = 61.0464*in;
     zLocWelds[1] = 81.0151*in;
     zLocWelds[2] = 100.9839*in;
     zLocWelds[3] = 116.5*in; // Cheat a bit, place it upstream to make sure it does not overlap with the end
     // April 2 .. Zeongtae notice this cheat, it manifest itself as a visible gap of a bout 1/2 inch in Z 
     // Let us fix this by re-adjusting the position of this weld, which, after checking the end and 
     // beginning of the sub section number 5 and flange below, we now have: 
     zLocWelds[3] = 117.1126*in - 12.0*mm; 
     // final adjustment to avoid collision with the flange.. Wehereby substract 1/2 of the length., + 
     if (placeWelds) {
       for (size_t iW=0; iW !=zLocWelds.size(); iW++) { 
         std::ostringstream nameStrStr; nameStrStr << "Horn1DownstrPart1Weld" << iW+2;
         G4String nameStr(nameStrStr.str());
         const double length = 24.0*mm; //Cover two real sections... 
         const double zW = zLocWelds[iW];   
         double rTmp1 = fHorn1Equations[7].GetVal(zW + length) + 0.015*mm + hornWaterLayerThick;
         // To make it a bit nice on fancy plots. 
         if (iW == 3) rTmp1 += 0.150*mm; // To be safe at the downstream end. 
         const double rTmp2 = rTmp1 + 1.8*mm; // 
         G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, 
	                           length/2.   , 0., 360.0*deg);
         std::cerr << " Horn1, Weld " << iW  << " volume name " << nameStr << " rTmp1 " 
                 << rTmp1 << " rTmp2 " << rTmp2 << std::endl;			   
         G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
         G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
         posTmp[2] =  -1.0*(maxLengthHorn1)/2. + zW + zShiftDrawingDownstr + length/2.;			      
         new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
       }
     }
   }
    {
   //Flange for the Inner Downstream, drawing 8875-112 363096 .. Two tubes
       // Upstream part 
       G4String nameStr("Horn1InnerDownstrFlangePart0");
       const double rTmp1 = (7.750*in/2.0);
       const double rTmp2 = (8.50*in/2.0);; // 
       const double length = (12.244 - 1.10)*in - 12.5*mm; 
       // Subtract the 1/2 the length weld to avoid collision with the Horn1DownstrPart1Weld
       G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, length/2.   , 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
       const double zDrawing = (117.1126*in) + 12.5*mm; // small shift to handle collisions 
       posTmp[2] = -1.0*(maxLengthHorn1)/2. + zDrawing + zShiftDrawingDownstr + length/2.;			      
       new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
      }
     {
   //Flange per-se drawing 8875-112 363096 ..
       G4String nameStr("Horn1InnerDownstrFlangePart1");
       const double rTmp1 = 7.750*in/2.0 + 1.0*mm;
       const double rTmp2 = 11.271*in/2.0 + 1.0*mm; // 
       const double length = (1.25)*in; // Add a bit for the connectors.
       G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, length/2.   , 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(aHorn1InnerCondMat), nameStr);
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
       const double zDrawing = 117.1126*in + ((12.244 - 1.10)*in + 0.5*mm); 
       posTmp[2] =  -1.0*(maxLengthHorn1)/2. + zDrawing + zShiftDrawingDownstr + length/2.;			      
       new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
      }
   
   
  }  // The downstream part of the real section that has the neck. 
  
  // The outer flange (downstream connector and bulk heads) Just a relatively thick tube. 
   {
       G4String nameStr("Horn1OuterTubeDowsntreamFlanges");
       const double rTmp1 = aHorn1OuterTubeOuterRad + 2.0*mm;
       const double rTmp2 = 23.5*in/2.; // 
       const double length = 3.0*in; 
       const double zDrawing =  (117.1126 + 6.0)*in; // 6" is still aproximate
       G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, length/2.   , 0., 360.0*deg);
       G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(std::string("Aluminum")), nameStr);
       G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.; 
       posTmp[2] =-1.0*(maxLengthHorn1)/2. + zDrawing + zShiftDrawingDownstr + length/2.;	;
       new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
                        motherTop->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);	
  }
 
  // The strip lines connector plates, optionally.  
   if (fHorn1StripLinesThick > 0.) {
       G4String nameStr("Horn1StripLineConnection");// include Inner and Outer.. 
       const double rTmp1 = (7.75/2)*in; // Drawing MD-363096
       const double rTmp2 = 13.0*in; // Approximate.. 
       const double length = fHorn1StripLinesThick; 
       const double zDrawing =  theEndOfHorn1wrtTGAR + 5*CLHEP::mm; // Approximate.. 
       for (int kPhi = 0; kPhi != 4; kPhi++) {
         G4String nameStrPhi(nameStr); 
	 std::ostringstream strstrPhi; strstrPhi << "Phi" << static_cast<int>(45+kPhi*90);
	 nameStrPhi += strstrPhi.str();
         G4Tubs *aTubs = new G4Tubs(nameStrPhi, rTmp1, rTmp2, length/2. , (33.75 + 90.0*kPhi)*deg, 22.5*deg);
         G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial(std::string("Aluminum")), nameStrPhi);
         G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.; 
         posTmp[2] = zDrawing + 0.5*length;
         new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStrPhi + std::string("_P"), 
                        TGAR->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);
      }	
  }
 
  // 
  // We now smooth out the fEffectiveRadiiForFieldMap
  //
  fEffectiveRadiiForFieldMap[fEffectiveRadiiForFieldMap.size() -1] = 4.25*25.4; // drawing 363093
  for (size_t k=0; k !=  fEffectiveRadiiForFieldMap.size(); k++) {
    if (fEffectiveRadiiForFieldMap[k] < 0.) {
      size_t kk = k+1;
      while ((fEffectiveRadiiForFieldMap[kk] < 0.) && (kk < fEffectiveRadiiForFieldMap.size())) kk++;
      const double rLow = fEffectiveRadiiForFieldMap[k-1];
      const double rHigh = fEffectiveRadiiForFieldMap[kk];
      const double deltazkk = static_cast<double>( kk - k + 1); 
      for (size_t kkk=k; kkk !=  kk; kkk++) 
        fEffectiveRadiiForFieldMap[kkk] = rLow + (rHigh - rLow)*(kkk - k)/deltazkk;
    }
  }
  // And load it up on the field map 
  //
  numiMagField->SetEffectiveRadiiForFieldMap(fEffectiveRadiiForFieldMap);
  
//  std::ofstream fOutCheckEffectiveRadii ("./Horn1CheckEffectiveRadii.txt");
////  fOutCheckEffectiveRadii  << " iz rr "<< std::endl;
//  for (size_t k=0; k !=  fEffectiveRadiiForFieldMap.size(); k++) 
//    fOutCheckEffectiveRadii << " " << k << " " << fEffectiveRadiiForFieldMap[k] << std::endl;
//  fOutCheckEffectiveRadii.close();
}
//
// Subdivide a conical section such that the radial equations are correct within a 
// a tolerance of 5 microns. We assume that the number of subdivisions
// estimated at the upstream end of the section  will 
// be adequate over the entire section. 
//
int NumiDetectorConstruction::GetNumberOfInnerHornSubSections(size_t eqn, double z1, double z2, int nMax) const {

   int numSubSect = 1;
   double zEnd = z2;
   const double zLengthTotal = z2 - z1;
   double zLength = zLengthTotal;
   while (true) { 
     const double r0 = fHorn1Equations[eqn].GetVal(z1);
     const double r1 = fHorn1Equations[eqn].GetVal(zEnd);
     const double rMid = fHorn1Equations[eqn].GetVal((z1 + zEnd)/2.);
//     std::cerr << " GetNumberOfInnerHornSubSections z1 " << z1 << " zEnd " << zEnd << 
//                     " r0 " << r0 << " r1 " << r1 << " rMid " << rMid << std::endl;
     if (std::abs(rMid - 0.5*(r0+r1)) < 0.050*mm) break; // 50 microns good enough for Gov. work. 
                                                         // meachanical tolerance is one 5 mills, (127 microns)
     zLength /=2.;
     zEnd = z1 + zLength;
     numSubSect++;
     if (numSubSect > nMax) {
       G4Exception("NumiDetectorConstruction::GetNumberOfInnerHornSubSections", " ", FatalErrorInArgument, " Crazy subdivision! ");
       break;
     }
   }
   return numSubSect;
}
//
// 2nd argument is in drawing coordinate system, the upstream coordinate of the beginning of the ring
//  Third argument, in the reference frame of the mother volume, the long. center position 
//
void NumiDetectorConstruction::Horn1InstallSpiderHanger(const G4String &nameStrH, 
                                                    double zLocTweakedDC, double zPosMotherVolume, double outerRad, 
						   G4PVPlacement *vMother ) {


  NumiDataInput* ND=NumiDataInput::GetNumiDataInput();
  const bool aCheckVolumeOverLapWC = true;
  const double aWaterLayerThickInHorns = ND->GetHornWaterLayerThick(); // July 16-19 2014, P.L.
  const double in = 2.54*cm;
  G4String nameStr(nameStrH);nameStr += G4String("Ring");
  const double length = 0.750*in;
  const int eqnNum = (zLocTweakedDC < (41.076*in)) ? 5 : 7;
  const double zSignLength = (eqnNum == 5) ? -1.0 : 1.0; // to avoid collision with the inner conductor outer radius 
                                                         // at the upstream or downstream end .. Remnant from the LBNE split Horn1.. but harmless.
  double rTmp1 = fHorn1Equations[eqnNum].GetVal(zLocTweakedDC + length*zSignLength) 
                          + 0.0015*mm + aWaterLayerThickInHorns;

  const double rTmp2 = rTmp1 + 0.24*in; // Deduced from 363104 and equation 6
  G4Tubs *aTubs = new G4Tubs(nameStr, rTmp1, rTmp2, 
     				length/2.   , 0., 360.0*deg);
  G4LogicalVolume *pCurrent = new G4LogicalVolume(aTubs, G4Material::GetMaterial("Aluminum"), nameStr);
  G4ThreeVector posTmp; posTmp[0] = 0.; posTmp[1] = 0.;
  posTmp[2] = zPosMotherVolume;  			   
  new G4PVPlacement((G4RotationMatrix *) 0, posTmp, pCurrent, nameStr + std::string("_P"), 
  		     vMother->GetLogicalVolume(), false, 1, aCheckVolumeOverLapWC);    
  
  //The connecting piece ring to the hangers.. There are three of them, at 120 degrees from each other. 
  
  G4String nameStr2(nameStrH); nameStr2 += G4String("Riser");
  const double heightRiser = 0.270*in - 0.020*mm; // Slight correction in height, P.L., November 21 2014. 
  const double widthH = 1.5*in; // See drawing 8875.112-MD 363115
  const double thickH = 0.184*2*in; 
  G4Box *aBoxRiser = new G4Box(nameStr2, widthH/2., heightRiser/2.0, thickH/2.0);  
  G4LogicalVolume *pCurrentRiser = 
    new G4LogicalVolume(aBoxRiser, G4Material::GetMaterial("Aluminum"), nameStr2);
    
  G4String nameStr3(nameStrH); nameStr3 += G4String("Hanger");
  const double heightH = outerRad - rTmp2 - 1.0*mm - heightRiser;
  const double widthH2 = 1.0*in; // 363115 Note: we collapsed both hanger along the horizontal, transverse 
                                // direction. 
  const double thickH2 = 0.031*in; // Average over transverse phi.. 
  G4Box *aBoxHanger = new G4Box(nameStr3, widthH2/2., heightH/2.0, thickH2/2.0);  
  G4LogicalVolume *pCurrentHanger = 
    new G4LogicalVolume(aBoxHanger, G4Material::GetMaterial("Aluminum"), nameStr3);
  
  for (int iRot=0; iRot != 3; iRot++) {
    std::ostringstream rnStrStr; rnStrStr << "_" << (iRot+1);
    G4String rnStr(rnStrStr.str());
  // Same Z position as above... 
    G4RotationMatrix * rMatPr = 0;
     if (iRot == 1) {
      rMatPr = new G4RotationMatrix; 
      rMatPr->rotateZ(2.0*M_PI/3.);
    } else if (iRot == 2) {
      rMatPr = new G4RotationMatrix; 
      rMatPr->rotateZ(-2.0*M_PI/3.);
    }
    
    const double dHRiser = rTmp2 + 0.010*mm + heightRiser/2.;
    posTmp[0] = 0.;  posTmp[1] = dHRiser;
    if (iRot != 0) {
      posTmp[0] = dHRiser*rMatPr->xy();
      posTmp[1] = dHRiser*rMatPr->yy();
    }
    if (iRot == 0) { 
       new G4PVPlacement(rMatPr, posTmp, pCurrentRiser, nameStr2 + std::string("_P") + rnStr, 
  		     vMother->GetLogicalVolume(), false, iRot, aCheckVolumeOverLapWC);
    } else {
       new G4PVPlacement(G4Transform3D(*rMatPr, posTmp), pCurrentRiser, nameStr2 + std::string("_P") + rnStr, 
  		     vMother->GetLogicalVolume(), false, iRot, aCheckVolumeOverLapWC);
    }
// Now the hanger it self 
    
    const double dHHanger = rTmp2 + 0.010*mm + 0.5*mm + heightRiser + heightH/2.;
    posTmp[0] = 0.;  posTmp[1] = dHHanger;
    if (iRot != 0) {
      posTmp[0] = dHHanger*rMatPr->xy();
      posTmp[1] = dHHanger*rMatPr->yy();
    }
  // Same Z position as above... 
    if (iRot == 0) { 
       new G4PVPlacement(rMatPr, posTmp, pCurrentHanger, nameStr3 + std::string("_P") + rnStr, 
  		     vMother->GetLogicalVolume(), false, iRot, aCheckVolumeOverLapWC);    
     } else {
       new G4PVPlacement(G4Transform3D(*rMatPr, posTmp), pCurrentHanger, nameStr3 + std::string("_P") + rnStr, 
  		     vMother->GetLogicalVolume(), false, iRot, aCheckVolumeOverLapWC);
     } 
 } // on the 120 degree symmetry point.
}
double LBNEHornRadialEquation::inchDef = 2.54*cm;

LBNEHornRadialEquation::LBNEHornRadialEquation(double rc, double zc, double rOff, bool isParabolic):
parabolic(isParabolic),
rCoeff(rc),
zCoeff(zc),
rOff(rOff),
rResc(1.0),
zResc(1.0)
{ ; } 

double LBNEHornRadialEquation::GetVal(double z) const {
  // z = 0 above is by arbitrary choice the Z coordinate of the starting point of the Horn1TopLevelUpstr logical volume
  // We shift the drawing coordinate system
 // const double zD = z + 3.2852*inchDef*zResc; // Only valid for Horn1 !!!!! 
// By definition, drawing 8875.000-ME-363028 (Z=0 is ACTRNT)
 
 const double zR = z/inchDef; // back in inches. 
 const double argR = rCoeff + zR*zCoeff/zResc;
 if (argR < 0.) {
    std::ostringstream mStrStr; mStrStr << " Negative argument, z = " << z 
                                        << " argR " << argR << " zResc " << zResc << std::endl; 
    G4String mStr(mStrStr.str());
    G4Exception("LBNEHornRadialEquation::GetVal", " ", FatalErrorInArgument, mStr.c_str());
  } 
 const double radius = parabolic ? (std::sqrt(argR) + rOff) : argR;
// return radius*zResc*inchDef; v3r0p10
 return radius*rResc*inchDef;
}

void LBNEHornRadialEquation::test1() const {

  //Test at the end of first section of Horn1 (8875.112-MD 363104) 
  const double argZ = (3.2645 + 17.876 - 0.031);
  const double rTest = this->GetVal(argZ*25.4*mm*zResc);
  std::cerr << " LBNEHornRadialEquation::test1, argZ " << argZ << "  rTest (mm) " << rTest <<  std::endl;
  std::cerr << " inchDef " << inchDef << " zCoeff " << zCoeff << " rCoeff " << rCoeff << " rOff " << rOff << std::endl;
  const double delta = 2.0*rTest - 1.6326*25.4*mm*rResc; // Actually, the drawing says 1.6 !!! mistake of 700 microns. 
  std::cerr << " delta (mm) " << delta << " zResc " << zResc << " rResc " << rResc <<  std::endl;
  if (std::abs(delta) > 0.127*mm) { // 5 mill tolerance 
    G4Exception("LBNEHornRadialEquation::test1", " ", FatalErrorInArgument,
                " Horn1 Equation 0 inconsistent with drawing 363104"); 
  } 
}
