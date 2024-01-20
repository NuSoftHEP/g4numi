#include "NumiDetectorConstruction.hh"

#include "G4Material.hh"
#include "NumiDataInput.hh"
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
#include "G4UserLimits.hh"
static const G4double in=2.54*cm;

void NumiDetectorConstruction::ConstructHorn2(G4ThreeVector hornpos, G4RotationMatrix hornrot)
{
  //see NumiHorn1.cc for material changes etc details
  G4ThreeVector translation;
  G4RotationMatrix rotation;
  NumiDataInput* ND=NumiDataInput::GetNumiDataInput();
  const double hornWaterLayerThick = ND->GetHornWaterLayerThick(); // July 16-19 2014, P.L.
     if ( hornWaterLayerThick > 10.) {
     std::ostringstream messageOStr; messageOStr << " Unreasonable amount of water " << hornWaterLayerThick;
     std::string message(messageOStr.str());
#ifndef MODERN_G4
     G4Exception("NumiDetectorConstruction::ConstructLHorn2");
#else
     G4Exception("numiHorn1","NumiHorn1",FatalException,"NumiDetectorConstruction::ConstructHorn2");
#endif
     exit(2); // only under 4.9.2 .. not reachable under 4.9.5 
   }
   const bool placeWaterLayer = (hornWaterLayerThick > 0.001*mm);

  // subtract TargetHallPosition to get origin at the face of Horn2
  G4ThreeVector TargetHallPosition=G4ThreeVector(0,0,NumiData->TargetAreaLength/2.+NumiData->TargetAreaZ0);
  //G4double MHorn2Length=133.966*2.54*cm+3.938*in+1.75*in+2.5*in;
  G4double Horn2Z0=-1.889*in;// HornZ0 in Horn coordinate system
  //G4double Horn2Z1=
  G4double OCZ0=0.295*in;
  G4double OCZ1=135.861*in;
  G4double ICZ0=0.295*in;
  G4double ICZ1=139.22*in;
  G4double frontRmin=1.859*in; // Drawing ME - 363385 says this is 1.869 inches... 
  G4double frontRmax=2.184*in;
  G4double frontRtor=12.756*in;
  G4double MVgap=0.1*mm;
  G4double Fgap=0.5*mm;
  G4ThreeVector MHorn2Origin=G4ThreeVector(0,0,0);
  G4double FZ0=0.*in;
  G4double FZ1=137.96*in-Fgap;
  G4double epsilon=.0000001*mm;
  G4double maxDev=0.5*mm;
  
  G4int nPoints=G4int((ND->PHorn2EndZ0[ND->NPHorn2EndN-1]+ND->PHorn2EndLength[ND->NPHorn2EndN-1]+frontRmax)/(.05*mm));
  G4double deltaZ=(ND->PHorn2EndZ0[ND->NPHorn2EndN-1]+frontRmax)/nPoints;
  
  G4int nOut(0),nIn(0),nMV(0),nF(0);
  G4double zPos(0);
  vdouble_t OCzPos,OCRout,OCRin, ICzPos,ICRout,ICRin;
  vdouble_t FzPos, FRin, FRout, MVzPos,MVRout,MVRin;
  vdouble_t FRinW, FRoutW;
  
  OCzPos.push_back(OCZ0); OCRin.push_back(PHorn2OCRin(OCzPos[0])); OCRout.push_back(PHorn2OCRout(OCzPos[0]));
  ICzPos.push_back(ICZ0); ICRin.push_back(PHorn2ICRin(ICzPos[0])); ICRout.push_back(PHorn2ICRout(ICzPos[0]));
  FzPos.push_back(ICZ0) ; FRin.push_back(PHorn2ICRout(FzPos[0])+Fgap) ; FRout.push_back(PHorn2OCRin(FzPos[0])-Fgap);
  MVzPos.push_back(Horn2Z0-MVgap); MVRin.push_back(PHorn2ICRin(MVzPos[0])-MVgap); MVRout.push_back(PHorn2OCRout(MVzPos[0])+MVgap);

  G4UserLimits *MyLimits = new G4UserLimits();
  if(ND->raytracing){
    MyLimits->SetMaxAllowedStep(1*cm);
  }
  //
  // Imperfect Current equalizer section support. 
  // To generate the RZ map, one needs a the accurate position of the IC, + thickness. 
  // Define a simple array.. 
  fEffectiveRadiiForFieldMapH2.clear();
  // fixed step size of 1 mm, over the entire length of the Horn1.
  // Coordinate system is local to H2, Z0 = Iz0 = 0.
   
  for (size_t k=0; k!= static_cast<size_t>(142.91*in); k++) fEffectiveRadiiForFieldMapH2.push_back(-1.);
  fEffectiveRadiiForFieldMapH2[0] = 10.8870*in; // Drawing 363385 
  
  
  G4double lastICzPos=ICzPos[0];
  G4double maxR,minR,endZ;
  maxR=ND->PHorn2EndRout[0]; minR=ND->PHorn2EndRin[0]; endZ=ND->PHorn2EndZ0[0]+ND->PHorn2EndLength[0];
  for (G4int ii=1;ii<ND->NPHorn2EndN;ii++){
    if (maxR<ND->PHorn2EndRout[ii]) maxR=ND->PHorn2EndRout[ii];
    if (minR<ND->PHorn2EndRin[ii]) minR=ND->PHorn2EndRin[ii];
    if (endZ<ND->PHorn2EndZ0[ii]+ND->PHorn2EndLength[ii]) endZ=ND->PHorn2EndZ0[ii]+ND->PHorn2EndLength[ii];
  }
  for (G4int ii=1;ii<nPoints;ii++){
    zPos=Horn2Z0+deltaZ*ii;
    if ((fabs(PHorn2OCRout(zPos)-(PHorn2OCRout(zPos-deltaZ)+PHorn2OCRout(zPos+deltaZ))/2.)>maxDev)||
	(fabs(PHorn2OCRin(zPos)-(PHorn2OCRin(zPos-deltaZ)+PHorn2OCRin(zPos+deltaZ))/2.)>maxDev)||
	((fabs(PHorn2OCRout(zPos)-PHorn2OCRout(zPos-deltaZ))<epsilon)&&(fabs(PHorn2OCRout(zPos)-PHorn2OCRout(zPos+deltaZ))>epsilon))||
	((fabs(PHorn2OCRout(zPos)-PHorn2OCRout(zPos-deltaZ))>epsilon)&&(fabs(PHorn2OCRout(zPos)-PHorn2OCRout(zPos+deltaZ))<epsilon)&&(zPos>OCZ0&&zPos<OCZ1))||
	((fabs(PHorn2OCRin(zPos)-PHorn2OCRin(zPos-deltaZ))<epsilon)&&(fabs(PHorn2OCRin(zPos)-PHorn2OCRin(zPos+deltaZ))>epsilon))||
	((fabs(PHorn2OCRin(zPos)-PHorn2OCRin(zPos-deltaZ))>epsilon)&&(fabs(PHorn2OCRin(zPos)-PHorn2OCRin(zPos+deltaZ))<epsilon)&&(zPos>ICZ0)))  
      { 
	if (zPos>OCZ0&&zPos<OCZ1){
	  nOut++;
	  OCzPos.push_back(zPos); OCRout.push_back(PHorn2OCRout(zPos)); OCRin.push_back(PHorn2OCRin(zPos));
	  if (zPos>=FZ0&&zPos<FZ1){
	    nF++;
	    FzPos.push_back(zPos); FRin.push_back(PHorn2ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn2OCRin(FzPos[nF])-Fgap);}
	  nIn++;
	  ICzPos.push_back(zPos); ICRout.push_back(PHorn2ICRout(zPos)); ICRin.push_back(PHorn2ICRin(zPos));
	  nMV++;
	  MVzPos.push_back(zPos); MVRout.push_back(PHorn2OCRout(zPos)+MVgap);	MVRin.push_back(PHorn2ICRin(zPos)-MVgap);
	}
	else if (zPos>=OCZ1){
	  if (zPos>=FZ0&&zPos<FZ1){
	    nF++;
	    FzPos.push_back(zPos); FRin.push_back(PHorn2ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn2OCRin(FzPos[nF])-Fgap);}
	  nMV++;	  
	  MVzPos.push_back(zPos); 
	  if (maxR>=PHorn2OCRout(zPos)) MVRout.push_back(maxR+MVgap);
	  else MVRout.push_back(PHorn2OCRout(zPos)+MVgap);
	  if (minR<=PHorn2ICRin(zPos)) MVRin.push_back(minR-MVgap);
	  else 	MVRin.push_back(PHorn2ICRin(zPos)-MVgap);
	}
      }
    if ((fabs(PHorn2ICRout((lastICzPos+zPos)/2.)-(PHorn2ICRout(lastICzPos)+PHorn2ICRout(zPos))/2.)>maxDev)||
	((fabs(PHorn2ICRin((lastICzPos+zPos)/2.)-(PHorn2ICRin(lastICzPos)+PHorn2ICRin(zPos))/2.)>maxDev))||
	((fabs(PHorn2ICRout(zPos)-PHorn2ICRout(zPos-deltaZ))<epsilon)&&(fabs(PHorn2ICRout(zPos)-PHorn2ICRout(zPos+deltaZ))>epsilon))||
	((fabs(PHorn2ICRout(zPos)-PHorn2ICRout(zPos-deltaZ))>epsilon)&&(fabs(PHorn2ICRout(zPos)-PHorn2ICRout(zPos+deltaZ))<epsilon)))
      {
	lastICzPos=zPos;
	if (zPos>ICZ0&&zPos<=ICZ1){
	  nIn++;
	  ICzPos.push_back(zPos); ICRout.push_back(PHorn2ICRout(zPos)); ICRin.push_back(PHorn2ICRin(zPos));
	  if (zPos>=FZ0&&zPos<FZ1){
	    nF++;
	    FzPos.push_back(zPos); FRin.push_back(PHorn2ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn2OCRin(FzPos[nF])-Fgap);}
	  nMV++;
	  MVzPos.push_back(zPos); MVRout.push_back(PHorn2OCRout(zPos)+MVgap); MVRin.push_back(PHorn2ICRin(zPos)-MVgap);
	}
	else {
	  nMV++;
	  MVzPos.push_back(zPos); MVRout.push_back(PHorn2OCRout(zPos)+MVgap); MVRin.push_back(PHorn2ICRin(zPos)-MVgap);
	}
      }
      // October 2017: adding a simple array of radii fort the CEQ 3D field map. 
      if ((zPos > 1.) && (zPos < 142.91*in))  {
       size_t izzz = static_cast<size_t>(zPos);
       if (izzz <  fEffectiveRadiiForFieldMapH2.size()) {
         fEffectiveRadiiForFieldMapH2[izzz] = PHorn2ICRout(zPos);
	 // This correction is also done for the field map array FRin, see below.. 
         if ((zPos > 1500.) && (PHorn2ICRout(zPos) > 277.533*CLHEP::mm)) 
	    fEffectiveRadiiForFieldMapH2[izzz]  = 277.533*CLHEP::mm; // 277.554 mm is 21.853 in/2. 
       }
//       std::cerr << " Adding Point at z " << zPos 
//                 << " rIn " <<  MVRin[MVRin.size() -1] << " rOut " <<  MVRout[MVRin.size() -1] << std::endl;
      }
  }
  fEffectiveRadiiForFieldMapH2[fEffectiveRadiiForFieldMapH2.size()-1] = 277.7533*CLHEP::mm; // Drawing 363392
  //
  // We finish the mapp.. Linear interpolation. 
  //
  for (size_t k=0; k !=  fEffectiveRadiiForFieldMapH2.size(); k++) {
    if (fEffectiveRadiiForFieldMapH2[k] < 0.) {
      size_t kk = k+1;
      while ((fEffectiveRadiiForFieldMapH2[kk] < 0.) && (kk < fEffectiveRadiiForFieldMapH2.size())) kk++;
      const double rLow = fEffectiveRadiiForFieldMapH2[k-1];
      const double rHigh = fEffectiveRadiiForFieldMapH2[kk];
      const double deltazkk = static_cast<double>( kk - k + 1); 
      for (size_t kkk=k; kkk !=  kk; kkk++) 
        fEffectiveRadiiForFieldMapH2[kkk] = rLow + (rHigh - rLow)*(kkk - k)/deltazkk;
    }
  }
//  std::ofstream fOutCheckEffectiveRadii ("./Horn2CheckEffectiveRadii.txt");
//  fOutCheckEffectiveRadii  << " iz rr "<< std::endl;
//  for (size_t k=0; k !=  fEffectiveRadiiForFieldMapH2.size(); k++) 
//    fOutCheckEffectiveRadii << " " << k << " " << fEffectiveRadiiForFieldMapH2[k] << std::endl;
//  fOutCheckEffectiveRadii.close();
//  std::cerr << " And quit after dumping radii for Horn2 " << std::endl; exit(2);
  // And load it up on the field map 
  //
  numiMagField->SetEffectiveRadiiForFieldMapH2(fEffectiveRadiiForFieldMapH2);
  //
  // We also need the Z offset corresponding to these radii with respect to the global coordinate 
  // system. This is given by Hornpos, plus a shift of Z=0 local H2 coordinate system with respect to 
  // the start if the IC, define here at the junction between the 1/2 torus of the I/O transition 
  // and the linear portion of the I/O transition. On drawing 363385,  
  numiMagField->SetField3DMapCurrentEqualizerZOffsetH2(hornpos[2] + (2.184 - 1.889)*in );
  numiMagField->SetHorn2CurrentEqualizerLongAbsLength(fHorn2CurrentEqualizerLongAbsLength);
  numiMagField->SetHorn2CurrentEqualizerQuadAmpl(fHorn2CurrentEqualizerQuadAmpl); 
  numiMagField->SetHorn2CurrentEqualizerOctAmpl(fHorn2CurrentEqualizerOctAmpl); 

  
  nF++;
  FzPos.push_back(FZ1); FRin.push_back(PHorn2ICRout(FzPos[nF])+Fgap); FRout.push_back(PHorn2OCRin(FzPos[nF])-Fgap);
  nIn++;
  ICzPos.push_back(ICZ1); ICRin.push_back(PHorn2ICRin(ICzPos[nIn]));ICRout.push_back(PHorn2ICRout(ICzPos[nIn]));
  nOut++;
  OCzPos.push_back(OCZ1); OCRin.push_back(PHorn2OCRin(OCzPos[nOut]));OCRout.push_back(PHorn2OCRout(OCzPos[nOut]));
  nMV++;
  MVzPos.push_back(endZ+MVgap);
  if (maxR>=PHorn2OCRout(zPos)) MVRout.push_back(maxR+MVgap);
  else MVRout.push_back(PHorn2OCRout(zPos)+MVgap);
  if (minR<=PHorn2ICRin(zPos)) MVRin.push_back(minR-MVgap);
  else MVRin.push_back(PHorn2ICRin(zPos)-MVgap);
  
  if (placeWaterLayer) { // July 2014, P.L. 
    FRinW.clear(); FRoutW.clear();
    for (size_t i=0; i!= ICRin.size(); i++) {
      FRinW.push_back(ICRout[i] + epsilon); // Not quite enough.. Suspecting precision round off..?? 
      FRoutW.push_back(ICRout[i] + hornWaterLayerThick + epsilon);
    }
//    std::cerr << " check size of inner radii array ... IC " << ICRin.size() << " and FRIn " << FRin.size() << std::endl;
    for (size_t i=0; i!= std::min(ICRin.size(), FRin.size()); i++) {
       FRin[i] = FRoutW[i] + epsilon;
    }
  }

    
  // Create Mother Volume
  G4VSolid* sMHorn2;
//  G4Material* material=Vacuum; 
  G4Material* material=Air; // Air inside Air allows for cracks without changing material budget. 
  // Volume overlap detected, July 2014, minor, < 1 mm 
  // It does not hurt to reduce RIn by one mm .. P.L., July 2014. 
  for (size_t iSect = 0; iSect != static_cast<size_t>(nMV+1); iSect++) {
      MVRin[iSect] -= 1.0*mm;
      if (MVzPos[iSect] > 2500.) MVRin[iSect] -= 2.0*mm;
      if (MVzPos[iSect] > 3000.) MVRin[iSect] -= 3.0*cm; // Again, air in air does not change things, and avoid the volume overlaps. 
  }
// Dump the mother volume info to locate the overlap. 
  std::cerr << " Horn2 Polycone mother volume with " << nMV+1 << " sections " << std::endl;
  for (size_t iSect = 0; iSect != static_cast<size_t>(nMV+1); iSect++) {
    std::cerr << " Section " << iSect << " Zl " << MVzPos[iSect] 
	      << " Rin " << MVRin[iSect] << " R Out " << MVRout[iSect] << std::endl;
  }   
  sMHorn2= new G4Polycone("sMH",0.,360.*deg,nMV+1,&MVzPos[0],&MVRin[0],&MVRout[0]);
  G4LogicalVolume *lvMHorn2 = new G4LogicalVolume(sMHorn2,material,"lvMHorn2",0,0,0);
  G4VisAttributes* invisible=new G4VisAttributes(false);
  lvMHorn2->SetVisAttributes(invisible);
  if(ND->raytracing){
    lvMHorn2->SetUserLimits(MyLimits);
  }
  rotation=hornrot;
  translation=hornpos-TargetHallPosition;
  std::cerr << " NumiDetectorConstruction::ConstructHorn2, hornPos " << hornpos << " trans " 
            << translation << std::endl;
  G4VPhysicalVolume* pvMHorn2 = new G4PVPlacement(G4Transform3D(rotation,translation),"pvMHorn2Mother",lvMHorn2,TGAR,false,0);
  pvMHorn2 -> CheckOverlaps();
      
  //Front part
  G4VSolid* sHorn2Front;
  G4Torus* sFrontTorus=new G4Torus("sFrontTorus",frontRmin,frontRmax,frontRtor,0,360.*deg);
  G4Box* sBox=new G4Box("sBox",frontRtor*2.,frontRtor*2.,frontRmax*2.);
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,frontRmax*2);
  sHorn2Front=new G4SubtractionSolid("sHorn2Front",sFrontTorus,sBox,G4Transform3D(rotation,translation)); //need only half of torus
  material=GetMaterial(ND->hrnmat);
  G4LogicalVolume* lvHorn2Front=new G4LogicalVolume(sHorn2Front,material,"lvHorn2Front",0,0,0);
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=-MHorn2Origin+G4ThreeVector(0.,0.,OCZ0);
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn2Front",lvHorn2Front,pvMHorn2,false,0, true);
    
  //Outer Conductor
  G4Polycone* sPHorn2OC=new G4Polycone("sPHorn2OC",0.,360.*deg,nOut+1,&OCzPos[0],&OCRin[0],&OCRout[0]);
  G4LogicalVolume* lvPHorn2OC=new G4LogicalVolume(sPHorn2OC,GetMaterial(ND->hrnmat),"lvPHorn2OC",0,0,0);
  G4FieldManager* FieldMgr2 = new G4FieldManager(numiMagFieldOC); //create a local field
  FieldMgr2->SetDetectorField(numiMagFieldOC); //set the field 
  FieldMgr2->CreateChordFinder(numiMagFieldOC); //create the objects which calculate the trajectory
  lvPHorn2OC->SetFieldManager(FieldMgr2,true); //attach the local field to logical volume
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0.)-MHorn2Origin;
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn2OC",lvPHorn2OC,pvMHorn2,false,0, true);

  // P.L., October 2017. 
  // Geantino study: the IC radial function not quite right in the downstream part.. Check;
  //
