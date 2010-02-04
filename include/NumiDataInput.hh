//----------------------------------------------------------------------
//
// $Id: NumiDataInput.hh,v 1.22.2.6 2010/02/04 22:00:03 martens Exp $
//----------------------------------------------------------------------

#ifndef NumiDataInput_h
#define NumiDataInput_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "NumiHornSpiderSupport.hh"
#include "G4LogicalVolume.hh"

typedef std::vector<G4double> vdouble_t;
typedef std::vector<G4int> vint_t;
typedef std::vector<G4String> vstring_t;
typedef std::vector<G4bool> vbool_t;
typedef std::vector<NumiHornSpiderSupport> vNumiHornSpiderSupport_t;

class NumiDataInput
{
public:
  NumiDataInput();
  ~NumiDataInput();

  static NumiDataInput* GetNumiDataInput();
private:
  static NumiDataInput* fNumiDataInput;

public:
  void SetTargetZ0(G4double val) {
    TargetZ0 = val;
    //Modified by Vamis Xhagjika to meet Nova criteria
    HPBaffleZ0 = (val - 697.6364*mm - HPBaffleLength); //OLD (val + 0.45*m);
  }
   void SetHornCurrent(G4double val) {
      HornCurrent = val;
   }
  void SetDebugOn(G4bool val) {
     debugOn = val;
  }
  G4bool IsDebugOn(){
     return debugOn;
  }
   void SetNImpWeight(G4bool val) {
    NImpWeightOn = val;
   }
   void SetTestTheta(G4float t){
      testTheta = t*M_PI/180.;
   }
   void SetTestBeam(G4bool val) {
      useTestBeam = val;
   }
   void SetFlukaInput(G4bool val) {
      useFlukaInput = val;
   }
   void SetMarsInput(G4bool val) {
      useMarsInput = val;
   }
   void SetMuonBeam(G4bool val) {
      useMuonBeam = val;
   }
   void SetSolidMuMons(G4bool val) {
      solidMuMons = val;
   }
   void SetMuonInput(G4bool val) {
      useMuonInput = val;
   }
   void SetRunNumber(G4String runNum){
      RunNumber=runNum;
   }
   G4String GetRunNumber(){
      return RunNumber;
   }
   void SetGeometryTag(G4String geoName){
      geometry = geoName;
   }
   G4String GetGeometryTag(){
      return geometry;
   }
   G4double GetMaterialSigma(){
      return materialSigma;
   }
   void SetExtNtupleFileName(G4String fileName){
      extNtupleFileName=fileName;
   }
   G4String GetExtNtupleFileName(){
      return extNtupleFileName;
   }
   void SetNuNtupleName(G4String fileName){
      nuNtupleName=fileName;
   }
   void SetHadmmNtupleName(G4String fileName){
    hadmmNtupleName=fileName;
   }
   G4String GetHadmmNtupleName(){
      return hadmmNtupleName;
   }
   void SetHadmmNtupleDir(G4String fileDir){
      hadmmNtupleDir=fileDir;
   }
   G4String GetHadmmNtupleDir(){
      return hadmmNtupleDir;
   }
   void SetNEvents(G4int events){
      fNEvents=events;
   }
   G4int GetNEvents(){
      return fNEvents;
   }
   
   void SetASCIIName(G4String fileName){
      asciiName=fileName;
   }
   void SetBXDRAWName(G4String fileName){
      bxdrawName=fileName;
   }
   void OutputNuNtuple(G4bool output){
      createNuNtuple=output;
   }
   void OutputHadmmNtuple(G4bool output){
      createHadmmNtuple=output;
   }
   void OutputASCII(G4bool output){
      createASCII=output;
   }
   void OutputBXDRAW(G4bool output) {
      createBXDRAW = output;
   }
   void SetDecayPipeSelect(G4bool val) {
      useDecayPipeSelect = val;
   }
   void SetStepLimit(G4double l){
      StepLimit = l;
   }
   void ApplyStepLimits(G4LogicalVolume *);
   
   G4bool GetKillTracking(){
      return KillTracking;
   }
   G4double GetKillTrackingThreshold(){
    return KillTrackingThreshold;
   }
   void  SetMacroBeam(G4bool val){
    useMacro=val;
   }
   G4bool GetMacroBeam(){
      return useMacro;
   }
   void SetZpNtupleName(G4String fileName){
      zpNtupleName=fileName;
   }
   void OutputZpNtuple(G4bool val){
    createZpNtuple=val;
   }
   G4bool GetMuonBeam(){
      return useMuonBeam;
   }
   G4bool GetSolidMuMons(){
      return solidMuMons;
   }
   void SetKillTracking(G4bool val){
      KillTracking = val;
   }
   void SetKillTrackingThreshold(G4double th ){
      KillTrackingThreshold=th;
   }
   


