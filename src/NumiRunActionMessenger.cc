//----------------------------------------------------------------------
// $Id
//----------------------------------------------------------------------

#include "NumiRunActionMessenger.hh"

#include "NumiRunAction.hh"
#include "NumiDataInput.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"
#include "NumiSteppingAction.hh"

NumiRunActionMessenger::NumiRunActionMessenger(NumiRunAction* RA)
  :runAction (RA)
{
  NumiData=NumiDataInput::GetNumiDataInput();

  if(NumiData->fPrintInfo > 0 || NumiData->IsDebugOn())
  {
     G4cout << "NumiRunActionMessenger Constructor Called." << G4endl;
  }

  RndmDir = new G4UIdirectory("/NuMI/rndm/");
  RndmDir->SetGuidance("Rndm status control.");
  
  readRndmCmd = new G4UIcmdWithAString("/NuMI/rndm/read",this);
  readRndmCmd->SetGuidance("get rndm status from an external file.");
  readRndmCmd->SetParameterName("fileName",true);
  readRndmCmd->SetDefaultValue ("");
  readRndmCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  

  showRndmCmd = new G4UIcmdWithoutParameter("/NuMI/rndm/show",this);
  showRndmCmd->SetGuidance("show rndm status.");
  showRndmCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  setRndmSeedCmd = new G4UIcmdWithAnInteger("/NuMI/rndm/setRndmSeed",this);
  setRndmSeedCmd->SetGuidance("set rndm seed.");
  setRndmSeedCmd->SetParameterName("rndmSeed",true);
  setRndmSeedCmd->SetDefaultValue (0);
  setRndmSeedCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  NumiRunDir = new G4UIdirectory("/NuMI/run/");
  NumiRunDir->SetGuidance("NuMI run management");

  //---------------------------------------
  //Commands for defining the simulation
  //

  useNuBeam = new G4UIcmdWithABool("/NuMI/run/useNuBeam",this);
  useNuBeam->SetGuidance("run the neutrino beam simulation");
  useNuBeam->SetParameterName("useNuBeam",true);
  useNuBeam->SetDefaultValue (NumiData->GetNuBeam());
  useNuBeam->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  useMuonBeam = new G4UIcmdWithABool("/NuMI/run/useMuonBeam",this);
  useMuonBeam->SetGuidance("run the muon monitor simulation");
  useMuonBeam->SetParameterName("useMuonBeam",true);
  useMuonBeam->SetDefaultValue (NumiData->GetMuonBeam());
  useMuonBeam->AvailableForStates(G4State_PreInit,G4State_Idle);

  simAbsBkg = new G4UIcmdWithABool("/NuMI/run/simAbsBkg",this);
  simAbsBkg->SetGuidance("run the Absorber Backgrounds simulation");
  simAbsBkg->SetParameterName("simAbsBkg",true);
  simAbsBkg->SetDefaultValue (NumiData->GetSimAbsBkg());
  simAbsBkg->AvailableForStates(G4State_PreInit,G4State_Idle);

  simDRays = new G4UIcmdWithABool("/NuMI/run/simDRays",this);
  simDRays->SetGuidance("run the Delta Ray simulation");
  simDRays->SetParameterName("simDRays",true);
  simDRays->SetDefaultValue (NumiData->GetSimDRays());
  simDRays->AvailableForStates(G4State_PreInit,G4State_Idle);
  //---------------------------------------

  //---------------------------------------
  //Commands for defining the sub-simulation
  //
  useWaterInTgt = new G4UIcmdWithABool("/NuMI/run/useWaterInTgt",this);
  useWaterInTgt->SetGuidance("Simulate Water in the Target");
  useWaterInTgt->SetParameterName("useWaterInTgt",true);
  useWaterInTgt->SetDefaultValue (NumiData->GetWaterInTgt());
  useWaterInTgt->AvailableForStates(G4State_PreInit,G4State_Idle);
  //---------------------------------------


  
  //---------------------------------------
  //Commands for defining the Beam
  //
  useDetailedProtonBeam = new G4UIcmdWithABool("/NuMI/run/useDetailedProtonBeam",this);
  useDetailedProtonBeam -> SetGuidance("Use a detailed proton beam. (For running the full simulation with the beam starting upstream of the baffle");
  useDetailedProtonBeam -> SetParameterName("useDetailedProtonBeam",true);
  useDetailedProtonBeam -> SetDefaultValue (NumiData->GetDetailedProtonBeam());
  useDetailedProtonBeam -> AvailableForStates(G4State_PreInit,G4State_Idle);
  
  useTestBeam = new G4UIcmdWithABool("/NuMI/run/useTestBeam",this);
  useTestBeam->SetGuidance("use test beam source");
  useTestBeam->SetParameterName("useTestBeam",true);
  useTestBeam->SetDefaultValue (NumiData->useTestBeam);
  useTestBeam->AvailableForStates(G4State_PreInit,G4State_Idle);

  useFlukaInput = new G4UIcmdWithABool("/NuMI/run/useFlukaInput",this);
  useFlukaInput->SetGuidance("use fluka input ntuple");
  useFlukaInput->SetParameterName("useFlukaInput",true);
  useFlukaInput->SetDefaultValue (NumiData->useFlukaInput);
  useFlukaInput->AvailableForStates(G4State_PreInit,G4State_Idle);

  useMarsInput = new G4UIcmdWithABool("/NuMI/run/useMarsInput",this);
  useMarsInput->SetGuidance("use mars input ntuple");
  useMarsInput->SetParameterName("useMarsInput",true);
  useMarsInput->SetDefaultValue (NumiData->useMarsInput);
  useMarsInput->AvailableForStates(G4State_PreInit,G4State_Idle);

  useMuonInput = new G4UIcmdWithABool("/NuMI/run/useMuonInput",this);
  useMuonInput->SetGuidance("use muon input ntuple");
  useMuonInput->SetParameterName("useMuonInput",true);
  useMuonInput->SetDefaultValue (NumiData->useMuonInput);
  useMuonInput->AvailableForStates(G4State_PreInit,G4State_Idle);
  //---------------------------------------


  DebugLevel = new G4UIcmdWithAnInteger("/NuMI/run/DebugLevel",this);
  DebugLevel ->SetGuidance("Set the debuging level. 0 = debug off");
  DebugLevel ->SetParameterName("DebugLevel",true);
  DebugLevel ->SetDefaultValue (NumiData->GetDebugLevel());
  DebugLevel ->AvailableForStates(G4State_PreInit,G4State_Idle);

  debugOn = new G4UIcmdWithABool("/NuMI/run/debugOn",this);
  debugOn->SetGuidance("Output some debugging info");
  debugOn->SetParameterName("debugOn",true);
  debugOn->SetDefaultValue (NumiData->IsDebugOn());
  debugOn->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  setRunID = new G4UIcmdWithAnInteger("/NuMI/run/setRunID",this);
  setRunID->SetGuidance("set run ID.");
  setRunID->SetParameterName("run ID number",true);
  setRunID->SetDefaultValue (0);
  setRunID->AvailableForStates(G4State_PreInit,G4State_Idle); 

  setMaterialSigma = new G4UIcmdWithADouble("/material/materialSigma",this);
  setMaterialSigma->SetGuidance("set Sigma");
  setMaterialSigma->SetParameterName("Sigma value of density/material",true);
  setMaterialSigma->SetDefaultValue (0);
  setMaterialSigma->AvailableForStates(G4State_PreInit,G4State_Idle); 

  useNImpWeight = new G4UIcmdWithABool("/NuMI/run/useNImpWeight",this);
  useNImpWeight->SetGuidance("use importance weighting (true/false)");
  useNImpWeight->SetParameterName("NImpWeight",true);
  useNImpWeight->SetDefaultValue (NumiData->NImpWeightOn);
  useNImpWeight->AvailableForStates(G4State_PreInit,G4State_Idle);

  
  NInputParts = new G4UIcmdWithAnInteger("/NuMI/run/NInputParts",this);
  NInputParts->SetGuidance("Number of parts to divide the muon input file into");
  NInputParts->SetParameterName("Number of parts to divide the muon input file into",true);
  NInputParts->SetDefaultValue (NumiData->GetNInputParts());
  NInputParts->AvailableForStates(G4State_PreInit,G4State_Idle);

  NInputPart = new G4UIcmdWithAnInteger("/NuMI/run/NInputPart",this);
  NInputPart->SetGuidance("The part of the muon input file to read in");
  NInputPart->SetParameterName("The part of the muon input file to read in",true);
  NInputPart->SetDefaultValue (NumiData->GetNInputPart());
  NInputPart->AvailableForStates(G4State_PreInit,G4State_Idle);

  reWeightDeltas = new G4UIcmdWithABool("/NuMI/run/reWeightDeltas",this);
  reWeightDeltas->SetGuidance("Split the delta rays reaching the Alcoves to get better edep sampling");
  reWeightDeltas->SetParameterName("reWeightDeltas",true);
  reWeightDeltas->SetDefaultValue (NumiData->GetReWeightDeltas());
  reWeightDeltas->AvailableForStates(G4State_PreInit,G4State_Idle);

  nSplitDeltas = new G4UIcmdWithAnInteger("/NuMI/run/nSplitDeltas",this);
  nSplitDeltas->SetGuidance("Total Number of deltas each delta spilt will represent ");
  nSplitDeltas->SetParameterName("nSplitDeltas",true);
  nSplitDeltas->SetDefaultValue (NumiData->GetNSplitDeltas());
  nSplitDeltas->AvailableForStates(G4State_PreInit,G4State_Idle);



  useZPosCut = new G4UIcmdWithABool("/NuMI/run/useZPosCut",this);
  useZPosCut->SetGuidance("Use/Not Use the Z Position cut for delta rays");
  useZPosCut->SetParameterName("useZPosCut",true);
  useZPosCut->SetDefaultValue (NumiData->GetUseZPosCut());
  useZPosCut->AvailableForStates(G4State_PreInit,G4State_Idle);

  muonBeamShape = new G4UIcmdWithAString("/NuMI/run/muonBeamShape",this);
  muonBeamShape->SetGuidance("Set the muon beam shape");
  muonBeamShape->SetParameterName("muonBeamShape",true);
  muonBeamShape->SetDefaultValue (NumiData->GetMuonBeamShape());
  muonBeamShape->AvailableForStates(G4State_PreInit,G4State_Idle);

  muonBeamMomentum = new G4UIcmdWithADoubleAndUnit("/NuMI/run/muonBeamMomentum",this);
  muonBeamMomentum->SetGuidance("Set the muon beam momentum");
  muonBeamMomentum->SetParameterName("muonBeamMomentum",true);
  muonBeamMomentum->SetDefaultValue (NumiData->GetMuonBeamMomentum());
  muonBeamMomentum->AvailableForStates(G4State_PreInit,G4State_Idle);

  muonBeamZPos = new G4UIcmdWithADoubleAndUnit("/NuMI/run/muonBeamZPos",this);
  muonBeamZPos->SetGuidance("Set the muon beam energy");
  muonBeamZPos->SetParameterName("muonBeamZPos",true);
  muonBeamZPos->SetDefaultValue (NumiData->GetMuonBeamZPos());
  muonBeamZPos->AvailableForStates(G4State_PreInit,G4State_Idle);

  muonBeamGaussXsig = new G4UIcmdWithADoubleAndUnit("/NuMI/run/muonBeamGaussXSig",this);
  muonBeamGaussXsig->SetGuidance("Set the X-sig for a gaussian beam");
  muonBeamGaussXsig->SetParameterName("muonBeamGaussXSig",true);
  muonBeamGaussXsig->SetDefaultValue (NumiData->GetGaussBeamXSig());
  muonBeamGaussXsig->AvailableForStates(G4State_PreInit,G4State_Idle);

  muonBeamGaussYsig = new G4UIcmdWithADoubleAndUnit("/NuMI/run/muonBeamGaussYSig",this);
  muonBeamGaussYsig->SetGuidance("Set the Y-sig for a gaussian beam");
  muonBeamGaussYsig->SetParameterName("muonBeamGaussYSig",true);
  muonBeamGaussYsig->SetDefaultValue (NumiData->GetGaussBeamYSig());
  muonBeamGaussYsig->AvailableForStates(G4State_PreInit,G4State_Idle);

  extNtupleFileName = new G4UIcmdWithAString("/NuMI/run/extNtupleFileName",this);
  extNtupleFileName->SetGuidance("set external (fluka/mars) ntuple file name");
  extNtupleFileName->SetParameterName("extNtupleFileName",true);
  extNtupleFileName->SetDefaultValue (NumiData->GetExtNtupleFileName());
  extNtupleFileName->AvailableForStates(G4State_PreInit,G4State_Idle);

  setNEvents = new G4UIcmdWithAnInteger("/NuMI/run/NEvents",this);
  setNEvents->SetGuidance("Set the number of Events to process");
  setNEvents->SetParameterName("NEvents",true);
  setNEvents->SetDefaultValue (NumiData->GetNEvents());
  setNEvents->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  NumiOutputDir = new G4UIdirectory("/NuMI/output/");
  NumiOutputDir->SetGuidance("NuMI output management");

  setNuNtupleFile = new G4UIcmdWithAString("/NuMI/output/setNuNtupleFile",this);
  setNuNtupleFile->SetGuidance("set neutrino ntuple file name");
  setNuNtupleFile->SetParameterName("fileName",true);
  setNuNtupleFile->SetDefaultValue (NumiData->nuNtupleName);
  setNuNtupleFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  outputNuNtuple = new G4UIcmdWithABool("/NuMI/output/outputNuNtuple",this);
  outputNuNtuple->SetGuidance("output neutrino ntuple (true/false)");
  outputNuNtuple->SetParameterName("outputNuNtuple",true);
  outputNuNtuple->SetDefaultValue (NumiData->createNuNtuple);
  outputNuNtuple->AvailableForStates(G4State_PreInit,G4State_Idle);

  seth1NtupleFile = new G4UIcmdWithAString("/NuMI/output/seth1NtupleFile",this);
  seth1NtupleFile->SetGuidance("set target ntuple file name");
  seth1NtupleFile->SetParameterName("fileName",true);
  seth1NtupleFile->SetDefaultValue (NumiData->h1trackingNtupleName);
  seth1NtupleFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  outputh1Ntuple = new G4UIcmdWithABool("/NuMI/output/outputh1Ntuple",this);
  outputh1Ntuple->SetGuidance("output target ntuple (true/false)");
  outputh1Ntuple->SetParameterName("outputh1Ntuple",true);
  outputh1Ntuple->SetDefaultValue (NumiData->createH1TrackingPlane);
  outputh1Ntuple->AvailableForStates(G4State_PreInit,G4State_Idle);

  setHadmmNtupleDir = new G4UIcmdWithAString("/NuMI/output/setHadmmNtupleDir",this);
  setHadmmNtupleDir->SetGuidance("set hadron and muon monitor ntuple file directory");
  setHadmmNtupleDir->SetParameterName("fileDir",true);
  setHadmmNtupleDir->SetDefaultValue (NumiData->hadmmNtupleDir);
  setHadmmNtupleDir->AvailableForStates(G4State_PreInit,G4State_Idle);

  setHadmmNtupleFile = new G4UIcmdWithAString("/NuMI/output/setHadmmNtupleFile",this);
  setHadmmNtupleFile->SetGuidance("set hadron and muon monitor ntuple file name");
  setHadmmNtupleFile->SetParameterName("fileName",true);
  setHadmmNtupleFile->SetDefaultValue (NumiData->hadmmNtupleName);
  setHadmmNtupleFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  outputHadmmNtuple = new G4UIcmdWithABool("/NuMI/output/outputHadmmNtuple",this);
  outputHadmmNtuple->SetGuidance("output hadron and muon monitor ntuple (true/false)");
  outputHadmmNtuple->SetParameterName("outputHadmmNtuple",true);
  outputHadmmNtuple->SetDefaultValue (NumiData->createHadmmNtuple);
  outputHadmmNtuple->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  setAbsBkgNtupleDir = new G4UIcmdWithAString("/NuMI/output/setAbsBkgNtupleDir",this);
  setAbsBkgNtupleDir->SetGuidance("set absorber background ntuple file directory");
  setAbsBkgNtupleDir->SetParameterName("fileDir",true);
  setAbsBkgNtupleDir->SetDefaultValue (NumiData->GetAbsBkgNtupleDir());
  setAbsBkgNtupleDir->AvailableForStates(G4State_PreInit,G4State_Idle);

  setAbsBkgNtupleFile = new G4UIcmdWithAString("/NuMI/output/setAbsBkgNtupleFile",this);
  setAbsBkgNtupleFile->SetGuidance("set absorber background ntuple file name");
  setAbsBkgNtupleFile->SetParameterName("fileName",true);
  setAbsBkgNtupleFile->SetDefaultValue (NumiData->GetAbsBkgNtupleName());
  setAbsBkgNtupleFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  outputAbsBkgNtuple = new G4UIcmdWithABool("/NuMI/output/outputAbsBkgNtuple",this);
  outputAbsBkgNtuple->SetGuidance("output absorber background ntuple (true/false)");
  outputAbsBkgNtuple->SetParameterName("outputAbsBkgNtuple",true);
  outputAbsBkgNtuple->SetDefaultValue (NumiData->GetCreateAbsBkgNtuple());
  outputAbsBkgNtuple->AvailableForStates(G4State_PreInit,G4State_Idle);

  setASCIIFile = new G4UIcmdWithAString("/NuMI/output/setASCIIFile",this);
  setASCIIFile->SetGuidance("set ASCII file name");
  setASCIIFile->SetParameterName("fileName",true);
  setASCIIFile->SetDefaultValue (NumiData->asciiName);
  setASCIIFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  outputASCIIFile = new G4UIcmdWithABool("/NuMI/output/outputASCIIFile",this);
  outputASCIIFile->SetGuidance("output ASCII file (true/false)");
  outputASCIIFile->SetParameterName("outputASCIIFile",true);
  outputASCIIFile->SetDefaultValue (NumiData->createASCII);
  outputASCIIFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  setBXDRAWFile = new G4UIcmdWithAString("/NuMI/output/setBXDRAWFile",this);
  setBXDRAWFile->SetGuidance("set BXDRAW file name");
  setBXDRAWFile->SetParameterName("fileName",true);
  setBXDRAWFile->SetDefaultValue (NumiData->bxdrawName);
  setBXDRAWFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  outputBXDRAWFile = new G4UIcmdWithABool("/NuMI/output/outputBXDRAWFile",this);
  outputBXDRAWFile->SetGuidance("output BXDRAW file (true/false)");
  outputBXDRAWFile->SetParameterName("outputBXDRAWFile",true);
  outputBXDRAWFile->SetDefaultValue (NumiData->createBXDRAW);
  outputBXDRAWFile->AvailableForStates(G4State_PreInit,G4State_Idle);

  useDecayPipeSelect = new G4UIcmdWithABool("/NuMI/run/useDecayPipeSelect",this);
  useDecayPipeSelect->SetGuidance("use decay pipe antineutrino selection");
  useDecayPipeSelect->SetParameterName("useDecayPipeSelect",true);
  useDecayPipeSelect->SetDefaultValue (NumiData->useDecayPipeSelect);
  useDecayPipeSelect->AvailableForStates(G4State_PreInit,G4State_Idle);

  setTestTheta = new G4UIcmdWithADouble("/NuMI/run/setTestTheta",this);
  setTestTheta->SetGuidance("set angle of test beam");
  setTestTheta->SetParameterName("setTestTheta",true);
  setTestTheta->SetDefaultValue (30.);
  setTestTheta->AvailableForStates(G4State_PreInit,G4State_Idle);

  setStepLimit = new G4UIcmdWithADoubleAndUnit("/NuMI/run/setStepLimit",this);
  setStepLimit->SetGuidance("Maximum step size in magnetized horns");
  setStepLimit->SetParameterName("setStepLimit",true);
  setStepLimit->SetDefaultValue(0.0);
  setStepLimit->SetRange("setStepLimit>=0.");
  setStepLimit->SetUnitCategory("Length");
  setStepLimit->AvailableForStates(G4State_PreInit,G4State_Idle);
   //----------------------Jasmine Added the Following
  UseMacro = new G4UIcmdWithABool("/NuMI/output/useMacroBeam",this);
  UseMacro->SetGuidance("sets macro input as final input");
  UseMacro->SetParameterName("useMacro",true);
  UseMacro->SetDefaultValue(NumiData->useMacro);
  UseMacro->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  outputZpNtuple = new G4UIcmdWithABool("/NuMI/output/outputZpNtuple",this);
  outputZpNtuple->SetGuidance("sets yes/no Zp Ntuple");
  outputZpNtuple->SetParameterName("createZpNtuple",true);
  outputZpNtuple->SetDefaultValue(NumiData->createZpNtuple);
  outputZpNtuple->AvailableForStates(G4State_PreInit,G4State_Idle);

  setZpNtupleFile = new G4UIcmdWithAString("/NuMI/output/setZpNtupleFile",this);
  setZpNtupleFile->SetGuidance("set neutrino ntuple file name");
  setZpNtupleFile->SetParameterName("fileName",true);
  setZpNtupleFile->SetDefaultValue (NumiData->zpNtupleName);
  setZpNtupleFile->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  KillTracking=new G4UIcmdWithABool("/NuMI/run/KillTracking",this);
  KillTracking->SetGuidance("Sets Kill Tracking on or off");
  KillTracking->SetParameterName("KillTracking",true);
  KillTracking->SetDefaultValue(NumiData->KillTracking);
  KillTracking->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  KillTrackingThreshold = new G4UIcmdWithADoubleAndUnit("/NuMI/run/KillTrackingThreshold",this);
  KillTrackingThreshold->SetGuidance("Sets Kill Tracking on or off");
  KillTrackingThreshold->SetParameterName("KillTrackingThreshold",true);
  KillTrackingThreshold->SetDefaultValue(NumiData->KillTrackingThreshold);
  KillTrackingThreshold->AvailableForStates(G4State_PreInit,G4State_Idle);

  //Set the Minerva playlist for dkmeta.
  setMinervaPlaylist = new G4UIcmdWithAString("/NuMI/run/setMinervaPlaylist",this);
  setMinervaPlaylist->SetGuidance("set the Minerva playlist for dkmeta");
  setMinervaPlaylist->SetParameterName("PlaylistName",true);
  setMinervaPlaylist->SetDefaultValue (NumiData->GetPlaylist());
  setMinervaPlaylist->AvailableForStates(G4State_PreInit,G4State_Idle);

  //
  // Geantino Studies..
  //
  SteppingActionGeantinoCmd  = new G4UIcmdWithAString("/NuMI/stepping/name",this);
  SteppingActionGeantinoCmd->SetGuidance("Name for the Geantino study in question");
  SteppingActionGeantinoCmd->SetParameterName("Name",true);
  SteppingActionGeantinoCmd->SetDefaultValue ("GeomPosGeantino101");
  SteppingActionGeantinoCmd->AvailableForStates(G4State_Idle);
    
  OutputASCIIFileNameCmd  = new G4UIcmdWithAString("/NuMI/stepping/filename",this);
  OutputASCIIFileNameCmd->SetGuidance("Ascii file Name for a plain ASCII file with positions of the geantino  ");
  OutputASCIIFileNameCmd->SetParameterName("FileName",true);
  OutputASCIIFileNameCmd->SetDefaultValue ("./steppingActionOut.txt");
  OutputASCIIFileNameCmd->AvailableForStates(G4State_Idle);
    
  KeyVolumeNameFrom  = new G4UIcmdWithAString("/NuMI/stepping/KeyVolumeNameFrom",this);
  KeyVolumeNameFrom->SetGuidance("A volume that will trigger output running geantino propagation ");
  KeyVolumeNameFrom->SetParameterName("KeyVolumeNameFrom",true);
  KeyVolumeNameFrom->SetDefaultValue ("blank");
  KeyVolumeNameFrom->AvailableForStates(G4State_Idle);
  
  KeyVolumeNameTo  = new G4UIcmdWithAString("/NuMI/stepping/KeyVolumeNameTo",this);
  KeyVolumeNameTo->SetGuidance(
   "A volume that will trigger output running geantino propagation, second one Post or pre step  ");
  KeyVolumeNameTo->SetParameterName("KeyVolumeNameTo ",true);
  KeyVolumeNameTo->SetDefaultValue ("blank");
  KeyVolumeNameTo->AvailableForStates(G4State_Idle);
  
}

