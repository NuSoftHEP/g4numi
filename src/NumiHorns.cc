#include "NumiDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4BREPSolidPCone.hh"
#include "NumiMagneticField.hh"
#include "G4FieldManager.hh"

void NumiDetectorConstruction::ConstructHorns()
{
  // Mother Volume position
  G4ThreeVector target_hall_position=G4ThreeVector(0,0,NumiData->TargetAreaLength/2.+NumiData->TargetAreaZ0);
  G4Material* material;
  //Horns
    G4double z[20];
    G4double rmin[20];
    G4double rmax[20];
    // Create mother volumes 
    for (G4int jj=0;jj<NumiData->PhornNphorn;jj++){
      for (G4int ii=0;ii<NumiData->PhornNpoint[jj]+1;ii++){
	z[ii]=((NumiData->PhornZ2[jj]-NumiData->PhornZ1[jj])/NumiData->PhornNpoint[jj]*ii);
	rmin[ii]=phornRgivenZ(NumiData->PhornAin[jj],NumiData->PhornBin[jj],NumiData->PhornCin[jj],z[ii]+NumiData->PhornZ1[jj]);
	rmax[ii]=NumiData->PhornROCout[jj];
      }
      char no[3];
      sprintf(no,"%d",jj+1);
      G4String vol_name="PM0";
      vol_name.append(no);
      vol_name.append("_solid"); 
      Horn_PM[jj]=new G4BREPSolidPCone(vol_name,0.,360.*deg,NumiData->PhornNpoint[jj]+1,0.,z,rmin,rmax);
      Horn_PM_lv[jj]=new G4LogicalVolume(Horn_PM[jj],Air,"Horn_PM_lv",0,0,0);
      vol_name="PM0";
      vol_name.append(no);
      vol_name.append("_lv"); 
      PHORN[jj]=new G4PVPlacement(0,G4ThreeVector(0,0,NumiData->PhornZ0[jj]+NumiData->PhornZ1[jj])-target_hall_position, vol_name,Horn_PM_lv[jj],TGAR,false,0);

      // Inner conductor
      for (G4int ii=0;ii<NumiData->PhornNpoint[jj]+1;ii++){
	z[ii]=((NumiData->PhornZ2[jj]-NumiData->PhornZ1[jj])/NumiData->PhornNpoint[jj]*ii);
	rmin[ii]=phornRgivenZ(NumiData->PhornAin[jj],NumiData->PhornBin[jj],NumiData->PhornCin[jj],z[ii]+NumiData->PhornZ1[jj]);
	rmax[ii]=phornRgivenZ(NumiData->PhornAout[jj],NumiData->PhornBout[jj],NumiData->PhornCout[jj],z[ii]+NumiData->PhornZ1[jj]);
      }

      G4BREPSolidPCone* Horn_in=new G4BREPSolidPCone("Horn_in",0.,360.*deg,NumiData->PhornNpoint[jj]+1,0.,z,rmin,rmax);
      material=GetMaterial(NumiData->PhornGEANTmat[jj]);
      G4LogicalVolume* Horn_in_lv=new G4LogicalVolume(Horn_in,material,"Horn_in_lv",0,0,0);
      // Magnetic field
      G4FieldManager* FieldMgr = new G4FieldManager(numiMagFieldIC); //create a local field		 
      FieldMgr->SetDetectorField(numiMagFieldIC); //set the field 
      FieldMgr->CreateChordFinder(numiMagFieldIC); //create the objects which calculate the trajectory
      Horn_in_lv->SetFieldManager(FieldMgr,true); //attach the local field to logical volume

      vol_name="PI0";
      vol_name.append(no);
      new G4PVPlacement(0,G4ThreeVector(0,0,0), vol_name,Horn_in_lv,PHORN[jj],false,0);

      // Outer conductor
      G4Tubs* Horn_out=new 
      	G4Tubs("Horn_out",NumiData->PhornROCin[jj],NumiData->PhornROCout[jj],(NumiData->PhornZ2[jj]-NumiData->PhornZ1[jj])/2.,0.,360.*deg);
      material=GetMaterial(NumiData->PhornGEANTmat[jj]);
      G4LogicalVolume* Horn_out_lv=new G4LogicalVolume(Horn_out,material,"Horn_out_lv",0,0,0);
      // Magnetic field
      G4FieldManager* FieldMgr2 = new G4FieldManager(numiMagFieldOC); //create a local field
      FieldMgr2->SetDetectorField(numiMagFieldOC); //set the field 
      FieldMgr2->CreateChordFinder(numiMagFieldOC); //create the objects which calculate the trajectory
      Horn_out_lv->SetFieldManager(FieldMgr2,true); //attach the local field to logical volume

      vol_name="PO0";
      vol_name.append(no);
      new G4PVPlacement(0,G4ThreeVector(0,0,((NumiData->PhornZ2[jj]-NumiData->PhornZ1[jj])/2.)), vol_name,Horn_out_lv,PHORN[jj],false,0);
     
      // Inside horns (field part)
      for (G4int kk=0;kk<NumiData->PhornNpoint[jj]+1;kk++){
	rmin[kk]=phornRgivenZ(NumiData->PhornAout[jj],NumiData->PhornBout[jj],NumiData->PhornCout[jj], z[kk]+NumiData->PhornZ1[jj]);
	rmax[kk]=NumiData->PhornROCin[jj];
      }
      G4BREPSolidPCone* Horn_inside=new G4BREPSolidPCone("Horn_inside",0.,360.*deg,NumiData->PhornNpoint[jj]+1,0.,z,rmin,rmax);
      G4LogicalVolume* Horn_inside_lv=new G4LogicalVolume(Horn_inside,Vacuum,"Horn_inside_lv",0,0,0);
      // Magnetic field  
      G4FieldManager* FieldMgr3 = new G4FieldManager(numiMagField); //create a local field      
      FieldMgr3->SetDetectorField(numiMagField); //set the field 
      FieldMgr3->CreateChordFinder(numiMagField); //create the objects which calculate the trajectory 
      Horn_inside_lv->SetFieldManager(FieldMgr3,true); //attach the local field to logical volume

vol_name="PF0";
      vol_name.append(no);
      new G4PVPlacement(0,G4ThreeVector(0,0,0), vol_name,Horn_inside_lv,PHORN[jj],false,0);
    
    // Front and End cover
      if (NumiData->PhornThickFront[jj]!=0){
	vol_name="PC0";
	vol_name.append(no);
	G4double rin=rmin[0];
	G4double rout=NumiData->PhornROCout[jj];
	G4Tubs* Horn_front_cover=new 
	  G4Tubs("Horn_front_cover",rin,rout,(NumiData->PhornThickFront[jj])/2.,0.,360.*deg);
	material=GetMaterial(NumiData->PhornGEANTmat[jj]);
	G4LogicalVolume* Horn_fc_lv=new G4LogicalVolume(Horn_front_cover,material,"Horn_front_cover_lv",0,0,0);
	G4ThreeVector PC_position=G4ThreeVector(0,0,NumiData->PhornZ0[jj]+NumiData->PhornZ1[jj]-(NumiData->PhornThickFront[jj])/2.)
	  -target_hall_position;
	new G4PVPlacement(0,PC_position, vol_name,Horn_fc_lv,TGAR,false,0);
      }
      if (NumiData->PhornThickEnd[jj]!=0){
	vol_name="PC0";
	vol_name.append(no);
	G4double rin=rmin[NumiData->PhornNpoint[jj]];
	G4double rout=NumiData->PhornROCout[jj];
	G4Tubs* Horn_end_cover=new 
	  G4Tubs("Horn_end_cover",rin,rout,(NumiData->PhornThickEnd[jj])/2.,0.,360.*deg);
	material=GetMaterial(NumiData->PhornGEANTmat[jj]);
	G4LogicalVolume* Horn_ec_lv=new G4LogicalVolume(Horn_end_cover,material,"Horn_end_cover_lv",0,0,0);
	G4ThreeVector PC_position=G4ThreeVector(0,0,NumiData->PhornZ0[jj]+NumiData->PhornZ2[jj]+(NumiData->PhornThickEnd[jj])/2.)
	  -target_hall_position;
	new G4PVPlacement(0,PC_position, vol_name,Horn_ec_lv,TGAR,false,0);
      }
    }
    G4cout << "Horns constructed" << G4endl;
     
}
G4double NumiDetectorConstruction::phornRgivenZ(G4double a, G4double b, G4double c, G4double z)
{
  G4double r;
  if (b!=0){
    if (a>0) 
      { r = sqrt((a-z)/b)+c;}
    else
      { r = sqrt((a+z)/b)+c;}
  }
  else
    {r=c;}

  return r;
}

