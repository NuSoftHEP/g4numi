//----------------------------------------------------------------------
// The muon alcoves have an in born rotation that is set in 
// NumiDataInput so that they are always oriented with respect to 
// vertical, instead of whatever the beam angle is. Also there is the 
// functionality to move the upstream and downstream alcove wall face
// in order to represent a shift of 1 sigma. Each of the 81 individual
// pixels in the muon monitors is modelled and acts as the active
// elements of the monitors.
//
// $Id: NumiSecMonitors.cc,v 1.7.4.4 2014/01/22 22:31:07 kordosky Exp $
//----------------------------------------------------------------------

#include "NumiDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
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
//  G4double z_MuAlcv0_mon = xp*cos(beam_angle) - yp*sin(beam_angle) + xo;
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
  G4LogicalVolume *LVHadCell = new G4LogicalVolume(SHadCell, SecMonitorHelium, "LVHadCell", 0, 0, 0);
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




  if(NumiData -> solidMuMons)
  {
     //
     //chamber dimensions
     //
     G4double ChamberWidth   = 7.62*cm;   
     G4double ChamberHeight  = 7.62*cm;
     G4double ChamberLength  = 3.0*mm;
     G4double ChamberSpacing = 25.4*cm;


     //
     //Ion chamber plate spacing
     //
     G4double IonChamberPlateWidth  = 10.16*cm;
     G4double IonChamberPlateHeight = 10.16*cm;
     G4double IonChamberPlateLength = 0.1*cm;

     //
     //Plate Geometry (this is the Al casing of the muon monitor tubes)
     //
     G4double PlateWidth  = 15.24*cm;
     G4double PlateHeight = 228.6*cm;
     G4double PlateLength = 0.3175*cm;
     G4double SidePlateWidth = PlateLength;
     G4double SidePlateHeight = PlateHeight;
     G4double ionPlateCasingWallSpacing = 1.9725*cm;

     //
     //Monitor dimensions
     //
     G4double MonWidth = (ChamberSpacing*4.0 + 0.5*PlateWidth)*2.0;
     G4double MonHeight = 228.6*cm;
     G4double MonLength = PlateLength*2 + ionPlateCasingWallSpacing*2 
        + IonChamberPlateLength*2 + ChamberLength;
     
     G4double SidePlateLength = MonLength - 2.0*PlateLength;
     
     G4double TubeGapWidth = (MonWidth - PlateWidth*9.0)/8.0;
     G4double TubeGapHeight = PlateHeight;
     G4double TubeGapLength = MonLength;
     
     //------------------------------
     // Monitor Logical Volume 
     //-----------------------------
     
     G4Box *SMuMon = new G4Box("SMuMon", MonWidth/2.0, MonHeight/2.0, MonLength/2.0);
     G4LogicalVolume *LVMuMon = new G4LogicalVolume(SMuMon, HeGas, "LVMuMon", 0, 0, 0);
     
     G4ThreeVector MuMon_pos0 = G4ThreeVector(0., -y_MuAlcv0_mon/cos(beam_angle), -Backfill_depth/2.0);
     G4ThreeVector MuMon_pos1 = G4ThreeVector(0., -y_MuAlcv1/cos(beam_angle), 0.);
     G4ThreeVector MuMon_pos2 = G4ThreeVector(0., -y_MuAlcv2/cos(beam_angle), 0.);
     
     //G4PVPlacement *MuMon0 = new G4PVPlacement(0, MuMon_pos0, "MuMon_0", LVMuMon, MuMonAlcv_0, false, 0);
     new G4PVPlacement(0, MuMon_pos0, "MuMon_0", LVMuMon, MuMonAlcv_0, false, 0);
     new G4PVPlacement(0, MuMon_pos1, "MuMon_1", LVMuMon, MuMonAlcv_1, false, 0);
     new G4PVPlacement(0, MuMon_pos2, "MuMon_2", LVMuMon, MuMonAlcv_2, false, 0);


     //**************************************************************************************************************
     //**************************************************************************************************************
     //**************************************************************************************************************
     //
     //if AbsorberConfig == "Full Coverage", "Center Coverage" or "Configured" create absorbers 
     //

     G4String AbsConfig = NumiData -> GetAbsorberConfig();

     if(AbsConfig == "Configured")
     {

        G4cout << "All Alcoves Absorber Configuration \"" << AbsConfig << "\" :" << G4endl;
        
        //------------------------------
        // Absorber MM0
        //------------------------------
        if(NumiData->GetAbsorberThickness(0) > 0.0)
        {   
           G4double Mon0AbsWidth = MonWidth;
           G4double Mon0AbsHeight = MonHeight;
           G4double Mon0AbsLength = NumiData->GetAbsorberThickness(0);
           G4double AbsMon0Dist = NumiData->GetAbsorberMonDist(0);
           G4Material* Mon0AbsMaterial = NumiData->GetAbsorberMaterial(0);

           G4cout << "Alcove 1 Absorber:" << G4endl
                  << "  Material = " << Mon0AbsMaterial->GetName() << G4endl
                  << "  Thickness = " <<  Mon0AbsLength/cm << " cm" << G4endl
                  << "  Distance from Mon = " <<  AbsMon0Dist/cm << " cm" << G4endl;
           
           G4Box *SMon0Abs = new G4Box("SMon0Abs", Mon0AbsWidth/2.0, Mon0AbsHeight/2.0, Mon0AbsLength/2.0);
           G4LogicalVolume *LVMon0Abs = new G4LogicalVolume(SMon0Abs, Mon0AbsMaterial, "LVMon0Abs", 0, 0, 0);
           
           G4double Mon0Abs_z = -((Mon0AbsLength/2.0)+AbsMon0Dist+(MonLength/2.0))-(Backfill_depth/2.0);
           if( Mon0Abs_z-(Mon0AbsLength/2.0) > MuAlcv0_length/2.0 ||
               Mon0Abs_z+(Mon0AbsLength/2.0) > -MonLength/2.0)
           {
              G4cout << "*****Problem in NumiSecMonitors::Construct - MM0 Absorber is outside MuMonAlcv_0 " << "and/or"
                     << " the Absorber overlaps with the Alcove " << G4endl;
           }
           
           G4ThreeVector Mon0AbsPosition = G4ThreeVector(0., -y_MuAlcv0_mon/cos(beam_angle), Mon0Abs_z);
           
           new G4PVPlacement(0,
                             Mon0AbsPosition,
                             LVMon0Abs,
                             "MonAbs_0",
                             LVmumonalcv_0,
                             false,
                             0);
        }
        
        //------------------------------
        // Absorber MM1
        //-----------------------------
        if(NumiData->GetAbsorberThickness(1) > 0.0)
        {   
           G4double Mon1AbsWidth = MonWidth;
           G4double Mon1AbsHeight = MonHeight;
           G4double Mon1AbsLength = NumiData->GetAbsorberThickness(1);
           G4double AbsMon1Dist = NumiData->GetAbsorberMonDist(1);
           G4Material* Mon1AbsMaterial = NumiData->GetAbsorberMaterial(1);
           
           G4cout << "Alcove 2 Absorber:" << G4endl
                  << "  Material = " << Mon1AbsMaterial->GetName() << G4endl
                  << "  Thickness = " <<  Mon1AbsLength/cm << " cm" << G4endl
                  << "  Distance from Mon = " <<  AbsMon1Dist/cm << " cm" << G4endl;
           
           G4Box *SMon1Abs = new G4Box("SMon1Abs", Mon1AbsWidth/2.0, Mon1AbsHeight/2.0, Mon1AbsLength/2.0);
           G4LogicalVolume *LVMon1Abs = new G4LogicalVolume(SMon1Abs, Mon1AbsMaterial, "LVMon1Abs", 0, 0, 0);
           
           G4double Mon1Abs_z = -((Mon1AbsLength/2.0)+AbsMon1Dist+(MonLength/2.0));
           if( Mon1Abs_z-(Mon1AbsLength/2.0) > MuAlcv1_length/2.0 ||
            Mon1Abs_z+(Mon1AbsLength/2.0) > -MonLength/2.0)
           {
              G4cout << "*****Problem in NumiSecMonitors::Construct - MM1 Absorber is outside MuMonAlcv_1 " << "and/or"
                     << " the Absorber overlaps with the Alcove " << G4endl;
           }
           
           G4ThreeVector Mon1AbsPosition = G4ThreeVector(0., -y_MuAlcv1/cos(beam_angle), Mon1Abs_z);
        
           new G4PVPlacement(0,
                             Mon1AbsPosition,
                             LVMon1Abs,
                             "MonAbs_1",
                             LVmumonalcv_1,
                             false,
                             0);
        }
        
        //------------------------------
        // Absorber MM2
        //------------------------------
        if(NumiData->GetAbsorberThickness(2) > 0.0)
        {
           G4double Mon2AbsWidth = MonWidth;
           G4double Mon2AbsHeight = MonHeight;
           G4double Mon2AbsLength = NumiData->GetAbsorberThickness(2);
           G4double AbsMon2Dist = NumiData->GetAbsorberMonDist(2);
           G4Material* Mon2AbsMaterial = NumiData->GetAbsorberMaterial(2);
           
           G4cout << "Alcove 3 Absorber:" << G4endl
                  << "  Material = " << Mon2AbsMaterial->GetName() << G4endl
                  << "  Thickness = " <<  Mon2AbsLength/cm << " cm" << G4endl
                  << "  Distance from Mon = " <<  AbsMon2Dist/cm << " cm" << G4endl;
           
        
           G4Box *SMon2Abs = new G4Box("SMon2Abs", Mon2AbsWidth/2.0, Mon2AbsHeight/2.0, Mon2AbsLength/2.0);
           G4LogicalVolume *LVMon2Abs = new G4LogicalVolume(SMon2Abs, Mon2AbsMaterial, "LVMon2Abs", 0, 0, 0);
           
           G4double Mon2Abs_z = -((Mon2AbsLength/2.0)+AbsMon2Dist+(MonLength/2.0));
           if( Mon2Abs_z-(Mon2AbsLength/2.0) > MuAlcv2_length/2.0 ||
               Mon2Abs_z+(Mon2AbsLength/2.0) > -MonLength/2.0)
           {
              G4cout << "*****Problem in NumiSecMonitors::Construct - MM2 Absorber is outside MuMonAlcv_2 " << "and/or"
                     << " the Absorber overlaps with the Alcove " << G4endl;
           }
           
           G4ThreeVector Mon2AbsPosition = G4ThreeVector(0., -y_MuAlcv2/cos(beam_angle), Mon2Abs_z);
           
           new G4PVPlacement(0,
                             Mon2AbsPosition,
                             LVMon2Abs,
                             "MonAbs_2",
                          LVmumonalcv_2,
                             false,
                             0);
           
        }
     }//end AbsorberConfig == "Configured"

     else if(AbsConfig == "Full Coverage" ||
             AbsConfig == "Center Coverage" )
     {

             
        //------------------------------
        // Absorbers
        //------------------------------
        
        G4double MonAbsWidth     = 6.0*in;
        G4double MonAbsHeight    = 6.0*in;
        G4double MonAbsLength    = 2.54*cm;
        G4double AbsMonDistClose = 24.13*cm; //for closest(downstream) panel from the monitor
        G4double AbsMonDistFar   = 24.13*cm + 8.17*in; //for farthest(upstream) panel from the monitor

        G4double BarThickness = 2.0*mm;
        
        G4double AbsPanelWidth  = 4.0*ChamberSpacing + MonAbsWidth + 2.0*BarThickness;
        G4double AbsPanelHeight = 8.0*ChamberSpacing + MonAbsHeight + 2.0*BarThickness;
        G4double AbsPanelLength = MonAbsLength;

        G4double ConnectorInnerRadius = 0*cm;
        G4double ConnectorOuterRadius = 0.8*cm;
        G4double ConnectorHalfHeight  = (ChamberSpacing - MonAbsHeight)/2.0;
        G4double ConnectorStartAngle  = 0*deg;
        G4double ConnectorSpanAngle   = 360*deg;

        G4double BarWidth = AbsPanelWidth - 2.0*BarThickness;
        G4double BarHeight = BarThickness;
        G4double BarLength = MonAbsLength;
        
        G4Material* MonAbsMaterial = Al;

        
        G4cout << "All Alcoves Absorber Configuration \"" << AbsConfig << "\" :" << G4endl
               << "  Material                   = " << MonAbsMaterial->GetName() << G4endl
               << "  Thickness                  = " <<  MonAbsLength/cm    << " cm" << G4endl
               << "  Distance Closest to Mon    = " <<  AbsMonDistClose/cm << " cm" << G4endl
               << "  Distance Farthest from Mon = " <<  AbsMonDistFar/cm   << " cm" << G4endl;
        
        
  
        


        G4Box *SAbsPanel            = new G4Box("SAbsPanel", AbsPanelWidth/2.0, AbsPanelHeight/2.0, AbsPanelLength/2.0);
        G4LogicalVolume *LVAbsPanel = new G4LogicalVolume(SAbsPanel, Air, "LVAbsPanel", 0, 0, 0);

        G4Box *SMonAbs              = new G4Box("SMonAbs", MonAbsWidth/2.0, MonAbsHeight/2.0, MonAbsLength/2.0);
        G4LogicalVolume *LVMonAbs   = new G4LogicalVolume(SMonAbs, MonAbsMaterial, "LVMonAbs", 0, 0, 0);

        G4Tubs *SAbsConn            = new G4Tubs("SAbsConn", ConnectorInnerRadius, ConnectorOuterRadius, ConnectorHalfHeight,
                                                ConnectorStartAngle, ConnectorSpanAngle);
        G4LogicalVolume *LVAbsConn  = new G4LogicalVolume(SAbsConn, MonAbsMaterial, "LVAbsConn", 0, 0, 0);

        G4Box *SHorzBar             = new G4Box("SHorzBar", BarWidth/2.0, BarHeight/2.0, BarLength/2.0);
        G4LogicalVolume *LVHorzBar  = new G4LogicalVolume(SHorzBar, MonAbsMaterial, "LVHorzBar", 0, 0, 0);

        G4Box *SVerBar              = new G4Box("SVerBar", BarHeight/2.0, AbsPanelHeight/2.0, BarLength/2.0);
        G4LogicalVolume *LVVerBar   = new G4LogicalVolume(SVerBar, MonAbsMaterial, "LVVerBar", 0, 0, 0);

        //
        // rotate the tube to "vertical" position
        //
        G4RotationMatrix *xrot = new G4RotationMatrix();
        xrot->rotateX(90*deg);
                 


        //
        //place pixels and connectors in the panel
        //
        for( int i = 0; i < 5; ++i)
        {
           for( int j = 0; j < 9; ++j)
           {
              
              G4ThreeVector MonAbsPosition   = G4ThreeVector(((float)(i-2))*ChamberSpacing, ((float)(4-j))*ChamberSpacing,     0.0);
              
              G4double y_Connector            = ((float)(4-j))*ChamberSpacing - ( ChamberSpacing/2.0 );
              G4ThreeVector AbsConnPosition   = G4ThreeVector(((float)(i-2))*ChamberSpacing, y_Connector, 0.0);
              
              new G4PVPlacement(0, MonAbsPosition, LVMonAbs, "MonAbs", LVAbsPanel, false, i*9+j);

              
              //
              //position absorber connectors
              //
              if(j < 8)
              {
                 new G4PVPlacement(xrot, AbsConnPosition,   LVAbsConn, "AbsConn", LVAbsPanel, false, i*8+j);
              }

           }
        }


        //
        //Place Bars in the panel
        //
        G4ThreeVector TopHorzBarPosition          = G4ThreeVector(0.0, (AbsPanelHeight-BarThickness)/2.0, 0.0);
        G4ThreeVector TopMiddleHorzBarPosition    = G4ThreeVector(0.0, ChamberSpacing+(MonAbsHeight/2.0)+BarThickness, 0.0);
        G4ThreeVector BottomMiddleHorzBarPosition = G4ThreeVector(0.0, -(2.0*ChamberSpacing-(MonAbsHeight/2.0)-BarThickness), 0.0);
        G4ThreeVector BottomHorzBarPosition       = G4ThreeVector(0.0, -(AbsPanelHeight-BarThickness)/2.0, 0.0);

        G4ThreeVector LeftVerBarPosition          = G4ThreeVector(-(AbsPanelWidth-BarThickness)/2.0, 0.0, 0.0);
        G4ThreeVector RightVerBarPosition         = G4ThreeVector((AbsPanelWidth-BarThickness)/2.0, 0.0, 0.0);

        new G4PVPlacement(0, TopHorzBarPosition,          LVHorzBar, "AbsTopBar",          LVAbsPanel, false, 0);
        new G4PVPlacement(0, TopMiddleHorzBarPosition,    LVHorzBar, "AbsTopMiddleBar",    LVAbsPanel, false, 0);
        new G4PVPlacement(0, BottomMiddleHorzBarPosition, LVHorzBar, "AbsBottomMiddleBar", LVAbsPanel, false, 0);
        new G4PVPlacement(0, BottomHorzBarPosition,       LVHorzBar, "AbsBottomBar",       LVAbsPanel, false, 0);

        new G4PVPlacement(0, LeftVerBarPosition,          LVVerBar,  "AbsLeftBar",         LVAbsPanel, false, 0);
        new G4PVPlacement(0, RightVerBarPosition,         LVVerBar,  "AbsRightBar",        LVAbsPanel, false, 0);




        G4double Mon0AbsClose_z = -((MonAbsLength/2.0) + AbsMonDistClose + (MonLength/2.0)) - (Backfill_depth/2.0);
        G4double Mon0AbsFar_z   = -((MonAbsLength/2.0) + AbsMonDistFar   + (MonLength/2.0)) - (Backfill_depth/2.0);
        if( Mon0AbsClose_z - (MonAbsLength/2.0) >  MuAlcv0_length/2.0 ||
            Mon0AbsClose_z + (MonAbsLength/2.0) > -MonLength/2.0      ||
            Mon0AbsFar_z   - (MonAbsLength/2.0) >  MuAlcv0_length/2.0 ||
            Mon0AbsFar_z   + (MonAbsLength/2.0) > -MonLength/2.0 )
        {
           G4cout << "*****Problem in NumiSecMonitors::Construct - MM0 Absorber is outside MuMonAlcv_0 " << "and/or"
                  << " the Absorber overlaps with the Alcove " << G4endl;
        }

        G4double Mon12AbsClose_z = -((MonAbsLength/2.0) + AbsMonDistClose + (MonLength/2.0));
        G4double Mon12AbsFar_z   = -((MonAbsLength/2.0) + AbsMonDistFar   + (MonLength/2.0));

        if( Mon12AbsClose_z - (MonAbsLength/2.0) > MuAlcv1_length/2.0 ||
            Mon12AbsClose_z - (MonAbsLength/2.0) > MuAlcv2_length/2.0 ||
            Mon12AbsClose_z + (MonAbsLength/2.0) > -MonLength/2.0     || 
            Mon12AbsFar_z   - (MonAbsLength/2.0) > MuAlcv1_length/2.0 ||
            Mon12AbsFar_z   - (MonAbsLength/2.0) > MuAlcv2_length/2.0 ||
            Mon12AbsFar_z   + (MonAbsLength/2.0) > -MonLength/2.0 ) 
        
        {
           G4cout << "*****Problem in NumiSecMonitors::Construct - MM1 or MM2 Absorber is outside MuMonAlcv_1 or 2 " << "and/or"
                  << " the Absorber overlaps with the Alcove " << G4endl;
        }

        if(AbsConfig == "Full Coverage")
        {
           //
           //place panels in position
           //
           G4ThreeVector Mon0AbsPositionClose = G4ThreeVector( 3.0*ChamberSpacing, -y_MuAlcv0_mon/cos(beam_angle), Mon0AbsClose_z);
           G4ThreeVector Mon0AbsPositionFar   = G4ThreeVector(-2.0*ChamberSpacing, -y_MuAlcv0_mon/cos(beam_angle), Mon0AbsFar_z);
           G4ThreeVector Mon1AbsPositionClose = G4ThreeVector(-3.0*ChamberSpacing, -y_MuAlcv1/cos(beam_angle),     Mon12AbsClose_z);
           G4ThreeVector Mon1AbsPositionFar   = G4ThreeVector( 2.0*ChamberSpacing, -y_MuAlcv1/cos(beam_angle),     Mon12AbsFar_z);
           G4ThreeVector Mon2AbsPositionClose = G4ThreeVector(-3.0*ChamberSpacing, -y_MuAlcv2/cos(beam_angle),     Mon12AbsClose_z);
           G4ThreeVector Mon2AbsPositionFar   = G4ThreeVector( 2.0*ChamberSpacing, -y_MuAlcv2/cos(beam_angle),     Mon12AbsFar_z);

           new G4PVPlacement(0, Mon0AbsPositionFar,   LVAbsPanel, "MonAbsFar_0", LVmumonalcv_0, false, 0);
           new G4PVPlacement(0, Mon1AbsPositionFar,   LVAbsPanel, "MonAbsFar_1", LVmumonalcv_1, false, 0);
           new G4PVPlacement(0, Mon2AbsPositionFar,   LVAbsPanel, "MonAbsFar_2", LVmumonalcv_2, false, 0);
        
           new G4PVPlacement(0, Mon0AbsPositionClose, LVAbsPanel, "MonAbsClose_0", LVmumonalcv_0, false, 1);
           new G4PVPlacement(0, Mon1AbsPositionClose, LVAbsPanel, "MonAbsClose_1", LVmumonalcv_1, false, 1);
           new G4PVPlacement(0, Mon2AbsPositionClose, LVAbsPanel, "MonAbsClose_2", LVmumonalcv_2, false, 1);
           
        
        }
        
        else if(AbsConfig == "Center Coverage")
        {
           //
           //place panels in position
           //
           G4ThreeVector Mon0AbsPositionClose = G4ThreeVector(0.0, -y_MuAlcv0_mon/cos(beam_angle), Mon0AbsClose_z);
           G4ThreeVector Mon0AbsPositionFar   = G4ThreeVector(0.0, -y_MuAlcv0_mon/cos(beam_angle), Mon0AbsFar_z);
           G4ThreeVector Mon1AbsPositionClose = G4ThreeVector(0.0, -y_MuAlcv1/cos(beam_angle),     Mon12AbsClose_z);
           G4ThreeVector Mon1AbsPositionFar   = G4ThreeVector(0.0, -y_MuAlcv1/cos(beam_angle),     Mon12AbsFar_z);
           G4ThreeVector Mon2AbsPositionClose = G4ThreeVector(0.0, -y_MuAlcv2/cos(beam_angle),     Mon12AbsClose_z);
           G4ThreeVector Mon2AbsPositionFar   = G4ThreeVector(0.0, -y_MuAlcv2/cos(beam_angle),     Mon12AbsFar_z);
           
                                  
           new G4PVPlacement(0, Mon0AbsPositionFar, LVAbsPanel, "MonAbsFar_0", LVmumonalcv_0, false, 0);
           new G4PVPlacement(0, Mon1AbsPositionFar, LVAbsPanel, "MonAbsFar_1", LVmumonalcv_1, false, 0);
           new G4PVPlacement(0, Mon2AbsPositionFar, LVAbsPanel, "MonAbsFar_2", LVmumonalcv_2, false, 0);
           
           new G4PVPlacement(0, Mon0AbsPositionClose, LVAbsPanel, "MonAbsClose_0", LVmumonalcv_0, false, 1);
           new G4PVPlacement(0, Mon1AbsPositionClose, LVAbsPanel, "MonAbsClose_1", LVmumonalcv_1, false, 1);
           new G4PVPlacement(0, Mon2AbsPositionClose, LVAbsPanel, "MonAbsClose_2", LVmumonalcv_2, false, 1);
           
           
        }
        
     }//end AbsorberConfig == "Full Coverage" || "Center Coverage"
     
     //
     // Done with Absorbers
     //**************************************************************************************************************
     //**************************************************************************************************************
     //**************************************************************************************************************
     
     //------------------------------
     // Gap Between Tubes 
     //-----------------------------
     
     G4Box *solidTubeGap = new G4Box("TubeGap", TubeGapWidth/2.0, TubeGapHeight/2.0, TubeGapLength/2.0);
     G4LogicalVolume *logicTubeGap = new G4LogicalVolume(solidTubeGap , Air, "TubeGap",0,0,0);  
     
     for( int i = 0; i < 8; ++i)
     {
        G4ThreeVector TubeGapPosition = G4ThreeVector(((float)(i-4))*ChamberSpacing+(PlateWidth/2.0)+(TubeGapWidth/2.0), 0, 0);
        new G4PVPlacement(0,                // no rotation
                          TubeGapPosition,  // at (x,y,z)
                          logicTubeGap,     // its logical volume  
                          "TubeGap",        // its name
                          LVMuMon,          // its mother  volume
                          false,            // no boolean operations
                          i);               // copy number
     }

     //------------------------------
     // Front Tube Plate
     //-----------------------------
     
     G4Box *solidFrontPlate = new G4Box("FrontTubePlate", PlateWidth/2.0, PlateHeight/2.0 , PlateLength/2.0);
     G4LogicalVolume *logicFrontPlate = new G4LogicalVolume(solidFrontPlate , Al, "FrontTubePlate",0,0,0);  
     
     for( int i = 0; i < 9; ++i)
     {
        G4ThreeVector FrontPlatePosition = G4ThreeVector(((float)(i-4))*ChamberSpacing, 0,-(MonLength/2.0)+(PlateLength/2.0));
        new G4PVPlacement(0,                // no rotation
                          FrontPlatePosition,    // at (x,y,z)
                          logicFrontPlate,       // its logical volume  
                          "FrontTubePlate",  // its name
                          LVMuMon,            // its mother  volume
                          false,                 // no boolean operations
                          i);                    // copy number
     }

     //------------------------------
     // Left Side Tube  Plate
     //-----------------------------
     
     G4Box *solidLeftSidePlate = new G4Box("LeftSidePlate", SidePlateWidth/2.0, SidePlateHeight/2.0 , SidePlateLength/2.0);
     G4LogicalVolume *logicLeftSidePlate = new G4LogicalVolume(solidLeftSidePlate, Al, "LeftSidePlate",0,0,0);  
     
     for( int i = 0; i < 9; ++i)
     {
        G4ThreeVector LeftSidePlatePosition = G4ThreeVector(((float)(i-4))*ChamberSpacing-(PlateWidth/2.0)+(SidePlateWidth/2.0), 0,0);
        new G4PVPlacement(0,                // no rotation
                          LeftSidePlatePosition,    // at (x,y,z)
                          logicLeftSidePlate,       // its logical volume  
                          "LeftSidePlate",  // its name
                          LVMuMon,            // its mother  volume
                          false,                 // no boolean operations
                          i);                    // copy number
     }

     //------------------------------
     // Front IonChamber Plate
     //-----------------------------

     G4Box *solidFrontIonPlate = new G4Box("FrontIonChamberPlate",
                                    IonChamberPlateWidth/2.0, IonChamberPlateHeight/2.0, IonChamberPlateLength/2.0);
     G4LogicalVolume *logicFrontIonPlate = new G4LogicalVolume(solidFrontIonPlate, Alumina, "FrontIonChamberPlate",0,0,0);
     
     for( int i = 0; i < 9; ++i)
     {
        for( int j = 0; j < 9; ++j)
        {
           G4ThreeVector FrontIonPlatePosition = G4ThreeVector(((float)(i-4))*ChamberSpacing, ((float)(4-j))*ChamberSpacing,
                                                               -(IonChamberPlateLength/2.0)-(ChamberLength/2.0));
           new G4PVPlacement(0,             // no rotation
                             FrontIonPlatePosition, // at (x,y,z)
                             logicFrontIonPlate,    // its logical volume  
                             "FrontIonChamberPlate",    // its name
                             LVMuMon,                 // its mother  volume
                             false,                        // no boolean operations
                             i*9+j);                           // copy number
        }
     }

/*
     //------------------------------ 
     // Chamber Layer
     //------------------------------
     //
     //Place a thin layer around the chambers so that the particle will step into it
     // and the energy depostion will only be that deposited in the chamber and
     // not the ion chamber plates.
     //
     
     G4Box *solidChamberLayer = new G4Box("ChamberLayer", (ChamberWidth)/2.0, (ChamberHeight)/2.0, (ChamberLength)/2.0); 
     G4LogicalVolume *logicChamberLayer = new G4LogicalVolume(solidChamberLayer, HeGas, "ChamberLayer", 0, 0, 0);
     
     for( int i = 0; i < 9; ++i)
     {
        for( int j = 0; j < 9; ++j)
        {
           G4ThreeVector ChamberLayerPosition = G4ThreeVector(((float)(i-4))*ChamberSpacing, ((float)(4-j))*ChamberSpacing, 0);
           new G4PVPlacement(0,                     // no rotation
                             ChamberLayerPosition,         // at (x,y,z)
                             logicChamberLayer,            // its logical volume  
                             "ChamberLayer",               // its name
                             LVMuMon,            // its mother  volume
                             false,                   // no boolean operations
                             i*9+j);                      // copy number
        }
     }

     //------------------------------ 
     // Chambers
     //------------------------------

     G4Box *solidChamber = new G4Box("Chamber", (ChamberWidth-LayerWidth)/2.0, (ChamberHeight-LayerWidth)/2.0, (ChamberLength-LayerWidth)/2.0); 
     G4LogicalVolume *logicChamber = new G4LogicalVolume(solidChamber, HeGas, "Chamber", 0, 0, 0);

     G4ThreeVector ChamberPosition = G4ThreeVector(0, 0, 0);
     new G4PVPlacement(0,                     // no rotation
                       ChamberPosition,         // at (x,y,z)
                       logicChamber,            // its logical volume  
                       "MuCell",               // its name
                       logicChamberLayer,            // its mother  volume
                       false,                   // no boolean operations
                       0);                      // copy number

*/
     //------------------------------ 
     // Chambers
     //------------------------------
     G4Box *solidChamber = new G4Box("Chamber", (ChamberWidth)/2.0, (ChamberHeight)/2.0, (ChamberLength)/2.0); 
     G4LogicalVolume *logicChamber = new G4LogicalVolume(solidChamber, HeGas, "Chamber", 0, 0, 0);
     
     for( int i = 0; i < 9; ++i)
     {
        for( int j = 0; j < 9; ++j)
        {
           G4ThreeVector ChamberPosition = G4ThreeVector(((float)(i-4))*ChamberSpacing, ((float)(4-j))*ChamberSpacing, 0);
           new G4PVPlacement(0,                     // no rotation
                             ChamberPosition,         // at (x,y,z)
                             logicChamber,            // its logical volume  
                             "MuCell",               // its name
                             LVMuMon,              // its mother  volume
                             false,                    // no boolean operations
                             i*9+j);                      // copy number
        }
     }

     //------------------------------
     // Back Ion Chamber Plate
     //-----------------------------
     
     G4Box *solidBackIonPlate = new G4Box("BackIonChamberPlate",
                                          IonChamberPlateWidth/2.0, IonChamberPlateHeight/2.0, IonChamberPlateLength/2.0);
     G4LogicalVolume *logicBackIonPlate = new G4LogicalVolume(solidBackIonPlate, Alumina, "BackIonChamberPlate",0,0,0);
     
     for( int i = 0; i < 9; ++i)
     {
        for( int j = 0; j < 9; ++j)
        {
           G4ThreeVector BackIonPlatePosition = G4ThreeVector(((float)(i-4))*ChamberSpacing, ((float)(4-j))*ChamberSpacing,
                                                              (IonChamberPlateLength/2.0)+(ChamberLength/2.0));
           new G4PVPlacement(0,                  // no rotation
                             BackIonPlatePosition,      // at (x,y,z)
                             logicBackIonPlate,         // its logical volume  
                             "BackIonChamberPlate",  // its name
                             LVMuMon,              // its mother  volume
                             false,                     // no boolean operations
                             i*9+j);                        // copy number
        }
     }

     //------------------------------
     // Right Side Tube  Plate
     //-----------------------------
     
     G4Box *solidRightSidePlate = new G4Box("RightSidePlate", SidePlateWidth/2.0, SidePlateHeight/2.0, SidePlateLength/2.0);
     G4LogicalVolume *logicRightSidePlate = new G4LogicalVolume(solidRightSidePlate , Al, "RightSidePlate",0,0,0);  
     
     for( int i = 0; i < 9; ++i)
     {
        G4ThreeVector RightSidePlatePosition = G4ThreeVector(((float)(i-4))*ChamberSpacing+(PlateWidth/2.0)-(SidePlateWidth/2.0), 0, 0);
        new G4PVPlacement(0,                // no rotation
                          RightSidePlatePosition,    // at (x,y,z)
                          logicRightSidePlate,       // its logical volume  
                          "RightSidePlate",  // its name
                          LVMuMon,            // its mother  volume
                          false,                 // no boolean operations
                          i);                    // copy number
     }

     //------------------------------
     // Back Tube Plate
     //-----------------------------
     
     G4Box *solidBackPlate = new G4Box("BackTubePlate", PlateWidth/2.0, PlateHeight/2.0, PlateLength/2.0);
     G4LogicalVolume *logicBackPlate = new G4LogicalVolume(solidBackPlate, Al, "BackTubePlate",0,0,0);
     
     for( int i = 0; i < 9; ++i)
     {
        G4ThreeVector BackPlatePosition = G4ThreeVector(((float)(i-4))*ChamberSpacing, 0, (MonLength/2.0)-(PlateLength/2.0));
        new G4PVPlacement(0,                // no rotation
                          BackPlatePosition,    // at (x,y,z)
                          logicBackPlate,       // its logical volume  
                          "BackTubePlate",  // its name
                          LVMuMon,            // its mother  volume
                          false,                 // no boolean operations
                          i);                    // copy number
     }
     
     
  }
  else
  {

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
     
     //G4PVPlacement *MuMon0 = new G4PVPlacement(0, MuMon_pos0, "MuMon_0", LVMuMon, MuMonAlcv_0, false, 0);
     new G4PVPlacement(0, MuMon_pos0, "MuMon_0", LVMuMon, MuMonAlcv_0, false, 0);
     new G4PVPlacement(0, MuMon_pos1, "MuMon_1", LVMuMon, MuMonAlcv_1, false, 0);
     new G4PVPlacement(0, MuMon_pos2, "MuMon_2", LVMuMon, MuMonAlcv_2, false, 0);


     //
     //chamber dimensions
     //
     G4double ChamberWidth   = 3*in;   
     G4double ChamberHeight  = 3*in;
     G4double ChamberLength  = 1.0*mm;
     G4double ChamberSpacing = 10*in;
     //
     //Chamber Layer 
     //
     G4double LayerWidth = 0.01*mm;

     //------------------------------ 
     // Chamber Layer
     //------------------------------
     //
     //Place a thin layer around the chambers so that the particle will step into it
     // and the energy depostion will only be that deposited in the chamber and
     // not the ion chamber plates.
     //
     
     G4Box *solidChamberLayer = new G4Box("ChamberLayer", (ChamberWidth)/2.0, (ChamberHeight)/2.0, (ChamberLength)/2.0); 
     G4LogicalVolume *logicChamberLayer = new G4LogicalVolume(solidChamberLayer, HeGas, "ChamberLayer", 0, 0, 0);
     
     for( int i = 0; i < 9; ++i)
     {
        for( int j = 0; j < 9; ++j)
        {
           G4ThreeVector ChamberLayerPosition = G4ThreeVector(((float)(i-4))*ChamberSpacing, ((float)(4-j))*ChamberSpacing, 0);
           new G4PVPlacement(0,                     // no rotation
                             ChamberLayerPosition,         // at (x,y,z)
                             logicChamberLayer,            // its logical volume  
                             "ChamberLayer",               // its name
                             LVMuMon,            // its mother  volume
                             false,                   // no boolean operations
                             i*9+j);                      // copy number
        }
     }

     //------------------------------ 
     // Chambers
     //------------------------------

     G4Box *solidChamber = new G4Box("Chamber", (ChamberWidth-LayerWidth)/2.0, (ChamberHeight-LayerWidth)/2.0, (ChamberLength-LayerWidth)/2.0); 
     G4LogicalVolume *logicChamber = new G4LogicalVolume(solidChamber, HeGas, "Chamber", 0, 0, 0);

     G4ThreeVector ChamberPosition = G4ThreeVector(0, 0, 0);
     new G4PVPlacement(0,                     // no rotation
                       ChamberPosition,         // at (x,y,z)
                       logicChamber,            // its logical volume  
                       "MuCell",               // its name
                       logicChamberLayer,            // its mother  volume
                       false,                   // no boolean operations
                       0);                      // copy number 
     /*
     G4Box *SMuCell = new G4Box( "SMuCell", 3*in/2.0, 3*in/2.0, 1*mm/2.0 );
     G4LogicalVolume *LVMuCell = new G4LogicalVolume(SMuCell, He, "LVMuCell", 0, 0, 0);  
     volName = "MuCell";
     
     for( int i = 0; i < 9; ++i )
     {
        for( int j = 0; j < 9; ++j )
        {
           cellPos = G4ThreeVector( (i-4)*10.*in, (j-4)*10.*in, 0 );
           new G4PVPlacement( 0, cellPos, volName, LVMuCell, MuMon0, false, i*9+j ); 
        }
     }
     */
     
  }

  
  G4cout << "Muon Monitor Alcoves(1,2,3) Constructed" << G4endl;
  
}