//  std::ofstream fOutHorn2IC("./Horn2ICDims.txt");
//  fOutHorn2IC << " zmm rInmm rOutmm zin rInin rOutin" << std::endl;  
//  for (size_t ko = 0; ko != ICzPos.size(); ko++) 
//    fOutHorn2IC << " " << ICzPos[ko] << " " << ICRin[ko] << " " << ICRout[ko] << 
//                   " " << ICzPos[ko]/in << " " << ICRin[ko]/in << " " << ICRout[ko]/in << std::endl;
//  fOutHorn2IC.close();
  //
  // O.K. Following 363392, Downstream Inner flang, the IC Inner radius is set to 21.103/2 inches. 
  // Correct this.. This is an ugly patch, one should correct the functions below, but which one of them? 
  //  This is a patch on top of a substantial correction. 
  for (size_t ko = 0; ko != ICzPos.size(); ko++) {
    if ((ICzPos[ko] > 3023.) && (ICzPos[ko] < 3450.)) {
      ICRin[ko] = (21.103/2)*in;
      ICRout[ko] = (21.853/2)*in;;
    } else if  (ICzPos[ko] > 3450.) {
      ICRin[ko] = (21.103/2)*in;
      ICRout[ko] = (25.04/2)*in;
    }
    // And a small overlap.. 
    ICzPos[ko] = std::min(ICzPos[ko], 3500.0); 
  } 
