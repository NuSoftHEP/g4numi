//----------------------------------------------------------------------
// The muon alcoves have an in born rotation that is set in 
// NumiDataInput so that they are always oriented with respect to 
// vertical, instead of whatever the beam angle is. Also there is the 
// functionality to move the upstream and downstream alcove wall face
// in order to represent a shift of 1 sigma. Each of the 81 individual
// pixels in the muon monitors is modelled and acts as the active
// elements of the monitors.
//
// $Id: NumiSecMonitors.cc,v 1.7.2.1 2009/09/24 16:39:13 martens Exp $
//----------------------------------------------------------------------

#include "NumiDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "NumiDataInput.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include <math.h>

static const G4double in = 2.54*cm;
static const G4double ft = 12.*in;

void NumiDetectorConstruction::ConstructSecMonitors()
{
  //--------------------------------------------------
  // Generating variables for systematic
  // studies

  G4double UpWall_offset    = 0.09*ft * NumiData->GetMaterialSigma();
  G4double DownWall_offset  = -0.54*ft * NumiData->GetMaterialSigma();

  //--------------------------------------------------
  // Only want to retrieve some information once, i.e.
  // z position of the end of the decay pipe.

  G4ThreeVector tunnelPos = G4ThreeVector(0, 0, NumiData->TunnelLength/2. + NumiData->TunnelZ0);
  G4double DP_end = NumiData->DecayPipeLength + NumiData->TunnelZ0;

  //--------------------------------------------------
  // Establishes the physical quantities associated
  // with the Muon Alcoves location and shotcrete.

  G4double Shotcrete_depth = 4*in;
  G4double Backfill_depth = 10*in;
  G4double MM0_downstream = 292.5057*ft;
  G4double MM1_upstream = 331.9597*ft + UpWall_offset;
  G4double MM1_downstream = 341.8983*ft + DownWall_offset;
  G4double MM2_upstream = 401.1375*ft;
  G4double MM2_downstream = 411.3207*ft;
  
  G4double MM01RockLength = MM1_upstream - MM0_downstream;
  G4double MM12RockLength = MM2_upstream - MM1_downstream;

  G4double  MuAlcv1_width  = 12*ft;
  G4double  MuAlcv1_height = 12*ft;
  G4double  MuAlcv1_length = MM1_downstream - MM1_upstream + 2*Shotcrete_depth;
  G4double  MuAlcv2_width  = 12*ft;
  G4double  MuAlcv2_height = 12*ft;
  G4double  MuAlcv2_length = MM2_downstream - MM2_upstream + 2*Shotcrete_depth;

  // MM0 is different than MM1 and MM2
  // in dimensions
  G4double  MuAlcv0_width = 12*ft;
  G4double  MuAlcv0_height =14*ft + 8*in;
  G4double  MuAlcv0_length = 9.*ft + 9.*in + Backfill_depth;

  // rotation to match vertical position of detector equipment 
  // to .058 radian downslope angle of tunnel
  G4RotationMatrix *zrot = new G4RotationMatrix();
  G4double beam_angle = NumiData->BeamAngle;
  zrot->rotateX(beam_angle);


  //-----------------------------------------------------------------------
  // The following is the code which rotates
  // the G4NuMI elements to accurately represent 
  // downslope of the beam angle.

  G4double xo = DP_end + 10.25*ft;
  G4double yo = -1.*in;

  G4double xp = NumiData->HadrBox_length - 6.0625*ft + MuAlcv0_length/2.0;
  G4double yp = MuAlcv0_height/2.0 - NumiData->HadrBox_height/2.0;
  G4double z_MuAlcv0 = xp*cos(beam_angle) - yp*sin(beam_angle) + xo;
  G4double y_MuAlcv0 = xp*sin(beam_angle) + yp*cos(beam_angle) + yo;

  // Since the muon monitor in Alcove 0 is not
  // in the center of the box, it needs to 
  // be handled differently. Otherwise it would
  // be located Backfill_depth/2.0 too far
  // downstream.

  xp = NumiData->HadrBox_length - 6.0625*ft + MuAlcv0_length/2.0 - Backfill_depth;
  yp = MuAlcv0_height/2.0 - NumiData->HadrBox_height/2.0;
  G4double z_MuAlcv0_mon = xp*cos(beam_angle) - yp*sin(beam_angle) + xo;
  G4double y_MuAlcv0_mon = xp*sin(beam_angle) + yp*cos(beam_angle) + yo;

  xp = NumiData->HadrBox_length - 6.0625*ft + MuAlcv0_length - Backfill_depth + MM01RockLength - Shotcrete_depth + MuAlcv1_length/2.0;
  yp = MuAlcv1_height/2.0 - NumiData->HadrBox_height/2.0;
  G4double z_MuAlcv1 = xp*cos(beam_angle) - yp*sin(beam_angle) + xo;
  G4double y_MuAlcv1 = xp*sin(beam_angle) + yp*cos(beam_angle) + yo;

  xp = NumiData->HadrBox_length - 6.0625*ft + MuAlcv0_length - Backfill_depth + MM01RockLength - Shotcrete_depth + MuAlcv1_length - Shotcrete_depth + MM12RockLength - Shotcrete_depth + MuAlcv2_length/2.0;
  yp = (MuAlcv2_height/2.0 - 4.02*ft) - NumiData->HadrBox_height/2.0;
  G4double z_MuAlcv2 = xp*cos(beam_angle) - yp*sin(beam_angle) + xo;
  G4double y_MuAlcv2 = xp*sin(beam_angle) + yp*cos(beam_angle) + yo;
  
  //---------------------------------------------------------------------------
  // Gotta put the Hadron Monitor inside the Concrete Shielding G4Box
  // otherwise the particle doesn't exit to the pvTUNE volume and
  // tunnels past the monitor.

  G4Box *Shadmon = new G4Box("SHadMon", 1.1*m, 1.1*m, 5*mm);
  G4LogicalVolume *LVhadmon = new G4LogicalVolume(Shadmon, Air, "LVHadMon", 0, 0, 0);
  G4ThreeVector hadmon_pos = G4ThreeVector(0., 0.2*m, 0.2*m);
  
  G4PVPlacement *HadMon = new G4PVPlacement(0, hadmon_pos, "PVHadMon", LVhadmon, ShldBox, false, 0);
  
  G4Box *SHadCell = new G4Box( "SHadCell", 3*in/2.0, 3*in/2.0, 1*mm/2.0 );
  G4LogicalVolume *LVHadCell = new G4LogicalVolume(SHadCell, He, "LVHadCell", 0, 0, 0);
  G4ThreeVector cellPos;
  G4String volName = "HadCell";

  for( int i = 0; i < 9; ++i ){
    for( int j = 0; j < 9; ++j ){
      cellPos = G4ThreeVector( (i-4)*11.4*cm, (j-4)*11.4*cm, 0 );
      new G4PVPlacement( 0, cellPos, volName, LVHadCell, HadMon, false, i*9+j ); 
    }
  }


  G4cout << "Hadron Monitor Constructed" << G4endl;

  //-----------------------------------------------------------------
  // Muon monitor 0 has slightly different dimensions than the other 
  // three alcoves.


  G4VPhysicalVolume* MuMonAlcv_0;
  G4VPhysicalVolume* MuMonAlcv_1;
  G4VPhysicalVolume* MuMonAlcv_2;
  
  G4Box *Smumonalcv_0 = new G4Box("SMuMonAlcv_0", MuAlcv0_width/2.0, MuAlcv0_height/2.0, MuAlcv0_length/2.0);
  G4LogicalVolume *LVmumonalcv_0 = new G4LogicalVolume(Smumonalcv_0, Air, "LVMuMonAlcv_0", 0, 0, 0);
  G4ThreeVector mumonalcv_0_pos = G4ThreeVector(0., y_MuAlcv0, z_MuAlcv0) - tunnelPos;
  MuMonAlcv_0 = new G4PVPlacement(zrot, mumonalcv_0_pos, "MuMonAlcv_0", LVmumonalcv_0, pvTUNE, false, 0);

  G4Box *Smumonalcv_1 = new G4Box("SMuMonAlcv_1", MuAlcv1_width/2.0, MuAlcv1_height/2.0, MuAlcv1_length/2.0);
  G4LogicalVolume *LVmumonalcv_1 = new G4LogicalVolume(Smumonalcv_1, Air,"LVMuMonAlcv_1", 0, 0, 0);
  G4ThreeVector mumonalcv_pos = G4ThreeVector(0., y_MuAlcv1, z_MuAlcv1);
  MuMonAlcv_1 = new G4PVPlacement(zrot, mumonalcv_pos, "MuMonAlcv_1", LVmumonalcv_1, ROCK, false, 0);

  G4Box *Smumonalcv_2 = new G4Box("SMuMonAlcv_2", MuAlcv2_width/2.0, MuAlcv2_height/2.0, MuAlcv2_length/2.0);
  G4LogicalVolume *LVmumonalcv_2 = new G4LogicalVolume(Smumonalcv_2, Air,"LVMuMonAlcv_2", 0, 0, 0);
  mumonalcv_pos = G4ThreeVector(0., y_MuAlcv2, z_MuAlcv2);
  MuMonAlcv_2 = new G4PVPlacement(zrot, mumonalcv_pos, "MuMonAlcv_2", LVmumonalcv_2, ROCK, false, 0);



  //--------------------------------------------------
  // Place the Shotcrete on the walls
  // of the Muon Alcoves
  
  G4Box *SMuMonAlcvShot = new G4Box("SMuMonAlcvShot", MuAlcv1_width/2.0, MuAlcv1_height/2.0, Shotcrete_depth/2.0);
  G4LogicalVolume *LVMuMonAlcvShot = new G4LogicalVolume(SMuMonAlcvShot, Shotcrete, "LVMuMonAlcvShot", 0, 0, 0);

  G4ThreeVector MuMonAlcvShot_1_UpPos = G4ThreeVector(0., 0., -MuAlcv1_length/2.0 + Shotcrete_depth/2.0);
  G4ThreeVector MuMonAlcvShot_1_DownPos = G4ThreeVector(0., 0., MuAlcv1_length/2.0 - Shotcrete_depth/2.0);
  G4ThreeVector MuMonAlcvShot_2_UpPos = G4ThreeVector(0., 0., -MuAlcv2_length/2.0 + Shotcrete_depth/2.0);
  G4ThreeVector MuMonAlcvShot_2_DownPos = G4ThreeVector(0., 0., MuAlcv2_length/2.0 - Shotcrete_depth/2.0);

  new G4PVPlacement(0, MuMonAlcvShot_1_UpPos, "MuMonAlcvShot_1_Up", LVMuMonAlcvShot, MuMonAlcv_1, false, 0);
  new G4PVPlacement(0, MuMonAlcvShot_1_DownPos, "MuMonAlcvShot_1_Down", LVMuMonAlcvShot, MuMonAlcv_1, false, 0);
  new G4PVPlacement(0, MuMonAlcvShot_2_UpPos, "MuMonAlcvShot_2_Up", LVMuMonAlcvShot, MuMonAlcv_2, false, 0);
  new G4PVPlacement(0, MuMonAlcvShot_2_DownPos, "MuMonAlcvShot_2_Down", LVMuMonAlcvShot, MuMonAlcv_2, false, 0);

  G4Box *SMuMonAlcvFill_0 = new G4Box("SMuMonAlcvFill_0", MuAlcv0_width/2.0, MuAlcv0_height/2.0, Backfill_depth/2.0);
  G4LogicalVolume *LVMuMonAlcvFill_0 = new G4LogicalVolume(SMuMonAlcvFill_0, Shotcrete, "LVMuMonAlcvFill_0", 0, 0, 0);
  G4ThreeVector MuMonAlcvFill_0 = G4ThreeVector(0., 0., MuAlcv0_length/2.0 - Backfill_depth/2.0);
  new G4PVPlacement(0, MuMonAlcvFill_0, "MuMonAlcvFill_0", LVMuMonAlcvFill_0, MuMonAlcv_0, false, 0);


  G4cout << "Muon Monitor Alcoves(1,2,3) Constructed" << G4endl;


  //--------------------------------------------------
  // The Muon Monitors will be 2.2m x 2.2m x 2.25in 
  // boxes which are perfectly active elements via the 
  // NumiSteppingAction.cc tracking a particle through 
  // the monitor. 

  G4double MuMon_x = 2.2*m;
  G4double MuMon_y = 2.2*m;
  G4double MuMon_z = 2.25*in;

  G4Box *SMuMon = new G4Box("SMuMon", MuMon_x/2.0, MuMon_y/2.0, MuMon_z/2.0);
  G4LogicalVolume *LVMuMon = new G4LogicalVolume(SMuMon, Air, "LVMuMon", 0, 0, 0);

  G4ThreeVector MuMon_pos0 = G4ThreeVector(0., -y_MuAlcv0_mon/cos(beam_angle), -Backfill_depth/2.0);
  G4ThreeVector MuMon_pos1 = G4ThreeVector(0., -y_MuAlcv1/cos(beam_angle), 0.);
  G4ThreeVector MuMon_pos2 = G4ThreeVector(0., -y_MuAlcv2/cos(beam_angle), 0.);

  G4PVPlacement *MuMon0 = new G4PVPlacement(0, MuMon_pos0, "MuMon_0", LVMuMon, MuMonAlcv_0, false, 0);
  new G4PVPlacement(0, MuMon_pos1, "MuMon_1", LVMuMon, MuMonAlcv_1, false, 0);
  new G4PVPlacement(0, MuMon_pos2, "MuMon_2", LVMuMon, MuMonAlcv_2, false, 0);


  G4Box *SMuCell = new G4Box( "SMuCell", 3*in/2.0, 3*in/2.0, 1*mm/2.0 );
  G4LogicalVolume *LVMuCell = new G4LogicalVolume(SMuCell, He, "LVMuCell", 0, 0, 0);  
  volName = "MuCell";

  for( int i = 0; i < 9; ++i ){
    for( int j = 0; j < 9; ++j ){
      cellPos = G4ThreeVector( (i-4)*10.*in, (j-4)*10.*in, 0 );
      new G4PVPlacement( 0, cellPos, volName, LVMuCell, MuMon0, false, i*9+j ); 
    }
  }

}
