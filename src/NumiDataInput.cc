//----------------------------------------------------------------------
//
//
// $Id: NumiDataInput.cc,v 1.31.2.11 2010/03/17 15:55:11 corwin Exp $
//----------------------------------------------------------------------


/**
 * Modification and additions made by Vamis Xhagjika are enclosed into VXADD or VXMOD comments, and old values for the modified ones are kept with a comment and begin with OLD
 **/

#include "NumiDataInput.hh"
#include "G4ThreeVector.hh"
#include "NumiHornSpiderSupport.hh"
#include "G4UserLimits.hh"
//#include "globals.hh"
#include <math.h>
#include <fstream>

static const G4double in=2.54*cm;

NumiDataInput* NumiDataInput::fNumiDataInput = 0;

NumiDataInput* NumiDataInput::GetNumiDataInput(){
    G4cout << "Requesting NumiDataInput " << G4endl;
    if (!fNumiDataInput) {
        G4cout << "Constructing NumiDataInput " << G4endl;
        fNumiDataInput = new NumiDataInput();    
    }
    return fNumiDataInput;
}

NumiDataInput::NumiDataInput()
{
  G4cout << "NumiDataInput Constructor Called" << G4endl;
  if (fNumiDataInput)
    { G4Exception("NumiDataInput constructed twice.");}
//  fNumiDataInput = this;

//Adding code, based on Alex Himmel to allow for the use of CURRENT and TARGETZ environment variables
  bool useFile = false;
  bool useSystFile = false;
  int runPeriodFile = -999;
  double targetZFile = -999.;
  double hornCurrentFile = -999.;
  
  std::ifstream datafile("../runConfig.inp");
  if (datafile.is_open()) {
    useFile = true;
    datafile  >> runPeriodFile >> targetZFile >> hornCurrentFile;
    datafile.close();
    G4cout << G4endl << G4endl << G4endl << G4endl
	   << G4endl << G4endl << G4endl << G4endl 
	   << G4endl << G4endl << G4endl << G4endl 
    << G4endl << G4endl << G4endl << G4endl 
        << "Using parameters from runConfig.inp: " << G4endl
           << "  Run Period = " << runPeriodFile << G4endl
           << "  Target Z = " << targetZFile << G4endl
           << "  Horn Current = " << hornCurrentFile << G4endl;
    if (hornCurrentFile == 185) hornCurrentFile = 182.1;
    else if (hornCurrentFile == 170) hornCurrentFile = 167.3;
    else if (hornCurrentFile == 200) hornCurrentFile = 196.9;
    else if (hornCurrentFile == 0) hornCurrentFile = 0;
    else {
        G4cout << "Several horn currents are translated from nominal values to actual values (e.g. 185 to 182.1).  This current is not one of them, so I will just use the nominal value." << G4endl;
    }
  }  

  //


  pSurfChk = true; // Set to pSurfChk to true to check for overlapping regions
  debugOn = false;
  NImpWeightOn = true; 
  createNuNtuple=false;  createHadmmNtuple=false;
  createASCII=false;     createBXDRAW = false;
  useFlukaInput = false; useMarsInput=false;

  //
  //can set useMuonInupt and useMuonBeam from macro
  //
  useMuonInput = false;  useMuonBeam = false; fNEvents = -1;
  useTestBeam = false;   
  useDecayPipeSelect = false;
  KillTracking = true; // false for ahimmel
  testTheta = M_PI/6.;
  
  StepLimit = 0.0; 

  extNtupleFileName=""; //fluka or mars or muon ntuple with particles coming of the target
  //Set the energy threshold for 'killing' particles
  KillTrackingThreshold = 0.05*GeV; //for defaut neutrino MC g4numi 

   //KillTrackingThreshold = 0.001*GeV; //for muon beam MC


   //base name for output files:
   nuNtupleName    = "nuNtuple";
   //
   //can set hadmmNtupleDir and hadmmNtupleName from macro
   //
   hadmmNtupleDir  = "./";
   hadmmNtupleName = "";
   asciiName       = "asciiOut";
   bxdrawName      = "bxdrawOut";
   RunNumber       = "0000";
   geometry        = "";

   //
   // Denotes the change in sigma to the
   // rock density and muon alcove wall location
   //
   materialSigma   = 0;
   

   //===================================
   //------ Use Macro---------------
   // useMacro = true if you actually want to change beam parameters
   // from the macro 
   
   useMacro = false;

  //====================================
  //-- Changes for "Air Horns"---------- >> G4numi validation
  //-- Changes for "Truncated Horns"-----
  airhrn =false; // airhrn must be changed before compilation
  vacuumworld=false;
  jCompare = false; // make horns have the same B field;
  g3Chase = true;
  
if(!vacuumworld && !airhrn){
  hrnmat = 9;   // Al
  hrnmatcr =31; // CT852
  hallmat=15;   // Air
}
 else {
   if(airhrn){
     hrnmat=15;
     hrnmatcr=15;
     hallmat=15;
    }
   if(vacuumworld){
     hrnmat=16;
     hrnmatcr=16;
     hallmat=16;
   }
 }
 
//======================================
 
//======================================
//--Ray Tracing parameter-------------
//--------------------------------------
 raytracing =false;
 //==============================
 //------------------------------
 
 //initialize zpoints
 NZpoint=48;
 Zpoint.push_back(0*m); //1
 Zpoint.push_back(0.5*m); //2
 Zpoint.push_back(0.7*m); //3
 Zpoint.push_back(0.75*m); //4
 Zpoint.push_back(0.8*m); //5
 Zpoint.push_back(0.85*m); //6
 Zpoint.push_back(0.9*m); //7
 Zpoint.push_back(1*m); //8
 Zpoint.push_back(1.25*m); //9
 Zpoint.push_back(1.5*m); //10
 Zpoint.push_back(1.75*m); //11
 Zpoint.push_back(2*m);//12
 Zpoint.push_back(2.25*m);//13
 Zpoint.push_back(2.5*m);//14
 Zpoint.push_back(2.75*m); //15
 Zpoint.push_back(3*m);//16
 Zpoint.push_back(3.25*m);//17
 Zpoint.push_back(3.5*m);//18
 Zpoint.push_back(4*m);//19
 Zpoint.push_back(6*m);//20
 Zpoint.push_back(9*m);//21
 Zpoint.push_back(9.5*m); //22
 Zpoint.push_back(9.25*m); //23
 Zpoint.push_back(10*m);//24
 Zpoint.push_back(10.25*m);//25
 Zpoint.push_back(10.5*m);//26
 Zpoint.push_back(10.75*m);//27
 Zpoint.push_back(11.80*m);//28
 Zpoint.push_back(11.9*m);//29
 Zpoint.push_back(11*m);//30
 Zpoint.push_back(11.1*m);//31
 Zpoint.push_back(11.2*m);//32
 Zpoint.push_back(11.25*m);//33
 Zpoint.push_back(11.5*m);//34
 Zpoint.push_back(11.75*m);//35
 Zpoint.push_back(12*m);//36
 Zpoint.push_back(12.25*m);//37
 Zpoint.push_back(12.5*m);//38
 Zpoint.push_back(12.75*m);//39
 Zpoint.push_back(13*m);//40
 Zpoint.push_back(13.25*m);//41
 Zpoint.push_back(13.5*m);//42
 Zpoint.push_back(13.75*m);//43
 Zpoint.push_back(14*m);//43
 Zpoint.push_back(14.5*m);//44
 Zpoint.push_back(15.*m);//45
 Zpoint.push_back(16.*m);//46
 Zpoint.push_back(20*m);//47
 Zpoint.push_back(45.7*m);//48
 createZpNtuple=false;
 zpNtupleName="zpNtuple";
 //==============================================================
  G4float beam_x_dir = 0;
  G4float beam_y_dir = 0;
  G4float beam_z_dir = 1;//cos(.01*pi/180);
  //actual dm is 5/13e-4 radians 
  G4float beam_x_pos = 0;
  G4float beam_y_pos = 0;
  G4float beam_z_pos = -3.9*m;
  // the reason for the beam_z_pos change was to move the beam to start
  // immediately before the target so that the beam spot interaction point
  // would remain constant, but the angle would change.

  protonMomentum = 120.*GeV;  
  beamSigmaY     = 0;//1.25*mm;
  beamSigmaX     = 0;//1.1*mm;
  beamDirection  = G4ThreeVector(beam_x_dir,beam_y_dir,beam_z_dir);
  beamPosition  = G4ThreeVector(beam_x_pos,beam_y_pos,beam_z_pos);

  protonKineticEnergy = sqrt(pow((.938*GeV),2)+pow(protonMomentum,2))-0.938*GeV;

  //Rock        1
  //=======================================================================
  RockRadius  = 10.0*m;
  RockHalfLen = 1200.0*m;
  RockDensity = 2.41*g/cm3; // not
  RockRadLen  = 0.0;        // used

  double TargetConfigZ = 0*cm; //Incorporating TARGETZ environment variable
  if (useFile) {
    TargetConfigZ = -1*targetZFile*cm; //TARGETZ is specified in negative cm
  }

  constructTarget = true;
  //TargetArea          1
  //=======================================================================
  TargetAreaZ0       = -6.7*m;//was -4.0*m (08/09/05);
  TargetAreaLength   = 52.398*m;//was 49.28*m (08/09/05);

  // TargetAreaHeight and TargetAreaWidth were 6.0 meters in Zarko's older version, 
  // but I had to extend them by 1 m to fit the concrete chase in TGAR 
  // (working from dimensions and placement given in the Numi Technical Design Handbook)
  // I (Zarko) had to add 1.5m more to fit all blocks
  TargetAreaHeight   = 8.5*m;
  TargetAreaWidth    = 8.5*m;
  TargetAreaGEANTmat = 15;
  

  // =================================================
  // Nova Medium Energy Target
  // =================================================

  //  48 Target segments 
  // + 1 Budal VFHS (Vertical Fin for Horizontal Scan)
  // + 1 Budal HFVS (Horizontal Fin for Vertical Scan)

  TargetDxdz           = 0.0; // doesn't
  TargetDydz           = 0.0; // work properly yet
  TargetSegLength      = 24.0*mm;
  TargetSegWidth       = 6.4*mm;
  TargetSegHeight      = 63.0*mm;
  TargetSegPitch       = 0.5*mm;
  TargetGraphiteHeight = 150.0*mm;
  TargetEndRounded     = true;
  TargetSegmentNo      = 48;

  BudalVFHSLength      = TargetSegLength;
  BudalVFHSWidth       = TargetSegWidth;
  BudalVFHSHeight      = TargetSegHeight;
  BudalVFHSPitch       = 4.5*mm;
  BudalVFHSEndRounded  = TargetEndRounded;

  BudalHFVSLength      = TargetSegLength;
  BudalHFVSWidth       = TargetSegWidth;
  BudalHFVSHeight      = TargetSegHeight;
  BudalHFVSPitch       = 5.0*mm;
  BudalHFVSEndRounded  = TargetEndRounded;

  // The material "Target" is defined in NumiMaterials.cc
  // using these properties
  TargetA              = 12.01*g/mole;
  TargetZ              = 6.0;
  TargetDensity        = 1.78*g/cm3;
   

  // Default location of the Target wrt the MCZERO location
  // Downstream end of target is fixed at -20 cm with respect to MCZERO
  // MCZERO is at location (0,0,0) in the ROCK volume.
  // Work upstream from there to find the upstream end of the Target
  TotalTargetLength =   
    TargetSegmentNo*TargetSegLength 
    + (TargetSegmentNo-1)*TargetSegPitch 
    + BudalVFHSLength + BudalVFHSPitch 
    + BudalHFVSLength + BudalHFVSPitch;
  TargetX0           = 0.0;
  TargetY0           = 0.0;
  TargetZ0           = (-20*cm - TotalTargetLength) + TargetConfigZ;//Allows TARGETZ to shift position of the target

  //  G4cout << "TargetZ0 = " << TargetZ0/cm << " (should be equal to -143.3 cm)" << G4endl;;


  // The distance between the downstream end of the target and the
  // downstream canister flange is 8 cm.
  // The centerline of the target canister is 4.15 cm below the beamline 
  TargetEndtoDnFlange = 8.0*cm;
  TargetCanisterCenterOffset = -4.15*cm;

  // The X0, YO, and Z0 for the flanges and cansiter are with respect to the
  // the canister center for X0 and Y0 and the start of the target material for Z0 
  // X0 and Y0 are the locations of the center of the volume, and Z0 is the location of the upstream end.
  TargetDnFlangeLength = 2.5*cm;
  TargetDnFlangeOutRad = 15.0*cm;
  TargetDnFlangeX0     = 0.0;
  TargetDnFlangeY0     = TargetCanisterCenterOffset;
  TargetDnFlangeZ0     = TotalTargetLength + TargetEndtoDnFlange;

  // Part of downstream flange is cutout to remove material
  // Part of downstream flange is cutout for the Be window
  TargetDnFlangeCutoutLength = 1.75*cm;
  TargetDnBeWindowRadius     = 60*mm;
  TargetDnBeWindowLength     = 1*mm;


  // The body of the target canister is modeled with three layers 
  // to approximate the actual geometry given in nova-doc 3681-v3 :
  //   The outer shell is 3mm thick aluminum
  TargetOutsideCasingOutRad = 15.0*cm;
  TargetOutsideCasingInRad  = 14.7*cm;
  TargetCasingWaterOutRad   = TargetOutsideCasingInRad;
  TargetCasingWaterInRad    = 13.37*cm;
  TargetInsideCasingOutRad  = TargetCasingWaterInRad;
  TargetInsideCasingInRad   = 12.3*cm;

  TargetCasingLength = 145*cm;
  TargetCasingX0     = 0.0;
  TargetCasingY0     = TargetCanisterCenterOffset;
  TargetCasingZ0     = TargetDnFlangeZ0 - TargetCasingLength;


  // Upstream flange
  TargetUpFlangeLength = 2.5 * cm;
  TargetUpFlangeOutRad = 15.0 * cm;
  TargetUpFlangeX0     = 0.0;
  TargetUpFlangeY0     = TargetCanisterCenterOffset;
  TargetUpFlangeZ0     = TargetCasingZ0 - TargetUpFlangeLength;


  // Upstream Be window flange
  TargetUpBeFlangeLength = 0.5*in ;
  TargetUpBeFlangeOutRad = 2.73*in / 2.0;
  TargetUpBeFlangeX0 = 0.0;
  TargetUpBeFlangeY0 = 0.0; 
  TargetUpBeFlangeZ0 =  TargetUpFlangeZ0 - TargetUpBeFlangeLength;

  TargetUpBeFlangeCutoutLength = 0.25*in;
  TargetUpBeFlangeCutoutRadius = 1.75/2.0*in;

  TargetUpBeWindowRadius = 0.5*in;
  TargetUpBeWindowLength = 0.01*in;

  // The target graphite material is clamped between the pressing plate and cooling plate 
  // Pressing plate 
  PressingPlateLength = TotalTargetLength;
  PressingPlateHeight = TargetGraphiteHeight - TargetSegHeight;
  PressingPlateWidth  = 20.0*mm;
  PressingPlateX0     = -PressingPlateWidth/2.0 - TargetSegWidth/2.0;
  PressingPlateY0     = -PressingPlateHeight/2.0 -TargetSegHeight + TargetSegWidth/2.0;
  PressingPlateZ0     = 0.0;

  PressingPlateCutoutWidth  = 5.0*mm;
  PressingPlateCutoutHeight = 60.0*mm;

  // Cooling plate
  CoolingPlateLength = TotalTargetLength;
  CoolingPlateHeight = PressingPlateHeight;
  CoolingPlateWidth  = 20.0*mm;
  CoolingPlateX0     = CoolingPlateWidth/2.0 + TargetSegWidth/2.0;
  CoolingPlateY0     = -CoolingPlateHeight/2.0 - TargetSegHeight + TargetSegWidth/2.0;
  CoolingPlateZ0     = 0.0;

  CoolingPlateCutoutWidth  = PressingPlateCutoutWidth;
  CoolingPlateCutoutHeight = PressingPlateCutoutHeight;

  // The water cooling runs through the cooling plate
  CoolingWaterPipeOutRad = 5.0*mm;
  CoolingWaterPipeX0     = 0.0*mm;
  CoolingWaterPipeY0     = CoolingPlateCutoutHeight/2.0 + (CoolingPlateHeight - CoolingPlateCutoutHeight)/4.0;


  //=======================================================================
  //  Baffle
  //=======================================================================
  HPBaffleLength     =  1.50*m;
  HPBaffleRin        =  5.5*mm;
  HPBaffleRout       =  3.*cm;
  HPBaffleX0         =  0.00;
  HPBaffleY0         =  0.00;
  HPBaffleZ0         =  -380*cm+ TargetConfigZ; //If the target is shifted, the baffle must move too.
  HPBaffleDXDZ       =  0.0;
  HPBaffleDYDZ       =  0.0;


  //Tunnel         1
  //=======================================================================
  TunnelZ0       = 45.6985*m; //was 45.28*m (08/09/05);
  TunnelRadius   = 3.3*m+.5*m; //added .5m because hadron absorber does not fit entirely inside 3.3
  TunnelLength   = 693.4415*m; //was 693.86*m (08/09/05);
  TunnelA        = 0.0;
  TunnelZ        = 0.0;
  TunnelGEANTmat = 15;
  BeamAngle      = 0.05835; // .05835 in radians.

  //ShieldNshield  5 
  //======================================================================= 
  ShieldX0       = 0.0; 
  ShieldY0       = 0.0; 
  ShieldZ0       = 45.699*m; //was 45.28*m (08/09/05);   
  ShieldDxdz     = 0.0; // not 
  ShieldDydz     = 0.0; // used
  ShieldLength   = 676.681*m; //was 677.1*m (08/09/05);  
  ShieldRout     = 2.23*m; 
  ShieldRin      = 1.0097*m; 
  ShieldGEANTmat = 17;
  
  //DecayPipe          1    
  //=======================================================================
  DecayPipeZ0        = 45.699*m; //was 45.28*m (08/09/05);
  //DecayPipeRadius    = 0.9716*m; // was 0.9906 but doesnt correlate w gnumi
  DecayPipeRadius    = 0.9906*m;
  DecayPipeLength    = 676.681*m; //was 677.1*m (08/09/05);
  DecayPipeFWinThick = 1.60E-3*m;
  DecayPipeEWinThick = 4.76E-3*m;
  DecayPipeWallThick = 1.905E-2*m;
  DecayPipeA         = 55.85;
  DecayPipeZ         = 26.0;
  DecayPipeGEANTmat  = 10;
  DecayPipeFWinmat   = 9;
  DecayPipeEWinmat   = 10;

  // New Target Hall by Zach Barnett 
  
  //==========================================================================
  // TargetHallChase       1
  
  NTHConcreteSectionsN = 6;
  
  // units in meters, dimensions based on Numi note.
  G4double THConcreteX0_[]= {3.36073, 2.60670, 0.0, -2.60670, -3.36073, 0.0};
  G4double THConcreteY0_[]= {2.801, 0.0, -2.801, 0, 2.801, 3.45665};
  // these Z0 values are set with respect to the ROCK volume, but NumiTargetHall.cc adjusts them to the TGAR volume 
  G4double THConcreteZ0_[]= {19.499, 19.499, 19.499, 19.499, 19.499, 19.499};
  G4double THConcreteLength_[]= {52.397, 52.397, 52.397, 52.397, 52.397, 52.397};
  G4double THConcreteHdx_[]={.754025,.46675,3.07345,.46675,.754025, 2.921};
  G4double THConcreteHdy_[]={.42705,2.37395,.42705,2.37395,.42705, .2286};
  G4double THConcreteDxdz_[]={0,0,0,0,0, 0.0};
  G4double THConcreteDydz_[]={0,0,0,0,0, 0.0};
  G4int THConcreteGeantMaterial_[] = {17,17,17,17,17, 17};
  G4String THConcreteName_[]={"Section1","Section2","Section3","Section4","Section5","Section6(lid)"};

  for (G4int ii=0;ii<NTHConcreteSectionsN;ii++){
	  THConcreteX0.push_back(THConcreteX0_[ii]*m);
	  THConcreteY0.push_back(THConcreteY0_[ii]*m);
	  THConcreteZ0.push_back(THConcreteZ0_[ii]*m);
	  THConcreteDxdz.push_back(THConcreteDxdz_[ii]*m);
	  THConcreteDydz.push_back(THConcreteDydz_[ii]*m);
	  THConcreteLength.push_back(THConcreteLength_[ii]*m);
	  THConcreteHdx.push_back(THConcreteHdx_[ii]*m);
	  THConcreteHdy.push_back(THConcreteHdy_[ii]*m);
	  THConcreteGeantMaterial.push_back(THConcreteGeantMaterial_[ii]);
	  THConcreteName.push_back(THConcreteName_[ii]);
  }
  
  //==========================================================================

  //======================================================================= 
  // Target Hall shielding (18 blocks, numbered 0-17)

  if(g3Chase){
    THBlockNblock = 24;
  }
  else{
    THBlockNblock=18;
  }
  // reminder: all length dimensions for the target shielding blocks are in meters.
  //These are the Duratek blocks mentioned in the Numi Technical Design Handbook.

  //Blocks 15-17 are not actually in the Numi Technical Design Handbook--they are used
  //to provide a covering over the top

  G4double THBlockX0_[]    = { 0.0,      //Block 0
							   1.3554,   //Block 1
							   -1.3554,  //Block 2
							   1.7158,    //Block 3
							   .6904,     //Block 4
							   -.6904,    //Block 5
							   -1.7158,   //Block 6
							   1.0254,      //Block 7 
							   -1.0254,     //Block 8
							   1.7158,    //Block 9
							   -1.7158,   //Block 10
							   1.0254,      //Block 11
							   -1.0254,     //Block 12
							   1.7158,    //Block 13
							   -1.7158,    //Block 14
							   0.6904,        //Block15 (top block1--one of the slightly enlarged blocks)
							   -0.6904,        //Block16 (top block2--one of the slightly enlarged blocks)
							   0.0,        //Block17 (top block3)
			       0.0,        // Block 18 (bottom gnumi block)
			       0.6373,  // Block 19 ( side 1 gnumi block)
			       -0.6373, // Block 20 (Side 2 gnumi block)
			       0.0, // Block 21 (top gnumi block bf Horn2)
			       0.0,//
			       0.0//
  };

  G4double THBlockY0_[]    = { -2.03895,      //Block 0
							   -2.03895,      //Block 1
							   -2.03895,      //Block 2
							   -1.03895,      //Block 3
							   -1.36895,      //Block 4
							   -1.36895,      //Block 5
							   -1.03895,      //Block 6
							   -0.36895,     //Block 7
							   -0.36895,     //Block 8
							   0.29105,     //Block 9
							   0.29105,     //Block 10
							   0.96105,      //Block 11
							   0.96105,      //Block 12
							   1.62105,     //Block 13
							   1.62105,      //Block 14
							   1.98395,        //Block15 (top block1--one of the slightly enlarged blocks)
							   1.98395,        //Block16 (top block2--one of the slightly enlarged blocks)
							   1.29355,        //Block17 (top block3)
			       -0.993215, //Block 18 (bottom gnumi block)
                               -0.0377, //Block 19 (side 1 gnumi block)
                               -0.0377, //Block 20 (side 2 gnumi block)
                               0.657075,//Block 21 (top gnumi block)
			       0.657075, // Block 22 (top gnumi block after h2)
                               0.672075 //Block 23 above Horn2
  };

  
  
  // these Z0 values are set with respect to the ROCK volume, but NumiTargetHall.cc adjusts them to the TGAR volume

  G4double THBlockZ0_[]= {19.499};
  G4double THBlockDxdz_[] = {0.0};
  G4double THBlockDydz_[] = {0.0};
	  
  /*	  
  // these Z0 values are set with respect to the ROCK volume, but NumiTargetHall.cc adjusts them to the TGAR volume 
  G4double THBlockZ0_[]    = { 20.64,        //Block 0
  20.64,        //Block 1
							   20.64,        //Block 2
							   20.64,        //Block 3
							   20.64,        //Block 4
							   20.64,        //Block 5
							   20.64,        //Block 6
							   20.64,        //Block 7
							   20.64,        //Block 8
							   20.64,        //Block 9
							   20.64,        //Block 10
							   20.64,        //Block 11
							   20.64,        //Block 12
							   20.64,        //Block 13
							   20.64,         //Block 14
							   20.64,        //Block15 (top block1--one of the slightly enlarged blocks)
							   20.64,        //Block16 (top block2--one of the slightly enlarged blocks)
							   20.64,        //Block17 (top block3)
							   19.499,//Block 18 bottom
							   19.499,//Block 19 side
							   19.499,//Block 20 side	
							   19.499,//Block 21 top
							   19.499,//Block 22 top
							   19.499,//Block 23 top

  };

	  
  G4double THBlockDxdz_[]  = { 0.0,        //Block 0
							   0.0,        //Block 1
							   0.0,        //Block 2
							   0.0,        //Block 3
							   0.0,        //Block 4
							   0.0,        //Block 5
							   0.0,        //Block 6
							   0.0,        //Block 7
							   0.0,        //Block 8
							   0.0,        //Block 9
							   0.0,        //Block 10
							   0.0,        //Block 11
							   0.0,        //Block 12
							   0.0,        //Block 13
							   0.0,         //Block 14
							   0.0,        //Block15 (top block1--one of the slightly enlarged blocks)
							   0.0,        //Block16 (top block2--one of the slightly enlarged blocks)
							   0.0,        //Block17 (top block3)
                             };

  G4double THBlockDydz_[]  = { 0.0,        //Block 0
			       0.0,        //Block 1
			       0.0,        //Block 2
			       0.0,        //Block 3
			       0.0,        //Block 4
			       0.0,        //Block 5
			       0.0,        //Block 6
			       0.0,        //Block 7
			       0.0,        //Block 8
			       0.0,        //Block 9
			       0.0,        //Block 10
			       0.0,        //Block 11
			       0.0,        //Block 12
			       0.0,        //Block 13
			       0.0,         //Block 14
			       0.0,        //Block15 (top block1--one of the slightly enlarged blocks)
			       0.0,        //Block16 (top block2--one of the slightly enlarged blocks)
			       0.0,        //Block17 (top block3)
                             };
  */
	  
  // all normal blocks have the same Length, Hdx, and Hdy values.  These values for the slightly larger blocks are set
  // in NumiTargetHall.cc, line 103
  G4double THBlockLength_[]= {52.397};

  G4double THBlockHdx_[]   = {1.33/2.0};

  G4double THBlockHdy_[]   = {0.67/2.0};

  G4int THBlockGeantMaterial_[] = {10};

  G4String THBlockName_[]  = {"BLK0",
			      "BLK1",
			      "BLK2",
			      "BLK3",
			      "BLK4",
			      "BLK5",
			      "BLK6",
			      "BLK7",
			      "BLK8",
			      "BLK9",
			      "BLK10",
			      "BLK11",
			      "BLK12",
			      "BLK13",
			      "BLK14",
			      "BLK15",
			      "BLK16",
			      "BLK17",
			      "BLK18",
			      "BLK19",
			      "BLK20",
			      "BLK21",
			      "BLK22",
			      "BLK23"

                              };

  //This next block puts all the block coordinates into vectors and assigns the unit "meters" to the values.
  // Mike Martens (Need to move Horn 2 by 9 meters downstream)
  // Lengths of blocks 21, 22, 23 are set in NumiTargetHall.cc
  for (G4int ii=0;ii<THBlockNblock;ii++){
    THBlockX0.push_back(THBlockX0_[ii]*m);
    THBlockY0.push_back(THBlockY0_[ii]*m);
    if(ii==21){
	
      //Moving blocks 21,22,and 23 to accomodate NOvA position for horn2
   	
      THBlockZ0.push_back(1.15*m+4.5*m);

    }
    else if(ii==22) THBlockZ0.push_back(30.3485*m+4.5*m);
    else if(ii==23) THBlockZ0.push_back(12*m+9*m);

    else    THBlockZ0.push_back(THBlockZ0_[0]*m);
    THBlockDxdz.push_back(THBlockDxdz_[0]*m);
    THBlockDydz.push_back(THBlockDydz_[0]*m);
    THBlockLength.push_back(THBlockLength_[0]*m);
    THBlockHdx.push_back(THBlockHdx_[0]*m);
    THBlockHdy.push_back(THBlockHdy_[0]*m);
    THBlockGeantMaterial.push_back(THBlockGeantMaterial_[0]);
    THBlockName.push_back(THBlockName_[ii]);
  }

  //======================================================================= 
  // Hadron Box Dimensions
  // 55.75' is the surveyed distance between the
  // downstream wall of Muon Alcove 1 and the
  // nominal center of the Decay Pipe.
  HadrBox_width = 324*.0254*m;
  HadrBox_height = 6.6294*m;
  HadrBox_length = 55.75*12*in-(4*12*in+2.24*in+9*12*in+9*in);

  //VXMOD
  HornCurrent=200000.*ampere;//OLD 182100.*ampere;
  //Added by L. Corwin to incorporate CURRENT environment variable
  if (useFile) {
    HornCurrent = hornCurrentFile*1000*ampere; // Convert to kA
  }

  G4cout << "Running with: " << G4endl
    // << "  Run Period = " << runPeriod << G4endl
       << "  Target Z = " << TargetConfigZ/cm << " cm" << G4endl
       << "  Horn Current = " << HornCurrent/ampere/1000. << " kA" << G4endl;




  //~VXMOD
  NPHorn2EndN=3;
  
  G4double PHorn2EndZ0_[]     ={135.861        ,137.611     ,139.486};
  if (jCompare) {
	PHorn2EndZ0_[0] = 118.11;
	PHorn2EndZ0_[1] = 119.86;
	PHorn2EndZ0_[2] = 122.048;	
  }
  G4double PHorn2EndLength_[] ={1.75           ,2.188       ,.625};
  G4double PHorn2EndRin_[]    ={12.719         ,12.532      ,11.};
  G4double PHorn2EndRout_[]   ={14.405         ,14.469      ,12.532};
  G4int PHorn2EndGeantMat_[]  ={31             ,  9         ,  9 };  
  G4String PHorn2EndVolName_[]={"PHorn2InsRing","PHorn2CPB1","PHorn2CPB2"};

  for (G4int ii=0;ii<NPHorn2EndN;ii++){
    PHorn2EndZ0.push_back(PHorn2EndZ0_[ii]*in);
    PHorn2EndLength.push_back(PHorn2EndLength_[ii]*in);
    PHorn2EndRin.push_back(PHorn2EndRin_[ii]*in);
    PHorn2EndRout.push_back(PHorn2EndRout_[ii]*in);
    if(airhrn){
      PHorn2EndGeantMat.push_back(15);
    }
    else{
    PHorn2EndGeantMat.push_back(PHorn2EndGeantMat_[ii]);
    }
    PHorn2EndVolName.push_back(PHorn2EndVolName_[ii]);
  }

  NPHorn1EndN=3;
  G4double PHorn1EndZ0_[]     ={126.092        ,127.842     ,129.718};
  if (jCompare) {
	PHorn1EndZ0_[0] = 118.11;
	PHorn1EndZ0_[1] = 119.86;
	PHorn1EndZ0_[2] = 122.048;	
  }
  G4double PHorn1EndLength_[] ={1.75           ,2.188       ,.624};
  G4double PHorn1EndRin_[]    ={6.00           ,5.815       ,4.25};
  G4double PHorn1EndRout_[]   ={7.687          ,7.75        ,5.815};
  G4int PHorn1EndGeantMat_[]  ={31             ,  9         ,  9   };
  G4String PHorn1EndVolName_[]={"PHorn1InsRing","PHorn1CPB1","PHorn1CPB2"};

  for (G4int ii=0;ii<NPHorn1EndN;ii++){
    PHorn1EndZ0.push_back(PHorn1EndZ0_[ii]*in);
    PHorn1EndLength.push_back(PHorn1EndLength_[ii]*in);
    PHorn1EndRin.push_back(PHorn1EndRin_[ii]*in);
    PHorn1EndRout.push_back(PHorn1EndRout_[ii]*in);
    if(airhrn){ 
      PHorn1EndGeantMat.push_back(15);
    }
    else{
      PHorn1EndGeantMat.push_back(PHorn1EndGeantMat_[ii]);
    }
    PHorn1EndVolName.push_back(PHorn1EndVolName_[ii]);
  }

  //Spider supports for Horn1
  NHorn1SpiderSupportPlanesN=3;
  NHorn1SpidersPerPlaneN=3;
  Horn1SpiderSupportZ0.push_back(19.261*in);
  Horn1SpiderSupportZ0.push_back(42.862*in);
  Horn1SpiderSupportZ0.push_back(82.444*in);

  NumiHornSpiderSupport dummy=NumiHornSpiderSupport();
  dummy.stripW=.5*in;
  dummy.stripH=3.685*in;
  dummy.stripL=.076*in;
  dummy.topW=1.437*in;
  dummy.topH=.108*in;
  dummy.topL=.4*in;
  dummy.bottomW=1.437*in;
  dummy.bottomH=.542*in;
  dummy.bottomL=.4*in;
  dummy.bottomThickMid=.3*in;  //Thickness of bottom part in the middle
  dummy.bottomR=1.188*in;
  dummy.ceramicRodR=.491*in;
  
  Horn1SS.push_back(dummy);
  
  dummy.stripW=.5*in;
  dummy.stripH=3.123*in;
  dummy.stripL=.063*in;
  dummy.topW=1.437*in;
  dummy.topH=.108*in;
  dummy.topL=.4*in;
  dummy.bottomW=1.437*in;
  dummy.bottomH=.454*in;
  dummy.bottomL=.4*in;
  dummy.bottomThickMid=.3*in;  //Thickness of bottom part in the middle
  dummy.bottomR=1.75*in;
  dummy.ceramicRodR=.491*in;
  
  Horn1SS.push_back(dummy);
 
  dummy.stripW=.5*in;
  dummy.stripH=1.56*in;
  dummy.stripL=.031*in;
  dummy.topW=1.437*in;
  dummy.topH=.108*in;
  dummy.topL=.4*in;
  dummy.bottomW=1.437*in;
  dummy.bottomH=.379*in;
  dummy.bottomL=.4*in;
  dummy.bottomThickMid=.3*in;  //Thickness of bottom part in the middle
  dummy.bottomR=3.313*in;
  dummy.ceramicRodR=.491*in;
  
  Horn1SS.push_back(dummy);

  //Spider supports for Horn2
  NHorn2SpiderSupportPlanesN=1;
  NHorn2SpidersPerPlaneN=3;
  Horn2SpiderSupportZ0.push_back(59.809*in);

  dummy.stripW=.5*in;
  dummy.stripH=7.033*in;
  dummy.stripL=.125*in;
  dummy.topW=1.437*in;
  dummy.topH=.108*in;
  dummy.topL=.4*in;
  dummy.bottomW=1.437*in;
  dummy.bottomH=.345*in;
  dummy.bottomL=.4*in;
  dummy.bottomThickMid=.3*in;  //Thickness of bottom part in the middle
  dummy.bottomR=5.812*in;
  dummy.ceramicRodR=.491*in;
 
  Horn2SS.push_back(dummy);
  //---------------------------------------------------------
  
  //Near & Far Detector location
  nNear=11;//was 9 without the different energy for the ND positions.
  nFar=2;
  G4double xdetNear[]    = {0     , 0.     , 7.     , 11.    , 14.    , 14.    , 14.   , 0.  , 25.84  , 4.8/2.       , -4.8/2.       };
  G4double ydetNear[]    = {0     , -3.    , -5.    , -5.    , -6.    , -3.    , 0.    , 71. , 78.42  , 3.8/2.       , -3.8/2.       };
  G4double zdetNear[]    = {1040  , 1010.  , 975.   , 958.   , 940.   , 840.   , 740.  , 940., 745.25 , 1040+16.6/2. , 1040-16.6/2.  };
  G4String detNameNear[] = {"Near","Nova1a","Nova1b","Nova1c","Nova2a","Nova2b","Nova3","MSB","MiniBooNE","Near +x +y +z","Near -x -y -z"};
  G4double xdetFar[]     = {0     , 28.81258   };
  G4double ydetFar[]     = {0     , 81.39258   };
  G4double zdetFar[]     = {735000, 811400     };
  G4String detNameFar[]  = {"Far" , "Ash River"};

  for(G4int ii=0;ii<nNear;ii++){
    xdet_near.push_back(xdetNear[ii]*m);
    ydet_near.push_back(ydetNear[ii]*m);
    zdet_near.push_back(zdetNear[ii]*m);
  }
  for(G4int ii=0;ii<nFar;ii++){
    xdet_far.push_back(xdetFar[ii]*m);
    ydet_far.push_back(ydetFar[ii]*m);
    zdet_far.push_back(zdetFar[ii]*m);
  }
}

NumiDataInput::~NumiDataInput()
{
}

void NumiDataInput::ApplyStepLimits(G4LogicalVolume *vol) {
  if (StepLimit == 0.0) return;
  vol->SetUserLimits(new G4UserLimits(StepLimit));
}