//  fOutHorn2IC.open("./Horn2ICDimsCorrected.txt");
//  fOutHorn2IC << " zmm rInmm rOutmm zin rInin rOutin" << std::endl;  
//  for (size_t ko = 0; ko != ICzPos.size(); ko++) 
//    fOutHorn2IC << " " << ICzPos[ko] << " " << ICRin[ko] << " " << ICRout[ko] << 
//                   " " << ICzPos[ko]/in << " " << ICRin[ko]/in << " " << ICRout[ko]/in << std::endl;
//  fOutHorn2IC.close();
  //Inner Conductor
  G4Polycone* sPHorn2IC=new G4Polycone("sPHorn2IC",0.,360.*deg,nIn+1,&ICzPos[0],&ICRin[0],&ICRout[0]);
  G4LogicalVolume* lvPHorn2IC=new G4LogicalVolume(sPHorn2IC,GetMaterial(ND->hrnmat),"lvPHorn2IC",0,0,0);
  G4FieldManager* FieldMgr = new G4FieldManager(numiMagFieldIC); //create a local field		 
  FieldMgr->SetDetectorField(numiMagFieldIC); //set the field 
  FieldMgr->CreateChordFinder(numiMagFieldIC); //create the objects which calculate the trajectory
  lvPHorn2IC->SetFieldManager(FieldMgr,true); //attach the local field to logical volume
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0)-MHorn2Origin;
  new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn2IC",lvPHorn2IC,pvMHorn2,false,0, true);
  // July 2014: Split this in 2 parts. 
  // Water and Air...
  //
  // October 2017: we laso have to patch the water layer. .. Same debatable idea: re-patch.
  //  ..as well as the field region..  
  // 
  //Field Part
  // Again, patch.. 
  const size_t numFieldSegment = 38;
  for (size_t ko = 0; ko != FRin.size(); ko++) FRin[ko] = ICRout[ko] + epsilon;
