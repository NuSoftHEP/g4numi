//----------------------------------------------------------------------
//
// $Id: NumiDataInput.hh,v 1.23.2.15.2.1 2016/09/02 21:09:29 rschroet Exp $
//----------------------------------------------------------------------

#ifndef NumiDataInput_h
#define NumiDataInput_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "NumiHornSpiderSupport.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

class G4Material;

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

   void Print();
   
   
   static NumiDataInput* GetNumiDataInput();
private:
   static NumiDataInput* fNumiDataInput;

   G4bool ConfigureRunPeriod(G4String &beamconfig);
   G4bool SetTargetConfig(G4String config);
   G4bool SetHornCurrentConfig(G4String config);
   G4bool SetHornOnePos(G4String config);
   G4bool SetHornTwoPos(G4String config);
   G4bool SetTargetDensity(G4String config);
   G4bool SetHornConfig(G4String config);

   void SetTargetZ0(G4double val)       { TargetZ0 = val;}
   void SetBaffleZ0(G4double val)       { HPBaffleZ0 = val;}
   void SetBeamZ0(G4double val)         { beamPosition[2] = val;}
   void SetHornCurrent(G4double val)    { HornCurrent = val;}

   void SetHorn1X0(G4double val)	  {Horn1X0=val;}
   void SetHorn2X0(G4double val)	  {Horn2X0=val;}
   void SetTargetDensityVal(G4double val) {TargetDensity=val;}
   
