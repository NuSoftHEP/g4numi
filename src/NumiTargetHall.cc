//----------------------------------------------------------------------
// Target hall chase and duratek blocks modifications by Zachary Barnett.
// $Id: NumiTargetHall.cc,v 1.11.4.5 2014/12/10 16:18:34 lebrun Exp $
//----------------------------------------------------------------------
#include "NumiDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "NumiDataInput.hh" 

void NumiDetectorConstruction::ConstructTargetHall()
{

        // Trung, 2011/20/21: Copy comment from NumiDataInput.cc.me_target
        // Need to change the block positions for the Nova/ME target. Must be here,
        // not in the constructor, to make sure /det/update work properly
        // Mike Martens (Need to move Horn 2 by 9 meters downstream)
    if (NumiData->THBlockNblock >=23) {
        NumiData->THBlockZ0.at(21) =  1.15*m + fTHBlockShift;
        NumiData->THBlockZ0.at(22) = 30.34*m + fTHBlockShift;
        NumiData->THBlockZ0.at(23) = 12.00*m + fTHBlockShift*2;
    }
    
//TGAR
  G4double TGAR_width=NumiData->TargetAreaWidth/2.;
  G4double TGAR_height=NumiData->TargetAreaHeight/2.;
  G4double TGAR_length=NumiData->TargetAreaLength/2.;
  G4ThreeVector targetHallPosition=G4ThreeVector(0,0,NumiData->TargetAreaLength/2.+NumiData->TargetAreaZ0);

  G4Box* sTGAR=new G4Box("sTGAR",TGAR_width,TGAR_height,TGAR_length);
  G4LogicalVolume *lvTGAR=new G4LogicalVolume(sTGAR,Air,"lvTGAR",0,0,0);
  TGAR = new G4PVPlacement(0,targetHallPosition,"TGAR",lvTGAR,ROCK,false,0);
 
  NumiData->ApplyStepLimits(lvTGAR);
  if (NumiData->NTHConcreteSectionsN!=0){
    // Begin constructing Target Hall Concrete Chase
    //=====================================================
    
    G4VSolid* tempConcrete1; // solid used for concrete sections 0 and 4
    G4VSolid* tempConcrete2; // solid used for concrete sections 1 and 3
    G4VSolid* tempConcrete3; // solid used for concrete section 2
    G4VSolid* tempConcrete4; // solid used for concrete sections 5 (the covering)

    G4RotationMatrix rotation; 
    G4ThreeVector translation;
    
    tempConcrete1 = new G4Box("sConcreteSection0and4", NumiData->THConcreteHdx[0], NumiData->THConcreteHdy[0], NumiData->THConcreteLength[0]/2.0);
    tempConcrete2 = new G4Box("sConcreteSection1and3", NumiData->THConcreteHdx[1], NumiData->THConcreteHdy[1], NumiData->THConcreteLength[1]/2.0);
    tempConcrete3 = new G4Box("sConcreteSection2", NumiData->THConcreteHdx[2], NumiData->THConcreteHdy[2], NumiData->THConcreteLength[2]/2.0);
    tempConcrete4 = new G4Box("sConcreteSection5Lid", NumiData->THConcreteHdx[5], NumiData->THConcreteHdy[5], NumiData->THConcreteLength[5]/2.0);
    
    G4LogicalVolume *lvConcreteSection0and4 = new G4LogicalVolume(tempConcrete1, Concrete, "lvConcreteSection0and4",0,0,0);
    G4LogicalVolume *lvConcreteSection1and3 = new G4LogicalVolume(tempConcrete2, Concrete, "lvConcreteSection1and3",0,0,0);
    G4LogicalVolume *lvConcreteSection2 = new G4LogicalVolume(tempConcrete3, Concrete, "lvConcreteSection2",0,0,0);
    G4LogicalVolume *lvConcreteSection5 = new G4LogicalVolume(tempConcrete4, Concrete, "lvConcreteSection5",0,0,0);
    
    // loop to place all the concrete sections
    for (G4int ii=0;ii<NumiData->NTHConcreteSectionsN;ii++) {
      
      translation = G4ThreeVector(NumiData->THConcreteX0[ii], NumiData->THConcreteY0[ii], NumiData->THConcreteZ0[ii]) - targetHallPosition;
      
      // place sections 0 and 4
      if (ii == 0 || ii == 4) {
	new G4PVPlacement(0, translation, "Concrete Chase Section", lvConcreteSection0and4, TGAR, false, 0);
	 }
      //place sections 1 and 3
      else if (ii == 1 || ii == 3) {
	new G4PVPlacement(0, translation, "Concrete Chase Section", lvConcreteSection1and3, TGAR, false, 0);
      }
      // place section 2
      else if (ii==2) {
	new G4PVPlacement(0, translation, "Concrete Chase Section", lvConcreteSection2, TGAR, false, 0);
      }
      // place section 5
	 else if (ii == 5) {
	   new G4PVPlacement(0, translation, "Concrete Chase Section", lvConcreteSection5, TGAR, false, 0);
	 }
    }
    G4cout <<"Concrete Chase Sections constructed" << G4endl;
    
    //======================================================
    //end Target Hall Concrete Chase Construction
    
    // Begin constructing Duratek Blocks
    //======================================================
    
    // tempDuratekBlock is the solid used for the Duratek Block construction
    G4VSolid* tempDuratekBlock;
    // tempCovering is a Duratek Block with slightly modified dimensions.  Two of these form the topmost Duratek covering
    G4VSolid* tempCovering;
    
    G4RotationMatrix *tallrotation;
    rotation = G4RotationMatrix(0.,0.,0.);
    tallrotation = new G4RotationMatrix(0.0, 0.0,90.0*deg);
  
    tempDuratekBlock = new G4Box("sDuratekBlock", NumiData->THBlockHdx[0], NumiData->THBlockHdy[0], NumiData->THBlockLength[0]/2.0);
    G4LogicalVolume *lvDuratekBlock = new G4LogicalVolume(tempDuratekBlock, Fe, "lvDuratekBlock",0,0,0);
    
    // tempCovering is a Duratek Block with slightly modified dimensions.  Two of these form the topmost Duratek covering
    // the tempCovering dimensions are set in the following line
    tempCovering = new G4Box("sDuratekBlockCovering", (1.3554/2.0)*m, (0.665/2.0)*m, NumiData->THBlockLength[0]/2.0);
    G4LogicalVolume *lvDuratekBlockCovering = new G4LogicalVolume(tempCovering, Fe, "lvDuratekBlockCovering",0,0,0);

    //
    //For gnumi like target hall
    //(only used for NumiData->g3Chase is true)
    //---------------------------------------------------------------------------------------------------------
    G4VSolid* tempgnumiA;
    G4VSolid* tempgnumiBC;
    G4VSolid* tempgnumiD; //before
    G4VSolid* tempgnumiD2; // after
    G4VSolid* tempgnumiD3; // above horn 2
    tempgnumiA = new G4Box("sDuratekgnumiA", 0.5842*m , 0.0407125*m , NumiData->THBlockLength[0]/2);
    G4LogicalVolume *lvDuratekBlockgnumiA  = new G4LogicalVolume(tempgnumiA, Fe, "lvDuratekBlockgnumiA", 0, 0, 0);
    
    tempgnumiBC = new G4Box("sDuratekgnumiBC", 0.0531*m ,0.99625*m, NumiData->THBlockLength[0]/2);
    G4LogicalVolume *lvDuratekBlockgnumiBC = new G4LogicalVolume(tempgnumiBC, Fe, "lvDuratekBlockgnumiBC", 0, 0, 0);

      // Mike Martens (Need to move Horn 2 by 9 meters downstream)
    tempgnumiD = new G4Box("sDuratekgnumiD", 0.5842*m,0.301475*m, 7.85*m + fDuratekShift);
    G4LogicalVolume *lvDuratekBlockgnumiD = new G4LogicalVolume(tempgnumiD, Fe, "lvDuratekBlockgnumiD", 0, 0,0);
    tempgnumiD3 = new G4Box("sDuratekgnumiD3", 0.5842*m, 0.286475*m, (6/2)*m);
    G4LogicalVolume *lvDuratekBlockgnumiD3 = new G4LogicalVolume(tempgnumiD3, Fe, "lvDuratekBlockgnumiD3",0,0,0);
    tempgnumiD2 = new G4Box("sDuratekgnumiD2", 0.5842*m, 0.301475*m,15.349*m - fDuratekShift);
    G4LogicalVolume *lvDuratekBlockgnumiD2 = new G4LogicalVolume(tempgnumiD2, Fe, "lvDuratekBlockgnumiD2",0,0,0);


    //-------------------------------------------------------------------------------------------------------
   
    G4ThreeVector DuratekCasing_position;
    
    //loop to place the Duratek blocks
    for(G4int ii=0; ii<NumiData->THBlockNblock; ii++) {
      
      DuratekCasing_position = G4ThreeVector(NumiData->THBlockX0[ii], NumiData->THBlockY0[ii], NumiData->THBlockZ0[ii]) - targetHallPosition ;

      // if statement controls which blocks are rotated about the Z axis for placement.  Also places the modified top covering blocks
      if (ii == 3 || (ii >= 6 && ii < 15)) {
	new G4PVPlacement(tallrotation, DuratekCasing_position, "DuratekBlock", lvDuratekBlock, TGAR, false, 0); 
      }
      else {
	if (ii == 15 || ii == 16) {
			 new G4PVPlacement(0, DuratekCasing_position, "DuratekBlockCovering", lvDuratekBlockCovering, TGAR, false, 0);    
	}
	else if(ii>=18 &&NumiData->g3Chase){//Added by Jasmine to make more like gnumi. 
	  if(ii==18) new G4PVPlacement(0, DuratekCasing_position,"DuratekBlockgnumi", lvDuratekBlockgnumiA, TGAR, false, 0);    
	  if(ii==19 || ii==20) new G4PVPlacement(0, DuratekCasing_position,"DuratekBlockgnumi", lvDuratekBlockgnumiBC, TGAR, false, 0);
	  if(ii==21) new G4PVPlacement(0, DuratekCasing_position, "DuratekBlockgnumi", lvDuratekBlockgnumiD, TGAR, false, 0);
          if(ii==22) new G4PVPlacement(0, DuratekCasing_position, "DuratekBlockgnumi", lvDuratekBlockgnumiD2, TGAR, false, 0);
          if(ii==23) new G4PVPlacement(0, DuratekCasing_position, "DuratekBlockgnumi", lvDuratekBlockgnumiD3, TGAR, false, 0);
   
	}
	
	else {
	  new G4PVPlacement(0, DuratekCasing_position, "DuratekBlock", lvDuratekBlock, TGAR, false, 0);     
	}
      }
    }
    G4cout <<"Duratek Block constructed" << G4endl;
    
    //=====================================================
    // end Duratek Block Construction
  
    
    G4cout << "Target Hall Constructed" << G4endl;
  }
  else {
    // G4VisAttributes invisible=G4VisAttributes(false);
    //Blocks
    for (G4int ii=0;ii<NumiData->THBlockNblock;ii++){
      char no[3];
      sprintf(no,"%d",ii+1);
      G4String volName="sTHBLK";
      volName.append(no);
      BLK_solid[ii] = new G4Box(volName,NumiData->THBlockHdx[ii],NumiData->THBlockHdy[ii],NumiData->THBlockLength[ii]/2.);
      volName="lvTHBLK"; volName.append(no);
      G4Material* material=GetMaterial(NumiData->THBlockGeantMaterial[ii]);
      G4LogicalVolume *lvTHBLK = new G4LogicalVolume(BLK_solid[ii],material,volName,0,0,0);
      //      BLK_log[ii]->SetVisAttributes(invisible);
      volName="THBLK"; volName.append(no);
      G4ThreeVector blockPosition=G4ThreeVector(NumiData->THBlockX0[ii],NumiData->THBlockY0[ii],NumiData->THBlockZ0[ii]+NumiData->THBlockLength[ii]/2.)-targetHallPosition;
      new G4PVPlacement(0,blockPosition,volName,lvTHBLK,TGAR,false,0);
    }
    G4cout << "GNUMI like Target Hall Constructed" << G4endl;
  }
}
 
