//----------------------------------------------------------------------
// $Id
//----------------------------------------------------------------------
#include <cctype>
#include <functional>
#include <algorithm>

#include "NumiDetectorMessenger.hh"
#include "NumiDetectorConstruction.hh"
#include "NumiDataInput.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UnitsTable.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#ifndef FLUGG
  #include "G4GDMLParser.hh"
#endif

NumiDetectorMessenger::NumiDetectorMessenger( NumiDetectorConstruction* NumiDet):NumiDetector(NumiDet)
{

        NumiDataInput *ND = NumiDataInput::GetNumiDataInput();

        if(ND->fPrintInfo > 0 || ND->IsDebugOn())
        {
           G4cout << "NumiDetectorMessenger Constructor Called." << G4endl;
        }

        NumiDir = new G4UIdirectory("/NuMI/");
        NumiDir->SetGuidance("UI commands for detector geometry");

        detDir = new G4UIdirectory("/NuMI/det/");
        detDir->SetGuidance("detector control");

        TargetGasCmd = new G4UIcmdWithAString("/NuMI/det/setTarGas",this);
        TargetGasCmd->SetGuidance("Select gas inside the target.");
        TargetGasCmd->SetParameterName("choice",false);
        TargetGasCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        ConstructTarget = new G4UIcmdWithABool("/NuMI/det/constructTarget",this);
        ConstructTarget->SetGuidance("Target construction on/off");
        ConstructTarget->SetParameterName("constructTarget",true);
        ConstructTarget->SetDefaultValue (ND->constructTarget);
        ConstructTarget->AvailableForStates(G4State_PreInit,G4State_Idle);

        HeInDecayPipe = new G4UIcmdWithABool("/NuMI/det/heInDecayPipe",this);
        HeInDecayPipe->SetGuidance("Insert 0.9atm, 300K He in the decay pipe - on/off");
        HeInDecayPipe->SetParameterName("heInDecayPipe",true);
        HeInDecayPipe->SetDefaultValue (ND->HeInDecayPipe);
        HeInDecayPipe->AvailableForStates(G4State_PreInit,G4State_Idle);

        applyDecayPipeMagneticField = new G4UIcmdWithABool("/NuMI/det/applyDecayPipeMagneticField",this);
        applyDecayPipeMagneticField->SetGuidance("Apply magnetic field  Bx=0.1, By=-0.3, Bz=-0.07 gauss - on/off");
        applyDecayPipeMagneticField->SetParameterName("applyDecayPipeMagneticField",true);
        applyDecayPipeMagneticField->SetDefaultValue (ND->applyDecayPipeMagneticField);
        applyDecayPipeMagneticField->AvailableForStates(G4State_PreInit,G4State_Idle);


        LengthOfWaterInTgt = new G4UIcmdWithADoubleAndUnit("/NuMI/det/LengthOfWaterInTgt",this);
        LengthOfWaterInTgt->SetGuidance("Set length of water in target for Water In the Target simulation.");
        LengthOfWaterInTgt->SetParameterName("LengthOfWaterInTgt",true);
        LengthOfWaterInTgt->SetRange("LengthOfWaterInTgt>=3.");
        LengthOfWaterInTgt->SetUnitCategory("Length");
        LengthOfWaterInTgt->SetDefaultValue(ND->GetLengthOfWaterInTgt());
        LengthOfWaterInTgt->AvailableForStates(G4State_PreInit,G4State_Idle);

        //TargetZ0Cmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setTargetZ0",this);
        //TargetZ0Cmd->SetGuidance("Set Z0 position of target");
        //TargetZ0Cmd->SetParameterName("Size",false);
        //TargetZ0Cmd->SetRange("Size<=0.");
        //TargetZ0Cmd->SetUnitCategory("Length");
        //TargetZ0Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        new G4UnitDefinition("kiloampere" , "kA", "Electric current", 1000.*CLHEP::ampere);
        //HornCurrentCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setHornCurrent",this);
        //HornCurrentCmd->SetGuidance("Set horn current");
        //HornCurrentCmd->SetParameterName("current",false);
        //HornCurrentCmd->SetDefaultValue(ND->HornCurrent);
        //HornCurrentCmd->SetRange("current>=0.");
        //HornCurrentCmd->SetUnitCategory("Electric current");
        //HornCurrentCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        ConstructSolidMuMons = new G4UIcmdWithABool("/NuMI/det/constructSolidMuMons",this);
        ConstructSolidMuMons->SetGuidance("Construct Full Solid Muon Monitors yes/no");
        ConstructSolidMuMons->SetParameterName("solidMuMons",true);
        ConstructSolidMuMons->SetDefaultValue (ND->GetSolidMuMons());
        ConstructSolidMuMons->AvailableForStates(G4State_PreInit,G4State_Idle);


        RunPeriod = new G4UIcmdWithAnInteger("/NuMI/det/RunPeriod",this);
        RunPeriod -> SetGuidance("Set the Run Period, default is -999 which is no run period at all. MUST SET IT. Run Period 0 is like a \"default\" run period, everything is aligned, it is not specific to any ACTUAL NuMI beam running period.");
        RunPeriod -> SetParameterName("RunPeriod",false);
        RunPeriod -> SetDefaultValue (ND->GetRunPeriod());
        RunPeriod -> AvailableForStates(G4State_PreInit,G4State_Idle);

        BeamConfig = new G4UIcmdWithAString("/NuMI/det/BeamConfig",this);
        BeamConfig -> SetGuidance("Set the Beam Configuration. Must be in the form \"LE#z#i\" or \"le#z#i\". Note cannot handle ME config right now.");
        BeamConfig -> SetParameterName("BeamConfig",false);
        BeamConfig -> SetDefaultValue (ND->GetBeamConfig());
        BeamConfig -> AvailableForStates(G4State_PreInit,G4State_Idle);

        UseCorrHornCurrent = new G4UIcmdWithABool("/NuMI/det/UseCorrHornCurrent",this);
        UseCorrHornCurrent -> SetGuidance("Use the Calibration Corrected Horn Current value of the horn current configuration defined by the BeamConfiguration, true/false.");
        UseCorrHornCurrent -> SetParameterName("UseCorrHornCurrent",false);
        UseCorrHornCurrent -> AvailableForStates(G4State_PreInit,G4State_Idle);

        UseHornMisalign = new G4UIcmdWithABool("/NuMI/det/UseHornMisalign",this);
        UseHornMisalign -> SetGuidance("Set to use a nonstandard mm trans. dist. of the horns - forms are  ###hot and ###htt");
        UseHornMisalign -> SetParameterName("UseHornMisalign",false);
        UseHornMisalign -> AvailableForStates(G4State_PreInit,G4State_Idle);

        UseTgtDensity = new G4UIcmdWithABool("/NuMI/det/UseTgtDensity",this);
        UseTgtDensity -> SetGuidance("Set to use a nonstandard target density (in ug/cm3) - forms are  #######tgd");
        UseTgtDensity -> SetParameterName("UseTgtDensity",false);
        UseTgtDensity -> AvailableForStates(G4State_PreInit,G4State_Idle);


        AbsorberConfig = new G4UIcmdWithAString("/NuMI/det/absorberConfig",this);
        AbsorberConfig -> SetGuidance("Set Absorber Configuration. Full Converage, Center Coverage, None, Configured.");
        AbsorberConfig -> SetParameterName("AbsorberConfig",false);
        AbsorberConfig -> SetDefaultValue (ND->GetAbsorberConfig());
        AbsorberConfig -> AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon0AbsMatCmd = new G4UIcmdWithAString("/NuMI/det/setMon0AbsMater",this);
        Mon0AbsMatCmd->SetGuidance("Select Material of the Absorber in front of Mon 0.");
        Mon0AbsMatCmd->SetParameterName("choice",false);
        Mon0AbsMatCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon1AbsMatCmd = new G4UIcmdWithAString("/NuMI/det/setMon1AbsMater",this);
        Mon1AbsMatCmd->SetGuidance("Select Material of the Absorber in front of Mon 1.");
        Mon1AbsMatCmd->SetParameterName("choice",false);
        Mon1AbsMatCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon2AbsMatCmd = new G4UIcmdWithAString("/NuMI/det/setMon2AbsMater",this);
        Mon2AbsMatCmd->SetGuidance("Select Material of the Absorber in front of Mon 2.");
        Mon2AbsMatCmd->SetParameterName("choice",false);
        Mon2AbsMatCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon0AbsThickCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setMon0AbsThick",this);
        Mon0AbsThickCmd->SetGuidance("Set Thickness of the Absorber in front of Mon 0.");
        Mon0AbsThickCmd->SetParameterName("Abs0Thick",false);
        Mon0AbsThickCmd->SetRange("Abs0Thick>=0.");
        Mon0AbsThickCmd->SetUnitCategory("Length");
        Mon0AbsThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon1AbsThickCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setMon1AbsThick",this);
        Mon1AbsThickCmd->SetGuidance("Set Thickness of the Absorber in front of Mon 1.");
        Mon1AbsThickCmd->SetParameterName("Abs1Thick",false);
        Mon1AbsThickCmd->SetRange("Abs1Thick>=0.");
        Mon1AbsThickCmd->SetUnitCategory("Length");
        Mon1AbsThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon2AbsThickCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setMon2AbsThick",this);
        Mon2AbsThickCmd->SetGuidance("Set Thickness of the Absorber in front of Mon 2.");
        Mon2AbsThickCmd->SetParameterName("Abs2Thick",false);
        Mon2AbsThickCmd->SetRange("Abs2Thick>=0.");
        Mon2AbsThickCmd->SetUnitCategory("Length");
        Mon2AbsThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon0AbsDistCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setMon0AbsDist",this);
        Mon0AbsDistCmd->SetGuidance("Set Distance between the Absorber and Mon 0.");
        Mon0AbsDistCmd->SetParameterName("Abs0Dist",false);
        Mon0AbsDistCmd->SetRange("Abs0Dist>=0.");
        Mon0AbsDistCmd->SetUnitCategory("Length");
        Mon0AbsDistCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon1AbsDistCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setMon1AbsDist",this);
        Mon1AbsDistCmd->SetGuidance("Set Distance between the Absorber and Mon 1.");
        Mon1AbsDistCmd->SetParameterName("Abs1Dist",false);
        Mon1AbsDistCmd->SetRange("Abs1Dist>=0.");
        Mon1AbsDistCmd->SetUnitCategory("Length");
        Mon1AbsDistCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        Mon2AbsDistCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setMon2AbsDist",this);
        Mon2AbsDistCmd->SetGuidance("Set Distance between the Absorber and Mon 2.");
        Mon2AbsDistCmd->SetParameterName("Abs2Dist",false);
        Mon2AbsDistCmd->SetRange("Abs2Dist>=0.");
        Mon2AbsDistCmd->SetUnitCategory("Length");
        Mon2AbsDistCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

        UpdateCmd = new G4UIcmdWithoutParameter("/NuMI/det/update",this);
        UpdateCmd->SetGuidance("Update NuMi geometry.");
        UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
        UpdateCmd->SetGuidance("if you changed geometrical value(s).");
        UpdateCmd->AvailableForStates(G4State_Idle);




        fBeamConfigDirectory = new G4UIdirectory("/NuMI/det/set/");
        fBeamConfigDirectory->SetGuidance("UI commands for changing geometry for the Nova/ME target");

        fDuratekShiftCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/set/duratekShift",this);
        fDuratekShiftCmd->SetParameterName("DuratekShift",false);
        fDuratekShiftCmd->SetGuidance("Set the change to the Duratek block for ME target");
        fDuratekShiftCmd->SetDefaultValue(0.0*CLHEP::m);

        fTHBlockShiftCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/set/thblockShift",this);
        fTHBlockShiftCmd->SetParameterName("THBlockShift",false);
        fTHBlockShiftCmd->SetGuidance("Set the change to the z positions of T.H blocks for ME target");
        fTHBlockShiftCmd->SetDefaultValue(0.0*CLHEP::m);

        fDeltaOuterThicknessCmd
            = new G4UIcmdWithADoubleAndUnit("/NuMI/det/set/deltaOuterThickness",this);
        fDeltaOuterThicknessCmd->SetParameterName("DeltaOuterThickness",false);
        fDeltaOuterThicknessCmd->SetGuidance("Change in the horn1 outer conductor thickness");
        fDeltaOuterThicknessCmd->SetDefaultValue(0.0);


        fBafflePositionCmd = new G4UIcmdWith3VectorAndUnit("/NuMI/det/set/bafflePosition", this);
        fBafflePositionCmd->SetParameterName("BafflePositionX", "BafflePositionY", "BafflePositionZ",  false);
        fBafflePositionCmd->SetGuidance("Set the baffle position");

        fTargetPositionCmd = new G4UIcmdWith3VectorAndUnit("/NuMI/det/set/targetPosition", this);
        fTargetPositionCmd->SetParameterName("targetPositionX", "targetPositionY", "targetPositionZ",  false);
        fTargetPositionCmd->SetGuidance("Set the target position");

        fHorn1PositionCmd = new G4UIcmdWith3VectorAndUnit("/NuMI/det/set/horn1Position", this);
        fHorn1PositionCmd->SetParameterName("Horn1PositionX", "Horn1PositionY", "Horn1PositionZ", false);
        fHorn1PositionCmd->SetGuidance("Set the horn1 position");

        fHorn2PositionCmd = new G4UIcmdWith3VectorAndUnit("/NuMI/det/set/horn2Position", this);
        fHorn2PositionCmd->SetParameterName("Horn2PositionX", "Horn2PositionY", "Horn2PositionZ", false);
        fHorn2PositionCmd->SetGuidance("Set the horn2 position");

        fHorn1RotationCmd = new G4UIcmdWith3VectorAndUnit("/NuMI/det/set/horn1Rotation", this);
        fHorn1RotationCmd->SetParameterName("Horn1RotationPhi", "Horn1RotationTheta", "Horn1RotationPsi", false);
        fHorn1RotationCmd->SetGuidance("Set the horn1 rotation");
        fHorn1RotationCmd->SetDefaultValue(G4ThreeVector(0.0*CLHEP::rad));

        fHorn2RotationCmd = new G4UIcmdWith3VectorAndUnit("/NuMI/det/set/horn2Rotation", this);
        fHorn2RotationCmd->SetParameterName("Horn2RotationPhi", "Horn2RotationTheta", "Horn2RotationPsi", false);
        fHorn2RotationCmd->SetGuidance("Set the horn2 rotation");
        fHorn2RotationCmd->SetDefaultValue(G4ThreeVector(0.0*CLHEP::rad));

        fBaffleOuterRadiusCmd
            = new G4UIcmdWithADoubleAndUnit("/NuMI/det/set/baffleOuterRadius",this);
        fBaffleOuterRadiusCmd->SetParameterName("BaffleOuterRadius",false);
        fBaffleOuterRadiusCmd->SetGuidance("Set baffle outer radius");
        fBaffleOuterRadiusCmd->SetDefaultValue(3.0*CLHEP::cm);

        fBaffleInnerRadiusCmd
            = new G4UIcmdWithADoubleAndUnit("/NuMI/det/set/baffleInnerRadius",this);
        fBaffleInnerRadiusCmd->SetParameterName("BaffleInnerRadius",false);
        fBaffleInnerRadiusCmd->SetGuidance("Set baffle inner radius");
        fBaffleInnerRadiusCmd->SetDefaultValue(5.5*CLHEP::mm);

        fBaffleLengthCmd
            = new G4UIcmdWithADoubleAndUnit("/NuMI/det/set/baffleLength",this);
        fBaffleLengthCmd->SetParameterName("BaffleLength",false);
        fBaffleLengthCmd->SetGuidance("Set baffle length");
        fBaffleLengthCmd->SetDefaultValue(1.5*CLHEP::m);

        fForcedOldTargetCmd = new G4UIcmdWithABool("/NuMI/det/set/forceOldTarget",this);
        fForcedOldTargetCmd->SetGuidance("Force to use the old target");
        fForcedOldTargetCmd->SetParameterName("forceOldTarget",true);
        fForcedOldTargetCmd->SetDefaultValue(false);
        //
        // Horn Water layer on the inner Conductor
        //
        fHornWaterLayerThick = new G4UIcmdWithADoubleAndUnit("/NuMI/det/HornWaterLayerThickness",this);
        fHornWaterLayerThick->SetGuidance("Set the Water Layer thicknes on Horn inner conductors");
        fHornWaterLayerThick->SetParameterName("Water Layer thicknes on Horn inner conductors",true);
        fHornWaterLayerThick->SetUnitCategory("Length");
        fHornWaterLayerThick->SetDefaultValue(ND->GetHornWaterLayerThick());
        fHornWaterLayerThick->AvailableForStates(G4State_PreInit,G4State_Idle);
        //
        // Horn 1 Inner conductor coating layer.
        //
        fHorn1ExtraLayerAlum = new G4UIcmdWithADoubleAndUnit("/NuMI/det/Horn1ExtraLayerAlum",this);
        fHorn1ExtraLayerAlum->SetGuidance("Set an extra layer of aluminum.  This extra coating can be negative ");
        fHorn1ExtraLayerAlum->SetParameterName("Extra Layer on Horn1 inner conductors",true);
        fHorn1ExtraLayerAlum->SetUnitCategory("Length");
        fHorn1ExtraLayerAlum->SetDefaultValue(ND->GetHorn1ExtraLayerAlum());
        fHorn1ExtraLayerAlum->AvailableForStates(G4State_PreInit, G4State_Idle);
        std::cerr << " I have defined the command Horn1ExtraLayerAlum at point value " << (void*) fHorn1ExtraLayerAlum << std::endl;
        //
        // Allow for the option of replacing the existing impementation of Horn1 with the one used for LBNE 700 kW systematics
        // studies.
        //
        fHorn1IsAlternate = new G4UIcmdWithABool("/NuMI/det/Horn1IsAlternate",this);
        fHorn1IsAlternate->SetGuidance("Build the Horn 1 inner conductors following set of drawings from AD ");
        fHorn1IsAlternate->SetParameterName("Horn1IsAlternate", true);
        fHorn1IsAlternate->SetDefaultValue(ND->GetHorn1IsAlternate());
        fHorn1IsAlternate->AvailableForStates(G4State_PreInit, G4State_Idle);
        //
        // Allow for the option of making small changes to the existing Numi Horn1 code to test the accuracy of the geometry.
        // P. L. Dec 1 2014.
        //
        fHorn1IsRefined = new G4UIcmdWithABool("/NuMI/det/Horn1IsRefined",this);
        fHorn1IsRefined->SetGuidance("Build the Horn 1 inner conductors making some asjustment in the geometry ");
        fHorn1IsRefined->SetParameterName("Horn1IsRefined", true);
        fHorn1IsRefined->SetDefaultValue(ND->GetHorn1IsRefined());
        fHorn1IsRefined->AvailableForStates(G4State_PreInit, G4State_Idle);
        //
        // September 2017, Paul Lebrun, just curious to see if the level of details matters for the ME running.
        //
        fHorn1UpstrIOisTorus = new G4UIcmdWithABool("/NuMI/det/Horn1UpstrIOisTorus",this);
        fHorn1UpstrIOisTorus->SetGuidance("Build the Horn 1 I/O transition of the CGS subtraction, box from a torus.  ");
        fHorn1UpstrIOisTorus->SetParameterName("Horn1UpstrIOisTorus", true);
        fHorn1UpstrIOisTorus->SetDefaultValue(true);
        fHorn1UpstrIOisTorus->AvailableForStates(G4State_PreInit, G4State_Idle);
        //
        // Back door to study the magnetic field maps.
        //
        fDumpBFieldPlease = new G4UIcmdWithABool("/NuMI/det/DumpBfieldPlease",this);
        fDumpBFieldPlease->SetGuidance("Dump the BField as tracking proceeds ");
        fDumpBFieldPlease->SetParameterName("DumpBFieldPlease", true);
        fDumpBFieldPlease->SetDefaultValue(ND->GetDumpBFieldPlease());
        fDumpBFieldPlease->AvailableForStates(G4State_PreInit, G4State_Idle);
        //
        // September 2017...
        //
        fIgnoreCEQBr = new G4UIcmdWithABool("/NuMI/det/Horn1FieldIgnoreCEQBr",this);
        fIgnoreCEQBr->SetGuidance(
              "Set CEQ field as a just a perturbation of the main component of the field (default is false)  ");
        fIgnoreCEQBr->SetParameterName("fIgnoreCEQBr",true); // in Detector data
        fIgnoreCEQBr->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn1FieldZCutUpstream = new G4UIcmdWithADoubleAndUnit("/NuMI/det/Horn1FieldZCutUpstream",this);
        fHorn1FieldZCutUpstream->SetGuidance(
              "Set the upstream Z cut off coordinate.  Field is 0 upstream of this coord. ");
        fHorn1FieldZCutUpstream->SetParameterName("fHorn1FieldZCutUpstream",true); // in Detector data
        fHorn1FieldZCutUpstream->SetUnitCategory("Length");
        fHorn1FieldZCutUpstream->SetDefaultValue(-32.);  // in mm
        fHorn1FieldZCutUpstream->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn1FieldZCutDwnstream = new G4UIcmdWithADoubleAndUnit("/NuMI/det/Horn1FieldZCutDwnstream",this);
        fHorn1FieldZCutDwnstream->SetGuidance(
              "Set the downstream Z cut off coordinate.  Field is 0 downstream of this coord. ");
        fHorn1FieldZCutDwnstream->SetParameterName("fHorn1FieldZCutDwnstream",true); // in Detector data
        fHorn1FieldZCutDwnstream->SetUnitCategory("Length");
        fHorn1FieldZCutDwnstream->SetDefaultValue(3277.);
           // in mm // Value extracted from the code circa Aug 2107
        fHorn1FieldZCutDwnstream->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn1CurrentEqualizerLongAbsLength = new G4UIcmdWithADoubleAndUnit("/NuMI/det/Horn1CurrentEqualizerLongAbsLength",this);
        fHorn1CurrentEqualizerLongAbsLength->SetGuidance(
              "Set the effective Current Equalizer abosrption length  ");
        fHorn1CurrentEqualizerLongAbsLength->SetParameterName("fHorn1CurrentEqualizerLongAbsLength",true); // in Detector data
        fHorn1CurrentEqualizerLongAbsLength->SetUnitCategory("Length");
        fHorn1CurrentEqualizerLongAbsLength->SetDefaultValue(0.);
         // The current is instantenously phi symmetric
        fHorn1CurrentEqualizerLongAbsLength->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn1CurrentEqualizerQuadAmpl = new G4UIcmdWithADouble("/NuMI/det/Horn1CurrentEqualizerQuadAmpl",this);
        fHorn1CurrentEqualizerQuadAmpl->SetGuidance(
              "Set the Strip line induced quadrupole amplitude (%, at Z = Z Zend ");
        fHorn1CurrentEqualizerQuadAmpl->SetParameterName("fHorn1CurrentEqualizerQuadAmpl",true); // in Detector data
        fHorn1CurrentEqualizerQuadAmpl->SetDefaultValue(0.);
         // The current is perfectly phi symmetric
        fHorn1CurrentEqualizerQuadAmpl->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn1CurrentEqualizerOctAmpl = new G4UIcmdWithADouble("/NuMI/det/Horn1CurrentEqualizerOctAmpl",this);
        fHorn1CurrentEqualizerOctAmpl->SetGuidance(
              "Set the Strip line induced octupole amplitude (%, at Z = Z Zend ");
        fHorn1CurrentEqualizerOctAmpl->SetParameterName("fHorn1CurrentEqualizerOctAmpl",true); // in Detector data
        fHorn1CurrentEqualizerOctAmpl->SetDefaultValue(0.);
         // The current is instantenously phi symmetric
        fHorn1CurrentEqualizerOctAmpl->AvailableForStates(G4State_PreInit, G4State_Idle);
//
// October 2017..
//
        fHorn2CurrentEqualizerLongAbsLength = new G4UIcmdWithADoubleAndUnit("/NuMI/det/Horn2CurrentEqualizerLongAbsLength",this);
        fHorn2CurrentEqualizerLongAbsLength->SetGuidance(
              "Set the effective Current Equalizer abosrption length  ");
        fHorn2CurrentEqualizerLongAbsLength->SetParameterName("fHorn2CurrentEqualizerLongAbsLength",true); // in Detector data
        fHorn2CurrentEqualizerLongAbsLength->SetUnitCategory("Length");
        fHorn2CurrentEqualizerLongAbsLength->SetDefaultValue(0.);
         // The current is instantenously phi symmetric
        fHorn2CurrentEqualizerLongAbsLength->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn2CurrentEqualizerQuadAmpl = new G4UIcmdWithADouble("/NuMI/det/Horn2CurrentEqualizerQuadAmpl",this);
        fHorn2CurrentEqualizerQuadAmpl->SetGuidance(
              "Set the Strip line induced quadrupole amplitude (%, at Z = Z Zend ");
        fHorn2CurrentEqualizerQuadAmpl->SetParameterName("fHorn2CurrentEqualizerQuadAmpl",true); // in Detector data
        fHorn2CurrentEqualizerQuadAmpl->SetDefaultValue(0.);
         // The current is perfectly phi symmetric
        fHorn2CurrentEqualizerQuadAmpl->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn2CurrentEqualizerOctAmpl = new G4UIcmdWithADouble("/NuMI/det/Horn2CurrentEqualizerOctAmpl",this);
        fHorn2CurrentEqualizerOctAmpl->SetGuidance(
              "Set the Strip line induced octupole amplitude (%, at Z = Z Zend ");
        fHorn2CurrentEqualizerOctAmpl->SetParameterName("fHorn2CurrentEqualizerOctAmpl",true); // in Detector data
        fHorn2CurrentEqualizerOctAmpl->SetDefaultValue(0.);
         // The current is instantenously phi symmetric
        fHorn2CurrentEqualizerOctAmpl->AvailableForStates(G4State_PreInit, G4State_Idle);
//
// Back to the data cards introduced for the ME studies, Sept. 2017.
//
        fNovaTargetHTilt = new G4UIcmdWithADouble("/NuMI/det/NovaTargetHTilt",this);
        fNovaTargetHTilt->SetGuidance(
              "Set the tilt of the nova target, in the Horizontal plane  ");
        fNovaTargetHTilt->SetParameterName("fNovaTargetHTilt",true); // in Detector data
        fNovaTargetHTilt->SetDefaultValue(0.);  // in radians
        fNovaTargetHTilt->AvailableForStates(G4State_PreInit, G4State_Idle);

        fNovaTargetVTilt = new G4UIcmdWithADouble("/NuMI/det/NovaTargetVTilt",this);
        fNovaTargetVTilt->SetGuidance(
              "Set the tilt of the nova target, in the Vertical plane  ");
        fNovaTargetVTilt->SetParameterName("fNovaTargetVTilt",true); // in Detector data
        fNovaTargetVTilt->SetDefaultValue(0.);  // in radians
        fNovaTargetVTilt->AvailableForStates(G4State_PreInit, G4State_Idle);

        fNovaTargetXOffset = new G4UIcmdWithADoubleAndUnit("/NuMI/det/NovaTargetXOffset",this);
        fNovaTargetXOffset->SetGuidance(
              "Set the X-offset with respect to the beam & Horn 1 axis ");
        fNovaTargetXOffset->SetParameterName("fNovaTargetXOffset",true); // in Detector data
        fNovaTargetXOffset->SetUnitCategory("Length");
        fNovaTargetXOffset->SetDefaultValue(0.);  // in mm
        fNovaTargetXOffset->AvailableForStates(G4State_PreInit, G4State_Idle);

        fNovaTargetYOffset = new G4UIcmdWithADoubleAndUnit("/NuMI/det/NovaTargetYOffset",this);
        fNovaTargetYOffset->SetGuidance(
              "Set the Y-offset with respect to the beam & Horn 1 axis ");
        fNovaTargetYOffset->SetParameterName("fNovaTargetYOffset",true); // in Detector data
        fNovaTargetYOffset->SetUnitCategory("Length");
        fNovaTargetYOffset->SetDefaultValue(0.);  // in mm
        fNovaTargetYOffset->AvailableForStates(G4State_PreInit, G4State_Idle);

        fNovaTargetExtraFlangeThick = new G4UIcmdWithADoubleAndUnit("/NuMI/det/NovaTargetExtraFlangeThick",this);
        fNovaTargetExtraFlangeThick->SetGuidance(
              " Add a bit of material downstream of the flange to represent the bolts holding the flange ");
        fNovaTargetExtraFlangeThick->SetParameterName("fNovaTargetExtraFlangeThick",true); // in Detector data
        fNovaTargetExtraFlangeThick->SetUnitCategory("Length");
        fNovaTargetExtraFlangeThick->SetDefaultValue(0.);  // in mm
        fNovaTargetExtraFlangeThick->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHorn1StripLinesThick = new G4UIcmdWithADoubleAndUnit("/NuMI/det/Horn1StripLinesThick",this);
        fHorn1StripLinesThick->SetGuidance(
              " Add a bit of material downstream of the flange to represent the strip lines connection plates. ");
        fHorn1StripLinesThick->SetParameterName("fHorn1StripLinesThick",true); // in Detector data
        fHorn1StripLinesThick->SetUnitCategory("Length");
        fHorn1StripLinesThick->SetDefaultValue(0.);  // in mm
        fHorn1StripLinesThick->AvailableForStates(G4State_PreInit, G4State_Idle);
        //
        // Flag to allow for the correction of the bug in coordinate systems in NumiMagneticField
        //
        fUsePosLocalCoordInMagField = new G4UIcmdWithABool("/NuMI/det/UsePosLocalCoordInMagField",this);
        fUsePosLocalCoordInMagField->SetGuidance(
         "Default is true. We correct a bug in the coordinate transform. Transverse Position in local coord. is used ");
        fUsePosLocalCoordInMagField->SetParameterName("UsePosLocalCoordInMagField", true);
        fUsePosLocalCoordInMagField->SetDefaultValue(true);
        fUsePosLocalCoordInMagField->AvailableForStates(G4State_PreInit, G4State_Idle);

        fUseRotLocalCoordInMagField = new G4UIcmdWithABool("/NuMI/det/UseRotLocalCoordInMagField",this);
        fUseRotLocalCoordInMagField->SetGuidance(
         "Default is true. We correct a bug in the coordinate transform. Transverse Position in local coord. is used ");
        fUseRotLocalCoordInMagField->SetParameterName("UseRotLocalCoordInMagField", true);
        fUseRotLocalCoordInMagField->SetDefaultValue(true);
        fUseRotLocalCoordInMagField->AvailableForStates(G4State_PreInit, G4State_Idle);

        //New cards for beam optimization (Leo, July 13, 2018)
        fNumberOfMEFins = new G4UIcmdWithAnInteger("/NuMI/det/NumberOfFins",this);
        fNumberOfMEFins-> SetGuidance("Set the Number of find for the ME target.");
        fNumberOfMEFins-> SetParameterName("NumberOfFins",false);
        fNumberOfMEFins->SetDefaultValue(48);
        fNumberOfMEFins->AvailableForStates(G4State_PreInit,G4State_Idle);

        fDistanceBetweenMEFins = new G4UIcmdWithADoubleAndUnit("/NuMI/det/DistanceBetweenFins",this);
        fDistanceBetweenMEFins->SetGuidance("Set the distance between");
        fDistanceBetweenMEFins->SetParameterName("DistanceBetweenFins",true); // in Detector data
        fDistanceBetweenMEFins->SetUnitCategory("Length");
        fDistanceBetweenMEFins->SetDefaultValue(0.5);  // in mm
        fDistanceBetweenMEFins->AvailableForStates(G4State_PreInit, G4State_Idle);

        fBudalMonitorMEPosition = new G4UIcmdWithADoubleAndUnit("/NuMI/det/BudalMonitorMEPosition",this);
        fBudalMonitorMEPosition->SetGuidance("Set position of the front of the first BM");
        fBudalMonitorMEPosition->SetParameterName("BudalMonitorMEPosition",true);
        fBudalMonitorMEPosition->SetUnitCategory("Length");
        fBudalMonitorMEPosition->SetDefaultValue(0.);    //check this!!
        fBudalMonitorMEPosition->AvailableForStates(G4State_PreInit, G4State_Idle);

        fWidthMEFin = new G4UIcmdWithADoubleAndUnit("/NuMI/det/WidthMEFin",this);
        fWidthMEFin->SetGuidance("Set fin width");
        fWidthMEFin->SetParameterName("WidthMEFin",true);
        fWidthMEFin->SetUnitCategory("Length");
        fWidthMEFin->SetDefaultValue(7.4);    //mm
        fWidthMEFin->AvailableForStates(G4State_PreInit, G4State_Idle);

        fWingedFin1 = new G4UIcmdWithAnInteger("/NuMI/det/WingedFin1",this);
        fWingedFin1-> SetGuidance("Set the first fin as winged.");
        fWingedFin1-> SetParameterName("WingedFin1",false);
        fWingedFin1->SetDefaultValue(0);
        fWingedFin1->AvailableForStates(G4State_PreInit,G4State_Idle);

        fWingedFin2 = new G4UIcmdWithAnInteger("/NuMI/det/WingedFin2",this);
        fWingedFin2-> SetGuidance("Set the second fin as winged.");
        fWingedFin2-> SetParameterName("WingedFin2",false);
        fWingedFin2->SetDefaultValue(1);
        fWingedFin2->AvailableForStates(G4State_PreInit,G4State_Idle);

        fWingedFin3 = new G4UIcmdWithAnInteger("/NuMI/det/WingedFin3",this);
        fWingedFin3-> SetGuidance("Set the third fin as winged.");
        fWingedFin3-> SetParameterName("WingedFin3",false);
        fWingedFin3->SetDefaultValue(2);
        fWingedFin3->AvailableForStates(G4State_PreInit,G4State_Idle);

        fWingedFin4 = new G4UIcmdWithAnInteger("/NuMI/det/WingedFin4",this);
        fWingedFin4-> SetGuidance("Set the forth fin as winged.");
        fWingedFin4-> SetParameterName("WingedFin4",false);
        fWingedFin4->SetDefaultValue(3);
        fWingedFin4->AvailableForStates(G4State_PreInit,G4State_Idle);

        fWingedFinRadius = new G4UIcmdWithADoubleAndUnit("/NuMI/det/WingedFinRadius",this);
        fWingedFinRadius-> SetGuidance("Set the winged radius.");
        fWingedFinRadius-> SetParameterName("WingedFinRadius",false);
        fWingedFinRadius->SetDefaultValue(1);
        fWingedFinRadius->AvailableForStates(G4State_PreInit,G4State_Idle);


        fGDMLOutputCmd = new G4UIcmdWithAString("/NuMI/output/writeGDML",this);
        fGDMLOutputCmd->SetGuidance("Write GDML file");
        fGDMLOutputCmd->SetParameterName("choice",false);
        fGDMLOutputCmd->AvailableForStates(G4State_Init,G4State_Idle);

        fGDMLStoreRefCmd = new G4UIcmdWithABool("/NuMI/output/GDMLref",this);
        fGDMLStoreRefCmd->SetGuidance("GDML file will have ptr references for uniqueness");
        fGDMLStoreRefCmd->SetParameterName("choice",false);
        fGDMLStoreRefCmd->AvailableForStates(G4State_Init,G4State_Idle);
        fGDMLStoreReferences = false;


}