public:
  
   G4bool SetBeamConfig(G4String config);

   G4bool SetPlaylist(G4String pl);
   
   void SetDetailedProtonBeam(G4bool val);

   void SetLengthOfWaterInTgt(G4double val);
   
   void ApplyStepLimits(G4LogicalVolume *);
   void ApplyTimeLimits(G4LogicalVolume *);

   void SetNuBeam(G4bool val)
      {
         fUseNuBeam = val;
         fSimulation = "Neutrino Beam Simulation";
      }
   void SetWaterInTgt(G4bool val)
      {
         fUseWaterInTgt = val;
         fSubSimulation = "Water in the Target";
      }
   void SetDebugLevel(G4int val)                {fDebugLevel = val;}
   void SetRunPeriod(G4int val)                 {fRunPeriod = val;}
   void SetOkToRun(G4bool val)                  {fOkToRun = val;}
   void SetUseCorrHornCurrent(G4bool val)       {fUseCorrHornCurrent = val;}   
   void SetDebugOn(G4bool val)                  { debugOn = val; }
   void SetNImpWeight(G4bool val)               { NImpWeightOn = val;}
   void SetTestTheta(G4float t)                 { testTheta = t*M_PI/180.;}
   void SetTestBeam(G4bool val)                 { useTestBeam = val; }
   void SetFlukaInput(G4bool val)               { useFlukaInput = val; }
   void SetMarsInput(G4bool val)                { useMarsInput = val; }
   void SetRunNumber(G4String runNum)           { RunNumber=runNum; }
   void SetGeometryTag(G4String geoName)        { geometry = geoName; }
   void SetExtNtupleFileName(G4String fileName) { extNtupleFileName=fileName; }
   void SetNuNtupleName(G4String fileName)      { nuNtupleName=fileName;}
   void SetASCIIName(G4String fileName)         { asciiName=fileName; }
   void SetBXDRAWName(G4String fileName)        { bxdrawName=fileName; }
   void OutputNuNtuple(G4bool output)           { createNuNtuple=output;}
   void OutputTarNtuple(G4bool output)           { createTarNtuple=output;}
   void OutputASCII(G4bool output)              { createASCII=output; }
   void OutputBXDRAW(G4bool output)             { createBXDRAW = output; }
   void SetDecayPipeSelect(G4bool val)          { useDecayPipeSelect = val;}
   void SetStepLimit(G4double l)                { StepLimit = l; }
   void SetMacroBeam(G4bool val)                { useMacro=val;}
   void SetZpNtupleName(G4String fileName)      { zpNtupleName=fileName; }
   void SetTarNtupleName(G4String fileName)      { tarNtupleName=fileName; }
   void OutputZpNtuple(G4bool val)              { createZpNtuple=val;}
   void SetKillTracking(G4bool val)             { KillTracking = val;}
   void SetKillTrackingThreshold(G4double th )  { KillTrackingThreshold=th;}
   void SetjCompare(G4bool _jc);
   void Setg3Chase(G4bool _gc);
   void SetHornMisalign(G4bool val)		{fUseHornMisalign=val;}
   void SetTgtDensity(G4bool val)		{fUseTgtDensity=val;}


   G4int    GetRunPeriod()                    { return fRunPeriod;}
   G4int    GetDebugLevel()                   { return fDebugLevel;}

   G4bool   IsDebugOn()                       { return debugOn; }
   G4bool   GetOkToRun()                      { return fOkToRun;}
   G4bool   GetPrintGeometry()                { return fPrintGeometry;}
   G4bool   GetKillTracking()                 { return KillTracking;}
   G4bool   GetMacroBeam()                    { return useMacro;}
   G4bool   GetNuBeam()                       { return fUseNuBeam;}
   G4bool   GetWaterInTgt()                   { return fUseWaterInTgt;}
   G4bool   GetDetailedProtonBeam()           { return fUseDetailedProtonBeam;}
   
   G4String GetBeamConfig()                   { return fBeamConfig;}
   G4String GetPlaylist()                     { return fPlaylist;}
   G4String GetSimulation()                   { return fSimulation;}
   G4String GetRunNumber()                    { return RunNumber; }
   G4String GetGeometryTag()                  { return geometry; }
   G4String GetExtNtupleFileName()            { return extNtupleFileName;}

   G4double GetKillTrackingThreshold()        { return KillTrackingThreshold;}
   G4double GetProtonMomentum()               { return protonMomentum;}
   G4double GetProton_outR()                  { return fProton_outR;}
   G4double GetProton_inR()                   { return fProton_inR;}  
   G4double GetProtonDivergence()             { return fProtonDiv;}
   G4double GetProtonSpread()                 { return fProtonSpread;} 
   G4double GetProton_cosx()                  { return fProton_cosx;} 
   G4double GetProton_cosy()                  { return fProton_cosy;}
   G4double GetLengthOfWaterInTgt()           { return fLengthOfWaterInTgt;} 

   G4double GetHornWaterLayerThick() const    { return fHornWaterLayerThick; }
   G4bool GetHorn1IsAlternate()  const        { return fHorn1IsAlternate; }
   G4bool GetHorn1IsRefined()  const        { return fHorn1IsRefined; }
   G4double GetHorn1ExtraLayerAlum() const    { return fHorn1ExtraLayerAlum; }
   G4bool GetDumpBFieldPlease() const    { return fDumpBFieldPlease; }
   
   //--------------------------------------------------------------
   //Specifically for Muon Monitor simulation and Absorber background simulation
   //
   //
   void SetMuonBeam(G4bool val)                {useMuonBeam = val; fSimulation = "Muon Monitor Simulation";}
   void SetNInputParts(G4int val)              {NInputParts = val;}
   void SetNInputPart(G4int val)               {NInputPart = val;}
   void SetReWeightDeltas(G4bool val)          {reWeightDeltas = val;}
   void SetNSplitDeltas(G4int val)             {nSplitDeltas = val;}
   void SetSolidMuMons(G4bool val)             {solidMuMons = val;}
   void SetAbsorberConfig(G4String val)        {absorberConfig = val;}
   void SetSimAbsBkg(G4bool val)               {simAbsBkg = val; fSimulation = "Muon Monitor Absorber Background Simulation";}
   void SetSimDRays(G4bool val)                {simDRays = val;}
   void SetUseZPosCut(G4bool val)              {useZPosCut = val;}
   void SetMuonBeamShape(G4String val)         {muonBeamShape = val;}
   void SetMuonBeamMomentum(G4double val)      {muonBeamMomentum = val;}
   void SetMuonBeamZPos(G4double val)          {muonBeamZPos = val;}
   void SetGaussBeamXSig(G4double val)         {muonBeamGaussXsig = val;}
   void SetGaussBeamYSig(G4double val)         {muonBeamGaussYsig = val;}
   void SetMuonInput(G4bool val)               {useMuonInput = val;}
   void SetNEvents(G4int events)               {fNEvents=events;}
   void SetDefaultMaterial(G4Material* mat)    {DefaultMaterial = mat;}
   void OutputHadmmNtuple(G4bool output)       {createHadmmNtuple=output;}
   void SetHadmmNtupleName(G4String fileName)  {hadmmNtupleName=fileName;}
   void SetHadmmNtupleDir(G4String fileDir)    {hadmmNtupleDir=fileDir;}
   void OutputAbsBkgNtuple(G4bool output)      {createAbsBkgNtuple=output;}
   void SetAbsBkgNtupleName(G4String fileName) {absbkgNtupleName=fileName;}
   void SetAbsBkgNtupleDir(G4String fileDir)   {absbkgNtupleDir=fileDir;}
   void SetHornWaterLayerThick(G4double d)     {fHornWaterLayerThick = d;}
   void SetHorn1IsAlternate(G4bool t)          {fHorn1IsAlternate = t;}
   void SetHorn1IsRefined(G4bool t)          {fHorn1IsRefined = t;}
   void SetHorn1ExtraLayerAlum(G4double d)     {fHorn1ExtraLayerAlum = d;}
   void SetDumpBFieldPlease(G4bool t)          {fDumpBFieldPlease = t; }
 
   G4bool   GetMuonBeam()            {return useMuonBeam;}
   G4int    GetNInputParts()         {return NInputParts;}
   G4int    GetNInputPart()          {return NInputPart;}
   G4bool   GetReWeightDeltas()      {return reWeightDeltas;}
   G4int    GetNSplitDeltas()        {return nSplitDeltas;}
   G4bool   GetSolidMuMons()         {return solidMuMons;}
   G4String GetAbsorberConfig()      {return absorberConfig;}
   G4bool   GetSimAbsBkg()           {return simAbsBkg;}
   G4bool   GetSimDRays()            {return simDRays;}
   G4bool   GetUseZPosCut()          {return useZPosCut;}
   G4String GetMuonBeamShape()       {return muonBeamShape;}
   G4double GetMuonBeamMomentum()    {return muonBeamMomentum;}
   G4double GetMuonBeamZPos()        {return muonBeamZPos;}
   G4double GetGaussBeamXSig()       {return muonBeamGaussXsig;}
   G4double GetGaussBeamYSig()       {return muonBeamGaussYsig;}
   G4int    GetNEvents()             {return fNEvents;}
   G4Material* GetDefaultMaterial()  {return DefaultMaterial;}
   G4double GetMaterialSigma()       {return materialSigma;}
   G4String GetHadmmNtupleName()     {return hadmmNtupleName;}
   G4String GetHadmmNtupleDir()      {return hadmmNtupleDir;}
   G4bool   GetCreateAbsBkgNtuple()  {return createAbsBkgNtuple;}
   G4String GetAbsBkgNtupleName()    {return absbkgNtupleName;}
   G4String GetAbsBkgNtupleDir()     {return absbkgNtupleDir;}

   void SetAbsorberMaterial(G4Material* mat, G4int mon);
   void SetAbsorberThickness(G4double val, G4int mon);
   void SetAbsorberMonDist(G4double val, G4int mon);

   G4Material* GetAbsorberMaterial(G4int);
   G4double GetAbsorberThickness(G4int);
   G4double GetAbsorberMonDist(G4int);

   //
   //
   //-------------------------------------------------------

 private:
   
   G4bool debugOn;
   G4bool fPrintGeometry;
   G4bool fOkToRun;
   G4bool fUseNuBeam;
   G4bool fUseCorrHornCurrent;
   G4bool fUseDetailedProtonBeam;
   G4bool fUseWaterInTgt;
   G4bool fUseHornMisalign;
   G4bool fUseTgtDensity;

   G4String extNtupleFileName;
   G4String fBeamConfig;
   G4String fPlaylist;
   G4String fTargetConfig;
   G4String fIHornConfig;
   G4String fHornConfig;
   G4String fSimulation;
   G4String fSubSimulation;

   G4int fRunPeriod;
   G4int fDebugLevel;

   G4double fProton_outR;
   G4double fProton_inR;  
   G4double fProtonDiv;
   G4double fProtonSpread; 
   G4double fProton_cosx; 
   G4double fProton_cosy;

   G4double fLengthOfWaterInTgt;

 public:

   G4int fPrintInfo;

   //////////////////////////////
   
   G4bool NImpWeightOn, createNuNtuple, createTarNtuple, createHadmmNtuple, createASCII;
   G4bool useFlukaInput, useMarsInput;

   G4bool useMuonBeam, useMuonInput, solidMuMons, simAbsBkg, reWeightDeltas;
   G4String absorberConfig;
   G4bool fHorn1IsAlternate; // An different way of coding up Horn1 (P.L., Sept 2014) 
   G4bool fHorn1IsRefined; // An improved version of Horn1, using Numi existing code (P.L., November 2014) 
   G4int    NInputPart, NInputParts, nSplitDeltas;
   G4bool simDRays, useZPosCut, createAbsBkgNtuple;
   G4double muonBeamMomentum, muonBeamGaussXsig, muonBeamGaussYsig, muonBeamZPos;
   G4String muonBeamShape;
   G4String hadmmNtupleName, hadmmNtupleDir, absbkgNtupleName, absbkgNtupleDir;
   G4int fNEvents;
   G4bool KillTracking;
   G4String nuNtupleName, tarNtupleName, asciiName, RunNumber, geometry;
   G4Material* DefaultMaterial;
   G4Material* Mon1AbsorberMaterial;
   G4Material* Mon2AbsorberMaterial;
   G4Material* Mon3AbsorberMaterial;
   G4double Mon1AbsorberThickness, Mon2AbsorberThickness, Mon3AbsorberThickness;
   G4double Mon1AbsorberDist,  Mon2AbsorberDist,  Mon3AbsorberDist;
   
   G4double StepLimit;   
   G4double TimeLimit;

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
   G4double TargetX0, TargetY0, TargetZ0, TargetDxdz, TargetDydz;
   G4double TargetSLength, TargetSWidth, TargetSHeight, TargetA, TargetDensity;
   G4double  TargetZ, TargetRL;
   G4int TargetGEANTmat,TargetSegmentNo;
   G4double TargetSegmentPitch,TargetCPGRadius,TargetCPGPosition;
   G4bool TargetEndRounded;
   //
   // Added virtual (assembly) volume such that one can simulate the ME configuration, while 
   // having a virtual LE/MIPP target, for MIPP correction.. 
   G4double TargetVirtualCanisterHeight;
   G4double TargetVirtualCanisterWidth;
   
   //Rings holding target and cooling pipes
   G4int NTgtRingN;
   vdouble_t TgtRingZ0, TgtRingLength, TgtRingRin,TgtRingRout;
   vint_t TgtRingGeantMaterial;
   vstring_t TgtRingVolName;
   
   //Budal
   G4double BudalX0,BudalY0,BudalZ0,BudalDxdz,BudalDydz;
   
  //HPBaffle 
  G4int HPBaffle, HPBaffleGEANTMat;
  G4double HPBaffleX0,HPBaffleY0,HPBaffleZ0,HPBaffleDXDZ,HPBaffleDYDZ;
  G4double HPBaffleLength,HPBaffleRin,HPBaffleRout;

  //Cooling pipes
  G4int NCPipeN;
  vbool_t CPipeFilledWater;
  vdouble_t CPipeX0,CPipeY0,CPipeZ0,CPipeLength,CPipeRadiusIn,CPipeRadiusOut,CPipeWallThick;
  vdouble_t CPipeDXDZ,CPipeDYDZ;
  vint_t CPGeantMat;
  vdouble_t CPipeCurvRad,CPipeOpenAng,CPipeCloseAng;
  vstring_t CPipeVolName;
  
  //Container
  G4int NContainerN;
  vdouble_t CTubeZ0,CTubeLength,CTubeRin,CTubeRout;
  vint_t CTubeGeantMat;
  vstring_t CTubeVolName;

  // Tunnel
  G4double TunnelZ0, TunnelRadius, TunnelLength, TunnelA, TunnelZ;
  G4int TunnelGEANTmat;
  G4double BeamAngle;//nominally .058 radians

  // Decay Pipe
  G4double DecayPipeZ0,DecayPipeRadius,DecayPipeLength,DecayPipeFWinThick,DecayPipeEWinThick,DecayPipeWallThick;
  G4double DecayPipeA,DecayPipeZ;
  G4int DecayPipeGEANTmat,DecayPipeFWinmat, DecayPipeEWinmat;
  G4bool HeInDecayPipe;
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
  
   //////////////////////////////
   // July 2014: Adding the layer of water, Paul Lebrun. Declared public.. Why placing access functon them
   // But other variables are declared this way. 
   //
   
   G4double fHornWaterLayerThick;
   G4double fHorn1ExtraLayerAlum;
   
   //
   // A logical flag to indicate the request to dump the magnetic field map 
   //
   G4bool fDumpBFieldPlease; 

   //-----------------
   //do these variables get used?!
   //
   G4int PhornNphorn;
   
   vdouble_t PhornZ1, PhornZ2;  
   vint_t  PhornNpoint;  
   vdouble_t PhornAin, PhornBin, PhornCin, PhornAout, PhornBout, PhornCout;
   vdouble_t  PhornROCin, PhornROCout;
   vdouble_t PhornThickFront, PhornThickEnd;  
   vdouble_t PhornX0, PhornY0, PhornZ0, PhornDXDZ, PhornDYDZ, PhornCurrent;
   vint_t PhornGEANTmat; 
   vstring_t PhornName;
   //-----------------
  
  // Flux Area
   G4int nNear,nFar;
   vdouble_t xdet_near,ydet_near,zdet_near;
   vdouble_t xdet_far,ydet_far,zdet_far;
   vstring_t det_near_name, det_far_name;

   G4double HornCurrent; //old
	
   // Horn 1

   //----------------
   //these variables are very important (Laura)
   //
   G4double Horn1X0, Horn1Y0, Horn1Z0;
   //----------------
   
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

   //----------------
   //these variables are very important (Laura)
   //
   G4double Horn2X0, Horn2Y0, Horn2Z0;
   //----------------
   
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

        /// NOvA target
   G4bool pSurfChk;
   G4double TargetSegLength;
   G4double TargetSegWidth;
   G4double TargetSegHeight;
   G4double TargetSegPitch;
   G4double TargetGraphiteHeight;	

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

   G4double TotalTargetLength;

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
//
// To be use to fill the Ntuple, info about the particle 6D phase space exiting the target. 
//
   inline G4String GetHornConfig() const { return fHornConfig; } 

    //Variables needed for custumized dk2nu storing (Leo Aliaga, Feb17, 2015)
  //Absorption:
  G4int nGenAbs;  //generations under absorption study 
  G4int nVolAbs;  //volumes under absorption study 
  G4int nVdblTot;  //number of values to be store in vdbl 
  vstring_t GenAbsName;  
  vstring_t VolAbsName;
  vstring_t VolVdblName;
  
  G4int nVintTot;  //number of values to be store in vint
  vstring_t VolVintName;

   
};
#endif
