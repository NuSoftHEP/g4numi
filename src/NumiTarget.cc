/**
 * Author Vamis Xhagjika
 * Module that constructs the target for the simulation.
 **/
 
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
#include "NumiDataInput.hh"

void NumiDetectorConstruction::ConstructTarget()
{
  G4RotationMatrix rotation;
  G4ThreeVector translation;
  G4String Sname,PVname,LVname,vol_name;
  G4double tgtGap=0.025*mm;
  char no[3];

  // Mother volume (TGAR) position
  G4ThreeVector target_hall_position=G4ThreeVector(0,0,NumiData->TargetAreaLength/2.+NumiData->TargetAreaZ0);
  G4ThreeVector targetPosition=G4ThreeVector(NumiData->TargetX0,NumiData->TargetY0,NumiData->TargetZ0);
  G4ThreeVector containerDisplacement=G4ThreeVector(0.,NumiData->TargetContYOffset,0.);
    
  //Create Target Region
  //G4Region* TargetRegion = new G4Region("Numi Target");

  //Create Mother Volume
  G4VSolid* sTargetMotherVol;
  G4VSolid* dummySolid;
  G4double mvGap=0.05*mm;
  
  G4ThreeVector TargetMVOrigin=G4ThreeVector(0., 0., NumiData->TargetContHalfLength) + containerDisplacement;
  sTargetMotherVol = new G4Tubs("sTargetMotherVol", 0., NumiData->TargetContRout, NumiData->TargetContHalfLength, 0., 360.*deg);

  G4ThreeVector TargetMotherVolPosition= TargetMVOrigin - target_hall_position + targetPosition;
  
  G4LogicalVolume* lvTargetMotherVol=new G4LogicalVolume(sTargetMotherVol, Air, "lvTargetMother", 0, 0, 0);
  //TargetRegion->AddRootLogicalVolume(lvTargetMotherVol);
  G4VPhysicalVolume* pvTargetMotherVol=new G4PVPlacement(0,TargetMotherVolPosition,"pvTargetMother",lvTargetMotherVol,TGAR,false,0);

  vol_name= "pvTargetMother_HeliumFill";
  G4VSolid* HelSolid=new G4Tubs("ContSolid", 0., NumiData->TargetContRin, NumiData->TargetContHalfLength - 2*NumiData->UpstrFlangeL - NumiData->UpstrBerSupportL, 0., 360.*deg);
  G4ThreeVector HelVolPos=G4ThreeVector(0., 0., NumiData->TargetContHalfLength + NumiData->UpstrFlangeL)-TargetMVOrigin + containerDisplacement;
  G4LogicalVolume* lvHelTube=new G4LogicalVolume(HelSolid, He, vol_name.append("LV"),0,0,0);
  G4VPhysicalVolume* pvTargetMotherVolHelFill = new G4PVPlacement(0,HelVolPos,vol_name,lvHelTube,pvTargetMotherVol,true,0);
  
  //------------------------------------------------------------------------------------------------
  //Target
  //------------------------------------------------------------------------------------------------
  //First create one segment
  G4double TGT_w=NumiData->TargetSWidth/2.;
  G4double TGT_h=NumiData->TargetSHeight/2.;
  G4double TGT_l=NumiData->TargetSLength/2.; //these are 0.5*true dimension
  // Carbon Target
  
  G4VSolid* TGT1_solid;
  
  if (NumiData->TargetEndRounded){
    TGT_l=TGT_l - NumiData->TargetSWidth/2.;
    G4Box* TGT2_solid   =new G4Box("TGT2_solid",TGT_w,TGT_h,TGT_l);
    G4Tubs* TGTRE_solid =new G4Tubs("TGTRE_solid",0.,TGT_w,TGT_h,0.,360.*deg);
    rotation    =G4RotationMatrix(0,0,0); rotation.rotateX(90.*deg);
    translation =G4ThreeVector(0,0,-TGT_l);
    TGT1_solid  =new G4UnionSolid("TGT1_solid",TGT2_solid,TGTRE_solid,G4Transform3D(rotation,translation));
    translation =G4ThreeVector(0,0,TGT_l);
    TGT1_solid  =new G4UnionSolid("TGT1_solid",TGT1_solid,TGTRE_solid,G4Transform3D(rotation,translation));
  }
    else
  {
    TGT1_solid=new G4Box("TGT1_solid",TGT_w,TGT_h,TGT_l);
  }
  
  G4LogicalVolume* LVTargetFin=new G4LogicalVolume(TGT1_solid,Target,"LVTargetFin",0,0,0);
  //Now create TargetSegmentNo of target fins and place them
  for (G4int ii=0; ii<NumiData->TargetSegmentNo; ii++){
    rotation=G4RotationMatrix(0,0,0);
    rotation.rotateX(atan(NumiData->TargetDydz));
    rotation.rotateY(atan(NumiData->TargetDxdz));
    // with this translation rotation axis is at the begining of the volume (x0,y0,z0) and not at its center
    translation=G4ThreeVector(-(sin(atan(NumiData->TargetDxdz)))*cos(atan(NumiData->TargetDydz))*NumiData->TargetSLength/2.,(sin(atan(NumiData->TargetDydz)))*NumiData->TargetSLength/2.,(1-cos(atan(NumiData->TargetDxdz))*cos(atan(NumiData->TargetDydz)))*NumiData->TargetSLength/2.);
    G4ThreeVector targetSegmentPosition
        = G4ThreeVector( NumiData->TargetSXOffset - NumiData->TargetSWidth/2.,
                         NumiData->TargetSYOffset - NumiData->TargetSHeight/2.,
                         NumiData->TargetSZOffset + TGT_l + ((NumiData->TargetSegmentPitch + NumiData->TargetSLength)*ii))
          - TargetMVOrigin
          - translation;
    new G4PVPlacement(G4Transform3D(rotation, targetSegmentPosition), "TGT1", LVTargetFin, pvTargetMotherVolHelFill, true, ii);
  }
  
  // Budal Monitor
  rotation=G4RotationMatrix(0,0,0);
  rotation.rotateX(atan(NumiData->BudalDydz));
  rotation.rotateY(atan(NumiData->BudalDxdz));
  rotation.rotateZ(90.*deg);
  // with this translation rotation axis is at the begining of the volume (x0,y0,z0) and not at its center
  translation=G4ThreeVector(-(sin(atan(NumiData->BudalDxdz)))*cos(atan(NumiData->BudalDydz))*NumiData->TargetSLength/2.,(sin(atan(NumiData->BudalDydz)))*NumiData->TargetSLength/2.,(1-cos(atan(NumiData->BudalDxdz))*cos(atan(NumiData->BudalDydz)))*NumiData->TargetSLength/2.);
  G4ThreeVector budalMonitorPosition=G4ThreeVector(NumiData->BudalX0, NumiData->BudalY0 ,NumiData->BudalZ0 + NumiData->TargetSLength/2.) - TargetMVOrigin - translation;
  G4LogicalVolume* LVBudalMonitor=new G4LogicalVolume(TGT1_solid,Target,"LVBudalMonitor",0,0,0);
  new G4PVPlacement(G4Transform3D(rotation,budalMonitorPosition),"BudalMonitor",LVBudalMonitor,pvTargetMotherVolHelFill,true,0);
  
  //Container
  
  G4LogicalVolume* lvContTube;
  G4VSolid* sContSolid;
  
  for (G4int ii=0; ii<NumiData->NContainerN; ii++)
  {
    vol_name    =NumiData->CTubeVolName[ii];
    sContSolid  =new G4Tubs("ContSolid",NumiData->CTubeRin[ii],NumiData->CTubeRout[ii],NumiData->CTubeLength[ii] - 2*NumiData->UpstrFlangeL - NumiData->UpstrBerSupportL,0.,360.*deg);
    G4ThreeVector ContPosition=G4ThreeVector(0.,0.,NumiData->CTubeZ0[ii]+NumiData->CTubeLength[ii] + NumiData->UpstrFlangeL) - TargetMVOrigin + containerDisplacement;
    lvContTube  =new G4LogicalVolume(sContSolid,GetMaterial(NumiData->CTubeGeantMat[ii]),vol_name.append("LV"),0,0,0);
    CNT[ii]     =new G4PVPlacement(0,ContPosition,vol_name,lvContTube,pvTargetMotherVol,false,1);
  }

  vol_name    = "ContainerWaterRing";
  sContSolid  =new G4Tubs("ContSolid", NumiData->TargetContRin + ((3.*mm*16.5*mm)/14*mm), NumiData->TargetContRout - 3*mm, NumiData->TargetContHalfLength - 2*NumiData->UpstrFlangeL - NumiData->UpstrBerSupportL, 0., 360.*deg);
  G4ThreeVector ContPosition=G4ThreeVector(0., 0., NumiData->TargetContHalfLength + NumiData->UpstrFlangeL)-TargetMVOrigin + containerDisplacement;
  lvContTube  =new G4LogicalVolume(sContSolid,Water,vol_name.append("LV"),0,0,0);
  new G4PVPlacement(0,ContPosition,vol_name,lvContTube,pvTargetMotherVol,false,1);

  //VXADD Adding berilium upstream window
  G4double UpstreamHoleL      = 2*NumiData->UpstrBerSupportL + 2*NumiData->UpstrFlangeL;
  
  G4LogicalVolume* lvUpstrFlange;
  G4VSolid*        sFinalSolid;
  G4VSolid*        sTempSolid;
  
  
  vol_name = "UpstreamFlange";
  
  //Create the support for the Berilium window.
  sFinalSolid = new G4Tubs("UpstreamSupport", 0., NumiData->UpstrBerSupportRout, NumiData->UpstrBerSupportL, 0., 360.*deg);
  //Create the Flange and unite that with the support.
  sTempSolid  = new G4Tubs("UpstreamFlange", 0., NumiData->UpstrFlangeRout, NumiData->UpstrFlangeL, 0., 360.*deg);
  G4ThreeVector FlangePos = G4ThreeVector(0.,0.,NumiData->UpstrFlangeL + NumiData->UpstrBerSupportL) + containerDisplacement;
  rotation    = G4RotationMatrix(0,0,0);
  sFinalSolid = new G4UnionSolid("sTempSolid", sFinalSolid, sTempSolid, G4Transform3D(rotation, FlangePos));

  //Now create the hole that passes that throught
  sTempSolid  = new G4Tubs("tempObject", 0., NumiData->UpstrBerLayerRout, UpstreamHoleL, 0., 360.*deg);
  G4ThreeVector HolePos = G4ThreeVector(0.,0.,NumiData->UpstrBerSupportL);
  rotation    = G4RotationMatrix(0,0,0);
  sFinalSolid = new G4SubtractionSolid("sTempSolid", sFinalSolid, sTempSolid, G4Transform3D(rotation, HolePos));

  //Now create the hole for teh berilium window to be placed in
  sTempSolid  = new G4Tubs("tempObject", 0., NumiData->UpstrBerLayerRout, NumiData->UpstrBerSupportL/2, 0., 360.*deg);
  HolePos     = G4ThreeVector(0.,0.,NumiData->UpstrBerSupportL/2);
  rotation    = G4RotationMatrix(0,0,0);
  sFinalSolid = new G4SubtractionSolid("sTempSolid", sFinalSolid, sTempSolid, G4Transform3D(rotation, HolePos));

  G4ThreeVector FinalBerPos =  G4ThreeVector(0., 0., NumiData->UpstrBerSupportL) - TargetMVOrigin;
  
  lvUpstrFlange = new G4LogicalVolume(sFinalSolid , GetMaterial(NumiData->UpstrFlangeMat), vol_name.append("LV"), 0, 0, 0);
  new G4PVPlacement(0, FinalBerPos, vol_name, lvUpstrFlange, pvTargetMotherVol, false, 1);

  //Now Add the actual berilium window
  vol_name      ="BeUp1";
  sFinalSolid   = new G4Tubs("UpstreamFlange", 0., NumiData->UpstrBerLayerRout, NumiData->UpstrBerLayerL, 0., 360.*deg);
  G4ThreeVector BerPos = G4ThreeVector(0., 0., NumiData->UpstrBerLayerL) - TargetMVOrigin;
  rotation      = G4RotationMatrix(0,0,0);
  lvUpstrFlange = new G4LogicalVolume(sFinalSolid , GetMaterial(NumiData->UpstrBerLayerMat), vol_name.append("LV"), 0, 0, 0);
  new G4PVPlacement(0, BerPos, vol_name, lvUpstrFlange, pvTargetMotherVol, false, 1);

  
  //Create the downstream berilium window starting from the Flange
  vol_name    = "TGTExitCyl1";
  sFinalSolid = new G4Tubs(vol_name, 0., NumiData->UpstrFlangeRout, NumiData->UpstrFlangeL, 0., 360.*deg);
  sTempSolid  = new G4Tubs(vol_name, 0., NumiData->DwstrBerWindowRout, NumiData->UpstrFlangeL + 2.*mm, 0., 360.*deg);
  HolePos     = G4ThreeVector(0.,0.,0.) - containerDisplacement;
  rotation    = G4RotationMatrix(0,0,0);
  sFinalSolid = new G4SubtractionSolid(vol_name, sFinalSolid, sTempSolid, G4Transform3D(rotation, HolePos));

  G4ThreeVector finalDest = G4ThreeVector(0., 0., NumiData->TargetContHalfLength * 2 - NumiData->UpstrFlangeL) - TargetMVOrigin + containerDisplacement;
  
  lvUpstrFlange = new G4LogicalVolume(sFinalSolid , GetMaterial(NumiData->UpstrFlangeMat), vol_name.append("LV"), 0, 0, 0);
  new G4PVPlacement(0, finalDest, vol_name, lvUpstrFlange, pvTargetMotherVol, false, 1);

  //Now Add the actual berilium window
  vol_name      ="BeDW";
  sFinalSolid   = new G4Tubs("UpstreamFlange", 0., NumiData->DwstrBerWindowRout, NumiData->UpstrBerLayerL, 0., 360.*deg);
  finalDest     = G4ThreeVector(0., 0., NumiData->TargetContHalfLength * 2 - NumiData->UpstrBerLayerL) - TargetMVOrigin;
  rotation      = G4RotationMatrix(0,0,0);
  lvUpstrFlange = new G4LogicalVolume(sFinalSolid , GetMaterial(NumiData->UpstrBerLayerMat), vol_name.append("LV"), 0, 0, 0);
  new G4PVPlacement(0, finalDest, vol_name, lvUpstrFlange, pvTargetMotherVol, false, 1);

  /**
   * Construct Cooling for the fins. This are all relative to the target fins variables so that it does not need much specific variables in DataInput
   */
  
  //Number of fins * length of a fin
  G4double    FinCoolerZLength;
  //Solid for the scheleton the same will be used for both left and right side so we need that to create the Volume
  G4VSolid*    FinCoolerOuterBox;
  G4VSolid*    FinCoolerWaterTubeOuterBox;
  
  //Solid to shape the FincCoolerOuterBox
  G4VSolid*    FinCoolerBoxIn;
  //Solid for the water pipes
  G4VSolid*    FinCoolerPipe;

  //Volume variables
  G4LogicalVolume* NoWaterFinCooler;
  G4LogicalVolume* WatterFinCooler;
  G4LogicalVolume* FinCoolingWaterPipe;
  
  FinCoolerZLength  = NumiData->TargetSegmentNo * (NumiData->TargetSegmentPitch + NumiData->TargetSLength);
  FinCoolerZLength -= NumiData->TargetSegmentPitch;
  
  FinCoolerOuterBox = new G4Box("CoolerBox", NumiData->FinCoolerXlength/2, NumiData->FinCoolerYLength/2, FinCoolerZLength/2);
  FinCoolerWaterTubeOuterBox = new G4Box("WaterCoolerBox", NumiData->FinCoolerXlength/2, NumiData->FinCoolerYLength/2, FinCoolerZLength/2);
  FinCoolerBoxIn    = new G4Box("InternalNoFillBox",NumiData->FinCoolerBoxInXLength/2, NumiData->FinCoolerBoxInYLength/2, FinCoolerZLength/2);

  G4ThreeVector FinCoolerBoxInPos = G4ThreeVector(NumiData->FinCoolerXlength/2 - NumiData->FinCoolerBoxInXLength/2, 0., 0.);
  rotation       = G4RotationMatrix(0,0,0);
  FinCoolerOuterBox = new G4SubtractionSolid("CoolerBox", FinCoolerOuterBox, FinCoolerBoxIn, G4Transform3D(rotation, FinCoolerBoxInPos));

  //Create the tunnel to place the the water tubes
  FinCoolerPipe  = new G4Tubs("WaterSolid", 0., NumiData->WaterPipeRout, FinCoolerZLength/2, 0., 360.*deg);

  //Cooling part with the water tubes
  G4ThreeVector WaterPipePos = G4ThreeVector(0., NumiData->FinCoolerYLength/2 - NumiData->WaterPipeRout, 0.);
  FinCoolerWaterTubeOuterBox = new G4SubtractionSolid("WaterCoolerBox", FinCoolerWaterTubeOuterBox, FinCoolerPipe, G4Transform3D(rotation, WaterPipePos));
  
  WaterPipePos = G4ThreeVector(0.,-NumiData->FinCoolerYLength/2 + NumiData->WaterPipeRout, 0.);
  FinCoolerWaterTubeOuterBox = new G4SubtractionSolid("WaterCoolerBox", FinCoolerWaterTubeOuterBox, FinCoolerPipe, G4Transform3D(rotation, WaterPipePos));

  //place the non material tunnel in the middle of the clamps
  FinCoolerBoxInPos = G4ThreeVector(-NumiData->FinCoolerXlength/2 + NumiData->FinCoolerBoxInXLength/2, 0., 0.);
  FinCoolerWaterTubeOuterBox = new G4SubtractionSolid("WaterCoolerBox", FinCoolerWaterTubeOuterBox, FinCoolerBoxIn, G4Transform3D(rotation, FinCoolerBoxInPos));
  
  //Place the no water Clamp into the target mother volume
  vol_name = "NoWaterCoolingPart";
  finalDest = G4ThreeVector(NumiData->TargetSXOffset - NumiData->TargetSWidth - NumiData->FinCoolerXlength/2., NumiData->TargetSYOffset - NumiData->FinCoolingYOffset - NumiData->FinCoolerYLength/2, NumiData->TargetSZOffset + FinCoolerZLength/2.) - TargetMVOrigin;
  NoWaterFinCooler    = new G4LogicalVolume(FinCoolerOuterBox, Al, vol_name.append("LV"), 0, 0, 0);
  new G4PVPlacement(0, finalDest, vol_name, NoWaterFinCooler, pvTargetMotherVolHelFill, false, 1);

  //Place the water cooled Clamp into the target mother volume
  vol_name = "WaterCoolingPart";
  finalDest = G4ThreeVector(NumiData->TargetSXOffset + NumiData->FinCoolerXlength/2., NumiData->TargetSYOffset - NumiData->FinCoolingYOffset - NumiData->FinCoolerYLength/2, NumiData->TargetSZOffset + FinCoolerZLength/2.) - TargetMVOrigin;
  FinCoolingWaterPipe = new G4LogicalVolume(FinCoolerWaterTubeOuterBox, Al, vol_name.append("LV"), 0, 0, 0);
  new G4PVPlacement(0, finalDest, vol_name, FinCoolingWaterPipe, pvTargetMotherVolHelFill, false, 1);
  
  //Place the water tubes into the water cooled Clamp
  vol_name = "FinCoolingWaterPipe";
  FinCoolingWaterPipe = new G4LogicalVolume(FinCoolerPipe, Water, vol_name.append("LV"), 0, 0, 0);
    
  finalDest = G4ThreeVector(NumiData->TargetSXOffset + NumiData->FinCoolerXlength/2.,  NumiData->TargetSYOffset - NumiData->FinCoolingYOffset - NumiData->WaterPipeRout, NumiData->TargetSZOffset + FinCoolerZLength/2.) - TargetMVOrigin;
  new G4PVPlacement(0, finalDest, vol_name, FinCoolingWaterPipe, pvTargetMotherVolHelFill, false, 0);
  
  finalDest = G4ThreeVector(NumiData->TargetSXOffset + NumiData->FinCoolerXlength/2.,  NumiData->TargetSYOffset - NumiData->FinCoolingYOffset - NumiData->FinCoolerYLength + NumiData->WaterPipeRout, NumiData->TargetSZOffset + FinCoolerZLength/2.) - TargetMVOrigin;
  new G4PVPlacement(0, finalDest, vol_name, FinCoolingWaterPipe, pvTargetMotherVolHelFill, false, 1);
  
  G4cout << "Target Constructed" << G4endl;
}