NumiRunActionMessenger::~NumiRunActionMessenger()
{
  delete readRndmCmd;
  delete showRndmCmd;
  delete setRndmSeedCmd;
  delete RndmDir;

  delete useNuBeam;
  delete useMuonBeam;
  delete simAbsBkg;
  delete simDRays;

  delete useWaterInTgt;

  delete useMuonInput;
  delete useTestBeam;
  delete useFlukaInput;
  delete useMarsInput;

  delete DebugLevel;
  delete debugOn;
  delete setRunID;
  delete setMaterialSigma;
  delete useNImpWeight;
  delete NumiRunDir;
  delete useZPosCut;
  delete muonBeamShape;
  delete muonBeamMomentum;
  delete muonBeamZPos;
  delete muonBeamGaussXsig;
  delete muonBeamGaussYsig;
  
  delete NInputParts;
  delete NInputPart;
  delete reWeightDeltas;
  delete nSplitDeltas;
  delete setAbsBkgNtupleDir;
  delete setAbsBkgNtupleFile;
  delete outputAbsBkgNtuple;
  delete setNuNtupleFile;
  delete outputNuNtuple;
  delete seth1NtupleFile;
  delete outputh1Ntuple;
  delete setHadmmNtupleFile;
  delete outputHadmmNtuple;
  delete setNEvents;
  delete setASCIIFile;
  delete outputASCIIFile;
  delete NumiOutputDir;
  delete useDecayPipeSelect;
  delete setTestTheta;
  delete setStepLimit;
  //=============================
  //==  For Raytracing ==========
  //-----------------------------
  delete UseMacro;
  delete outputZpNtuple;
  delete setZpNtupleFile;
  delete KillTracking;
  delete KillTrackingThreshold;

  //playlist name for dkmeta:
  delete setMinervaPlaylist;

 //
 // Geantino studies..
 //
 delete SteppingActionGeantinoCmd;
 delete OutputASCIIFileNameCmd;
 delete KeyVolumeNameFrom;
 delete KeyVolumeNameTo;
 
}

void NumiRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValues)
{

   
   NumiDataInput *NumiData = NumiDataInput::GetNumiDataInput();

   if(NumiData->fPrintInfo > 0 || NumiData->IsDebugOn())
   {
      G4cout << "NumiRunActionMessenger::SetNewValue - Setting Parameter values from input macro." << G4endl;
   }
   
  if (command == readRndmCmd)
    { 
      G4cout << "\n---> rndm status restored from file: " << newValues << G4endl;
      G4String rndmFile="rndm/";
      rndmFile.append(newValues);
      CLHEP::HepRandom::restoreEngineStatus(rndmFile);
    }   

  if (command == showRndmCmd)
    { 
      CLHEP::HepRandom::showEngineStatus();
    }  

  if (command == setRndmSeedCmd)
    { 
      CLHEP::HepRandom::setTheSeed(setRndmSeedCmd->GetNewIntValue(newValues));
    }



  //---------------------------------------
  //Commands for defining the simulation
  //
  if (command == useNuBeam)   { NumiData->SetNuBeam(useNuBeam->GetNewBoolValue(newValues));     }
  if (command == useMuonBeam) { NumiData->SetMuonBeam(useMuonBeam->GetNewBoolValue(newValues)); }
  if (command == simAbsBkg)   { NumiData->SetSimAbsBkg(simAbsBkg->GetNewBoolValue(newValues));  }
  if (command == simDRays)    { NumiData->SetSimDRays(simDRays->GetNewBoolValue(newValues));    }
  //---------------------------------------

  //---------------------------------------
  //Commands for defining the sub-simulation
  //
  if (command == useWaterInTgt) { NumiData->SetWaterInTgt(useWaterInTgt->GetNewBoolValue(newValues)); }
  //---------------------------------------
     
  //---------------------------------------
  //Commands for defining the Beam
  //
  if (command == useDetailedProtonBeam) { NumiData->SetDetailedProtonBeam(useDetailedProtonBeam->GetNewBoolValue(newValues));}
  if (command == useTestBeam)           { NumiData->SetTestBeam(useTestBeam->GetNewBoolValue(newValues));}
  if (command == useFlukaInput)         { NumiData->SetFlukaInput(useFlukaInput->GetNewBoolValue(newValues));}
  if (command == useMarsInput)          { NumiData->SetMarsInput(useMarsInput->GetNewBoolValue(newValues)); }
  if (command == useMuonInput)          { NumiData->SetMuonInput(useMuonInput->GetNewBoolValue(newValues)); }
  //---------------------------------------

  if (command == DebugLevel)       { NumiData->SetDebugLevel(DebugLevel->GetNewIntValue(newValues)); }
  if (command == debugOn)          { NumiData->SetDebugOn(debugOn->GetNewBoolValue(newValues)); }

  if (command == setRunID)
  {
    G4RunManager* runManager = G4RunManager::GetRunManager();
    runManager->SetRunIDCounter(setRunID->GetNewIntValue(newValues));
  }

  if (command == setMaterialSigma) { NumiData->materialSigma = setMaterialSigma->GetNewDoubleValue(newValues); }
  if (command == useNImpWeight)    { NumiData->SetNImpWeight(useNImpWeight->GetNewBoolValue(newValues)); }


  
  
  if (command == useZPosCut){
    NumiData->SetUseZPosCut(useZPosCut->GetNewBoolValue(newValues));
  }
  if (command == muonBeamShape){
      NumiData->SetMuonBeamShape(newValues);
  }
  if (command == muonBeamMomentum){
     NumiData->SetMuonBeamMomentum(muonBeamMomentum->GetNewDoubleValue(newValues));
  }
  if (command == muonBeamZPos){
     NumiData->SetMuonBeamZPos(muonBeamZPos->GetNewDoubleValue(newValues));
  }
  if (command == muonBeamGaussXsig){
     NumiData->SetGaussBeamXSig(muonBeamGaussXsig->GetNewDoubleValue(newValues));
  }
  if (command == muonBeamGaussYsig){
     NumiData->SetGaussBeamYSig(muonBeamGaussYsig->GetNewDoubleValue(newValues));
  }  
  
  if (command == NInputParts){
    NumiData->SetNInputParts(NInputParts->GetNewIntValue(newValues));
  }
  if (command == NInputPart){
    NumiData->SetNInputPart(NInputPart->GetNewIntValue(newValues));
  }
  if (command == reWeightDeltas){
    NumiData->SetReWeightDeltas(reWeightDeltas->GetNewBoolValue(newValues));
  }
  if (command == nSplitDeltas){
    NumiData->SetNSplitDeltas(nSplitDeltas->GetNewIntValue(newValues));
  }
  if (command == extNtupleFileName){
    NumiData->SetExtNtupleFileName(newValues);
  }
  if (command == setNuNtupleFile){
      NumiData->SetNuNtupleName(newValues);
  }
  if (command == outputNuNtuple){
      NumiData->OutputNuNtuple(outputNuNtuple->GetNewBoolValue(newValues));
  }
  if (command == seth1NtupleFile){
      NumiData->SetTarNtupleName(newValues);
  }
  if (command == outputh1Ntuple){
      NumiData->OutputTarNtuple(outputh1Ntuple->GetNewBoolValue(newValues));
  }
  if (command == setHadmmNtupleDir){
      NumiData->SetHadmmNtupleDir(newValues);
  }
  if (command == setHadmmNtupleFile){
      NumiData->SetHadmmNtupleName(newValues);
  }
  if (command == outputHadmmNtuple){
      NumiData->OutputHadmmNtuple(outputHadmmNtuple->GetNewBoolValue(newValues));
  }
   if (command == setAbsBkgNtupleDir){
      NumiData->SetAbsBkgNtupleDir(newValues);
  }
  if (command == setAbsBkgNtupleFile){
      NumiData->SetAbsBkgNtupleName(newValues);
  }
  if (command == outputAbsBkgNtuple){
      NumiData->OutputAbsBkgNtuple(outputAbsBkgNtuple->GetNewBoolValue(newValues));
  }
  if (command == setNEvents){
      NumiData->SetNEvents(setNEvents->GetNewIntValue(newValues));
  }

  if (command == setASCIIFile){
      NumiData->SetASCIIName(newValues);
  }

  if (command == outputASCIIFile){
      NumiData->OutputASCII(outputASCIIFile->GetNewBoolValue(newValues));
  }

  if (command == setBXDRAWFile){
      NumiData->SetBXDRAWName(newValues);
  }

  if (command == outputBXDRAWFile){
      NumiData->OutputBXDRAW(outputBXDRAWFile->GetNewBoolValue(newValues));
  }

  if (command == useDecayPipeSelect){
	NumiData->SetDecayPipeSelect(useDecayPipeSelect->GetNewBoolValue(newValues));
  }
  
  if (command == setTestTheta){
	NumiData->SetTestTheta(setTestTheta->GetNewDoubleValue(newValues));
  }
  
  if (command == setStepLimit){
    NumiData->SetStepLimit(setStepLimit->GetNewDoubleValue(newValues));
  }  
  if (command== UseMacro){
    NumiData->SetMacroBeam(UseMacro->GetNewBoolValue(newValues));
  }
  
  if (command==outputZpNtuple){
    NumiData->OutputZpNtuple(outputZpNtuple->GetNewBoolValue(newValues));
  }
  
  if (command==setZpNtupleFile){
     NumiData->SetZpNtupleName(newValues);
  }
  if (command== KillTracking){
     NumiData->SetKillTracking(KillTracking->GetNewBoolValue(newValues));
  }
  if (command== KillTrackingThreshold){
     NumiData->SetKillTrackingThreshold(KillTrackingThreshold->GetNewDoubleValue(newValues));
  }
  if (command == setMinervaPlaylist){
     NumiData->SetPlaylist(newValues);
  }

   if (command == SteppingActionGeantinoCmd) {
     G4RunManager *grM = G4RunManager::GetRunManager();
     const NumiSteppingAction *NSA = dynamic_cast<const NumiSteppingAction *> (grM->GetUserSteppingAction());
     NSA->SetGeantinoStudyName(newValues);
   }
   if (command == OutputASCIIFileNameCmd) {
     G4RunManager *grM = G4RunManager::GetRunManager();
     const NumiSteppingAction *NSA = dynamic_cast<const NumiSteppingAction *> (grM->GetUserSteppingAction());
     NSA->OpenOutStudyGeantino(newValues.c_str());
   }
   if (command == KeyVolumeNameFrom) {
     G4RunManager *grM = G4RunManager::GetRunManager();
     const NumiSteppingAction *NSA = dynamic_cast<const NumiSteppingAction *>(grM->GetUserSteppingAction());
     NSA->SetKeyVolumeNameFrom(newValues);
   }
   if (command == KeyVolumeNameTo) {
     G4RunManager *grM = G4RunManager::GetRunManager();
     const NumiSteppingAction *NSA = dynamic_cast<const NumiSteppingAction *>(grM->GetUserSteppingAction());
     NSA->SetKeyVolumeNameTo(newValues);
   }
}

