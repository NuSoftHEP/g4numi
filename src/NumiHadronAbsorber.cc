//----------------------------------------------------------------------
// NumiHadronAbsorber.cc covers the elements of the Hadron Absorber in
// the G4NuMI framework. The blocks and elements within the parent
// volumes are all oriented in a refernce frame that has gravity going
// straight down. To properly align the NuMI Decay tunnel and beam,
// which have a downward slope, with the elements of the MC, the parent
// volumes are rotated. In the G4NuMI world, the beam is horizontal,
// while all the MC elements are rotated. 
//
// $Id: NumiHadronAbsorber.cc,v 1.8.4.3 2014/01/22 22:31:07 kordosky Exp $
//----------------------------------------------------------------------

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
#include "G4PVReplica.hh"
#include "G4AssemblyVolume.hh"
#include "NumiDataInput.hh"
#include <math.h>

void NumiDetectorConstruction::ConstructHadronAbsorber()
{
  G4cout << "Starting Hadron Absorber construction... ";
  G4ThreeVector tunnelPos = G4ThreeVector(0,0,NumiData->TunnelLength/2.+NumiData->TunnelZ0);
  G4double in = .0254*m;
  //G4double ft = 12.*in;
  G4double CShld_length = 72*in;

  // Blue block dimensions, all lengths in Geant4 are half-lengths
  G4double bb_length = 52.30*in; // modified length of the BluBlocks
  G4double bb_width = 52*in;
  G4double bb_height = 26*in;

  // Core Aluminum and Steel block dimensions
  G4double Al_width = 51*in;
  G4double Al_height = 51*in;
  G4double Al_length = 12*in;
  G4double Stl_width = 51*in;
  G4double Stl_height = 51*in;
  G4double Stl_length = 9.1*in;

  // Concrete Shielding block dimensions
  G4double A_width = 1.5*12*in;
  G4double A_height = 7.5*12*in;
  G4double A_length = 3*12*in;
  G4double B_width = 3*12*in;
  G4double B_height = 7.5*12*in;
  G4double B_length = 3*12*in;
  G4double C_width = 3*12*in;
  G4double C_height = 3*12*in;
  G4double C_length = 6*12*in;
  G4double D_width = 1.5*12*in;
  G4double D_height = 3*12*in;
  G4double D_length = 6*12*in;
  G4double J_width = 12*12*in;
  G4double J_height = 1.5*12*in;
  G4double J_length = 3*12*in;

  //------------------------------------------------------------
  // Steel slabs wedged between into the gap between the blublocks
  // and Absorber core. There are steel shims and plates in the
  // Absorber that level the BluBlocks and core. These values
  // are better known than the BluBlocks so I've created a set
  // of dimensions for them because during their machining they
  // were matched to perfect BluBlock dimensions of 52 x 26 x 52
  // inches which is different than the 52.27 x 25.96 x 52.34 
  // that I found from the shipping reports. More detail in the
  // NuMI-note. Steel Shim dimensions should not be tied to 
  // BluBlock dimensions.(DJK)

  G4double StlSlb_width = (5/8.0)*in;
  G4double StlSlb_height = 50*in;
  G4double StlSlb_length = 108*in;

  G4double StlShm_length = 208*in;

  //------------------------------------------------------------
  // Only want to retrieve some information once, 
  // i.e. z position of the end of the decay pipe.
  G4double DP_end = NumiData->DecayPipeZ0 + NumiData->DecayPipeLength;

  G4RotationMatrix *zrot=new G4RotationMatrix();
  G4double beam_angle = NumiData->BeamAngle;
  zrot->rotateX(beam_angle);// replicates downward angle of the beam

  //------------------------------------------------------------
  // This is for the Hadron Absorber Hall w/o pre-absorber concrete, 
  // rotated around
  // the point where the beam hits the center of the 5/6th aluminum block.
  // Originally the carrier plate was 1" and now it is 2". This
  // means that the point of rotation which used to be
  // (DP_end + 10.25*12*in, 0) is now (DP_end + 10.25*12*in, -1*in)
  // in the original coordinate frame. 

  G4double xo = DP_end + 10.25*12*in;
  G4double yo = -1*in; 
  G4double xp = NumiData->HadrBox_length/2.0 - 6.0625*12*in;
  G4double yp = 0;
  G4double z_hadr = xp*cos(beam_angle) - yp*sin(beam_angle) + xo;
  G4double y_hadr = xp*sin(beam_angle) + yp*cos(beam_angle) + yo;

  //------------------------------------------------------------
  // Pre-Absorber conrete rotated around same point as Absorber Hall.

  xp = (-6.0625*12*in - CShld_length/2.0);
  yp = 0;
  G4double z_shld = xp*cos(beam_angle) - yp*sin(beam_angle) + xo;
  G4double y_shld = xp*sin(beam_angle) + yp*cos(beam_angle) + yo;
  
  G4Material* al = GetMaterial(9);
  G4Material* stl = GetMaterial(11); // using the Slab steel ASTM836
  G4Material* blu_material = GetMaterial(12);//BluBlocks from Oak Ridge lab
  G4Material* concrete = GetMaterial(19);//Rebar Concrete
  G4Material* var_Al = GetMaterial(21);
  G4Material* var_Stl = GetMaterial(22);
  char no[3];
  G4String volName;

  //------------------------------------------------------------
  // This constructs a Hadron Absorber volume to put everything in. 

  G4Box*  expHadrBox = new G4Box("expHadrBox", NumiData->HadrBox_width/2., NumiData->HadrBox_height/2.0, NumiData->HadrBox_length/2.0);
  HadrBox_log = new G4LogicalVolume(expHadrBox, Air, "LVHadrBox", 0, 0, 0);
  G4ThreeVector pos = G4ThreeVector(0, y_hadr, z_hadr) - tunnelPos;
  G4String nam = "HadronAbsorber";
  HadrBox = new G4PVPlacement(zrot, pos, nam, HadrBox_log, pvTUNE, false, 0);//(rotation, G4ThreeVector, name, logical volume, mother, boolean operators, copy number)

  //------------------------------------------------------------
  // The following code was introduced with the idea to include 
  // some variable density bits of the stl and al core in order
  // to simulate the cooling pipes. 

  G4Box* Alhole_solid = new G4Box("hole1", 7.5*in/2.0, 42.75*in/2.0, Al_length/2.0);
  G4Box* Stlhole_solid = new G4Box("hole2", 4.5*in/2.0, 42.75*in/2.0, Stl_length/2.0);
  G4LogicalVolume* Alhole_log = new G4LogicalVolume(Alhole_solid, var_Al, "hole1", 0, 0, 0);
  G4LogicalVolume* Stlhole_log = new G4LogicalVolume(Stlhole_solid, var_Stl, "hole2", 0, 0, 0);
  G4ThreeVector AlholePosL = G4ThreeVector(-Stl_width/2.0 + 7.5*in/2.0, -1.875*in, 0);
  G4ThreeVector AlholePosR = G4ThreeVector(Stl_width/2.0 - 7.5*in/2.0, -1.875*in, 0);
  G4ThreeVector StlholePosL = G4ThreeVector(-22.0*in, -1.875*in, 0);
  G4ThreeVector StlholePosR = G4ThreeVector(22.0*in, -1.875*in, 0);
  G4ThreeVector blockPos;

  G4double CoreX_pos = 0;
  G4double CoreY_pos = 1*in; // the carriers plate is 2" instead of 1" 
  G4double CoreZ_pos = 0;
  G4double core_offset = 12.75*in; // This is the offset between the beginning edge of the blublocks and 
                                    // the front edge of the core.

  // The Absorber core positioning and placement
  //-----------------------------------------------------------------------

  for(G4int ii = 0;ii<8;ii++){
    CoreZ_pos = ii*Al_length + Al_length/2.0 - NumiData->HadrBox_length/2.0 + core_offset; 
    sprintf(no, "%d", ii+1);
    volName = "Al_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, Al_width/2.0, Al_height/2.0, Al_length/2.0);
    volName = "Al_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], al, volName, 0, 0, 0);
    volName = "Al_BLK"; 
    volName.append(no);
    blockPos = G4ThreeVector(CoreX_pos, CoreY_pos, CoreZ_pos);
    new G4PVPlacement(0, AlholePosL, Alhole_log, "AlholeL", BLK_log[ii], false, 0);
    new G4PVPlacement(0, AlholePosR, Alhole_log, "AlholeR", BLK_log[ii], false, 0);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }	


  //Stl core block positioning
  for(G4int ii = 0;ii<10;ii++){
    CoreZ_pos = ii*Stl_length + Stl_length/2.0 - NumiData->HadrBox_length/2.0 + 8*Al_length+core_offset; 
    sprintf(no, "%d", ii+1);
    volName = "Stl_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, Stl_width/2.0, Stl_height/2.0, Stl_length/2.0);
    volName = "Stl_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], stl, volName, 0, 0, 0);
    volName = "Stl_BLK"; 
    volName.append(no);
    blockPos = G4ThreeVector(CoreX_pos, CoreY_pos, CoreZ_pos);
    new G4PVPlacement(0, StlholePosL, Stlhole_log, "Stlhole", BLK_log[ii], false, 0);
    new G4PVPlacement(0, StlholePosR, Stlhole_log, "Stlhole", BLK_log[ii], false, 0);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }	

  //-------------------------------------------------------------------
  // Base,  side and top levels surrounding the Absorber Core

  G4double bbx_pos = 0;
  G4double bby_pos = 0;
  G4double bbz_pos = 0;

  // Blu block positioning
  // Bottom two layers
  for (G4int ii = 0;ii<24;ii++){
    bbx_pos = (ii%3)*bb_width - bb_width;
    bby_pos = -ii/12*bb_height - Al_height/2.0 - bb_height/2.0 - 2*bb_height-in;
    bbz_pos = (ii%12)/3*bb_length - NumiData->HadrBox_length/2.0 + bb_length/2.0; 
    sprintf(no, "%d", ii+1);
    volName = "blu_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, bb_width/2.0, bb_height/2.0, bb_length/2.0);
    volName = "blu_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], blu_material, volName, 0, 0, 0);
    volName = "blu_BLK"; 
    volName.append(no);
     blockPos = G4ThreeVector(bbx_pos, bby_pos, bbz_pos);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }
  //layer immediatetly under the core
  for (G4int ii=0;ii<8;ii++){
    bbx_pos = 0;
    bby_pos = -ii/4*bb_height - Al_height/2.0 - bb_height/2.0 - in;
    bbz_pos = ii%4*bb_length - NumiData->HadrBox_length/2.0 + bb_length/2.0; 
    sprintf(no, "%d", ii+25);
    volName = "blu_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, bb_width/2.0, bb_height/2.0, bb_length/2.0);
    volName = "blu_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], blu_material, volName, 0, 0, 0);
    volName = "blu_BLK"; 
    volName.append(no);
     blockPos = G4ThreeVector(bbx_pos, bby_pos, bbz_pos);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);

    // creates the Absorber Core carrier plate, steel shims between the 2 & 3 layer,
    // steel plate on top of the steel blocks in the Absorber Core and the steel
    // on top of the BluBlock sides, which is what the BluBlock top section rest upon.

    if(ii==0){
      volName = "stl_slab1_s";//carrier plate
      volName.append(no);
      G4Box *stl_slab1_s = new G4Box(volName, 52*in/2.0, 2*in/2.0, 195.38/2.0*in);
      volName = "stl_slab1_lv";
      volName.append(no);
      G4LogicalVolume *stl_slab1_lv = new G4LogicalVolume(stl_slab1_s, stl, volName, 0, 0, 0);
      volName = "stl_slab1";
      G4ThreeVector slabpos1 = G4ThreeVector(0, -Al_height/2.0 + CoreY_pos - 2*in/2.0, -NumiData->HadrBox_length/2.0 + core_offset + 195.38*in/2.0);
      new G4PVPlacement(0, slabpos1, volName, stl_slab1_lv, HadrBox, false, 0);

      volName = "stl_slab2_s";
      volName.append(no);
      G4Box *stl_slab2_s = new G4Box(volName, 52*in/2.0, 3.0*in/2.0, StlShm_length/2.0);
      volName = "stl_slab2_lv";
      G4LogicalVolume *stl_slab2_lv = new G4LogicalVolume(stl_slab2_s, stl, volName, 0, 0, 0);
      volName = "stl_slab2";
      volName.append(no);
      G4ThreeVector slabpos2 = G4ThreeVector(-bb_width, -bb_height*2.0 + 3.0*in/2.0 - 1.0*in - Al_height/2, -NumiData->HadrBox_length/2.0 + 2*bb_length);
      new G4PVPlacement(0, slabpos2, volName, stl_slab2_lv, HadrBox, false, 0);

      volName = "stl_slab3_s";
      G4Box *stl_slab3_s = new G4Box(volName, 52*in/2.0, 3.0*in/2.0, StlShm_length/2.0);
      volName = "stl_slab3_lv";
      volName.append(no);
      G4LogicalVolume *stl_slab3_lv = new G4LogicalVolume(stl_slab3_s, stl, volName, 0, 0, 0);
      volName = "stl_slab3";
      G4ThreeVector slabpos3 = G4ThreeVector(bb_width, -bb_height*2.0 + 3.0*in/2.0 - 1.0*in - Al_height/2.0, -NumiData->HadrBox_length/2.0 + 2*bb_length);
      new G4PVPlacement(0, slabpos3, volName, stl_slab3_lv, HadrBox, false, 0);

      volName = "stl_slab4_s";
      volName.append(no);
      G4Box *stl_slab4_s = new G4Box(volName, 3*52*in/2.0, in/2.0, StlShm_length/2.0);
      volName = "stl_slab4_lv";
      volName.append(no);
      G4LogicalVolume *stl_slab4_lv = new G4LogicalVolume(stl_slab4_s, stl, volName, 0, 0, 0);
      volName = "stl_slab4";
      G4ThreeVector slabpos4 = G4ThreeVector(0, Al_height/2.0 + 3.5*in, -NumiData->HadrBox_length/2.0 + 2*bb_length);
      new G4PVPlacement(0, slabpos4, volName, stl_slab4_lv, HadrBox, false, 0);

      // steel slab on top of the steel blocks of the Absorber Core.

      volName = "stl_slab5_s";
      volName.append(no);
      G4Box *stl_slab5_s = new G4Box(volName, 52*in/2.0, 2.0*in/2.0, 10.0*Stl_length/2.0);
      volName = "stl_slab5_lv";
      volName.append(no);
      G4LogicalVolume *stl_slab5_lv = new G4LogicalVolume(stl_slab5_s, stl, volName, 0, 0, 0);
      volName = "stl_slab5";
      G4ThreeVector slabpos5 = G4ThreeVector(0, Al_height/2.0 + CoreY_pos + 2.0*in/2.0, -NumiData->HadrBox_length/2.0 + core_offset + 8*Al_length + 10*Stl_length/2.0);
      new G4PVPlacement(0, slabpos5, volName, stl_slab5_lv, HadrBox, false, 0);
    }
  }

  //-------------------------------------------------------------------
  // The steel plates in the gap appear to be tackwelded
  // to the side of the Steel Absorber core. They 'rest'
  // on the top of the 4th layer of side BluBlocks. The
  // top of the gap pieces match the top of the Core
  // so the gap pieces are raised an 1" verssus the Core.

  volName = "stl_slabL_s";
  volName.append(no);
  G4Box *stl_slabL_s = new G4Box(volName, StlSlb_width/2.0, StlSlb_height/2.0, StlSlb_length/2.0);
  volName = "stl_slabL_lv";
  volName.append(no);
  G4LogicalVolume *stl_slabL_lv = new G4LogicalVolume(stl_slabL_s, stl, volName, 0, 0, 0);
  volName = "stl_slabL";
  G4ThreeVector slabpos5 = G4ThreeVector(-Al_width/2 - StlSlb_width/2.0,  CoreY_pos + in/2.0, -NumiData->HadrBox_length/2.0 + 4*bb_length - StlSlb_length/2.0);
  new G4PVPlacement(0, slabpos5, volName, stl_slabL_lv, HadrBox, false, 0);

  volName = "stl_slabR_s";
  volName.append(no);
  G4Box *stl_slabR_s = new G4Box(volName, StlSlb_width/2.0, StlSlb_height/2.0, StlSlb_length/2.0);
  volName = "stl_slabR_lv";
  volName.append(no);
  G4LogicalVolume *stl_slabR_lv = new G4LogicalVolume(stl_slabR_s, stl, volName, 0, 0, 0);
  volName = "stl_slabR";
  G4ThreeVector slabpos6 = G4ThreeVector(Al_width/2 + StlSlb_width/2.0, CoreY_pos + in/2.0, -NumiData->HadrBox_length/2.0 + 4*bb_length - StlSlb_length/2.0);
  new G4PVPlacement(0, slabpos6, volName, stl_slabR_lv, HadrBox, false, 0);

  //-------------------------------------------------------------------
  // Layer left(beam-view) and under of the core, and above the 
  // 3" steel shim slab. The side blocks need to moved in the
  // lateral (+/- x) in order to compensate the gap steel pieces
  // being 5/8" instead of 4/8" of an inch.

  for (G4int ii=0;ii<8;ii++){
    bbx_pos = -bb_width;
    bby_pos = ii/4*bb_height - Al_height/2.0 - bb_height*1.5 + 3.0*in - 1.0*in;
    bbz_pos = ii%4*bb_length-NumiData->HadrBox_length/2.0 + bb_length/2.0;
    sprintf(no, "%d", ii+33);
    volName = "blu_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, bb_width/2.0, bb_height/2.0, bb_length/2.0);
    volName = "blu_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], blu_material, volName, 0, 0, 0);
    volName = "blu_BLK"; 
    volName.append(no);
    blockPos = G4ThreeVector(bbx_pos, bby_pos, bbz_pos);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }

  for (G4int ii=0;ii<8;ii++){
    bbx_pos = bb_width;
    bby_pos = ii/4*bb_height - Al_height/2.0 - bb_height*1.5 + 3.0*in - 1.0*in;
    bbz_pos = ii%4*bb_length - NumiData->HadrBox_length/2.0 + bb_length/2.0; 
    sprintf(no, "%d", ii+41);
    volName = "blu_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, bb_width/2.0, bb_height/2.0, bb_length/2.0);
    volName = "blu_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], blu_material, volName, 0, 0, 0);
    volName = "blu_BLK"; 
    volName.append(no);
     blockPos = G4ThreeVector(bbx_pos, bby_pos, bbz_pos);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }

  // Blocks to the right of the core (Beam-view looking downstream)
  for (G4int ii=0;ii<8;ii++){
    bbx_pos = bb_width/2.0 + bb_height/2.0 + (ii%2)*bb_height + 1/8.0*in;
    bby_pos = 2.0*in + (bb_width - Al_height)/2.0;
    bbz_pos = (ii/2)*bb_length - NumiData->HadrBox_length/2.0 + bb_length/2.0; 
    sprintf(no, "%d", ii+49);
    volName = "blu_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, bb_height/2.0, bb_width/2.0, bb_length/2.0);
    volName = "blu_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], blu_material, volName, 0, 0, 0);
    volName = "blu_BLK"; 
    volName.append(no);
     blockPos = G4ThreeVector(bbx_pos, bby_pos, bbz_pos);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }

  // Blocks to the left of the core
  for (G4int ii=0;ii<8;ii++){
    bbx_pos = -bb_width/2.0 - bb_height/2.0 - (ii%2)*bb_height - 1/8.0*in;
    bby_pos = 2.0*in + (bb_width - Al_height)/2.0;
    bbz_pos = (ii/2)*bb_length - NumiData->HadrBox_length/2.0 + bb_length/2.0; 
    sprintf(no, "%d", ii+57);
    volName = "blu_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, bb_height/2.0, bb_width/2.0, bb_length/2.0);
    volName = "blu_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], blu_material, volName, 0, 0, 0);
    volName = "blu_BLK"; 
    volName.append(no);
     blockPos = G4ThreeVector(bbx_pos, bby_pos, bbz_pos);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }

  // Blocks above the Core
  //2" shim difference + 1" Steel plate overburden + 1" difference in Al and blublock heights
  for (G4int ii=0;ii<24;ii++){
    bbx_pos = ii%3*(-bb_width) + bb_width;
    bby_pos = ii/12*bb_height + (4.0*in + Al_height/2.0 + bb_height/2.0);
    bbz_pos = (ii%12)/3*(bb_length) - NumiData->HadrBox_length/2.0 + bb_length/2.0; 
    sprintf(no, "%d", ii+65);
    volName = "blu_sBLK"; 
    volName.append(no);
    BLK_solid[ii] = new G4Box(volName, bb_width/2.0, bb_height/2.0, bb_length/2.0);
    volName = "blu_lvBLK"; 
    volName.append(no);
    BLK_log[ii] = new G4LogicalVolume(BLK_solid[ii], blu_material, volName, 0, 0, 0);
    volName = "blu_BLK"; 
    volName.append(no);
     blockPos = G4ThreeVector(bbx_pos, bby_pos, bbz_pos);
    new G4PVPlacement(0, blockPos, volName, BLK_log[ii], HadrBox, false, 0);
  }

  //------------------------------------------------------------------------
  // Concrete blocks surrounding core
  // The 81" in the +x and -x directions are taken from drawing 427334.

  G4double conc_width = .9144*m;
  G4double conc_length = 7.0*.9144*m;
  G4double conc_height = 2*7.5*12*in;
  BLK_solid[90]= new G4Box("conc_sBLK", conc_width/2.0, conc_height/2.0, conc_length/2.0);
  BLK_log[90] = new G4LogicalVolume(BLK_solid[90], concrete, "conc_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(81*in + conc_width/2.0, -in - Al_height/2.0 - 4*bb_height + conc_height/2.0, -NumiData->HadrBox_length/2.0+conc_length/2.0);
  new G4PVPlacement(0, blockPos, "conc_BLK", BLK_log[90], HadrBox, false, 0);

  BLK_solid[91] =  new G4Box("conc_sBLK", conc_width/2.0, conc_height/2.0, conc_length/2.0-.9144/2.0*m);
  BLK_log[91] = new G4LogicalVolume(BLK_solid[91], concrete, "conc_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(-81*in - conc_width/2.0, -in - Al_height/2.0 - 4*bb_height + conc_height/2.0, -NumiData->HadrBox_length/2.0+(conc_length-.9144*m)/2.0);
  new G4PVPlacement(0, blockPos, "conc_BLK", BLK_log[91], HadrBox, false, 0);

  BLK_solid[92] =  new G4Box("conc_sBLK", conc_length/2.0-2*.9144/2.0*m, conc_height/2.0, conc_width/2.0);
  BLK_log[92] = new G4LogicalVolume(BLK_solid[92], concrete, "conc_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(-81*in - conc_width/2.0 + (conc_length-2*.9144*m)/2.0, -in-Al_height/2.0-4*bb_height+conc_height/2.0, -NumiData->HadrBox_length/2.0 + conc_length + 10*in - conc_width/2.0);
  new G4PVPlacement(0, blockPos, "conc_BLK", BLK_log[92], HadrBox, false, 0);

  BLK_solid[93] =  new G4Box("topstl1_sBLK", 3*bb_width/2.0, 18.22*in/2.0, 4*bb_width/2.0);
  BLK_log[93] = new G4LogicalVolume(BLK_solid[93], stl, "topstl1_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(0, 4*in + Al_height/2.0 + bb_height*2.0 + 18.22*in/2.0, -NumiData->HadrBox_length/2.0 + 4*bb_width/2.0);
  new G4PVPlacement(0, blockPos, "topstl1_BLK", BLK_log[93], HadrBox, false, 0);

  BLK_solid[94] =  new G4Box("topstl2_sBLK", conc_width/2.0, 18.22*in/2.0, conc_length/2.0);
  BLK_log[94] = new G4LogicalVolume(BLK_solid[94], stl, "topstl2_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(-81*in - conc_width/2.0, -in - Al_height/2.0-4*bb_height+conc_height+18.22*in/2.0, -NumiData->HadrBox_length/2.0 + conc_length/2.0);
  new G4PVPlacement(0, blockPos, "topstl2_BLK", BLK_log[94], HadrBox, false, 0);

  BLK_solid[95] =  new G4Box("topstl3_sBLK", conc_width/2.0, 18.22*in/2.0, conc_length/2.0);
  BLK_log[95] = new G4LogicalVolume(BLK_solid[95], stl, "topstl3_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(81*in + conc_width/2.0, -in - Al_height/2.0 - 4*bb_height + conc_height + 18.22*in/2.0, -NumiData->HadrBox_length/2.0 + conc_length/2.0);
  new G4PVPlacement(0, blockPos, "topstl3_BLK", BLK_log[95], HadrBox, false, 0);

  //------------------------------------------------------------------------
  // This rests upstream on a small ledge welded onto the back of the
  // BluBlock layer above the Core and downstream on the Concrete
  // blocks.

  BLK_solid[96] =  new G4Box("topstl4_sBLK", 3*bb_width/2.0, 18.22*in/2.0, 2*30*in/2.0);
  BLK_log[96] = new G4LogicalVolume(BLK_solid[96], stl, "topstl4_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(0, -NumiData->HadrBox_height/2.0 + conc_height + 18.22*in/2.0, -NumiData->HadrBox_length/2.0 + 4*bb_length + 2*30*in/2.0);
  new G4PVPlacement(0, blockPos, "topstl4_BLK", BLK_log[96], HadrBox, false, 0);

  //------------------------------------------------------------------------
  // The top left, right, downstream (looking downstream) 
  // layer of steel that surrounds BluBlock steel shielding. 
  // This is a semi-vertical layer designed to provide 
  // shiedling and cover horizontal cracks in the top 18" layer
  // of steel that covers the area immediately on top of the topmost
  // BluBlock layer. This will be covered to some degree in the 
  // NuMI-note.

  BLK_solid[97] =  new G4Box("topstl5_sBLK", 2*9.1*in/2.0, 30*in/2.0, 4*bb_width/2.0);
  BLK_log[97] = new G4LogicalVolume(BLK_solid[97], stl, "topstl5_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(-3*bb_width/2.0 - 2*9.1*in/2.0, -NumiData->HadrBox_height/2.0 + conc_height + 18.22*in + 30*in/2.0, -NumiData->HadrBox_length/2.0 + 4*bb_length/2.0);
  new G4PVPlacement(0, blockPos, "topstl5_BLK", BLK_log[97], HadrBox, false, 0);

  BLK_solid[98] =  new G4Box("topstl6_sBLK", 2*9.1*in/2.0, 30*in/2.0, 4*bb_width/2.0);
  BLK_log[98] = new G4LogicalVolume(BLK_solid[98], stl, "topstl6_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(3*bb_width/2.0 + 2*9.1*in/2.0, -NumiData->HadrBox_height/2.0 + conc_height + 18.22*in + 30*in/2.0, -NumiData->HadrBox_length/2.0 + 4*bb_length/2.0);
  new G4PVPlacement(0, blockPos, "topstl6_BLK", BLK_log[98], HadrBox, false, 0);

  BLK_solid[99] =  new G4Box("topstl7_sBLK", 3*bb_width/2.0 + 2*18.22, 30*in/2.0, 2*9.1*in/2.0);
  BLK_log[99] = new G4LogicalVolume(BLK_solid[99], stl, "topstl7_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(0, -NumiData->HadrBox_height/2.0 + conc_height + 18.22*in + 30*in/2.0, -NumiData->HadrBox_length/2.0 + 4*bb_length + 2*9.1*in/2.0);
  new G4PVPlacement(0, blockPos, "topstl7_BLK", BLK_log[99], HadrBox, false, 0);

  G4cout << "Hadron Absorber Constructed" << G4endl;

  //-------------------------------------------------------------------------
  // Constructs a volume in which to put the pre-absorber concrete shielding.

  G4Box*  expShldBox = new G4Box("expShldBox", NumiData->HadrBox_width/2., NumiData->HadrBox_height/2.0, CShld_length/2.0);
  ShldBox_log =new G4LogicalVolume(expShldBox, Air, "LVShldBox", 0, 0, 0);
  G4ThreeVector shldpos = G4ThreeVector(0, y_shld, z_shld)-tunnelPos;
  nam = "ConcShield";
  ShldBox = new G4PVPlacement(zrot, shldpos, nam, ShldBox_log, pvTUNE, false, 0);
  
  CShld_solid[0] = new G4Box("CShld_sBLK0", (A_width+B_width)/2.0, A_height/2.0, 2*A_length/2.0);
  CShld_log[0] = new G4LogicalVolume(CShld_solid[0], concrete, "CShld_lvBLK0", 0, 0, 0);
  blockPos = G4ThreeVector(1/2.0*A_height+(A_width+B_width)/2.0, -NumiData->HadrBox_height/2.0+B_height/2.0, -CShld_length/2.0+B_length);
  new G4PVPlacement(0, blockPos, "CShld_BLK0", CShld_log[0], ShldBox, false, 0);
  CShld_solid[1] = new G4Box("CShld_sBLK1", (A_width+B_width)/2.0, A_height/2.0, 2*A_length/2.0);
  CShld_log[1] = new G4LogicalVolume(CShld_solid[1], concrete, "CShld_lvBLK1", 0, 0, 0);
  blockPos = G4ThreeVector(-1/2.0*A_height-(A_width+B_width)/2.0, -NumiData->HadrBox_height/2.0+B_height/2.0, -CShld_length/2.0+B_length);
  new G4PVPlacement(0, blockPos, "CShld_BLK1", CShld_log[1], ShldBox, false, 0);

  CShld_solid[2] = new G4Box("CShld_sBLK2", (A_width+B_width)/2.0, A_height/2.0, 2*A_length/2.0);
  CShld_log[2] = new G4LogicalVolume(CShld_solid[2], concrete, "CShld_lvBLK2", 0, 0, 0);
  blockPos = G4ThreeVector(-1/2.0*A_height-(A_width+B_width)/2.0, -NumiData->HadrBox_height/2.0+1.5*B_height, -CShld_length/2.0+B_length);
  new G4PVPlacement(0, blockPos, "CShld_BLK2", CShld_log[2], ShldBox, false, 0);

  CShld_solid[3] = new G4Box("CShld_sBLK3", (A_width+B_width)/2.0, A_height/2.0, A_length/2.0);
  CShld_log[3] = new G4LogicalVolume(CShld_solid[3], concrete, "CShld_lvBLK3", 0, 0, 0);
  blockPos = G4ThreeVector(1/2.0*A_height+(A_width+B_width)/2.0, -NumiData->HadrBox_height/2.0+1.5*B_height, -CShld_length/2.0+B_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK3", CShld_log[3], ShldBox, false, 0);

  CShld_solid[4] = new G4Box("CShld_sBLK4", B_width/2.0, B_height/2.0, B_length/2.0);
  CShld_log[4] = new G4LogicalVolume(CShld_solid[4], concrete, "CShld_lvBLK4", 0, 0, 0);
  blockPos = G4ThreeVector(1/2.0*A_height+B_width/2.0, -NumiData->HadrBox_height/2.0+1.5*B_height, -CShld_length/2.0+1.5*B_length);
  new G4PVPlacement(0, blockPos, "CShld_BLK4", CShld_log[4], ShldBox, false, 0);

  CShld_solid[5] = new G4Box("CShld_sBLK5", C_width/2.0, C_height/2.0, C_length/2.0);
  CShld_log[5] = new G4LogicalVolume(CShld_solid[5], concrete, "CShld_lvBLK5", 0, 0, 0);
  blockPos = G4ThreeVector(1/2.0*D_width+1/2.0*C_width, -NumiData->HadrBox_height/2.0+C_height/2.0, -CShld_length/2.0+C_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK5", CShld_log[5], ShldBox, false, 0);

  CShld_solid[6] = new G4Box("CShld_sBLK6", C_width/2.0, C_height/2.0, C_length/2.0);
  CShld_log[6] = new G4LogicalVolume(CShld_solid[6], concrete, "CShld_lvBLK6", 0, 0, 0);
  blockPos = G4ThreeVector(-1/2.0*D_width-1/2.0*C_width, -NumiData->HadrBox_height/2.0+C_height/2.0, -CShld_length/2.0+C_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK6", CShld_log[6], ShldBox, false, 0);

  CShld_solid[7] = new G4Box("CShld_sBLK7", C_width/2.0, C_height/2.0, C_length/2.0);
  CShld_log[7] = new G4LogicalVolume(CShld_solid[7], concrete, "CShld_lvBLK7", 0, 0, 0);
  blockPos = G4ThreeVector(1/2.0*D_width+1/2.0*C_width, -NumiData->HadrBox_height/2.0+C_height+A_width+C_height/2.0, -CShld_length/2.0+C_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK7", CShld_log[7], ShldBox, false, 0);

  CShld_solid[8] = new G4Box("CShld_sBLK8", C_width/2.0, C_height/2.0, C_length/2.0);
  CShld_log[8] = new G4LogicalVolume(CShld_solid[8], concrete, "CShld_lvBLK8", 0, 0, 0);
  blockPos = G4ThreeVector(-1/2.0*D_width-1/2.0*C_width, -NumiData->HadrBox_height/2.0+C_height+A_width+C_height/2.0, -CShld_length/2.0+C_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK8", CShld_log[8], ShldBox, false, 0);

  CShld_solid[9] = new G4Box("CShld_sBLK9", A_height/2.0, A_width/2.0, A_length);
  CShld_log[9] = new G4LogicalVolume(CShld_solid[9], concrete, "CShld_lvBLK9", 0, 0, 0);
  blockPos = G4ThreeVector(0, -NumiData->HadrBox_height/2.0+C_height+A_width/2.0, -CShld_length/2.0+A_length);
  new G4PVPlacement(0, blockPos, "CShld_BLK9", CShld_log[9], ShldBox, false, 0);

  CShld_solid[10] = new G4Box("CShld_sBLK10", D_width/2.0, D_height/2.0, D_length/2.0);
  CShld_log[10] = new G4LogicalVolume(CShld_solid[10], concrete, "CShld_lvBLK10", 0, 0, 0);
  blockPos = G4ThreeVector(0, -NumiData->HadrBox_height/2.0+D_height/2.0, -CShld_length/2.0+D_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK10", CShld_log[10], ShldBox, false, 0);

  CShld_solid[11] = new G4Box("CShld_sBLK11", D_width/2.0, D_height/2.0, D_length/2.0);
  CShld_log[11] = new G4LogicalVolume(CShld_solid[11], concrete, "CShld_lvBLK11", 0, 0, 0);
  blockPos = G4ThreeVector(0, -NumiData->HadrBox_height/2.0+D_height+A_width+D_height/2.0, -CShld_length/2.0+D_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK11", CShld_log[11], ShldBox, false, 0);

  CShld_solid[12] = new G4Box("CShld_sBLK12", J_width/2.0, 2*J_height/2.0, 2*J_length/2.0);
  CShld_log[12] = new G4LogicalVolume(CShld_solid[12], concrete, "CShld_lvBLK12", 0, 0, 0);
  blockPos = G4ThreeVector(0, -NumiData->HadrBox_height/2.0+2*B_height+J_height+3*in, -CShld_length/2.0+D_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_BLK12", CShld_log[12], ShldBox, false, 0);

  //-------------------------------------------------------------------------
  // creates the block of 18.22" steel that covers the top of
  // the Pre-Absorber concrete shielding. This was to 
  // shield the surrounding groundwater from activation.

  G4Box *CShld_stl_solid = new G4Box("CShld_stl_sBLK", 3*bb_width/2.0, 18.22*in/2.0, 2*J_length/2.0);
  G4LogicalVolume *CShld_stl_log = new G4LogicalVolume(CShld_stl_solid, stl, "CShld_stl_lvBLK", 0, 0, 0);
  blockPos = G4ThreeVector(0, -NumiData->HadrBox_height/2.0 + 2*B_height + 2*J_height + 3*in + 18.22*in/2.0, -CShld_length/2.0 + D_length/2.0);
  new G4PVPlacement(0, blockPos, "CShld_stl,BLK", CShld_stl_log, ShldBox, false, 0);

  // This is to create a block of air near the end of the beam pipe to compensate for a smaller block position.
  CShld_solid[15] = new G4Box("air_sBLK15", 1.5*12*in/2.0, 1.5*12*in/2.0, 1.5*12*in/2.0);
  CShld_log[15] = new G4LogicalVolume(CShld_solid[15], Air, "air_lvBLK15", 0, 0, 0);
  blockPos = G4ThreeVector((A_width+B_width)/2.0 - 1.5*12*in/2.0, A_height/2.0 - 1.5*12*in/2.0, -2*A_length/2.0 + 1.5*12*in/2.0);
  new G4PVPlacement(0, blockPos, CShld_log[15], "air_BLK15", CShld_log[2], false, 0);

  G4cout << "Decay Pipe end cap Shielding Constructed" << G4endl;

}