//  fOutHorn2IC.open("./Horn2ICDimsCorrectedField.txt");
//  fOutHorn2IC << " zmm rInmm rOutmm zin rInin rOutin" << std::endl;  
//  for (size_t ko = 0; ko !=numFieldSegment; ko++) 
//    fOutHorn2IC << " " << ICzPos[ko] << " " << FRin[ko] << " " << FRout[ko] << 
//  		 " " << ICzPos[ko]/in << " " << FRin[ko]/in << " " << FRout[ko]/in << std::endl;
//  fOutHorn2IC.close();
  // We don't 
  G4Polycone* sPConeF=new G4Polycone("sPCone2F",0.,360.*deg,numFieldSegment,&FzPos[0],&FRin[0],&FRout[0]);
  G4Torus* sTorusF=new G4Torus("sTorusF",0.,frontRmin-Fgap,frontRtor,0,360.*deg);
  rotation=G4RotationMatrix(0.,0.,0.); translation =G4ThreeVector(0.,0.,Horn2Z0+frontRmax);
  G4UnionSolid *sPHorn2F=new G4UnionSolid("sPHorn2F",sPConeF,sTorusF,G4Transform3D(rotation,translation));
  //  G4LogicalVolume* lvPHorn2F=new G4LogicalVolume(sPHorn2F,Ar,"lvPHorn2F",0,0,0);