NumiDetectorMessenger::~NumiDetectorMessenger() {

        delete TargetGasCmd;
        //delete TargetZ0Cmd;
        //delete HornCurrentCmd;
        delete ConstructSolidMuMons;
        delete RunPeriod;
        delete BeamConfig;
        delete AbsorberConfig;
        delete Mon0AbsMatCmd;
        delete Mon1AbsMatCmd;
        delete Mon2AbsMatCmd;
        delete Mon0AbsThickCmd;
        delete Mon1AbsThickCmd;
        delete Mon2AbsThickCmd;
        delete Mon0AbsDistCmd;
        delete Mon1AbsDistCmd;
        delete Mon2AbsDistCmd;
        delete UpdateCmd;
        delete LengthOfWaterInTgt;

        delete detDir;
        delete NumiDir;

        delete fBeamConfigDirectory;
        delete fDuratekShiftCmd;
        delete fTHBlockShiftCmd;
        delete fDeltaOuterThicknessCmd;
        delete fBafflePositionCmd;
        delete fTargetPositionCmd;
        delete fHorn1PositionCmd;
        delete fHorn2PositionCmd;
        delete fHorn1RotationCmd;
        delete fHorn2RotationCmd;
        delete fBaffleOuterRadiusCmd;
        delete fBaffleInnerRadiusCmd;
        delete fBaffleLengthCmd;
        delete fForcedOldTargetCmd;

        delete fHornWaterLayerThick;
        delete fHorn1IsAlternate;
        delete fHorn1IsRefined;
        delete fHorn1UpstrIOisTorus;
        delete fHorn1ExtraLayerAlum;
        delete fDumpBFieldPlease;
        delete fIgnoreCEQBr;
        delete fHorn1FieldZCutUpstream;
        delete fHorn1FieldZCutDwnstream;
        delete fHorn1CurrentEqualizerLongAbsLength;
        delete fHorn1CurrentEqualizerQuadAmpl;
        delete fHorn1CurrentEqualizerOctAmpl;
        delete fHorn2CurrentEqualizerLongAbsLength;
        delete fHorn2CurrentEqualizerQuadAmpl;
        delete fHorn2CurrentEqualizerOctAmpl;

        delete fNovaTargetHTilt;
        delete fNovaTargetVTilt;
        delete fNovaTargetXOffset;
        delete fNovaTargetYOffset;
        delete fNovaTargetExtraFlangeThick;
        delete fHorn1StripLinesThick;

        delete fUsePosLocalCoordInMagField;
        delete fUseRotLocalCoordInMagField;

         //New cards for beam optimization (Leo, July 13, 2018)
        delete fNumberOfMEFins;
        delete fDistanceBetweenMEFins;
        delete fBudalMonitorMEPosition;
        delete fWidthMEFin;
        delete fWingedFin1;
        delete fWingedFin2;
        delete fWingedFin3;
        delete fWingedFin4;
        delete fWingedFinRadius;


        delete fGDMLOutputCmd;
        delete fGDMLStoreRefCmd;


}


void NumiDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

   NumiDataInput *NumiData = NumiDataInput::GetNumiDataInput();

   if(NumiData->fPrintInfo > 0 || NumiData->IsDebugOn())
   {
      G4cout << "NumiDetectorMessenger::SetNewValue - Setting Parameter value from input macro." << G4endl;
   }


   if( command == RunPeriod )          { NumiData->SetRunPeriod(RunPeriod->GetNewIntValue(newValue)); }
   if( command == BeamConfig )         {
       std::transform(newValue.begin(), newValue.end(), newValue.begin(),
                      std::ptr_fun<int,int>(std::tolower));

       NumiData->SetBeamConfig(newValue);
       NumiDetector->SetBeamType(newValue);
   }
   if( command == UseCorrHornCurrent)  { NumiData->SetUseCorrHornCurrent(UseCorrHornCurrent->GetNewBoolValue(newValue)); }
   if( command == LengthOfWaterInTgt)  { NumiData->SetLengthOfWaterInTgt(LengthOfWaterInTgt->GetNewDoubleValue(newValue));}

   if( command == UseHornMisalign)	{ NumiData->SetHornMisalign(UseHornMisalign->GetNewBoolValue(newValue)); }
   if( command == UseTgtDensity)        { NumiData->SetTgtDensity(UseTgtDensity->GetNewBoolValue(newValue)); }

   if ( command == TargetGasCmd ) {
      //NumiDetector->SetTargetGas(newValue);
   }
   //if ( command == TargetZ0Cmd ) {
   //   NumiDetector->SetTargetZ0(TargetZ0Cmd->GetNewDoubleValue(newValue));
   //}
   //if ( command == HornCurrentCmd ) {
   //   NumiDetector->SetHornCurrent(HornCurrentCmd->GetNewDoubleValue(newValue));
   //}
   if ( command == ConstructTarget ) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->constructTarget=ConstructTarget->GetNewBoolValue(newValue);
   }
   if ( command == HeInDecayPipe ) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->HeInDecayPipe=HeInDecayPipe->GetNewBoolValue(newValue);
   }

   if ( command == applyDecayPipeMagneticField ) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->applyDecayPipeMagneticField=applyDecayPipeMagneticField->GetNewBoolValue(newValue);
   }


   if ( command == ConstructSolidMuMons ) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->SetSolidMuMons(ConstructSolidMuMons->GetNewBoolValue(newValue));
   }
   if( command == AbsorberConfig )
   {
      NumiDetector->SetAbsorberConfig(newValue);
   }

   if( command == Mon0AbsMatCmd )
   {
      NumiDetector->SetMonAbsorberMaterial(newValue, 0);
   }
   if( command == Mon1AbsMatCmd )
   {
           NumiDetector->SetMonAbsorberMaterial(newValue, 1);
   }
   if( command == Mon2AbsMatCmd )
   {
      NumiDetector->SetMonAbsorberMaterial(newValue, 2);
   }
   if( command == Mon0AbsThickCmd )
   {
      NumiDetector->SetMonAbsorberThickness(Mon0AbsThickCmd->GetNewDoubleValue(newValue), 0);
   }
   if( command == Mon1AbsThickCmd )
   {
      NumiDetector->SetMonAbsorberThickness(Mon1AbsThickCmd->GetNewDoubleValue(newValue), 1);
   }
   if( command == Mon2AbsThickCmd )
   {
      NumiDetector->SetMonAbsorberThickness(Mon2AbsThickCmd->GetNewDoubleValue(newValue), 2);
   }
   if( command == Mon0AbsDistCmd )
   {
      NumiDetector->SetAbsorberDistFromMon(Mon0AbsDistCmd->GetNewDoubleValue(newValue), 0);
   }
   if( command == Mon1AbsDistCmd )
   {
      NumiDetector->SetAbsorberDistFromMon(Mon1AbsDistCmd->GetNewDoubleValue(newValue), 1);
   }
   if( command == Mon2AbsDistCmd )
   {
      NumiDetector->SetAbsorberDistFromMon(Mon2AbsDistCmd->GetNewDoubleValue(newValue), 2);
   }

   if ( command == UpdateCmd ) {
#ifndef FLUGG
      std::cerr << " Updating the geometry, i.e., calling NumiDetector construction again.. " << std::endl;
      NumiDetector->UpdateGeometry();
#endif
   }

   if (command == fDuratekShiftCmd) {
       G4double shift = fDuratekShiftCmd->GetNewDoubleValue(newValue);
       NumiDetector->SetDuratekShift(shift);

   } else if (command == fTHBlockShiftCmd) {
       G4double delta = fTHBlockShiftCmd->GetNewDoubleValue(newValue);
       NumiDetector->SetBlockShift(delta);

   } else if (command == fDeltaOuterThicknessCmd) {
       double delta = fDeltaOuterThicknessCmd->GetNewDoubleValue(newValue);
       NumiDetector->SetDeltaOuterThickness(delta);

   } else if (command == fBafflePositionCmd) {
       G4ThreeVector bafflePos = fBafflePositionCmd->GetNew3VectorValue(newValue);
       NumiDetector->SetBafflePosition(bafflePos);

   } else if (command == fTargetPositionCmd) {
       G4ThreeVector targetPos = fTargetPositionCmd->GetNew3VectorValue(newValue);
       NumiDetector->SetTargetPosition(targetPos);

   } else if (command == fHorn1PositionCmd) {
       G4ThreeVector horn1Pos = fHorn1PositionCmd->GetNew3VectorValue(newValue);
       NumiDetector->SetHorn1Position(horn1Pos);

   } else if (command == fHorn2PositionCmd) {
       G4ThreeVector horn2Pos = fHorn2PositionCmd->GetNew3VectorValue(newValue);
       NumiDetector->SetHorn2Position(horn2Pos);

   } else if (command == fHorn1RotationCmd) {
       G4ThreeVector horn1Rot = fHorn1RotationCmd->GetNew3VectorValue(newValue);
       NumiDetector->SetHorn1Rotation(horn1Rot);

   } else if (command == fHorn2RotationCmd) {
       G4ThreeVector horn2Rot = fHorn2RotationCmd->GetNew3VectorValue(newValue);
       NumiDetector->SetHorn2Rotation(horn2Rot);

   } else if (command == fBaffleOuterRadiusCmd) {
       double Rout = fBaffleOuterRadiusCmd->GetNewDoubleValue(newValue);
       NumiDetector->SetBaffleOuterRadius(Rout);

   } else if (command == fBaffleInnerRadiusCmd) {
       double Rin = fBaffleInnerRadiusCmd->GetNewDoubleValue(newValue);
       NumiDetector->SetBaffleInnerRadius(Rin);

   } else if (command == fBaffleLengthCmd) {
       double length = fBaffleLengthCmd->GetNewDoubleValue(newValue);
       NumiDetector->SetBaffleLength(length);

   } else if (command == fForcedOldTargetCmd) {
       G4bool forced = fForcedOldTargetCmd->GetNewBoolValue(newValue);
       NumiDetector->SetForcedOldTarget(forced);

   }  else if (command == fHornWaterLayerThick) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->SetHornWaterLayerThick(fHornWaterLayerThick->GetNewDoubleValue(newValue));
   }  else if (command == fHorn1IsAlternate) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->SetHorn1IsAlternate(fHorn1IsAlternate->GetNewBoolValue(newValue));
      std::cerr << " Setting Alternate Horn1 " << std::endl;
   }  else if (command == fHorn1IsRefined) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->SetHorn1IsRefined(fHorn1IsAlternate->GetNewBoolValue(newValue));
      std::cerr << " Using refined precision for  Horn1 " << std::endl;
   }  else if (command == fHorn1UpstrIOisTorus) {
       NumiDetector->SetHorn1UpstrIOisTorus(fHorn1UpstrIOisTorus->GetNewBoolValue(newValue));
      std::cerr << " Using The Torus version of the I/O transition of Horn1 " << std::endl;
   }  else if (command == fDumpBFieldPlease) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->SetDumpBFieldPlease(fDumpBFieldPlease->GetNewBoolValue(newValue));
      std::cerr << " Setting SetDumpBFieldPlease " << std::endl;
   }  else if (command == fHorn1ExtraLayerAlum) {
      NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
      NumiData->SetHorn1ExtraLayerAlum(fHorn1ExtraLayerAlum->GetNewDoubleValue(newValue));
   }  else if (command == fIgnoreCEQBr) {
       const bool tt = fIgnoreCEQBr->GetNewBoolValue(newValue);
       NumiDetector->SetIgnoreCEQBr(tt);
       if (tt) std::cerr << " Ignoring the radial component of the CEQ field map   " << std::endl;
       else std::cerr << " Full 2D map. Please use the file pointed by env CEQMAPFILE   " << std::endl;
   }  else if (command == fHorn1FieldZCutUpstream) {
       NumiDetector->SetHorn1FieldZCutUpstream(fHorn1FieldZCutUpstream->GetNewDoubleValue(newValue));
   }  else if (command == fHorn1FieldZCutDwnstream) {
       NumiDetector->SetHorn1FieldZCutDwnstream(fHorn1FieldZCutDwnstream->GetNewDoubleValue(newValue));
   }  else if (command == fHorn1CurrentEqualizerLongAbsLength) {
       NumiDetector->SetHorn1CurrentEqualizerLongAbsLength(fHorn1CurrentEqualizerLongAbsLength->GetNewDoubleValue(newValue));
   }  else if (command == fHorn1CurrentEqualizerQuadAmpl) {
       NumiDetector->SetHorn1CurrentEqualizerQuadAmpl(fHorn1CurrentEqualizerQuadAmpl->GetNewDoubleValue(newValue));
   }  else if (command == fHorn1CurrentEqualizerOctAmpl) {
       NumiDetector->SetHorn1CurrentEqualizerOctAmpl(fHorn1CurrentEqualizerOctAmpl->GetNewDoubleValue(newValue));
   }  else if (command == fHorn2CurrentEqualizerLongAbsLength) {
       NumiDetector->SetHorn2CurrentEqualizerLongAbsLength(fHorn2CurrentEqualizerLongAbsLength->GetNewDoubleValue(newValue));
   }  else if (command == fHorn2CurrentEqualizerQuadAmpl) {
       NumiDetector->SetHorn2CurrentEqualizerQuadAmpl(fHorn2CurrentEqualizerQuadAmpl->GetNewDoubleValue(newValue));
   }  else if (command == fHorn2CurrentEqualizerOctAmpl) {
       NumiDetector->SetHorn2CurrentEqualizerOctAmpl(fHorn2CurrentEqualizerOctAmpl->GetNewDoubleValue(newValue));
   }  else if (command == fNovaTargetHTilt) {
       NumiDetector->SetNovaTargetHTilt(fNovaTargetHTilt->GetNewDoubleValue(newValue));
   }  else if (command == fNovaTargetVTilt) {
       NumiDetector->SetNovaTargetVTilt(fNovaTargetVTilt->GetNewDoubleValue(newValue));
   }  else if (command == fNovaTargetXOffset) {
       NumiDetector->SetNovaTargetXOffset(fNovaTargetXOffset->GetNewDoubleValue(newValue));
   }  else if (command == fNovaTargetYOffset) {
       NumiDetector->SetNovaTargetYOffset(fNovaTargetYOffset->GetNewDoubleValue(newValue));
   }  else if (command == fNovaTargetExtraFlangeThick) {
       NumiDetector->SetNovaTargetExtraFlangeThick(fNovaTargetExtraFlangeThick->GetNewDoubleValue(newValue));
   }  else if (command == fHorn1StripLinesThick) {
       NumiDetector->SetHorn1StripLinesThick(fHorn1StripLinesThick->GetNewDoubleValue(newValue));
   }  else if (command == fUsePosLocalCoordInMagField) {
       NumiData->usePosLocalCoordInMagField = fUsePosLocalCoordInMagField->GetNewBoolValue(newValue);
       if (NumiData->usePosLocalCoordInMagField) std::cerr << " Bug in coord. transform Postion, fixed" << std::endl;
       else std::cerr << " Bug in coord. transform Postion, NOT fixed" << std::endl;
   }  else if (command == fUseRotLocalCoordInMagField) {
       NumiData->useRotLocalCoordInMagField = fUseRotLocalCoordInMagField->GetNewBoolValue(newValue);
       if (NumiData->useRotLocalCoordInMagField)
         std::cerr << " Bug in coord. transform field rotations, fixed" << std::endl;
       else std::cerr << " Bug in coord. transform field rotations, NOT fixed" << std::endl;
   } else if (command == fNumberOfMEFins) {
     //New cards for beam optimization (Leo, July 13, 2018)
     NumiData->SetNumberOfMEFins(fNumberOfMEFins->GetNewIntValue(newValue));
   } else if( command == fDistanceBetweenMEFins)  {
     NumiData->SetTargetSegPitch(fDistanceBetweenMEFins->GetNewDoubleValue(newValue));
   }else if( command == fBudalMonitorMEPosition)  {
     NumiData->SetBudalMonitorMEPosition(fBudalMonitorMEPosition->GetNewDoubleValue(newValue));
   }else if( command == fWidthMEFin)  {
     NumiData->SetTargetSegWidth(fWidthMEFin->GetNewDoubleValue(newValue));
   }else if (command == fWingedFin1) {
     NumiData->SetWingedFin1(fWingedFin1->GetNewIntValue(newValue));
   }else if (command == fWingedFin2) {
     NumiData->SetWingedFin2(fWingedFin2->GetNewIntValue(newValue));
   }else if (command == fWingedFin3) {
     NumiData->SetWingedFin3(fWingedFin3->GetNewIntValue(newValue));
   }else if (command == fWingedFin4) {
     NumiData->SetWingedFin4(fWingedFin4->GetNewIntValue(newValue));
   }else if (command == fWingedFinRadius) {
     NumiData->SetWingedFinRadius(fWingedFinRadius->GetNewDoubleValue(newValue));
   } 
   else {

//       std::cerr << " In messenger, Unknown Cmd " << (void *) (command) << " and I will quit " << std::endl;
//      exit(2);
  }


   if ( command == fGDMLStoreRefCmd ) {
     fGDMLStoreReferences = fGDMLStoreRefCmd->GetNewBoolValue(newValue);
   }
   if ( command == fGDMLOutputCmd ) {
     G4String outgdml = newValue;
     if ( outgdml != "" ) {
       G4cout << "%%% write output GDML " << outgdml << G4endl;
       G4VPhysicalVolume* pvol = 0;
       G4GDMLParser gdml_parser;
       // fGDMLStoreReferences: use ptr addresses to make name unique
       gdml_parser.Write(outgdml,pvol,fGDMLStoreReferences);
     }
   }

   return;


}
