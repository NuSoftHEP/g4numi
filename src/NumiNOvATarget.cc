/**
 * Author Mike Martens
 * Jan 26, 2010
 * (Modified version of file by Vamis Xhagjika and Alex Himmel)
 * 
 * Jan 26, 2010
 *   Updated/Corrected model of the "Medium Energy" Target for NOvA.
 *   Still need to add Target offset and angle for studies of mis-alignment
 *   Pressing plate and cooling plate are only approximate since target design is not final.
 *
 **/
 
#include "NumiDetectorConstruction.hh"
#include "NumiDataInput.hh"

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

#include "CLHEP/Units/PhysicalConstants.h"

void NumiDetectorConstruction::ConstructNOvATarget()
{

  std::cout<<"=============================================="<<std::endl;
  std::cout<<"Quick new inputs debug check:"<<std::endl;
  std::cout<<"==>> NumberOfMEFins         : "<< (NumiData->NumberOfMEFins) <<std::endl;
  std::cout<<"==>> DistanceBetweenMEFins  : "<< (NumiData->TargetSegPitch) <<std::endl;
  std::cout<<"==>> BudalMonitorMEPosition : "<< (NumiData->BudalMonitorMEPosition)<<std::endl;
  std::cout<<"==>> WidthMEFin             : "<< (NumiData->TargetSegWidth) <<std::endl;
  std::cout<<"==>> WingedFin1             : "<< (NumiData->WingedFin1) <<std::endl;
  std::cout<<"==>> WingedFin2             : "<< (NumiData->WingedFin2) <<std::endl;
  std::cout<<"==>> WingedFin3             : "<< (NumiData->WingedFin3) <<std::endl;
  std::cout<<"==>> WingedFin4             : "<< (NumiData->WingedFin4) <<std::endl;
  std::cout<<"==>> WingedFinRadius        : "<< (NumiData->WingedFinRadius) <<std::endl;
  std::cout<<"=============================================="<<std::endl;

  G4RotationMatrix rotation;
  G4ThreeVector translation;

  // =================================================
  // Nova Medium Energy Target
  // =================================================
  //
  // Model of the ME target is based on:
  //   IHEP DWG 9174-00-00-00 (9/1/2008) of the Medium Energy Target Assembly
  //   IHEP DWG 9174-01-00-00 (9/1/2008) of the Medium Energy Target Casing
  //   Fermilab DWG 8030.000-MB-449112 (1/3/2008) of the Upstream Beryllium Window 
  //   NOvA document 3435-v5 (10/9/2009) on the "Target and Horn Configuration for the NOvA Experiment"


  // -----------------------------------------
  // Create Target Mother Volume
  // -----------------------------------------
  //
  // Positions are relative to the start of the target material in z, and to the centerline of the canister in x and y.
  // Target Canister is comprised of 4 main parts
  //   Upstream Be Window Flange
  //   Upstream Flange
  //   Main Body with water cooling
  //   Downstream Flange with Be window

  G4VSolid* TargetMotherVolSolid;
  G4VSolid* dummySolid;
  G4double mvGap=0.05*CLHEP::mm;
  G4ThreeVector TargetMVOrigin;

  std::cerr << " Dump of Nova target parameters " << std::endl;

  rotation = G4RotationMatrix(0.,0.,0.);  
  TargetMVOrigin = G4ThreeVector(NumiData->TargetUpBeFlangeX0, 
				 NumiData->TargetUpBeFlangeY0, 
				 NumiData->TargetUpBeFlangeZ0 + NumiData->TargetUpBeFlangeLength/2.0);
  
  std::cerr << " Target Origin " << TargetMVOrigin << std::endl;
  TargetMotherVolSolid = new G4Tubs("TargetMotherVolSolid", 0., NumiData->TargetUpBeFlangeOutRad+mvGap, 
				    NumiData->TargetUpBeFlangeLength/2.0+mvGap, 0., 360.*CLHEP::deg);

  std::cerr << " ... Usptream Flange... Radius   " << NumiData->TargetUpBeFlangeOutRad 
                                               << " Length " << NumiData->TargetUpBeFlangeLength <<  std::endl;

  dummySolid = new G4Tubs("dummySolid",0.,NumiData->TargetUpFlangeOutRad+mvGap,
			  NumiData->TargetUpFlangeLength/2.+mvGap,0.,360.*CLHEP::deg);

  translation=G4ThreeVector(NumiData->TargetUpFlangeX0, 
			    NumiData->TargetUpFlangeY0, 
			    NumiData->TargetUpFlangeZ0 + NumiData->TargetUpFlangeLength/2.0) - TargetMVOrigin;
			    
  std::cerr << " ... Translated by .. " << translation << std::endl;

  TargetMotherVolSolid = new G4UnionSolid("TargetMotherVolSolid",TargetMotherVolSolid,
					  dummySolid,G4Transform3D(rotation,translation));

  dummySolid=new G4Tubs("dummySolid",0.,NumiData->TargetOutsideCasingOutRad+mvGap,
			NumiData->TargetCasingLength/2.+mvGap,0.,360.*CLHEP::deg);

  std::cerr << " ... Outsicasing Radius    " << NumiData->TargetOutsideCasingOutRad 
                                               << " Length " << NumiData->TargetCasingLength <<  std::endl;
					       
  translation=G4ThreeVector(NumiData->TargetCasingX0, 
			    NumiData->TargetCasingY0, 
			    NumiData->TargetCasingZ0 + NumiData->TargetCasingLength/2.0) - TargetMVOrigin;
  
  std::cerr << " ... Translated by .. " << translation << std::endl;
  TargetMotherVolSolid = new G4UnionSolid("TargetMotherVolSolid",TargetMotherVolSolid,
					  dummySolid,G4Transform3D(rotation,translation));
  //
  // September 2017.  P.L. Adding (optionally) a bit of material on the downstream flange, to simulate the bolds.. 
  //
  const double effTargetDnFlangeLength = NumiData->TargetDnFlangeLength + fNovaTargetExtraFlangeThick;
  
  dummySolid=new G4Tubs("dummySolid",0.,NumiData->TargetDnFlangeOutRad+mvGap,
			effTargetDnFlangeLength/2.+mvGap,0.,360.*CLHEP::deg);

  std::cerr << " ... Dn Flange Radius    " << NumiData->TargetDnFlangeOutRad
                                               << " Length " << effTargetDnFlangeLength<<  std::endl;
					       
  translation=G4ThreeVector(NumiData->TargetDnFlangeX0, 
			    NumiData->TargetDnFlangeY0, 
			    NumiData->TargetDnFlangeZ0 + effTargetDnFlangeLength/2.0) - TargetMVOrigin;
  
  std::cerr << " ... Translated by .. " << translation << std::endl;
  
  TargetMotherVolSolid = new G4UnionSolid("TargetMotherVolSolid",TargetMotherVolSolid,
					  dummySolid,G4Transform3D(rotation,translation));


  // Now place the TargetMotherVolume

  // Still need to add rotations and offsets
  //  rotation=G4RotationMatrix(0,0,0);
  //  rotation.rotateX(atan(NumiData->BudalDydz));
  //  rotation.rotateY(atan(NumiData->BudalDxdz));
  //  rotation.rotateZ(90.*CLHEP::deg);
  //  // with this translation rotation axis is at the begining of the volume (x0,y0,z0) and not at its center
  // translation=G4ThreeVector(-(sin(atan(NumiData->BudalDxdz)))*cos(atan(NumiData->BudalDydz))*NumiData->TargetSegLength/2.,(sin(atan(NumiData->BudalDydz)))*NumiData->TargetSegLength/2.,(1-cos(atan(NumiData->BudalDxdz))*cos(atan(NumiData->BudalDydz)))*NumiData->TargetSegLength/2.);
  //  G4ThreeVector budalMonitorPosition=G4ThreeVector(NumiData->BudalX0, NumiData->BudalY0 ,NumiData->BudalZ0 + NumiData->TargetSegLength/2.) - TargetMVOrigin - translation;
  //  G4LogicalVolume* LVBudalMonitor=new G4LogicalVolume(TGT1_solid,Target,"LVBudalMonitor",0,0,0);
  //  new G4PVPlacement(G4Transform3D(rotation,budalMonitorPosition),"BudalMonitor",LVBudalMonitor,pvTargetMotherVolHelFill,true,0,NumiData->pSurfChk);


  G4ThreeVector TargetHallPosition=G4ThreeVector(0,0,NumiData->TargetAreaZ0+NumiData->TargetAreaLength/2);
  G4ThreeVector TargetPosition=G4ThreeVector(NumiData->TargetX0,NumiData->TargetY0,NumiData->TargetZ0);
  G4ThreeVector TargetMotherVolPosition= TargetPosition + TargetMVOrigin - TargetHallPosition;

  std::cerr << " ... TargetMotherVolPosition .. " << TargetMotherVolPosition << std::endl;
      //G4LogicalVolume* TargetMotherVolLV = new G4LogicalVolume(TargetMotherVolSolid, He, "lvTargetMother", 0, 0, 0);
  G4LogicalVolume* TargetMotherVolLV
      = new G4LogicalVolume(TargetMotherVolSolid, TargetHelium, "lvTargetMother", 0, 0, 0);
  //
  // P.L. Add systematic studies to understand the wiggle. September 2017
  //
  G4RotationMatrix *aMatMPtr = 0;
  if ((std::abs(fNovaTargetHTilt) > 1.0e-10) || (std::abs(fNovaTargetVTilt) > 1.0e-10)) { 
    aMatMPtr =  &fRotationMotherNovaTarget;
    if (std::abs(fNovaTargetHTilt) > 1.0e-10) fRotationMotherNovaTarget.rotateY(fNovaTargetHTilt);
    if (std::abs(fNovaTargetVTilt) > 1.0e-10) fRotationMotherNovaTarget.rotateX(fNovaTargetVTilt);
  }
  G4ThreeVector finalTargetMotherVolPosition (TargetMotherVolPosition);
  finalTargetMotherVolPosition[0] += fNovaTargetXOffset;
  finalTargetMotherVolPosition[1] += fNovaTargetYOffset;
  
  G4VPhysicalVolume* pvTargetMotherVol = new G4PVPlacement(aMatMPtr, finalTargetMotherVolPosition,"TargetMotherVol",
							   TargetMotherVolLV,TGAR,false,0,NumiData->pSurfChk);


  //------------------------------------------------------------------------------------------------
  // Target Segments and Budal Monitors
  //------------------------------------------------------------------------------------------------

  //First create one segment
  G4double TGT_w = NumiData->TargetSegWidth/2.;
  G4double TGT_h = NumiData->TargetSegHeight/2.;
  G4double TGT_l = NumiData->TargetSegLength/2.;
  std::cout<<"==>>Fin segment dimensions (w,h,l) : "<<TGT_w<<" "<<TGT_h<<" "<<TGT_l<<std::endl;

  std::cerr << " TGT dims " << TGT_w << " / " << TGT_h << " / " <<  TGT_l << std::endl; 
//  exit(2);
  
  G4VSolid* TGT1_solid;
  G4VSolid* TGT1_solid_winged;
  G4double wing_rad = NumiData->WingedFinRadius;
  ////////////

  //  assert(NumiData->TargetEndRounded);
  if (NumiData->TargetEndRounded){
    //    G4cout<<"Rounded TGT1"<<G4endl;
    TGT_l=TGT_l - NumiData->TargetSegWidth/2.;
    G4Box* TGT2_solid   =new G4Box("TGT2_solid",TGT_w,TGT_h,TGT_l);
    G4Tubs* TGTRE_solid =new G4Tubs("TGTRE_solid",0.,TGT_w,TGT_h,0.,360.*CLHEP::deg);
    rotation    =G4RotationMatrix(0,0,0); rotation.rotateX(90.*CLHEP::deg);
    translation =G4ThreeVector(0,0,-TGT_l);
    TGT1_solid  =new G4UnionSolid("TGT1_solid",TGT2_solid,TGTRE_solid,G4Transform3D(rotation,translation));
    translation =G4ThreeVector(0,0,TGT_l);
    TGT1_solid  =new G4UnionSolid("TGT1_solid",TGT1_solid,TGTRE_solid,G4Transform3D(rotation,translation));


    //Winged
    rotation = G4RotationMatrix(0,0,0);
    rotation.rotateY(90.*CLHEP::deg);
    translation =G4ThreeVector(0,TGT_h + wing_rad,0);
    G4Tubs* TGTWing_solid = new G4Tubs("TGTWing_solid",0.,wing_rad,TGT_l,0.,360.*CLHEP::deg);
    TGT1_solid_winged     = new G4UnionSolid("TGT1_solid_winged",TGT1_solid,TGTWing_solid,G4Transform3D(rotation,translation));
  }
    else
  {
    TGT1_solid=new G4Box("TGT1_solid",TGT_w,TGT_h,TGT_l);
  }
  
  // The material "Target" is defined in NumiMaterials.cc
  G4LogicalVolume* LVTargetFin       = new G4LogicalVolume(TGT1_solid,Target       ,"LVTargetFin",0,0,0);
  G4LogicalVolume* LVWingedTargetFin = new G4LogicalVolume(TGT1_solid_winged,Target,"LVWingedTargetFin"      ,0,0,0);


  //Now create TargetSegmentNo of target fins and place them

  G4double TGT_x, TGT_y, TGT_z;


  // Budal Monitor. Horizontal Fin for Vertical Scan (HFVS) 
  TGT_x = -NumiData->TargetSegHeight/2.0 + NumiData->TargetSegWidth;
  TGT_y = 0.0;
  TGT_z = NumiData->BudalHFVSLength/2.0;
  rotation=G4RotationMatrix(0,0,0);
  rotation.rotateZ(90.*CLHEP::deg);
  translation=G4ThreeVector(TGT_x, TGT_y, TGT_z) - TargetMVOrigin;

  new G4PVPlacement(G4Transform3D(rotation, translation), "Budal_HFVS", LVTargetFin, pvTargetMotherVol, false, 0, NumiData->pSurfChk);

  // Budal Monitor. Vorizontal Fin for Hertical Scan (VFHS) 
  TGT_x = 0.0;
  TGT_y = -NumiData->TargetSegHeight/2.0 + NumiData->TargetSegWidth/2.0;
  TGT_z = NumiData->BudalHFVSLength + NumiData->BudalHFVSPitch + NumiData->BudalVFHSLength/2.0;
  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(TGT_x, TGT_y, TGT_z) - TargetMVOrigin;

  new G4PVPlacement(G4Transform3D(rotation, translation), "Budal_VFHS", LVTargetFin, pvTargetMotherVol, false, 0, NumiData->pSurfChk);

  // The target segments
  // MAK: There is no reason to make the number of target segments a variable
  // that appears in another header and is shared with the LE target
  // this just leads to bugs
  //const int NumberOfNonBudalFinsInMETarget=48;

  ///Leo: making the number of target segments variable for
  //optimization studies:
  int NumberOfNonBudalFinsInMETarget = NumiData->NumberOfMEFins;

  ////////
  G4int wfin1 = NumiData->WingedFin1;
  G4int wfin2 = NumiData->WingedFin2;
  G4int wfin3 = NumiData->WingedFin3;
  G4int wfin4 = NumiData->WingedFin4;
  /////////

  for (G4int ii=0; ii< NumberOfNonBudalFinsInMETarget; ii++){
    
    TGT_x = 0.0;
    TGT_y = -NumiData->TargetSegHeight/2.0 + NumiData->TargetSegWidth/2.0;
    TGT_z = NumiData->BudalHFVSLength + NumiData->BudalHFVSPitch + NumiData->BudalVFHSLength + NumiData->BudalVFHSPitch
            + ii*(NumiData->TargetSegLength + NumiData->TargetSegPitch)+NumiData->TargetSegLength/2.0;
    rotation=G4RotationMatrix(0,0,0);
    translation=G4ThreeVector(TGT_x, TGT_y, TGT_z) - TargetMVOrigin;
    if(ii == wfin1 || ii == wfin2 || ii == wfin3 || ii == wfin4){
      std::cout<<"==>> Creating a winged fin: "<<ii<<std::endl;
      new G4PVPlacement(G4Transform3D(rotation, translation), "TGT1", LVWingedTargetFin, pvTargetMotherVol, false, ii, NumiData->pSurfChk);
    }
    else{
      std::cout<<"==>> Creating a standard fin: "<<ii<<std::endl;
      new G4PVPlacement(G4Transform3D(rotation, translation), "TGT1", LVTargetFin, pvTargetMotherVol, false, ii, NumiData->pSurfChk);
    }
      // Temporarily define a different name for each copy... debugging overlaps.
//
//    std::ostringstream aNameTmpStrStr; aNameTmpStrStr << "TGT1-" << ii;
//    std::string aNameTmpStr(aNameTmpStrStr.str());
//    std::cerr << " Translation for " << aNameTmpStr << " is " << translation << std::endl;
//    new G4PVPlacement(G4Transform3D(rotation, translation), aNameTmpStr , LVTargetFin, pvTargetMotherVol, false, ii, NumiData->pSurfChk);
  }

//
//   
  //Now create the bottom portion of the graphite target bewteen the pressing plate and cooling plate.

  G4VSolid* TargetBottomSolid;
  G4LogicalVolume* TargetBottomLV;
  TGT_z = NumiData->TotalTargetLength/2.0 + fNovaTargetExtraFlangeThick/2. ;
  TargetBottomSolid = new G4Box("TargetBottomSolid",NumiData->TargetSegWidth/2.0,
				(NumiData->TargetGraphiteHeight - NumiData->TargetSegHeight)/2.0, 
				NumiData->TotalTargetLength/2.);

  TargetBottomLV = new G4LogicalVolume(TargetBottomSolid,Target,"TargetBottomLV",0,0,0);

  TGT_x = 0.0;
  TGT_y = -NumiData->TargetSegHeight + NumiData->TargetSegWidth/2.0 -(NumiData->TargetGraphiteHeight - NumiData->TargetSegHeight)/2.0;
  TGT_z = NumiData->TotalTargetLength/2.0;
  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(TGT_x, TGT_y, TGT_z) - TargetMVOrigin;

  new G4PVPlacement(G4Transform3D(rotation, translation),"TargetBottom",TargetBottomLV,pvTargetMotherVol,false, 0, NumiData->pSurfChk);

  // ---------------------------------------------------------------
  // Target Cansiter, Flanges, and Be Windows
  // ---------------------------------------------------------------


  // Outside of the body of target container
  G4VSolid* OutsideCasingSolid;
  G4LogicalVolume* OutsideCasingLV;

  OutsideCasingSolid = new G4Tubs("OutsideCasingSolid",
				  NumiData->TargetOutsideCasingInRad  - mvGap,
				  NumiData->TargetOutsideCasingOutRad  - mvGap,
				  NumiData->TargetCasingLength/2.0  - mvGap,
				  0.0,360.*CLHEP::deg);
  translation=G4ThreeVector(0.,
			    -NumiData->TargetSegHeight/2.0 + NumiData->TargetSegWidth/2.0,
			    NumiData->TargetCasingZ0+NumiData->TargetCasingLength/2.0) - TargetMVOrigin;
//  translation[1] -= 6.5*CLHEP::mm;
  translation[1] = NumiData->TargetCasingY0;
//  std::cerr << " Params for position Outside casing TargetSegHeight" << NumiData->TargetSegHeight <<
//               " TargetSegWidth " << NumiData->TargetSegWidth/2.0  << 	" TargetCasingLength " 
//	       << NumiData->TargetCasingLength << std::endl;		    
  OutsideCasingLV = new G4LogicalVolume(OutsideCasingSolid,Al,"OutsideCasingLV",0,0,0);
  
//   std::cerr << " ... No Z translation... Translated by .. " << translation 
//             << " All dims - Gap L - 1 Z p0.0 Y p - TgtCasingY0" << std::endl;

   std::cerr << " TargetCasingZ0 " << 	NumiData->TargetCasingZ0 << " TargetMVOrigin "  << TargetMVOrigin << std::endl;
 new G4PVPlacement(0,translation,"OutsideCasing",OutsideCasingLV,pvTargetMotherVol,false,0,NumiData->pSurfChk);


  // Water channel of target container
  G4VSolid* CasingWaterSolid;
  G4LogicalVolume* CasingWaterLV;

  CasingWaterSolid = new G4Tubs("CasingWaterSolid",
				NumiData->TargetCasingWaterInRad - 2.0*mvGap,
				NumiData->TargetCasingWaterOutRad - 2.0*mvGap,
				NumiData->TargetCasingLength/2.0 - 2.0*mvGap,
				0.0,360.*CLHEP::deg);
  translation = G4ThreeVector(NumiData->TargetCasingX0,
			      NumiData->TargetCasingY0,
			      NumiData->TargetCasingZ0+NumiData->TargetCasingLength/2.0) - TargetMVOrigin;
  CasingWaterLV = new G4LogicalVolume(CasingWaterSolid,Water,"CasingWaterLV",0,0,0);
  new G4PVPlacement(0,translation,"CasingWater",CasingWaterLV,pvTargetMotherVol,false,0,NumiData->pSurfChk);


  // Inside of the body of the target container
  G4VSolid* InsideCasingSolid;
  G4LogicalVolume* InsideCasingLV;

  InsideCasingSolid = new G4Tubs("InsideCasingSolid",
				  NumiData->TargetInsideCasingInRad - 3.0*mvGap,
				  NumiData->TargetInsideCasingOutRad - 3.0*mvGap,
				  NumiData->TargetCasingLength/2.0 - 3.0*mvGap,
				  0.0,360.*CLHEP::deg);
  translation  = G4ThreeVector(NumiData->TargetCasingX0,
			       NumiData->TargetCasingY0,
			       NumiData->TargetCasingZ0+NumiData->TargetCasingLength/2.0) - TargetMVOrigin;
  InsideCasingLV = new G4LogicalVolume(InsideCasingSolid,Al,"InsideCasingLV",0,0,0);
  new G4PVPlacement(0,translation,"InsideCasing",InsideCasingLV,pvTargetMotherVol,false,0,NumiData->pSurfChk);
//
//  std::cerr << " InsideCasingSolid rads...  .. " << 	NumiData->TargetInsideCasingInRad 
//            << " " << NumiData->TargetInsideCasingOutRad << " Length " 
//	    << NumiData->TargetCasingLength << " and quit ... " << std::endl; 
//  exit(2);	    			
//
// Install an assembly volume, physically virtual, that simulates the LE target outside boundary. 
// 
  
  const double lengthInsideLEModel = 
     NumberOfNonBudalFinsInMETarget*(NumiData->TargetSegLength + NumiData->TargetSegPitch) + NumiData->TargetSegPitch - 0.2*CLHEP::mm;
 
  const double zOffsetBudal = NumiData->BudalHFVSLength + NumiData->BudalHFVSPitch + NumiData->BudalVFHSLength + NumiData->BudalVFHSPitch;    
  
  const double zOffInsideLEModel = lengthInsideLEModel/2. +  zOffsetBudal + 0.5*NumiData->TargetSegLength; // ???? ? 
   std::cerr << " Length Target Casing " << NumiData->TargetCasingLength << " LEModel " << lengthInsideLEModel 
             << " zOffInsideLEModel " << zOffInsideLEModel 
	     << " TargetCasingZ0 "  << NumiData->TargetCasingZ0 << " Budal Z-off " << 
	      zOffsetBudal << std::endl;
   std::cerr << " Width TGT + epsil " << NumiData->TargetVirtualCanisterWidth	<<
                " Height TGT + epsil " << NumiData->TargetVirtualCanisterHeight << std::endl;     
  // 
  // Make 4 thin sides..that can be use to detect detect particle escaping the target.. (related to MIPP analysis..)  
  //
  G4VSolid* InsideLEModelSolidVert = new G4Box("InsideLEModelVSolid",
				  0.1*CLHEP::mm,
				  NumiData->TargetVirtualCanisterHeight/2. - 0.050*CLHEP::mm,
				  lengthInsideLEModel/2.0 - 0.01*CLHEP::mm);
  G4VSolid* InsideLEModelSolidHor = new G4Box("InsideLEModelHSolid",
				  NumiData->TargetVirtualCanisterWidth/2.,
				  0.1*CLHEP::mm,
				  lengthInsideLEModel/2.0 - 0.01*CLHEP::mm); 
 const double yOffsetLEModel = -NumiData->TargetSegHeight/2.0 + NumiData->TargetSegWidth/2.0; // same as the fins.. but weird.. 
 std::cerr << " yOffsetLEModel " << yOffsetLEModel << std::endl;				  
 G4LogicalVolume* InsideLEModelVertLV = new G4LogicalVolume(InsideLEModelSolidVert, TargetHelium,"InsideLEModelSolidVertLV",0,0,0);
 translation  = G4ThreeVector( - (NumiData->TargetVirtualCanisterWidth/2. + 0.15*CLHEP::mm),
			       yOffsetLEModel,
			       zOffInsideLEModel) - TargetMVOrigin;
  std::cerr << "  Translation for InsideLEModelVertLV is " << translation << std::endl;
  new G4PVPlacement(0,translation,"InsideLEModelVertLeft", InsideLEModelVertLV, pvTargetMotherVol,false,0,NumiData->pSurfChk);
 translation  = G4ThreeVector(NumiData->TargetCasingX0 + (NumiData->TargetVirtualCanisterWidth/2. + 0.015*CLHEP::mm),
			       yOffsetLEModel,
			       zOffInsideLEModel) - TargetMVOrigin;
  new G4PVPlacement(0,translation,"InsideLEModelVertRight", InsideLEModelVertLV, pvTargetMotherVol,false,1,NumiData->pSurfChk);

  G4LogicalVolume* InsideLEModelHorLV = new G4LogicalVolume(InsideLEModelSolidHor, TargetHelium,"InsideLEModelSolidHorLV",0,0,0);
  translation  = G4ThreeVector(0.0,
			       yOffsetLEModel - (NumiData->TargetVirtualCanisterHeight/2. + 0.15*CLHEP::mm),
			       zOffInsideLEModel) - TargetMVOrigin;
  std::cerr << " Delta Y for InsideLEModelHorBot	" << yOffsetLEModel - (NumiData->TargetVirtualCanisterHeight/2. + 0.15*CLHEP::mm) << 
              " Delta Z " << NumiData->TargetCasingZ0+zOffInsideLEModel << " Translation " << translation << std::endl;
	      		       
//
// We do not place this volumes. I clashed with target botton.. So we have a real volume on which we can trigger 
// for detecting the tracks that escape the NoVa target. 
//
//  new G4PVPlacement(0,translation,"InsideLEModelHorBot", InsideLEModelHorLV, pvTargetMotherVol,false,0,NumiData->pSurfChk);
  translation  = G4ThreeVector(0.0,
			       yOffsetLEModel + (NumiData->TargetVirtualCanisterHeight/2. + 0.15*CLHEP::mm),
			       zOffInsideLEModel) - TargetMVOrigin;
  new G4PVPlacement(0,translation,"InsideLEModelHorTop", InsideLEModelHorLV, pvTargetMotherVol,false,0,NumiData->pSurfChk);

  G4VSolid* InsideLEModelSolidVertEnd = new G4Box("InsideLEModelESolid",
				  NumiData->TargetVirtualCanisterWidth/2. - 0.05*CLHEP::mm,
				  NumiData->TargetVirtualCanisterHeight/2. - 0.05*CLHEP::mm,
				  0.1*CLHEP::mm);
  G4LogicalVolume* InsideLEModelVertEndLV = new G4LogicalVolume(InsideLEModelSolidVertEnd, TargetHelium,"InsideLEModelSolidVertELV",0,0,0);
  const double zOffInsideLEModelEnd =  zOffsetBudal + NumiData->TargetSegLength + 
                                        NumberOfNonBudalFinsInMETarget*(NumiData->TargetSegLength + NumiData->TargetSegPitch);
				  
  translation  = G4ThreeVector(0.,
			       yOffsetLEModel,
			       zOffInsideLEModelEnd) - TargetMVOrigin;
  new G4PVPlacement(0,translation,"InsideLEModelVertEndTop", InsideLEModelVertEndLV, pvTargetMotherVol,false,0,NumiData->pSurfChk);

  // Downstream Flange
  G4VSolid* TargetDnFlangeSolid;
  G4VSolid* DnFlangeCutout;
  G4VSolid* DnFlangeHole;
  G4LogicalVolume* TargetDnFlangeLV;

  TargetDnFlangeSolid = new G4Tubs("TargetDnFlangeSolid", 0., 
				   NumiData->TargetDnFlangeOutRad, effTargetDnFlangeLength/2.0, 
				   0., 360.*CLHEP::deg);

  DnFlangeCutout = new G4Tubs("DnFlangeCutout", 0., 
			      NumiData->TargetInsideCasingInRad, NumiData->TargetDnFlangeCutoutLength/2.0, 
			      0., 360.*CLHEP::deg);
  
  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(0., 0., -(effTargetDnFlangeLength - NumiData->TargetDnFlangeCutoutLength)/2.0);
  TargetDnFlangeSolid = new G4SubtractionSolid("TargetDnFlangeSolid", TargetDnFlangeSolid, 
					       DnFlangeCutout, G4Transform3D(rotation, translation));

  DnFlangeHole = new G4Tubs("DnFlangeHole", 0., 
			    NumiData->TargetDnBeWindowRadius, effTargetDnFlangeLength/2.0, 
			    0., 360.*CLHEP::deg);

  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(0., -NumiData->TargetCanisterCenterOffset, 0.);
  TargetDnFlangeSolid = new G4SubtractionSolid("TargetDnFlangeSolid", TargetDnFlangeSolid, 
					       DnFlangeHole, G4Transform3D(rotation, translation));

  TargetDnFlangeLV = new G4LogicalVolume(TargetDnFlangeSolid, Al, "TargetDnFlangeLV", 0, 0, 0);
  translation = G4ThreeVector(NumiData->TargetDnFlangeX0,
			      NumiData->TargetDnFlangeY0,
			      NumiData->TargetDnFlangeZ0 + effTargetDnFlangeLength/2.0) - TargetMVOrigin;

  new G4PVPlacement(0, translation, "TargetDnFlange", TargetDnFlangeLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);


  // Downstream Be window.
  G4VSolid* TargetDnBeWindowSolid;
  G4LogicalVolume* TargetDnBeWindowLV;

  TargetDnBeWindowSolid = new G4Tubs("TargetDnBeWindowSoild", 0., 
			      NumiData->TargetDnBeWindowRadius, NumiData->TargetDnBeWindowLength/2.0, 
			      0., 360.*CLHEP::deg);
  TargetDnBeWindowLV = new G4LogicalVolume(TargetDnBeWindowSolid, Be, "TargetDnBeWindowLV", 0, 0, 0);

  translation=G4ThreeVector(NumiData->TargetDnFlangeX0,
			    NumiData->TargetDnFlangeY0 - NumiData->TargetCanisterCenterOffset,
			    NumiData->TargetDnFlangeZ0 + NumiData->TargetDnFlangeCutoutLength + NumiData->TargetDnBeWindowLength/2.0) 
    - TargetMVOrigin;
  
  new G4PVPlacement(0, translation, "TargetDnBeWindow", TargetDnBeWindowLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);


  // Upstream Flange
  G4VSolid* TargetUpFlangeSolid;
  G4VSolid* UpFlangeHole;
  G4LogicalVolume* TargetUpFlangeLV;


  TargetUpFlangeSolid = new G4Tubs("TargetUpFlangeSolid", 
				   0., NumiData->TargetUpFlangeOutRad, NumiData->TargetUpFlangeLength/2.0, 
				   0., 360.*CLHEP::deg);

  UpFlangeHole = new G4Tubs("UpFlangeHole", 
			    0., NumiData->TargetUpBeWindowRadius, NumiData->TargetUpFlangeLength/2.0, 
			    0., 360.*CLHEP::deg);

  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(0., -NumiData->TargetCanisterCenterOffset, 0.);
  TargetUpFlangeSolid = new G4SubtractionSolid("TargetUpFlangeSolid", TargetUpFlangeSolid, 
					       UpFlangeHole, G4Transform3D(rotation, translation));



  TargetUpFlangeLV = new G4LogicalVolume(TargetUpFlangeSolid, Al, "TargetUpFlangeLV", 0, 0, 0);
  translation = G4ThreeVector(NumiData->TargetUpFlangeX0,
			      NumiData->TargetUpFlangeY0,
			      NumiData->TargetUpFlangeZ0 + NumiData->TargetUpFlangeLength/2.0) - TargetMVOrigin;


  new G4PVPlacement(0, translation, "TargetUpFlange", TargetUpFlangeLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);


  // Upstream Be Flange
  G4VSolid* TargetUpBeFlangeSolid;
  G4VSolid* UpBeFlangeHole;
  G4VSolid* UpBeFlangeCutout;
  G4LogicalVolume* TargetUpBeFlangeLV;


  TargetUpBeFlangeSolid = new G4Tubs("TargetUpBeFlangeSolid", 
				   0., NumiData->TargetUpBeFlangeOutRad, NumiData->TargetUpBeFlangeLength/2.0, 
				   0., 360.*CLHEP::deg);

  G4double fudge_factor = 0.1*CLHEP::mm; // This makes the graphics work for HepRapp.

  UpBeFlangeCutout = new G4Tubs("UpBeFlangeCutout", 
  				0., NumiData->TargetUpBeFlangeCutoutRadius, (NumiData->TargetUpBeFlangeCutoutLength+fudge_factor)/2.0, 
				0., 360.*CLHEP::deg);

  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(0., 0., -(NumiData->TargetUpBeFlangeLength - NumiData->TargetUpBeFlangeCutoutLength)/2.0 -fudge_factor/2.0);
  TargetUpBeFlangeSolid = new G4SubtractionSolid("TargetUpBeFlangeSolid", TargetUpBeFlangeSolid,
						 UpBeFlangeCutout, G4Transform3D(rotation, translation));

  UpBeFlangeHole = new G4Tubs("UpBeFlangeHole", 
			    0., NumiData->TargetUpBeWindowRadius, NumiData->TargetUpBeFlangeLength/2.0, 
			    0., 360.*CLHEP::deg);

  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(0., 0., 0.);
  TargetUpBeFlangeSolid = new G4SubtractionSolid("TargetUpBeFlangeSolid", TargetUpBeFlangeSolid, 
					       UpBeFlangeHole, G4Transform3D(rotation, translation));

  TargetUpBeFlangeLV = new G4LogicalVolume(TargetUpBeFlangeSolid, Al, "TargetUpBeFlangeLV", 0, 0, 0);
  translation = G4ThreeVector(NumiData->TargetUpBeFlangeX0,
			      NumiData->TargetUpBeFlangeY0,
			      NumiData->TargetUpBeFlangeZ0 + NumiData->TargetUpBeFlangeLength/2.0) - TargetMVOrigin;

  new G4PVPlacement(0, translation, "TargetUpBeFlange", TargetUpBeFlangeLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);


  // Upstream Be window.
  G4VSolid* TargetUpBeWindowSolid;
  G4LogicalVolume* TargetUpBeWindowLV;

  TargetUpBeWindowSolid = new G4Tubs("TargetUpBeWindowSoild", 
				     0., NumiData->TargetUpBeWindowRadius, NumiData->TargetUpBeWindowLength/2.0, 
				     0., 360.*CLHEP::deg);
  TargetUpBeWindowLV = new G4LogicalVolume(TargetUpBeWindowSolid, Be, "TargetUpBeWindowLV", 0, 0, 0);

  translation=G4ThreeVector(NumiData->TargetUpBeFlangeX0,
			    NumiData->TargetUpBeFlangeY0,
			    NumiData->TargetUpBeFlangeZ0 + NumiData->TargetUpBeFlangeCutoutLength + NumiData->TargetUpBeWindowLength/2.0) 
    - TargetMVOrigin;
  
  new G4PVPlacement(0, translation, "TargetUpBeWindow", TargetUpBeWindowLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);


  //  vol_name    = "TGTExitCyl1";
  //  sFinalSolid = new G4Tubs(vol_name, 0., NumiData->UpstrFlangeRout, NumiData->UpstrFlangeL, 0., 360.*CLHEP::deg);
  //  sTempSolid  = new G4Tubs(vol_name, 0., NumiData->DwstrBerWindowRout, NumiData->UpstrFlangeL + 2.*CLHEP::mm, 0., 360.*CLHEP::deg);
  //  HolePos     = G4ThreeVector(0.,0.,0.) - containerDisplacement;
  //  rotation    = G4RotationMatrix(0,0,0);
  //  sFinalSolid = new G4SubtractionSolid(vol_name, sFinalSolid, sTempSolid, G4Transform3D(rotation, HolePos));

  //  G4ThreeVector finalDest = G4ThreeVector(0., 0., NumiData->TargetContHalfLength * 2 - NumiData->UpstrFlangeL) - TargetMVOrigin + containerDisplacement;
  
  // lvUpstrFlange = new G4LogicalVolume(sFinalSolid , GetMaterial(NumiData->UpstrFlangeMat), vol_name.append("LV"), 0, 0, 0);
  // new G4PVPlacement(0, finalDest, vol_name, lvUpstrFlange, pvTargetMotherVol, false, 1,NumiData->pSurfChk);



  // ---------------------------------------------------------------
  // Cooling Channels
  // ---------------------------------------------------------------


  // Pressing Plate
  G4VSolid*        PressingPlateSolid;
  G4VSolid*        PressingPlateCutoutSolid;
  G4LogicalVolume* PressingPlateLV;

  PressingPlateSolid = new G4Box("PressingPlateSolid",NumiData->PressingPlateWidth/2.0,
				 NumiData->PressingPlateHeight/2.0, NumiData->PressingPlateLength/2.0);

  PressingPlateCutoutSolid = new G4Box("PressingPlateCutoutSolid",NumiData->PressingPlateCutoutWidth/2.0,
				 NumiData->PressingPlateCutoutHeight/2.0, NumiData->PressingPlateLength/2.0);


  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(NumiData->PressingPlateWidth/2.0 - NumiData->PressingPlateCutoutWidth/2.0, 0.0, 0.0);

  PressingPlateSolid = new G4SubtractionSolid("PressingPlateSolid", PressingPlateSolid,
					      PressingPlateCutoutSolid, G4Transform3D(rotation, translation));

  PressingPlateLV = new G4LogicalVolume(PressingPlateSolid, Al, "PressingPlateLV", 0, 0, 0);
  translation = G4ThreeVector(NumiData->PressingPlateX0,
			      NumiData->PressingPlateY0,
			      NumiData->PressingPlateZ0 + NumiData->PressingPlateLength/2.0) - TargetMVOrigin;

  new G4PVPlacement(0, translation, "PressingPlate", PressingPlateLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);



  // Cooling Plate
  G4VSolid*        CoolingPlateSolid;
  G4VSolid*        CoolingPlateCutoutSolid;
  G4VSolid*        CoolingWaterSolid;

  G4LogicalVolume* CoolingPlateLV;
  G4LogicalVolume* CoolingWaterLV;


  CoolingPlateSolid = new G4Box("CoolingPlateSolid",NumiData->CoolingPlateWidth/2.0,
				 NumiData->CoolingPlateHeight/2.0, NumiData->CoolingPlateLength/2.0);

  CoolingPlateCutoutSolid = new G4Box("CoolingPlateCutoutSolid",NumiData->CoolingPlateCutoutWidth/2.0,
				 NumiData->CoolingPlateCutoutHeight/2.0, NumiData->CoolingPlateLength/2.0);

  CoolingWaterSolid = new G4Tubs("CoolingWaterSolid", 
				 0., NumiData->CoolingWaterPipeOutRad, NumiData->CoolingPlateLength/2.0,
				 0., 360.*CLHEP::deg);



  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(-NumiData->CoolingPlateWidth/2.0 + NumiData->CoolingPlateCutoutWidth/2.0, 0.0, 0.0);

  CoolingPlateSolid = new G4SubtractionSolid("CoolingPlateSolid", CoolingPlateSolid,
					     CoolingPlateCutoutSolid, G4Transform3D(rotation, translation));

  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(NumiData->CoolingWaterPipeX0, NumiData->CoolingWaterPipeY0, 0.0);

  CoolingPlateSolid = new G4SubtractionSolid("CoolingPlateSolid", CoolingPlateSolid,
					      CoolingWaterSolid, G4Transform3D(rotation, translation));


  rotation=G4RotationMatrix(0,0,0);
  translation=G4ThreeVector(NumiData->CoolingWaterPipeX0, -NumiData->CoolingWaterPipeY0,0.0);

  CoolingPlateSolid = new G4SubtractionSolid("CoolingPlateSolid", CoolingPlateSolid,
					      CoolingWaterSolid, G4Transform3D(rotation, translation));



  CoolingPlateLV = new G4LogicalVolume(CoolingPlateSolid, Al, "CoolingPlateLV", 0, 0, 0);

  translation = G4ThreeVector(NumiData->CoolingPlateX0,
			      NumiData->CoolingPlateY0,
			      NumiData->CoolingPlateZ0 + NumiData->CoolingPlateLength/2.0) - TargetMVOrigin;
  new G4PVPlacement(0, translation, "CoolingPlate", CoolingPlateLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);


  // Water channels

  CoolingWaterLV = new G4LogicalVolume(CoolingWaterSolid, Water, "CoolingWaterLV", 0, 0, 0);


  translation = G4ThreeVector(NumiData->CoolingPlateX0 + NumiData->CoolingWaterPipeX0,
			      NumiData->CoolingPlateY0 + NumiData->CoolingWaterPipeY0,
			      NumiData->CoolingPlateZ0 + NumiData->CoolingPlateLength/2.0) - TargetMVOrigin;

  new G4PVPlacement(0, translation, "CoolingWater", CoolingWaterLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);


  translation = G4ThreeVector(NumiData->CoolingPlateX0 + NumiData->CoolingWaterPipeX0,
			      NumiData->CoolingPlateY0 - NumiData->CoolingWaterPipeY0,
			      NumiData->CoolingPlateZ0 + NumiData->CoolingPlateLength/2.0) - TargetMVOrigin;

  new G4PVPlacement(0, translation, "CoolingWater", CoolingWaterLV, pvTargetMotherVol, false, 0, NumiData->pSurfChk);

  G4cout << "NOvA Medium Energy Target Constructed" << G4endl;
}