 private:
  G4bool debugOn;
  G4String extNtupleFileName;

 public:
   G4bool pSurfChk;
   G4bool NImpWeightOn, createNuNtuple,createHadmmNtuple, createASCII;
   G4bool useFlukaInput, useMarsInput, useMuonBeam, useMuonInput, solidMuMons;
   G4int fNEvents;
   G4bool KillTracking;
   G4String nuNtupleName, hadmmNtupleName, hadmmNtupleDir, asciiName, RunNumber, geometry;
   
   G4double StepLimit;
   
   G4bool createBXDRAW;
   G4bool useTestBeam, useDecayPipeSelect;
   G4String bxdrawName;
   G4float testTheta;   // TestBeam Angle
   
  //=================================
  //  for Raytracing=================
  //----------------------------------
   G4bool useMacro;
   G4int NZpoint ;
   vdouble_t Zpoint ; // The Z points
   G4String zpNtupleName;
   G4bool createZpNtuple;// name of the zp tuple and the bool of zpntuple
   G4bool raytracing;//
   //==================================
   
   //==================================
   //-- For making the horns the same
   //----------------------------------
   G4bool airhrn;
   G4bool vacuumworld;
   G4bool jCompare;
   G4bool g3Chase;
   G4int hrnmat;
   G4int hrnmatcr;
   G4int hallmat;
   //===================================

   G4double protonMomentum, beamSigmaX, beamSigmaY, protonKineticEnergy, materialSigma;
   G4double KillTrackingThreshold;
   G4ThreeVector beamPosition, beamDirection;
   
   // World Volume
   G4double RockRadius, RockHalfLen, RockDensity, RockRadLen;
   
   // Target Area
   G4double TargetAreaZ0, TargetAreaLength, TargetAreaHeight,TargetAreaWidth;
   G4int TargetAreaGEANTmat;
   
   // added by Zachary Barnett - Target Area ConcretePit
   G4int NTHConcreteSectionsN;
   vdouble_t THConcreteX0, THConcreteY0, THConcreteZ0;
   vdouble_t THConcreteLength, THConcreteHdx, THConcreteHdy;
   vdouble_t THConcreteDxdz, THConcreteDydz;
   vint_t THConcreteGeantMaterial;
   vstring_t THConcreteName;
   //-------------------------------------
   
   // Target
   G4bool constructTarget;

   G4double TargetSegLength;
   G4double TargetSegWidth;
   G4double TargetSegHeight;
   G4double TargetSegPitch;
   G4double TargetGraphiteHeight;	
   G4bool   TargetEndRounded;
   G4double TargetSegmentNo;

   G4double BudalVFHSLength;
   G4double BudalVFHSWidth;
   G4double BudalVFHSHeight;
   G4double BudalVFHSPitch;
   G4bool   BudalVFHSEndRounded;

   G4double BudalHFVSLength;
   G4double BudalHFVSWidth;
   G4double BudalHFVSHeight;
   G4double BudalHFVSPitch;
   G4bool   BudalHFVSEndRounded;

   G4double TargetA;
   G4double TargetZ;
   G4double TargetDensity;

   G4double TotalTargetLength;
   G4double TargetX0;
   G4double TargetY0;
   G4double TargetZ0;
   G4double TargetDxdz;
   G4double TargetDydz;

   G4double TargetEndtoDnFlange;
   G4double TargetCanisterCenterOffset;

   G4double TargetDnFlangeLength;
   G4double TargetDnFlangeOutRad;
   G4double TargetDnFlangeX0;
   G4double TargetDnFlangeY0;
   G4double TargetDnFlangeZ0;

   G4double TargetDnFlangeCutoutLength;
   G4double TargetDnBeWindowRadius;
   G4double TargetDnBeWindowLength;

   G4double TargetOutsideCasingOutRad;
   G4double TargetOutsideCasingInRad;
   G4double TargetCasingWaterOutRad;
   G4double TargetCasingWaterInRad;
   G4double TargetInsideCasingOutRad;
   G4double TargetInsideCasingInRad;

   G4double TargetCasingLength;
   G4double TargetCasingX0;
   G4double TargetCasingY0;
   G4double TargetCasingZ0;

   G4double TargetUpFlangeLength;
   G4double TargetUpFlangeOutRad;
   G4double TargetUpFlangeX0;
   G4double TargetUpFlangeY0;
   G4double TargetUpFlangeZ0;

   G4double TargetUpBeFlangeLength;
   G4double TargetUpBeFlangeOutRad;
   G4double TargetUpBeFlangeX0;
   G4double TargetUpBeFlangeY0;
   G4double TargetUpBeFlangeZ0;

   G4double TargetUpBeFlangeCutoutLength;
   G4double TargetUpBeFlangeCutoutRadius;