//  G4LogicalVolume* lvPHorn2F=new G4LogicalVolume(sPHorn2F,Air,"lvPHorn2F",0,0,0);
// Revert back to argon, because Jim Hylen stated in his e-mail, July 31 2014, that it is indeed Argon.. 
//
  G4LogicalVolume* lvPHorn2F=new G4LogicalVolume(sPHorn2F, Ar, "lvPHorn2F",0,0,0);
  lvPHorn2F->SetVisAttributes(invisible);
  G4FieldManager* FieldMgr3 = new G4FieldManager(numiMagField); //create a local field      
  FieldMgr3->SetDetectorField(numiMagField); //set the field 
  FieldMgr3->CreateChordFinder(numiMagField); //create the objects which calculate the trajectory 
  lvPHorn2F->SetFieldManager(FieldMgr3,true); //attach the local field to logical volume
  
  rotation=G4RotationMatrix(0.,0.,0.);
  translation=G4ThreeVector(0.,0.,0)-MHorn2Origin;
  G4VPhysicalVolume *pvPHorn2F=new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn2F",lvPHorn2F,pvMHorn2,false,0, true);
  G4LogicalVolume* lvPHorn2ICW = 0;
  // Placed the water layer in the field region defined above.  
  if (placeWaterLayer) {
    for (size_t ko = 0; ko != FRinW.size(); ko++) {
        FRinW[ko] = ICRout[ko] + epsilon;
        FRoutW[ko] = FRinW[ko] + hornWaterLayerThick + epsilon;
    }
    // We stop this water layer a bit upstream of the flange
    G4Polycone* sPHorn2ICW=new G4Polycone("sPHorn2ICWater",0.,360.*deg, 37, &ICzPos[0],&FRinW[0],&FRoutW[0]);
    lvPHorn2ICW=new G4LogicalVolume(sPHorn2ICW, Water,"lvPHorn2ICWater",0,0,0);
    ND->ApplyStepLimits(lvPHorn2ICW); // Limit Step Size
    rotation=G4RotationMatrix(0.,0.,0.);
    translation=G4ThreeVector(0.,0.,0)-MHorn2Origin;
    new G4PVPlacement(G4Transform3D(rotation,translation),"PHorn2ICWater",lvPHorn2ICW, pvPHorn2F,false,0, true);
  }
      

  //Spider support
  for (G4int ii=0;ii<G4int(ND->Horn2SS.size());ii++){
    for (G4int jj=0;jj<ND->NHorn2SpidersPerPlaneN;jj++){
      G4double angle=G4double(360.*deg*jj/ND->NHorn2SpidersPerPlaneN);
      G4double rIn=PHorn2ICRout(ND->Horn2SpiderSupportZ0[ii])+Fgap;//
      G4double rOut=PHorn2OCRin(ND->Horn2SpiderSupportZ0[ii])-Fgap;//In and out radius of mother vol.
      // Avoid a volume overlap P.L. July 2014. 
      if (placeWaterLayer) {
         rIn += 4.0*epsilon + hornWaterLayerThick + 1.25*Fgap; // 
       } else { 
          rIn += 1.25*Fgap; // Fix small overlap.. 
       } 
      std::cerr << " Constructing Spider Support " << jj << " Rin " << rIn << " Fgap " << Fgap << std::endl;
      ConstructSpiderSupport(&(ND->Horn2SS[ii]),angle,ND->Horn2SpiderSupportZ0[ii],rIn,rOut,pvPHorn2F,ii+jj);
    }
  }
  
  //Horn end
  G4VSolid* sPHorn2End;
  for (G4int ii=0;ii<ND->NPHorn2EndN;ii++)
    {
      G4String volName=ND->PHorn2EndVolName[ii];
//      std::cerr << " Horn2 End section " << volName << " RIn " << ND->PHorn2EndRin[ii] << 
//                    " Rout " <<  ND->PHorn2EndRout[ii] << " length " << ND->PHorn2EndLength[ii] << std::endl;
      sPHorn2End=new G4Tubs(volName.append("s"),ND->PHorn2EndRin[ii],ND->PHorn2EndRout[ii],ND->PHorn2EndLength[ii]/2.,0.,360.*deg);
      G4LogicalVolume* lvPHorn2End=new G4LogicalVolume(sPHorn2End,GetMaterial(ND->PHorn2EndGeantMat[ii]),volName.append("lv"),0,0,0);
      rotation=G4RotationMatrix(0.,0.,0.);
      translation=G4ThreeVector(0.,0.,ND->PHorn2EndZ0[ii]+ND->PHorn2EndLength[ii]/2.)-MHorn2Origin;
      new G4PVPlacement(G4Transform3D(rotation,translation),volName,lvPHorn2End,pvMHorn2,false,0, true);
    }


  G4cout << "Horn 2 Constructed" << G4endl;
}
G4double NumiDetectorConstruction::PHorn2OCRout(G4double z)
{
  G4double r=0;
   if (z<0.295*in){
     r=14.94*in; // for mother vol.
  }

   //OC dimensions from drawings
   else if ((z>=0.295*in)&&(z<0.97*in)){
    r=14.94*in;
  }
  else if ((z>=0.97*in)&&(z<3.26*in)){
    r=16.75*in;
  }
  else if ((z>=3.26*in)&&(z<125.34*in)){
    r=15.567*in;
  }
  else if ((z>=125.34*in)&&(z<126.212*in)){
    r=(15.567+(z/in-125.34)/(126.212-125.34)*(16.5-15.567))*in;
  }
  else if ((z>=126.212*in)&&(z<133.455*in)){
    r=16.5*in;
  }
  else if ((z>=133.455*in)&&(z<134.955*in)){
    r=18.5*in;
  }
   else if ((z>=134.955*in)&&(z<135.861*in)){
     r=14.41*in;
   }
   
   //for mother vol. purposes
   else if ((z>=135.861*in)&&(z<137.611*in)){
     r=14.405*in; // for mother vol.
   }
   else if ((z>=137.611*in)){
     r=14.469*in; // for mother vol.
   }
   return r;
}
G4double NumiDetectorConstruction::PHorn2OCRin(G4double z)
{
  G4double r=0;
  if (z<0.295*in){
    r=14.615*in; // for mother vol.
  }
  
  //OC dimensions from drawings
  else if ((z>=0.295*in)&&(z<1.97*in)){
    r=14.615*in;
  }
  else if ((z>=1.97*in)&&(z<125.34*in)){
    r=14.567*in;
  }
  else if ((z>=125.34*in)&&(z<126.773*in)){
    r=(14.567+(z/in-125.34)/(126.773-125.34)*(16.-14.567))*in;
  }
  else if ((z>=126.773*in)&&(z<132.111*in)){
    r=16.*in;
  }
  else if ((z>=132.111*in)&&(z<133.922*in)){
    r=(16.-(z/in-132.111)/(133.922-132.111)*(16.-12.719))*in;
  }

  else if ((z>=133.922*in)&&(z<137.611*in)){
    r=12.532*in;
    //r=12.719*in; // for mother vol. & for field vol
  }
  else if ((z>=137.611*in)&&(z<139.486*in)){
    r=12.532*in; // for mother vol. & for field vol
  }
  else if (z>=139.486*in){
    r=12.532*in; // for mother vol. & for field vol
  }
  return r;
}
G4double NumiDetectorConstruction::PHorn2ICRout(G4double z)
{
  G4double r=0.;
  if (z<-0.00331*in){
    r=(10.9108+(0.1780*0.00331))*in; //for MV
  } 

  //IC from gnumi
  else if((z/cm)>=-0.00840   && (z/cm)< 97.617)
  {
    r= (sqrt((100-(z/cm))/0.1351))*cm;
  }
  else if( (z/cm)>=97.617 && (z/cm)<104.803)
  { // Neck
    r = 4.2*cm;
  }
  else if((z/cm)>=104.803 && (z/cm)<118.156*in)
  {
    r= (sqrt(((z/cm)-100)/0.2723))*cm ;
  }
  
  //IC from drawings
  // These are from Zarko's drawings
  /*
  else if ((z>=-0.00331*in)&&(z<5.862*in)){
    r=(10.9108-(0.1780)*(z/in))*in;
  }
  //else if ((z>=5.3874*in)&&(z<5.8620*in)){
  //  r=9.9675*in; //! this is not from eq.
  // }
  //else if ((z>=5.862*in)&&(z<6.3629*in)){
  //  r=(9.9675-(z/in-5.8622)/(6.3629-5.862)*(0.15997))*in; //! this is not from eq.
  //}
  else if ((z>=5.862*in)&&(z<30.3611*in)){
    r=sqrt(114.73006-(2.91414)*(z/in))*in;
  }
  //else if ((z>=29.4686*in)&&(z<30.3611*in)){
  //  r=sqrt(114.73006-(2.91414)*(z/in))*in+1.*mm; //! this is not from eq.
  //}
  else if ((z>=30.3611*in)&&(z<36.8888*in)){
    r=sqrt(114.73006-(2.91414)*(z/in))*in;
  }   
  else if ((z>=36.8888*in)&&(z<38.2521*in)){
    r=(22.68402-(0.54203)*(z/in))*in;
  }
  // I think I got this right
  else if ((z>=38.2521*in)&&(z<39.1092*in)){
    r=(1.7325+(1.8-sqrt(1.8*1.8-sqr(z/in-39.1092))))*in; //! this is not from eq. ! neck
  }
  else if ((z>=39.1092*in)&&(z<40.8688*in)){
    r=1.7325*in; //! this is not from eq. ! neck
  }
   else if ((z>=40.8688*in)&&(z<41.3864*in)){
    r=(1.7325+(1.8-sqrt(1.8*1.8-sqr(z/in-40.8688))))*in; //! this is not from eq. ! neck
  }

  else if ((z>=41.3864*in)&&(z<43.3660*in)){
    r=(-10.63139+(0.30058)*(z/in))*in;
  } 
  else if ((z>=43.3660*in)&&(z<50.2725*in)){
    r=sqrt(-56.922631+(1.44583)*(z/in))*in;
  }

  //else if ((z>=49.3279*in)&&(z<50.2725*in)){
  //  r=sqrt(-56.922631+(1.44583)*(z/in))*in;// joint , need to correct this
  //}

  else if ((z>=50.2725*in)&&(z<69.892*in)){
    r=sqrt(-56.922631+(1.44583)*(z/in))*in;
  }
 
  //else if ((z>=69.3337*in)&&(z<69.8920*in)){
  //  r=sqrt(-56.922631+(1.44583)*(z/in))*in;// joint , need to correct this
  //}
  //else if ((z>=69.8920*in)&&(z<70.3095*in)){
  //  r=(0.12835+(0.0932)*(z/in))*in;// joint , need to correct this
  //}

  else if ((z>=69.8920*in)&&(z<93.892*in)){
    r=(0.12835+(0.0932)*(z/in))*in;
  }

  //else if ((z>=93.3359*in)&&(z<93.8920*in)){
  //  r=(0.12835+(0.0932)*(z/in))*in;// joint , need to correct this
  //}
  //else if ((z>=93.8920*in)&&(z<=94.3159*in)){
  //  r=(1.93227+(0.07398)*(z/in))*in;// joint , need to correct this
  //}

  else if ((z>=93.892*in)&&(z<=118.156*in)){
    r=(1.93227+(0.07398)*(z/in))*in;
  }
  */
  //else if ((z>=117.6060*in)&&(z<118.156*in)){
  //  r=(1.93227+(0.07398)*(z/in))*in;// joint , need to correct this
  //}
  //Flange inner downstream
  else if ((z>=118.156*in)&&(z<=121.388*in)){
   r=21.498/2.*in+(z/in-118.156)/(121.388-118.156)*(21.853-21.498)/2.;
  }
 else if ((z>=121.388*in)&&(z<137.96*in)){
   r=21.853/2.*in;
  }
 else if ((z>=137.96*in)&&(z<139.22*in)){
   r=24.706/2.*in;
  }

  //for MV
  else if (z>=139.22*in){
    r=24.706/2.*in;
  }

 
return r;
}
G4double NumiDetectorConstruction::PHorn2ICRin(G4double z)
{
 G4double r=0.;
 
  if (z<0.0*in){
    r=sqrt(114.73006)*in-0.11811*in-0.05*in;//for MV; added 0.05 because ic was outside of mv
  }

  // These are equations from gnumi
  else if(z/cm>=0.0 && (z/cm)< 97.617)
  {
  r= (sqrt((100-(z/cm))/0.1351)-0.2)*cm;
  }
  else if( (z/cm)>=97.617 && (z/cm)<104.803)
  { // Neck
    r = 4*cm;
  }
  else if((z/cm)>=104.803 && (z/cm)<118.156*in)
  {
    r= (sqrt(((z/cm)-100)/0.2723)-0.2)*cm ;
  }
  // IC from drawings
  // This is from Zarko's drawings
  /*
  else if ((z>=0.0*in)&&(z<5.8*in)){
    r=sqrt(114.73006-(2.91414)*(z/in))*in-0.11811*in;
  }
  else if ((z>=5.8*in)&&(z<29.8300*in)){
    r=sqrt(114.73007-(2.91414)*(z/in))*in-0.11811*in;
  }
  else if ((z>=29.8300*in)&&(z<37.9435*in)){
    r=sqrt(114.73006-(2.91414)*(z/in))*in-0.11811*in;
  }   
  
  else if ((z>=37.9435*in)&&(z<39.12373*in)){
    r=(1.5355+(2.-sqrt(4.-sqr(z/in-39.12373))))*in; //! NECK !
  }  
  else if ((z>=39.12373*in)&&(z<40.86009*in)){
    r=1.5355*in; //! NECK !
  }  
  else if ((z>=40.86009*in)&&(z<41.6144*in)){
    r=(1.5355+(2.-sqrt(4.-sqr(z/in-40.86009))))*in; //! NECK !
  }  

  else if ((z>=41.6144*in)&&(z<49.8920*in)){
    r=sqrt(-56.92263+(1.44583)*(z/in))*in-0.11811*in;
  } 
  else if ((z>=49.8920*in)&&(z<69.8920*in)){
    r=sqrt(-56.922631+(1.44583)*(z/in))*in-0.11811*in;
  } 
  else if ((z>=69.8920*in)&&(z<93.8920*in)){
    r=(0.01064+(0.0932)*(z/in))*in;
  }
  else if ((z>=93.8920*in)&&(z<118.156*in)){
    r=(1.81456+(0.07398)*(z/in))*in;
  }
  */
  //Flange inner downstream
 else if ((z>=118.156*in)&&(z<139.21*in)){
//   r=21.342/2.*in;
   r=21.103/2.*in; // P.L., October 2017.  See Drawing 363392 
  }

  //for mother volume i need r defined up to the end of horn
  else if (z>=139.21*in){
//    r=21.342/2.*in;
   r=21.103/2.*in; // P.L., October 2017.  See Drawing 363392 
  }
return r;
}