   G4double TargetUpBeWindowRadius;
   G4double TargetUpBeWindowLength;

   G4double PressingPlateLength;
   G4double PressingPlateHeight;
   G4double PressingPlateWidth;
   G4double PressingPlateX0;
   G4double PressingPlateY0;
   G4double PressingPlateZ0;

   G4double PressingPlateCutoutWidth;
   G4double PressingPlateCutoutHeight;

   G4double CoolingPlateLength;
   G4double CoolingPlateHeight;
   G4double CoolingPlateWidth;
   G4double CoolingPlateX0;
   G4double CoolingPlateY0;
   G4double CoolingPlateZ0;

   G4double CoolingWaterPipeOutRad;
   G4double CoolingWaterPipeX0;
   G4double CoolingWaterPipeY0;

   G4double CoolingPlateCutoutWidth;
   G4double CoolingPlateCutoutHeight;


   
  //HPBaffle 
  G4double HPBaffleLength;
  G4double HPBaffleRin;
  G4double HPBaffleRout;
  G4double HPBaffleX0;
  G4double HPBaffleY0;
  G4double HPBaffleZ0;
  G4double HPBaffleDXDZ;
  G4double HPBaffleDYDZ;


  G4int HPBaffle;


  // Tunnel
  G4double TunnelZ0, TunnelRadius, TunnelLength, TunnelA, TunnelZ;
  G4int TunnelGEANTmat;
  G4double BeamAngle;//nominally .058 radians

  // Decay Pipe
  G4double DecayPipeZ0,DecayPipeRadius,DecayPipeLength,DecayPipeFWinThick,DecayPipeEWinThick,DecayPipeWallThick;
  G4double DecayPipeA,DecayPipeZ;
  G4int DecayPipeGEANTmat,DecayPipeFWinmat, DecayPipeEWinmat;
  
  // Decay Pipe Shield
  G4double ShieldX0,ShieldY0,ShieldZ0,ShieldDxdz, ShieldDydz,ShieldLength,ShieldRout,ShieldRin;
  G4int ShieldGEANTmat;
  
  //Target Hall Blocks
  G4double THBlockNblock;
  vdouble_t THBlockX0,THBlockY0,THBlockZ0, THBlockDxdz,THBlockDydz;
  vdouble_t THBlockLength,THBlockHdx,THBlockHdy;
  vint_t THBlockGeantMaterial;
  vstring_t THBlockName;

  //Hadron Absorber Box
  G4double HadrBox_width, HadrBox_height, HadrBox_length;

  // Horn 1 & 2
  G4int PhornNphorn;

  vdouble_t PhornZ1, PhornZ2;  
  vint_t  PhornNpoint;  
  vdouble_t PhornAin, PhornBin, PhornCin, PhornAout, PhornBout, PhornCout;
  vdouble_t  PhornROCin, PhornROCout;
  vdouble_t PhornThickFront, PhornThickEnd;  
  vdouble_t PhornX0, PhornY0, PhornZ0, PhornDXDZ, PhornDYDZ, PhornCurrent;
  vint_t PhornGEANTmat; 
  vstring_t PhornName;
  
  // Flux Area
  G4int nNear,nFar;
  vdouble_t xdet_near,ydet_near,zdet_near;
  vdouble_t xdet_far,ydet_far,zdet_far;

  G4double HornCurrent; //old
	
 // Horn 1
  G4int NPHorn1OCN,NPHorn1ICN,NPHorn1EndN;
  
  vdouble_t PHorn1OCRout,PHorn1OCRin,PHorn1OCZ0;
  vdouble_t PHorn1ICZ0;
  vint_t PHorn1ICNpoint;

  vdouble_t PHorn1EndZ0,PHorn1EndLength,PHorn1EndRout,PHorn1EndRin;
  vint_t PHorn1EndGeantMat;
  vstring_t PHorn1EndVolName;

  G4int NHorn1SpiderSupportPlanesN,NHorn1SpidersPerPlaneN;
  vdouble_t Horn1SpiderSupportZ0;
  vNumiHornSpiderSupport_t Horn1SS;
  
  // Horn 2
  G4int NPHorn2OCN,NPHorn2ICN,NPHorn2EndN;
  
  vdouble_t PHorn2OCRout,PHorn2OCRin,PHorn2OCZ0;
  vdouble_t PHorn2ICZ0;
  vint_t PHorn2ICNpoint;

  vdouble_t PHorn2EndZ0,PHorn2EndLength,PHorn2EndRout,PHorn2EndRin;
  vint_t PHorn2EndGeantMat;
  vstring_t PHorn2EndVolName;

  G4int NHorn2SpiderSupportPlanesN,NHorn2SpidersPerPlaneN;
  vdouble_t Horn2SpiderSupportZ0;
  vNumiHornSpiderSupport_t Horn2SS;
};
#endif
