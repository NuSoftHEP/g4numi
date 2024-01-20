//----------------------------------------------------------------------
//NumiMagneticField 
// $Id
//----------------------------------------------------------------------
#include <stdlib.h>
#include "NumiMagneticField.hh"
#include "G4ios.hh"
#include "G4TransportationManager.hh"
#include "NumiDataInput.hh" 
#include "G4VPhysicalVolume.hh"
#include "G4RunManager.hh"
#include "NumiTrackingAction.hh"
#include "NumiDataInput.hh"
#include "NumiDetectorConstruction.hh"

//magnetic field between conductors ====================================================
NumiMagneticField::NumiMagneticField():
      fHorn1FieldZCutUpstream(-32.0),
      fHorn1FieldZCutDwnstream(3277.),
      fHorn1CurrentEqualizerLongAbsLength(0.),
      fHorn1CurrentEqualizerQuadAmpl(0.),
      fHorn1CurrentEqualizerOctAmpl(0.),
      fHorn2CurrentEqualizerLongAbsLength(0.),
      fHorn2CurrentEqualizerQuadAmpl(0.),
      fHorn2CurrentEqualizerOctAmpl(0.)
{
  NumiData=NumiDataInput::GetNumiDataInput();
  dumpHasBeenDump = false; fHorn1IsTilted=false; fHorn2IsTilted=false;
  //
  // temporary debugging... To be commented out when done.. Avoiding a G4UI card that
  //  must defined at detector construction time.. 
  //
  fDebugIsOn = false;
  fEffectiveLengthHorn1 = 3150.; // valid only for Horn1, where we apply the Current Equalizer section. 
  fEffectiveLengthHorn1 = 142.91*25.4; // Horn2 length of conductors.  
  fOuterRadiusHorn1 = 11.5*25.4;
  fOuterRadiusHorn2 = (29.134*25.4)/2.;  // Drawing 363384
  fField3DMapCurrentEqualizerRadSqAtZ.clear();
  fIgnoreCEQBr = false;
  fEffectiveLengthHorn2 = 3427.88; // See method NumiDetectorConstruction::ConstructHorn2 
  fField3DMapCurrentEqualizerZOffsetH2  = 10007.5; // default position of Horn2, le config. 
  fHorn2FieldZCutDwnstream = fField3DMapCurrentEqualizerZOffsetH2 + fEffectiveLengthHorn2; 
}
void NumiMagneticField::rotateHorns() {
  //
  // Matrix rotation for the field itself. 
  if ((std::abs(NumiData->Horn1Phi) > 1.0e-6) || 
      (std::abs(NumiData->Horn1Theta) > 1.0e-6) ||
      (std::abs(NumiData->Horn1Psi) > 1.0e-6)) {
        fHorn1IsTilted = true;
	fRotMatrixHorn1Container = G4RotationMatrix(NumiData->Horn1Phi, NumiData->Horn1Theta, NumiData->Horn1Psi);
	fRotMatrixHorn1Inverse = fRotMatrixHorn1Container.inverse();
   }
  if ((std::abs(NumiData->Horn2Phi) > 1.0e-6) || 
      (std::abs(NumiData->Horn2Theta) > 1.0e-6) ||
      (std::abs(NumiData->Horn2Psi) > 1.0e-6)) {
        fHorn2IsTilted = true;
	fRotMatrixHorn2Container = G4RotationMatrix(NumiData->Horn2Phi, NumiData->Horn2Theta, NumiData->Horn2Psi);
	fRotMatrixHorn2Inverse = fRotMatrixHorn2Container.inverse();
   }
}

NumiMagneticField::~NumiMagneticField(){;}

void NumiMagneticField::GetFieldValue(const double PointGlobal[3],double *Bfield) const
{

  static bool first = true;
  double Point[3];
  for (int kkk=0; kkk != 3; kkk++) Point[kkk] = PointGlobal[kkk];
  // Approximate coordinate transformation, in case of rotation and/or translation. 
  /* found a bit too crummy 
  if (fHorn1IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] < 4500.) ) { 
    CLHEP::Hep3Vector PtTmp(PointGlobal[0], PointGlobal[1], (PointGlobal[2] - NumiData->Horn1Z0 + 1715.65));
      CLHEP::Hep3Vector PtLocal = fRotMatrixHorn1Inverse(PtTmp);// To Local Coordinate system
//      CLHEP::Hep3Vector PtLocal = fRotMatrixHorn1Container(PtTmp);// To Correct (almost!) Local Coordinate system
      Point[0] = PtLocal.x(); Point[1] = PtLocal.y();
  }
  if (fHorn2IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] > 4500.) ) { 
    CLHEP::Hep3Vector PtTmp(PointGlobal[0], PointGlobal[1], (PointGlobal[2] - NumiData->Horn2Z0 + 1803.5));
      CLHEP::Hep3Vector PtLocal = fRotMatrixHorn2Container(PtTmp);// To Local Coordinate system
      Point[0] = PtLocal.x(); Point[1] = PtLocal.y();
  }
  if (NumiData->usePosLocalCoordInMagField) { 
    Point[0] -= NumiData->Horn1X0; // To Local Coordinate system 
    Point[1] -= NumiData->Horn1Y0;
  }
  */
  //
  // We should use the Geant4 utilities for this.. 
  //
  bool doTransform = (fHorn1IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] < 4500.)) ||
                     (fHorn2IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] > 4500.)) || 
		     (NumiData->usePosLocalCoordInMagField && 
		            ((std::abs(NumiData->Horn1X0) > 1.0e-6) || (std::abs(NumiData->Horn1X0) > 1.0e-6)));		       
  if (doTransform) {
    G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
    G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
    G4ThreeVector Position=G4ThreeVector(PointGlobal[0],PointGlobal[1],PointGlobal[2]);
    //G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
    G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
    G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position); 
    delete numinavigator;
//    std::cerr << " G4Transform, at Z = " << PointGlobal[2] << "  X= " << localPosition[0] << " approx " << Point[0] 
//               << " Y = " << localPosition[1] << " approx " << Point[1] << std::endl;
    Point[0] = localPosition[0];
    Point[1] = localPosition[1];
   
  }
  
  G4double current = NumiData->HornCurrent/ampere/1000.;
  
  //G4cout << "NumiData->HornCurrent = " << NumiData->HornCurrent/ampere << " A" << G4endl;
  //G4cout << "current = " << current << G4endl;

//  std::cerr  << "  NumiMagneticField::GetFieldValue, Point " 
//             << Point[0] << " / " << Point[1] << " / " << Point[2] << G4endl;
//  std::cerr  << "NumiData->HornCurrent = " << NumiData->HornCurrent/ampere << " A" << G4endl;
//  std::cerr  << "current = " << current << G4endl;

  
  G4double radius = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);    
  G4double B = current / (5.*radius/cm)/10*tesla;  //B(kG)=i(kA)/[5*r(cm)], 1T=10kG
  //
  // Check that we are indeed in the field region.. 
  //
  if (Point[2] < 4500.) {
    const G4RunManager* pRunMgr = G4RunManager::GetRunManager();
    const NumiDetectorConstruction* pDet = reinterpret_cast<const NumiDetectorConstruction*>(pRunMgr->GetUserDetectorConstruction());
    const double ric = pDet->PHorn1ICRin((Point[2] - 30.)); // in Drawing coordinate system, MCZero = 30 mm
    if (radius < ric) B=0.;  
  }
  // Should do the same for Horn2, if need be!!! 
  
  //
  //  Avoid leakage downstream the end.., for Horn1.  Paul November 23 2014. 
  //  
  if ((Point[2] > fHorn1FieldZCutDwnstream) && (Point[2] < 4500.)) B = 0.;
  //
  //  Unknown field fpr Z < 0.  Paul Lebrun, November 24 2014. 
  // Upgrade to a variable, to study the impact of the integral of BdL in the I/O region. 
  //
  if (Point[2] < fHorn1FieldZCutUpstream ) B = 0.;
  if ((std::abs(fHorn1CurrentEqualizerLongAbsLength) > 0.1) && 
      (Point[2] > (fHorn1FieldZCutDwnstream - 3.0*fHorn1CurrentEqualizerLongAbsLength)) && 
      (Point[2] < fHorn1FieldZCutDwnstream)) {
       if (fIgnoreCEQBr) { 
         const double expFactor = (fHorn1FieldZCutDwnstream - Point[2])/fHorn1CurrentEqualizerLongAbsLength;
         const double phi = std::atan2(Point[1], Point[0]) + M_PI/4.;
         const double phiFact = fHorn1CurrentEqualizerQuadAmpl*std::sin(4.0*phi - M_PI/2.) + 
			   fHorn1CurrentEqualizerOctAmpl*std::sin(8.0*phi - M_PI/2.);
          const double corrEQ =  ( 1.0 + phiFact * std::exp(-1.*expFactor));  		   
//    std::cerr << " expFactor " <<  expFactor << " phi " << phi << " phiFact " 
//       << phiFact << " corrEQ " << corrEQ << std::endl;
// 
// The above does not has the radial component of the field. 
// 
        // From field map.. 
	B *= corrEQ;
	// Chasing the overwrite bug: enhance the field by 10, and we will comment out the loopers 
	// suppression in SteppingAction. 
//	B *= 10.;  // does not work, does not produce enough neutrinos, I guess... 
     }  else { 
       double bFieldFromMap[3];
	fDebugIsOn = false;
        this->getFieldFrom3DMapCurrentEqualizer(&Point[0], bFieldFromMap);
        Bfield[0] = bFieldFromMap[0];
        Bfield[1] = bFieldFromMap[1];
        Bfield[2] = 0.; // still 2D 
        //
	if (NumiData->useRotLocalCoordInMagField && fHorn1IsTilted) this->rotateHorn1Field(Bfield);
        return; // No Correction for skin depth effect.  Will be included in the map, if need be...  
      }
  }
  if ((std::abs(fHorn2CurrentEqualizerLongAbsLength) > 0.1) && 
      (Point[2] > fField3DMapCurrentEqualizerZOffsetH2) &&   
      ((Point[2] - fField3DMapCurrentEqualizerZOffsetH2) > 
          (fEffectiveLengthHorn2 - 3.0*fHorn2CurrentEqualizerLongAbsLength))) {
       double bFieldFromMap[3];
	fDebugIsOn = false;
        this->getFieldFrom3DMapCurrentEqualizerH2(&Point[0], bFieldFromMap);
        Bfield[0] = bFieldFromMap[0];
        Bfield[1] = bFieldFromMap[1];
        Bfield[2] = 0.; // still 2D
        // 
	if (NumiData->useRotLocalCoordInMagField && fHorn2IsTilted) this->rotateHorn2Field(Bfield);
        return; // No Correction for skin depth effect.  Will be included in the map, if need be...  
      
  } 
    
  Bfield[0] = -B*Point[1]/radius;
  Bfield[1] = B*Point[0]/radius;
  Bfield[2] = 0.;
  if (NumiData->useRotLocalCoordInMagField && fHorn1IsTilted && (Point[2] < 4500.)) this->rotateHorn1Field(Bfield);
  if (NumiData->useRotLocalCoordInMagField && fHorn2IsTilted && (Point[2] > 4500.)) this->rotateHorn2Field(Bfield);

  if (!dumpHasBeenDump && NumiData->GetDumpBFieldPlease()) {
     dumpHasBeenDump= true;
//    std::cerr << " NumiMagneticField::NumiMagneticField set up to dumpt field, but quit here " << std::endl; exit(2);
    G4String aNameD("/scratch/minerva/lebrun/G4/BFieldDumpMainTmp");
    if (NumiData->GetHorn1IsAlternate()) aNameD += G4String("Alt");
    else aNameD += G4String("Nom");
    aNameD += G4String(".txt");
    fSteppingStream.open(aNameD.c_str());
    fSteppingStream << " id x y z Bx By Bz " << std::endl;
//    double currNow = NumiData->HornCurrent * 1000.;
//    NumiData->SetHornCurrent(currNow);
//    std::cerr << " Horn Current has been multiplied by 1 e3 for Muon Geantino use, Hor Current is now  " 
//                 << NumiData->HornCurrent << std::endl;
  } else {
//    std::cerr << " NumiMagneticField::NumiMagneticField NOT set up to dumpt field, but quit here " << std::endl; exit(2);
  
  }
  if(NumiData->jCompare) {// Make gnumi like horns - this is for validation
    G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
    G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
    G4ThreeVector Position=G4ThreeVector(Point[0],Point[1],Point[2]);
    //G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
    G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
    G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position);
    
    delete numinavigator;
    
    
    if(localPosition.z()>3*m || localPosition.z()<0*m)  {
      if (first) {
        G4cout << "Applying jCompare in NumiMagneticField." << G4endl;
        first = false;
      }
      Bfield[0]=0;
      Bfield[1]=0;
      Bfield[2]=0;
    }     
  }
  if (fSteppingStream.is_open()) {
    const G4RunManager * pRunManager = G4RunManager::GetRunManager();
    fSteppingStream << " " << pRunManager->GetCurrentEvent()->GetEventID();
    fSteppingStream << " " << Point[0] << " " << Point[1] << " " << Point[2];
    fSteppingStream << " " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] << std::endl;;
  }
}
void NumiMagneticField::getFieldFrom3DMapCurrentEqualizer(
                              const double Point[3], double *Bfield) const {
  
  if (fField3DMapCurrentEqualizerRadSqAtZ.size() == 0) 
    this->fill3DMapCurrentEqualizer();
  
  if (fDebugIsOn) std::cerr << " NumiMagneticField::getFieldFrom3DMapCurrentEqualizer xyz  " 
                            << Point[0] << " / " << Point[1] << " / " <<  Point[2] << std::endl;
  for (int k=0; k !=3; k++) Bfield[k] = 0.;
  if (fField3DMapCurrentEqualizer.size() == 0) return; // should not happen
  if (Point[2] < fField3DMapCurrentEqualizerZMin) return;
  const unsigned int iZ1 = 
    static_cast<unsigned int>(std::floor((Point[2] - fField3DMapCurrentEqualizerZMin)/fField3DMapCurrentEqualizerZStep));
  if( iZ1 >= fNumZPtsForCE) return;
  const unsigned int iZ2 = (iZ1 == (fNumZPtsForCE-1)) ? iZ1 : (iZ1 + 1); 
  if(fDebugIsOn) std::cerr << " ..... Z " << Point[2] << " indices " << iZ1 << " / " <<  iZ2 << std::endl;
  double phi = std::atan2(Point[1], Point[0]); 
  if (phi < 0.) phi += 2.0*M_PI;
  const unsigned int iQuadrant = (unsigned int) std::floor(2.0*phi/M_PI);
  const double phiFirst = phi - iQuadrant*M_PI/2.;
  const unsigned int iPhi1 = 
    static_cast<unsigned int>(phiFirst/fField3DMapCurrentEqualizerPhiStep);
  const unsigned int iPhi2 = (iPhi1 == (fNumPhiPtsForCE-1)) ? 0 : (iPhi1 + 1); 
    
  if (fDebugIsOn) std::cerr << " ..... phi " << phi << " phiFirst " << phiFirst 
                            << " indices " << iPhi1 << " / " <<  iPhi2 << std::endl;
  const double rSqRequested = Point[0]*Point[0] + Point[1]*Point[1];
  const double rSqZ1 =  rSqRequested - fField3DMapCurrentEqualizerRadSqAtZ[iZ1]; 
  const double rSqZ2 =  rSqRequested - fField3DMapCurrentEqualizerRadSqAtZ[iZ2]; 
  if ((rSqZ1 < 0.) && (rSqZ2 < 0.)) return;
  double radOCAtZ = 0.; 
  const unsigned int iR1Z1 = (rSqZ1 < 0.) ? 0 : std::floor(rSqZ1 /fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ1]);
  const unsigned int iR2Z1 = (rSqZ1 < 0.) ? 0 : ((iR1Z1 == (fNumRPtsForCE-1)) ? iR1Z1 : (iR1Z1 + 1));   
  const unsigned int iR1Z2 = (rSqZ2 < 0.) ? 0 : std::floor(rSqZ2 /fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ2]);
  const unsigned int iR2Z2 = (rSqZ2 < 0.) ? 0 : ((iR1Z2 == (fNumRPtsForCE-1)) ? iR1Z2 : (iR1Z2 + 1)); 
  if ((iR1Z1 >=  fNumRPtsForCE) || (iR1Z2 >=  fNumRPtsForCE)) return;
  // bilinear interposlation on the phiXRsq plane, the linear interpolation on Z 
  if (fDebugIsOn) std::cerr << " ..... RsQ, Z1 " << rSqZ1 << " at Z2 " << rSqZ2 << " indices " 
                            << iR1Z1 << " / " <<  iR2Z1 << " / " << iR1Z2 << " / " <<  iR2Z2 << std::endl;
  
  const unsigned int ii1 = fNumZPtsForCE*(fNumRPtsForCE*iPhi1 + iR1Z1) + iZ1;
  const unsigned int ii2 = fNumZPtsForCE*(fNumRPtsForCE*iPhi1 + iR2Z1) + iZ1;
  const unsigned int ii3 = fNumZPtsForCE*(fNumRPtsForCE*iPhi2 + iR2Z1) + iZ1;
  const unsigned int ii4 = fNumZPtsForCE*(fNumRPtsForCE*iPhi2 + iR1Z1) + iZ1;
  const double ui = (iPhi2 == 0) ?  (phiFirst - (fNumPhiPtsForCE - 1)* 
                                   fField3DMapCurrentEqualizerPhiStep)/ fField3DMapCurrentEqualizerPhiStep : 
                     (phiFirst - iPhi1*fField3DMapCurrentEqualizerPhiStep)/fField3DMapCurrentEqualizerPhiStep;
  		    
  if (fDebugIsOn) std::cerr << " ..... ii indices " << ii1 << " / " << ii2 << " / " <<  ii3 << " / " << ii4 
            << " ui " << ui << std::endl;
  double bxIz1 = 0.; double byIz1 = 0.;
  // Same at the point iZ2.. 
  //
  const unsigned int jj1 = fNumZPtsForCE*(fNumRPtsForCE*iPhi1 + iR1Z2) + iZ2;
  const unsigned int jj2 = fNumZPtsForCE*(fNumRPtsForCE*iPhi1 + iR2Z2) + iZ2;
  const unsigned int jj3 = fNumZPtsForCE*(fNumRPtsForCE*iPhi2 + iR2Z2) + iZ2;
  const unsigned int jj4 = fNumZPtsForCE*(fNumRPtsForCE*iPhi2 + iR1Z2) + iZ2;
  double bxIz2 = 0.; double byIz2 = 0.;
  if (((iR2Z1 == iR1Z1) || (iR2Z2 == iR1Z2)) && (iR1Z1 != 0) &&
       (iR1Z2 != 0) && (iR2Z1 != 0) && (iR2Z2 != 0) ) { // only do the interpolation in phi . Far away from the IC, 
                                             // does not matter which interpolation table section we use.. Field too small.
    std::map<unsigned int, std::pair<float, float> >::const_iterator itm1 = 
         fField3DMapCurrentEqualizer.find(ii1);
     std::map<unsigned int, std::pair<float, float> >::const_iterator itm4 = 
         fField3DMapCurrentEqualizer.find(ii4);
     const double bix1 = static_cast<double>(itm1->second.first);
     const double biy1 = static_cast<double>(itm1->second.second);     
     const double bix4 = static_cast<double>(itm4->second.first);
     const double biy4 = static_cast<double>(itm4->second.second);
     bxIz1 = (1. - ui)*bix1 + ui*bix4;
     byIz1 = (1. - ui)*biy1 + ui*biy4;
     if (iZ1 == iZ2) {
       Bfield[0] = bxIz1;
       Bfield[1] = byIz1;
     } else { 
       std::map<unsigned int, std::pair<float, float> >::const_iterator jtm1 = 
       fField3DMapCurrentEqualizer.find(jj1);
       std::map<unsigned int, std::pair<float, float> >::const_iterator jtm4 = 
       fField3DMapCurrentEqualizer.find(jj4);
       const double bjx1 = static_cast<double>(jtm1->second.first);
       const double bjy1 = static_cast<double>(jtm1->second.second);     
       const double bjx4 = static_cast<double>(jtm4->second.first);
       const double bjy4 = static_cast<double>(jtm4->second.second);
       bxIz2 = (1. - ui)*bjx1 + ui*bjx4;
       byIz2 = (1. - ui)*bjy1 + ui*bjy4;
       const double v = 
          (Point[2] - fField3DMapCurrentEqualizerZMin 
	            - iZ1*fField3DMapCurrentEqualizerZStep)/fField3DMapCurrentEqualizerZStep;
       Bfield[0] = (1.0 - v)*bxIz1 + v*bxIz2;
       Bfield[1] = (1.0 - v)*byIz1 + v*byIz2;
     }
     if (fDebugIsOn) std::cerr << " ....Phi/Z interpolation only.  " 
                              << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
   } else {
     if (rSqZ1 < 0.) {
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm1 = 
          fField3DMapCurrentEqualizer.find(jj1);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm2 = 
          fField3DMapCurrentEqualizer.find(jj2);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm3 = 
           fField3DMapCurrentEqualizer.find(jj3);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm4 = 
           fField3DMapCurrentEqualizer.find(jj4);
         const double bjx1 = static_cast<double>(jtm1->second.first);
         const double bjy1 = static_cast<double>(jtm1->second.second);     
         const double bjx2 = static_cast<double>(jtm2->second.first);
         const double bjy2 = static_cast<double>(jtm2->second.second);     
         const double bjx3 = static_cast<double>(jtm3->second.first);
         const double bjy3 = static_cast<double>(jtm3->second.second);     
         const double bjx4 = static_cast<double>(jtm4->second.first);
         const double bjy4 = static_cast<double>(jtm4->second.second); 
         const double tj = 
	  std::max(std::abs((rSqZ2 - radOCAtZ*radOCAtZ)/fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ2]), 1.); // Approximate!. 
         bxIz2 = (1. - ui)*(1.0 - tj)*bjx1 + (1.0 - ui)*tj*bjx2 + ui*tj*bjx3 + (1.0 - tj)*ui*bjx4 ;
         byIz2 = (1. - ui)*(1.0 - tj)*bjy1 + (1.0 - ui)*tj*bjy2 + ui*tj*bjy3 + (1.0 - tj)*ui*bjy4 ;
         if (fDebugIsOn) {
           std::cerr << " tj " << tj << " bjxs " << bjx1 << " / " << bjx2 << " / " << bjx3 << " / " << bjx4 << std::endl;
           std::cerr << " bjys " << bjy1 << " / " << bjy2 << " / " << bjy3 << " / " << bjy4 << std::endl;
           std::cerr << " bxIz2 " << bxIz2 << " byIz2 " << byIz2 << std::endl;
         }
         const double v = 
          (Point[2] - fField3DMapCurrentEqualizerZMin 
	            - iZ1*fField3DMapCurrentEqualizerZStep)/fField3DMapCurrentEqualizerZStep;
         Bfield[0] =  v*bxIz2;
         Bfield[1] =  v*byIz2; 
         if (fDebugIsOn) std::cerr << " ....    v " << v << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
     } else if (rSqZ2 < 0.) { 
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm1 = 
         fField3DMapCurrentEqualizer.find(ii1);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm2 = 
         fField3DMapCurrentEqualizer.find(ii2);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm3 = 
         fField3DMapCurrentEqualizer.find(ii3);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm4 = 
         fField3DMapCurrentEqualizer.find(ii4);
       const double bix1 = static_cast<double>(itm1->second.first);
       const double biy1 = static_cast<double>(itm1->second.second);     
       const double bix2 = static_cast<double>(itm2->second.first);
       const double biy2 = static_cast<double>(itm2->second.second);     
       const double bix3 = static_cast<double>(itm3->second.first);
       const double biy3 = static_cast<double>(itm3->second.second);     
       const double bix4 = static_cast<double>(itm4->second.first);
       const double biy4 = static_cast<double>(itm4->second.second);
       const double ti =  
        std::max(std::abs((rSqZ1 - radOCAtZ*radOCAtZ)/fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ1]) , 1.);
       bxIz1 = (1. - ui)*(1.0 - ti)*bix1 + (1.0 - ui)*ti*bix2 + ui*ti*bix3 + (1.0 - ti)*ui*bix4 ;
       byIz1 = (1. - ui)*(1.0 - ti)*biy1 + (1.0 - ui)*ti*biy2 + ui*ti*biy3 + (1.0 - ti)*ui*biy4 ;
       if (fDebugIsOn) { 
         std::cerr << " ui " << ui << " ti " << ti << std::endl;
         std::cerr << " bixs " << bix1 << " / " << bix2 << " / " << bix3 << " / " << bix4 << std::endl;
         std::cerr << " biys " << biy1 << " / " << biy2 << " / " << biy3 << " / " << biy4 << std::endl;
         std::cerr << " bxIz1 " << bxIz1 << " byIz1 " << byIz1 << std::endl;
       }
         const double v = 
          (Point[2] - fField3DMapCurrentEqualizerZMin 
	            - iZ1*fField3DMapCurrentEqualizerZStep)/fField3DMapCurrentEqualizerZStep;
       Bfield[0] = (1.0 - v)*bxIz1;
       Bfield[1] = (1.0 - v)*byIz1; 
       if (fDebugIsOn) std::cerr << " ....    v " << v << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
     
     } else {
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm1 = 
         fField3DMapCurrentEqualizer.find(ii1);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm2 = 
         fField3DMapCurrentEqualizer.find(ii2);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm3 = 
         fField3DMapCurrentEqualizer.find(ii3);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm4 = 
         fField3DMapCurrentEqualizer.find(ii4);
       const double bix1 = static_cast<double>(itm1->second.first);
       const double biy1 = static_cast<double>(itm1->second.second);     
       const double bix2 = static_cast<double>(itm2->second.first);
       const double biy2 = static_cast<double>(itm2->second.second);     
       const double bix3 = static_cast<double>(itm3->second.first);
       const double biy3 = static_cast<double>(itm3->second.second);     
       const double bix4 = static_cast<double>(itm4->second.first);
       const double biy4 = static_cast<double>(itm4->second.second);
       const double ti = 
        (rSqZ1 - iR1Z1*fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ1])/fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ1];
       bxIz1 = (1. - ui)*(1.0 - ti)*bix1 + (1.0 - ui)*ti*bix2 + ui*ti*bix3 + (1.0 - ti)*ui*bix4 ;
       byIz1 = (1. - ui)*(1.0 - ti)*biy1 + (1.0 - ui)*ti*biy2 + ui*ti*biy3 + (1.0 - ti)*ui*biy4 ;
       if (fDebugIsOn) { 
         std::cerr << " ui " << ui << " ti " << ti << std::endl;
         std::cerr << " bixs " << bix1 << " / " << bix2 << " / " << bix3 << " / " << bix4 << std::endl;
         std::cerr << " biys " << biy1 << " / " << biy2 << " / " << biy3 << " / " << biy4 << std::endl;
         std::cerr << " bxIz1 " << bxIz1 << " byIz1 " << byIz1 << std::endl;
       }
       if (iZ1 == iZ2) {
         Bfield[0] = bxIz1;
         Bfield[1] = byIz1;
       } else { 
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm1 = 
          fField3DMapCurrentEqualizer.find(jj1);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm2 = 
          fField3DMapCurrentEqualizer.find(jj2);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm3 = 
           fField3DMapCurrentEqualizer.find(jj3);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm4 = 
           fField3DMapCurrentEqualizer.find(jj4);
         const double bjx1 = static_cast<double>(jtm1->second.first);
         const double bjy1 = static_cast<double>(jtm1->second.second);     
         const double bjx2 = static_cast<double>(jtm2->second.first);
         const double bjy2 = static_cast<double>(jtm2->second.second);     
         const double bjx3 = static_cast<double>(jtm3->second.first);
         const double bjy3 = static_cast<double>(jtm3->second.second);     
         const double bjx4 = static_cast<double>(jtm4->second.first);
         const double bjy4 = static_cast<double>(jtm4->second.second); 
         const double tj =
	  (rSqZ2 - iR1Z2*fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ1])/fField3DMapCurrentEqualizerRadSqRStepAtZ[iZ2];
         bxIz2 = (1. - ui)*(1.0 - tj)*bjx1 + (1.0 - ui)*tj*bjx2 + ui*tj*bjx3 + (1.0 - tj)*ui*bjx4 ;
         byIz2 = (1. - ui)*(1.0 - tj)*bjy1 + (1.0 - ui)*tj*bjy2 + ui*tj*bjy3 + (1.0 - tj)*ui*bjy4 ;
         if (fDebugIsOn) {
           std::cerr << " tj " << tj << " bjxs " << bjx1 << " / " << bjx2 << " / " << bjx3 << " / " << bjx4 << std::endl;
           std::cerr << " bjys " << bjy1 << " / " << bjy2 << " / " << bjy3 << " / " << bjy4 << std::endl;
           std::cerr << " bxIz2 " << bxIz2 << " byIz2 " << byIz2 << std::endl;
         }
         const double v = 
          (Point[2] - fField3DMapCurrentEqualizerZMin 
	            - iZ1*fField3DMapCurrentEqualizerZStep)/fField3DMapCurrentEqualizerZStep;
         Bfield[0] = (1.0 - v)*bxIz1 + v*bxIz2;
         Bfield[1] = (1.0 - v)*byIz1 + v*byIz2;
         if (fDebugIsOn) std::cerr << " ....    v " << v << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
       }
     } // both rSqZ1 and rSqZ2 positiv  
  }
  if (iQuadrant == 0) return;
  double bfQ[2]; bfQ[0] = Bfield[0]; bfQ[1] = Bfield[1];
  switch (iQuadrant ) {
    case 1:
       Bfield[0] = -bfQ[1]; Bfield[1] = bfQ[0]; break;
    case 2:
       Bfield[0] = -bfQ[0]; Bfield[1] = -bfQ[1]; break;
    case 3:
       Bfield[0] = bfQ[1]; Bfield[1] = -bfQ[0]; break;
  }     
  return;
}
void NumiMagneticField::fill3DMapCurrentEqualizer() const {
   //
   const char* ceqMapFileEnv = getenv("CEQMAPFILE");
   if (ceqMapFileEnv != 0) {
     this->readFieldMapCurrentEqualizer(ceqMapFileEnv);
     fDebugIsOn = false;
     return;
   }  
   fDebugIsOn = true;
   if (fDebugIsOn) std::cerr << " NumiMagneticField::fill3DMapCurrentEqualizer, starting .. " << std::endl;
   const G4double current = NumiData->HornCurrent/CLHEP::ampere/1000.;
   double magBField = current / (5./CLHEP::cm)/10*CLHEP::tesla; //B(kG)=i(kA)/[5*r(cm)], 1T=10kG 
  // The 1/r dependence is included in the map. 
   fField3DMapCurrentEqualizerRadSqAtZ.clear();
   const double AQuad = fHorn1CurrentEqualizerQuadAmpl;
   const double AOct = fHorn1CurrentEqualizerOctAmpl;
   fNumZPtsForCE = 200; // a bit arbitrary... 
   fNumRPtsForCE = 400;
   fNumPhiPtsForCE = 256;
   fField3DMapCurrentEqualizerZMin = fHorn1FieldZCutDwnstream - 3.0*fHorn1CurrentEqualizerLongAbsLength;
   if (fField3DMapCurrentEqualizerZMin < 0.) fField3DMapCurrentEqualizerZMin = 0.;
   fField3DMapCurrentEqualizerZStep = (3.0*fHorn1CurrentEqualizerLongAbsLength)/(fNumZPtsForCE-1);
   fField3DMapCurrentEqualizerPhiStep = (M_PI/2.)/(fNumPhiPtsForCE-1); // Only one quadrant.. 
   const int numWires = 5000; // Really arbitrary.. 
   const double phiWStep = 2.0*M_PI/numWires; // Over two pi, full sources. 
   for (unsigned int iZ = 0; iZ != fNumZPtsForCE; iZ++) {
//     std::cerr << " At Z index " << iZ << std::endl;
     const double z = fField3DMapCurrentEqualizerZMin + iZ * fField3DMapCurrentEqualizerZStep;
     const double radOC = this->fillHornPolygonRadii(z);
     fField3DMapCurrentEqualizerRadSqAtZ.push_back(radOC*radOC);
     double stepRadial = (fOuterRadiusHorn1*fOuterRadiusHorn1 - radOC*radOC)/(fNumRPtsForCE-1);
     fField3DMapCurrentEqualizerRadSqRStepAtZ.push_back(stepRadial);
     double zFactAzi = (fHorn1FieldZCutDwnstream - z)/fHorn1CurrentEqualizerLongAbsLength;
     const double factCE = std::exp(-1.0*std::abs(zFactAzi));
     if (fDebugIsOn) {
         std::cerr << " At z = " << z << " radOC " 
                               << radOC << " zFactAzi " <<  factCE << " " << magBField << std::endl;
         std::cerr << " Size of the map " << fField3DMapCurrentEqualizer.size() << std::endl;
      }
     for (unsigned int iR=0; iR != fNumRPtsForCE; iR++) {
     // Choose a quadratic spacing... 
       const double r = 1.0e-12 + sqrt(radOC*radOC + iR*stepRadial); // 1 microns to avoid numerics 
       for (unsigned int iPhi=0; iPhi != fNumPhiPtsForCE; iPhi++) {
         const double phi = iPhi*fField3DMapCurrentEqualizerPhiStep;
         const double xP = r * std::cos(phi);
	 const double yP = r * std::sin(phi);
	 double bX = 0.; double bY = 0.;
	 // Discretize the current density on the conductor. Normalize to the unform current density, 
	 //  
	 double sumCurrent = 0.;
         for (int iW=0;  iW != numWires; iW++) {
           const double phiW =  1.0e-4 + phiWStep*(iW-1);
	   const double xW = radOC*std::cos(phiW);
	   const double yW = radOC*std::sin(phiW);
           const double xWf = xP - xW;
           const double yWf = yP - yW;
           const double phiff = atan2(yWf, xWf);
           const double radff = std::sqrt(xWf*xWf + yWf*yWf);
	   // Bug fixed on Oct 2 2017, after plotting the current.. 
//           const double iDens = 1.0 + AQuad*factCE*std::abs(sin(2.0*phiW));
           double iDens = 1.0 + AQuad*factCE*sin(4.0*phiW - M_PI/2.) + 
	                              AOct*factCE*sin(8.0*phiW - M_PI/2.);
	   if (iDens < 0.) iDens = 0.;			      
	   sumCurrent += iDens;
	   const double iDensByR = iDens/radff;
	   bX -= iDensByR*std::sin(phiff);
	   bY += iDensByR*std::cos(phiff);
	   if (std::isnan(bX) || std::isnan(bY) || std::isinf(bX) || std::isinf(bY)) {
	     std::cerr << " Numerics problem, xP " << xP << " yP " << yP << " iW " 
	               << iW << " xWf " << xWf << " yWf " << yWf << std::endl;
		std::cerr << " And quit here ! " << std::endl;       
	     exit(2);       
	   }
	   // Add the magentic field produce by the return current... On the Outer Conductor.. 
	   //
	   const double xWo = fOuterRadiusHorn1*std::cos(phiW);
	   const double yWo = fOuterRadiusHorn1*std::sin(phiW);
           const double xWfo = xP - xWo;
           const double yWfo = yP - yWo;
           const double phiffo = atan2(yWfo, xWfo);
           const double radffo = std::sqrt(xWfo*xWfo + yWfo*yWfo);
	   const double iDensByRo = iDens/radffo;
	   bX -= -1.0*iDensByRo*std::sin(phiffo);
	   bY += -1.0*iDensByRo*std::cos(phiffo);
	   if (std::isnan(bX) || std::isnan(bY) || std::isinf(bX) || std::isinf(bY)) {
	     std::cerr << " Numerics problem, xP " << xP << " yP " << yP << " iW " 
	               << iW << " xWfo " << xWfo << " yWfo " << yWfo << std::endl;
		std::cerr << " And quit here ! " << std::endl;       
	     exit(2);       
	   }
	   
	 }
	 if (sumCurrent < 1.0e-10) { bX = 0.; bY = 0.; } else { 
	    bX *=magBField/sumCurrent; bY *= magBField/sumCurrent; 
	 }
	 unsigned int ii = fNumZPtsForCE*(fNumRPtsForCE*iPhi + iR) + iZ;
	 fField3DMapCurrentEqualizer[ii] =  
	   std::pair<float, float>(static_cast<float>(bX), static_cast<float>(bY));
       } // On phi;
    } // on R 
  } // on Z
  /*
   std::cerr << " ... size of final Field map .. " << fField3DMapCurrentEqualizer.size() << std::endl;//
   std::map<unsigned int, std::pair<float, float> >::const_iterator itCheck0 = 
     	fField3DMapCurrentEqualizer.find(84029);
   std::cerr << " Check map at location 84029 " << itCheck0->second.first << " / " << itCheck0->second.second << std::endl;
   std::map<unsigned int, std::pair<float, float> >::const_iterator itCheck1 = 
     	fField3DMapCurrentEqualizer.find(84009);
   std::cerr << " Check map at location 84009 " << itCheck1->second.first << " / " << itCheck1->second.second << std::endl;
   std::map<unsigned int, std::pair<float, float> >::const_iterator itCheck2 = 
     	fField3DMapCurrentEqualizer.find(86009);
   std::cerr << " Check map at location 86009 " << itCheck2->second.first << " / " << itCheck2->second.second << std::endl;
     	
  */	
   fDebugIsOn = false;
//   this->dumpField();
   //
   // Write the map out .. 
   //
   this->writeFieldMapCurrentEqualizer();
   this->dumpField(); 
   std::cerr << " And quit !. " << std::endl; exit(2);
   
}

void NumiMagneticField::writeFieldMapCurrentEqualizer() const {

   std::ostringstream fNameOutStrStr; 
   fNameOutStrStr << "./FieldMapCEQ_Horn_1"
                  << "_LAbs_" << static_cast<int>(fHorn1CurrentEqualizerLongAbsLength) 
		  << "_Quad_" << static_cast<int>(static_cast<int>(100.*fHorn1CurrentEqualizerQuadAmpl)) 
		  << "_Oct_" << static_cast<int>(static_cast<int>(100.*fHorn1CurrentEqualizerOctAmpl)) 
		  << "_NZ_" << fNumZPtsForCE << "_NR_" << fNumRPtsForCE 
		  << "_NP_" << fNumPhiPtsForCE << ".dat";
   std::string fNameOutStr(fNameOutStrStr.str()); 
   std::ofstream fOutDat (fNameOutStr.c_str(), std::ios::out | std::ios::binary);
   const char *pTmp1 = (const char*) &fHorn1CurrentEqualizerLongAbsLength;
   fOutDat.write(pTmp1, sizeof(double));
   const char *pTmp2 = (const char*) &fHorn1CurrentEqualizerQuadAmpl;
   fOutDat.write(pTmp2, sizeof(double));
   char *pTmp = (char*) &fNumZPtsForCE;
   fOutDat.write(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumRPtsForCE;
   fOutDat.write(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumPhiPtsForCE;
   fOutDat.write(pTmp, sizeof(unsigned int));
   size_t lTotMap =   fField3DMapCurrentEqualizer.size(); 		  
   pTmp = (char*) &lTotMap;
   fOutDat.write(pTmp, sizeof(size_t));   		  
   for (std::map<unsigned int, std::pair<float, float> >::const_iterator it = fField3DMapCurrentEqualizer.begin(); 
                     it != fField3DMapCurrentEqualizer.end(); it++) {
	unsigned int ii = it->first;
        pTmp = (char*) &ii;
        fOutDat.write(pTmp, sizeof(unsigned int));   		  
	float bb[2];
	bb[0] = it->second.first;
	bb[1] = it->second.second;
        pTmp = (char*) &bb[0];
        fOutDat.write(pTmp, 2*sizeof(float));   		  
   }
   fOutDat.close();
   std::cerr << " FieldMapCurrentEqualizer written .. " << std::endl;		     
//   this->dumpField();
//   std::cerr << " And quit !. " << std::endl; exit(2);
}

void NumiMagneticField::readFieldMapCurrentEqualizer(const char *fName) const {
   
   std::ifstream fInDat(fName, std::ios::in | std::ios::binary);
   if (!fInDat.is_open()) {
     std::string msg(" NumiMagneticField::readFieldMapCurrentEqualizer, could not open file ");
     msg +=  fName;
     G4Exception("NumiMagneticField::readFieldMapCurrentEqualizer", " ",  RunMustBeAborted, msg.c_str());		
   }
   char *pTmp1 = (char*) &fHorn1CurrentEqualizerLongAbsLength;
   fInDat.read(pTmp1, sizeof(double));
   char *pTmp2 = (char*) &fHorn1CurrentEqualizerQuadAmpl;
   fInDat.read(pTmp2, sizeof(double));
   char *pTmp = (char*) &fNumZPtsForCE;
   fInDat.read(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumRPtsForCE;
   fInDat.read(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumPhiPtsForCE;
   fInDat.read(pTmp, sizeof(unsigned int));
   size_t lTotMap = 0;
   pTmp = (char*) &lTotMap;
   fInDat.read(pTmp, sizeof(size_t));
   std::cerr << " NumiMagneticField::readFieldMapCurrentEqualizer " 
             << std::endl << " ...... Size of map in file " 
             <<  lTotMap << " Grid " << fNumZPtsForCE << " /  " << fNumRPtsForCE << " / " << fNumPhiPtsForCE << std::endl;  		  
   for (size_t i=0; i!= lTotMap; i++) {
	unsigned int ii = 0;
        pTmp = (char*) &ii;
        fInDat.read(pTmp, sizeof(unsigned int));   		  
	float bb[2];
        pTmp = (char*) &bb[0];
        fInDat.read(pTmp, 2*sizeof(float)); 
	fField3DMapCurrentEqualizer[ii] = std::pair<float, float>(bb[0], bb[1]);  		  
   }
   fField3DMapCurrentEqualizerZMin = 0.;
   fField3DMapCurrentEqualizerZStep = fEffectiveLengthHorn1/(fNumZPtsForCE - 1);
   fField3DMapCurrentEqualizerPhiStep = (M_PI/2.)/(fNumPhiPtsForCE-1); // Only one quadrant.. 
   fField3DMapCurrentEqualizerRadSqRStepAtZ.clear();
   fField3DMapCurrentEqualizerRadSqAtZ.clear();
   for (unsigned int iZ = 0; iZ != fNumZPtsForCE; iZ++) {
//     std::cerr << " At Z index " << iZ << std::endl;
     const double z = fField3DMapCurrentEqualizerZMin + iZ * fField3DMapCurrentEqualizerZStep;
     double radOC = this->fillHornPolygonRadii(z);
     fField3DMapCurrentEqualizerRadSqAtZ.push_back(radOC*radOC);
     double stepRadial = (fOuterRadiusHorn1*fOuterRadiusHorn1 - radOC*radOC)/(fNumRPtsForCE-1);
     fField3DMapCurrentEqualizerRadSqRStepAtZ.push_back(stepRadial);
   }
   fInDat.close();
//   this->testDivergence(0);
//   this->testDivergence(1);
//   this->dumpField();
//   std::cerr << " And quit after reading and dumping the map..Test divergence as well  " << std::endl;
//   exit(2);
   
}
//
// Moronic clone for H2.  However, the coordinate system is no Horn configuration dependent. 
// The position of Horn2 changed.  
void NumiMagneticField::testDivergence(int opt) const {
//
// Test that it satisfy Maxwell equation, div B  = 0
//
// Since we compute in polar coordinate system, do it Cartesian.. 
//
   G4ThreeVector dir;
   double pos[3]; 
   pos[2] = 2800.; 
   double radIC = this->fillHornPolygonRadii(pos[2]);
   double radOC= radIC + 1.5*CLHEP::mm;;   
   std::string fNameOut;
   switch (opt) {
     case 0:
        dir = G4ThreeVector(std::sqrt(1.0-0.01), 0.1, 0.);
	pos[0] = 1.1*radOC; pos[1] = 0.1; 
	fNameOut=std::string("./g4numi_TestDivergence_X_v1.txt");
	break;
     case 1: 
        dir = G4ThreeVector(0.1, std::sqrt(1.0-0.01), 0.);
	pos[1] = 1.1*radOC; pos[0] = 0.1; 
	fNameOut=std::string("./g4numi_TestDivergence_Y_v1.txt");
	break;
      case 2:
        dir = G4ThreeVector(0.480729, sqrt(1.0 - (0.480729*0.480729)), 0.);
	pos[0] = 1.1*radIC/std::sqrt(2.); pos[1] = pos[0]; 
	fNameOut=std::string("./g4numi_TestDivergence_U_v1.txt");
	break;
      default: 
         std::cerr << "g4numi::TestDivergence Valid switches so far are 0, 1 or 2" << std::endl;
	 exit(1);
   }
   std::ofstream fOut(fNameOut.c_str());
   fOut << " x y r bx by b dbdx dbdy obnablab " << std::endl;
   const double step = 0.2*CLHEP::mm;
   double deltaMaxCart = 0.;
   const double range = 100.0; 
   const int numPts = static_cast<int> (range/step);
   std::cerr << " g4numi::TestDivergence, number of pts " 
             << numPts << " Z location for scan " << pos[2] << " radii " 
	     << radIC << " / " << fOuterRadiusHorn1 << std::endl;
   double bFieldPrev[3]; double bField[3]; double posPrevious[3];
   for (size_t kk=0; kk != 3; kk++) { posPrevious[kk] = pos[kk];  bFieldPrev[kk] = 0.; }
   double dbVal[3]; dbVal[2] = 0.; // assume Bz = 0.
   double bdlIntegral = 0.;
   for (int iStep = 0; iStep != numPts; iStep++) {
     this->GetFieldValue(pos, bField);
     const double bNorm = std::sqrt( bField[0]*bField[0] + bField[1]*bField[1]);
     
     if (bNorm < 1.0e-10) {
       for (size_t k=0; k!=3; k++) {
         bFieldPrev[k] = 0.;
         posPrevious[k] = pos[k];
         pos[k] += step*dir[k];
       }
        continue;
     }
     bdlIntegral += bField[0]*step*dir[1] - bField[1]*step*dir[0];
     const double phi = std::atan2(pos[1], pos[0]);
     if (iStep != 0) {
       dbVal[0] = std::cos(phi) * (bField[0] - bFieldPrev[0])/(pos[0] - posPrevious[0]);
       dbVal[1] = std::sin(phi) * (bField[1] - bFieldPrev[1])/(pos[1] - posPrevious[1]);
       const double dd = (1.0/bNorm)*(dbVal[0] + dbVal[1]);
       fOut << " " << pos[0] << " " << pos[1] << " " << std::sqrt(pos[0]*pos[0] + pos[1]*pos[1]) 
            << " " << bField[0]/CLHEP::tesla << " " << bField[1]/CLHEP::tesla 
            << " " << bNorm/CLHEP::tesla << " " 
	    << dbVal[0]/bNorm << " " << dbVal[1]/bNorm << " " << dd << std::endl;
       deltaMaxCart = std::max(std::abs(dd), deltaMaxCart);
     }
     for (size_t k=0; k!=3; k++) {
       bFieldPrev[k] = bField[k];
       posPrevious[k] = pos[k];
       pos[k] += step*dir[k];
     }
  }
  std::cerr << " g4numi::TestDivergence, final max deviation (rel) " 
            << deltaMaxCart  << " Integral bdl " << bdlIntegral/(CLHEP::tesla*CLHEP::meter) << std::endl;
  fOut.close();	    
//  std::cerr << " And quit for now " << std::endl; exit(2);

}
void NumiMagneticField::dumpField() const {
  
  std::ofstream fOut("./FieldMapHorn1CEQ_v1.txt");
  std::cerr << " NumiMagneticField::dumpField " << std::endl;
  fOut << " z r phi x y z bx by bz br bphi " << std::endl;
  for (size_t iz = 0; iz != 100; iz++) {
    const double z = 2100. + iz*10.;
    double aPt[3];
    aPt[2] = z;
    double aField[3]; 
    std::cerr << " ..... at Z = " << z << std::endl;
    for (size_t ir =0; ir != 100; ir++) {
      const double r = 50. + ir*2.5;
      for (size_t iPhi = 0.; iPhi != 50; iPhi++) {
        const double phi = (iPhi + 0.5)*M_PI/25.;
	aPt[0] = r*std::cos(phi);
	aPt[1] = r*std::sin(phi);
	this->GetFieldValue(aPt, aField);
	const double br = aField[0]*std::cos(phi) + aField[1]*std::sin(phi);
	const double bphi = -aField[0]*std::sin(phi) + aField[1]*std::cos(phi);
	fOut << " " << z << " " << r << " " << phi << " " << aPt[0] << " " << aPt[1] << " " << aPt[2] 
	     << " " << aField[0]/CLHEP::tesla << " " << aField[1]/CLHEP::tesla 
	     << " " << aField[2]/CLHEP::tesla << " " << br/CLHEP::tesla << " " << bphi/CLHEP::tesla << std::endl;
      } 
    } 
  }
  fOut.close();
 std::ofstream fOut2("./FieldMapHorn1CEQ_fineV1.txt");
  std::cerr << " NumiMagneticField::dumpField " << std::endl;
  fOut2 << " z r phi x y z bx by bz br bphi " << std::endl;
  for (size_t iz = 0; iz != 2; iz++) {
    const double z = 2100. + iz*800.;
    double aPt[3];
    aPt[2] = z;
    double aField[3]; 
    std::cerr << " ..... at Z = " << z << std::endl;
    for (size_t ir =0; ir != 200; ir++) {
      const double r = 50. + ir*2.5;
      for (size_t iPhi = 0.; iPhi != 200; iPhi++) {
        const double phi = (iPhi + 0.5)*M_PI/400.;
	aPt[0] = r*std::cos(phi);
	aPt[1] = r*std::sin(phi);
	this->GetFieldValue(aPt, aField);
	const double br = aField[0]*std::cos(phi) + aField[1]*std::sin(phi);
	const double bphi = -aField[0]*std::sin(phi) + aField[1]*std::cos(phi);
	fOut2 << " " << z << " " << r << " " << phi << " " << aPt[0] << " " << aPt[1] << " " << aPt[2] 
	     << " " << aField[0]/CLHEP::tesla << " " << aField[1]/CLHEP::tesla 
	     << " " << aField[2]/CLHEP::tesla << " " << br/CLHEP::tesla << " " << bphi/CLHEP::tesla << std::endl;
      } 
    } 
  }
  fOut2.close();
}
// Accurate field map CEQ distortions for Horn2.. 
void NumiMagneticField::getFieldFrom3DMapCurrentEqualizerH2(
                              const double Point[3], double *Bfield) const {
  
  if (fField3DMapCurrentEqualizerRadSqAtZH2.size() == 0) 
    this->fill3DMapCurrentEqualizerH2();
  const double zLocalH2 = Point[2] - fField3DMapCurrentEqualizerZOffsetH2;
  if (fDebugIsOn) {
      std::cerr << " NumiMagneticField::getFieldFrom3DMapCurrentEqualizerH2 xyz  " 
                << Point[0] << " / " << Point[1] << " / " << zLocalH2  << std::endl;
			    
      std::cerr << " Size of the map :  " << fField3DMapCurrentEqualizerH2.size() 
                << " ZMin " << fField3DMapCurrentEqualizerZMinH2 << std::endl;
  }
  for (int k=0; k !=3; k++) Bfield[k] = 0.;
  if (fField3DMapCurrentEqualizerH2.size() == 0) return; // should not happen
  if (zLocalH2 < fField3DMapCurrentEqualizerZMinH2) return; // should also not happen, see conditonal call above
  
  const unsigned int iZ1 = 
    static_cast<unsigned int>(std::floor((zLocalH2 - fField3DMapCurrentEqualizerZMinH2)/
        fField3DMapCurrentEqualizerZStepH2));
  if( iZ1 >= fNumZPtsForCEH2) return;
  const unsigned int iZ2 = (iZ1 == (fNumZPtsForCEH2-1)) ? iZ1 : (iZ1 + 1); 
  if(fDebugIsOn) std::cerr << " ..... Z " << zLocalH2 << " indices " << iZ1 << " / " <<  iZ2 << std::endl;
  double phi = std::atan2(Point[1], Point[0]); 
  if (phi < 0.) phi += 2.0*M_PI;
  const unsigned int iQuadrant = (unsigned int) std::floor(2.0*phi/M_PI);
  const double phiFirst = phi - iQuadrant*M_PI/2.;
  const unsigned int iPhi1 = 
    static_cast<unsigned int>(phiFirst/fField3DMapCurrentEqualizerPhiStepH2);
  const unsigned int iPhi2 = (iPhi1 == (fNumPhiPtsForCEH2-1)) ? 0 : (iPhi1 + 1); 
    
  if (fDebugIsOn) std::cerr << " ..... phi " << phi << " phiFirst " << phiFirst 
                            << " indices " << iPhi1 << " / " <<  iPhi2 << std::endl;
  const double rSqRequested = Point[0]*Point[0] + Point[1]*Point[1];
  const double rSqZ1 =  rSqRequested - fField3DMapCurrentEqualizerRadSqAtZH2[iZ1]; 
  const double rSqZ2 =  rSqRequested - fField3DMapCurrentEqualizerRadSqAtZH2[iZ2]; 
  if (fDebugIsOn) std::cerr << " R req " << rSqRequested << " ..... rSqZ1 " << rSqZ1 
                            << " rSqZ2 " << rSqZ2  << std::endl;
  if ((rSqZ1 < 0.) && (rSqZ2 < 0.)) return;
  double radOCAtZ = 0.; 
  const unsigned int iR1Z1 = (rSqZ1 < 0.) ? 0 : std::floor(rSqZ1 /fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ1]);
  const unsigned int iR2Z1 = (rSqZ1 < 0.) ? 0 : ((iR1Z1 == (fNumRPtsForCEH2-1)) ? iR1Z1 : (iR1Z1 + 1));   
  const unsigned int iR1Z2 = (rSqZ2 < 0.) ? 0 : std::floor(rSqZ2 /fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ2]);
  const unsigned int iR2Z2 = (rSqZ2 < 0.) ? 0 : ((iR1Z2 == (fNumRPtsForCEH2-1)) ? iR1Z2 : (iR1Z2 + 1)); 
  if ((iR1Z1 >=  fNumRPtsForCEH2) || (iR1Z2 >=  fNumRPtsForCEH2)) return;
  // bilinear interposlation on the phiXRsq plane, the linear interpolation on Z 
  if (fDebugIsOn) std::cerr << " ..... RsQ, Z1 " << rSqZ1 << " at Z2 " << rSqZ2 << " indices " 
                            << iR1Z1 << " / " <<  iR2Z1 << " / " << iR1Z2 << " / " <<  iR2Z2 << std::endl;
  
  const unsigned int ii1 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi1 + iR1Z1) + iZ1;
  const unsigned int ii2 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi1 + iR2Z1) + iZ1;
  const unsigned int ii3 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi2 + iR2Z1) + iZ1;
  const unsigned int ii4 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi2 + iR1Z1) + iZ1;
  const double ui = (iPhi2 == 0) ?  (phiFirst - (fNumPhiPtsForCEH2 - 1)* 
                                   fField3DMapCurrentEqualizerPhiStepH2)/ fField3DMapCurrentEqualizerPhiStepH2 : 
                     (phiFirst - iPhi1*fField3DMapCurrentEqualizerPhiStepH2)/fField3DMapCurrentEqualizerPhiStepH2;
  		    
  if (fDebugIsOn) std::cerr << " ..... ii indices " << ii1 << " / " << ii2 << " / " <<  ii3 << " / " << ii4 
            << " ui " << ui << std::endl;
  double bxIz1 = 0.; double byIz1 = 0.;
  // Same at the point iZ2.. 
  //
  const unsigned int jj1 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi1 + iR1Z2) + iZ2;
  const unsigned int jj2 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi1 + iR2Z2) + iZ2;
  const unsigned int jj3 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi2 + iR2Z2) + iZ2;
  const unsigned int jj4 = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi2 + iR1Z2) + iZ2;
  double bxIz2 = 0.; double byIz2 = 0.;
  if (((iR2Z1 == iR1Z1) || (iR2Z2 == iR1Z2)) && (iR1Z1 != 0) &&
       (iR1Z2 != 0) && (iR2Z1 != 0) && (iR2Z2 != 0) ) { // only do the interpolation in phi . Far away from the IC, 
                                             // does not matter which interpolation table section we use.. Field too small.
    std::map<unsigned int, std::pair<float, float> >::const_iterator itm1 = 
         fField3DMapCurrentEqualizerH2.find(ii1);
     std::map<unsigned int, std::pair<float, float> >::const_iterator itm4 = 
         fField3DMapCurrentEqualizerH2.find(ii4);
     const double bix1 = static_cast<double>(itm1->second.first);
     const double biy1 = static_cast<double>(itm1->second.second);     
     const double bix4 = static_cast<double>(itm4->second.first);
     const double biy4 = static_cast<double>(itm4->second.second);
     bxIz1 = (1. - ui)*bix1 + ui*bix4;
     byIz1 = (1. - ui)*biy1 + ui*biy4;
     if (iZ1 == iZ2) {
       Bfield[0] = bxIz1;
       Bfield[1] = byIz1;
     } else { 
       std::map<unsigned int, std::pair<float, float> >::const_iterator jtm1 = 
       fField3DMapCurrentEqualizerH2.find(jj1);
       std::map<unsigned int, std::pair<float, float> >::const_iterator jtm4 = 
       fField3DMapCurrentEqualizerH2.find(jj4);
       const double bjx1 = static_cast<double>(jtm1->second.first);
       const double bjy1 = static_cast<double>(jtm1->second.second);     
       const double bjx4 = static_cast<double>(jtm4->second.first);
       const double bjy4 = static_cast<double>(jtm4->second.second);
       bxIz2 = (1. - ui)*bjx1 + ui*bjx4;
       byIz2 = (1. - ui)*bjy1 + ui*bjy4;
       const double v = 
          (zLocalH2 - fField3DMapCurrentEqualizerZMinH2 
	            - iZ1*fField3DMapCurrentEqualizerZStepH2)/fField3DMapCurrentEqualizerZStepH2;
       Bfield[0] = (1.0 - v)*bxIz1 + v*bxIz2;
       Bfield[1] = (1.0 - v)*byIz1 + v*byIz2;
     }
     if (fDebugIsOn) std::cerr << " ....Phi/Z interpolation only.  " 
                              << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
   } else {
     if (rSqZ1 < 0.) {
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm1 = 
          fField3DMapCurrentEqualizerH2.find(jj1);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm2 = 
          fField3DMapCurrentEqualizerH2.find(jj2);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm3 = 
           fField3DMapCurrentEqualizerH2.find(jj3);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm4 = 
           fField3DMapCurrentEqualizerH2.find(jj4);
         const double bjx1 = static_cast<double>(jtm1->second.first);
         const double bjy1 = static_cast<double>(jtm1->second.second);     
         const double bjx2 = static_cast<double>(jtm2->second.first);
         const double bjy2 = static_cast<double>(jtm2->second.second);     
         const double bjx3 = static_cast<double>(jtm3->second.first);
         const double bjy3 = static_cast<double>(jtm3->second.second);     
         const double bjx4 = static_cast<double>(jtm4->second.first);
         const double bjy4 = static_cast<double>(jtm4->second.second); 
         const double tj = 
	  std::max(std::abs((rSqZ2 - radOCAtZ*radOCAtZ)/fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ2]), 1.); // Approximate!. 
         bxIz2 = (1. - ui)*(1.0 - tj)*bjx1 + (1.0 - ui)*tj*bjx2 + ui*tj*bjx3 + (1.0 - tj)*ui*bjx4 ;
         byIz2 = (1. - ui)*(1.0 - tj)*bjy1 + (1.0 - ui)*tj*bjy2 + ui*tj*bjy3 + (1.0 - tj)*ui*bjy4 ;
         if (fDebugIsOn) {
           std::cerr << " tj " << tj << " bjxs " << bjx1 << " / " << bjx2 << " / " << bjx3 << " / " << bjx4 << std::endl;
           std::cerr << " bjys " << bjy1 << " / " << bjy2 << " / " << bjy3 << " / " << bjy4 << std::endl;
           std::cerr << " bxIz2 " << bxIz2 << " byIz2 " << byIz2 << std::endl;
         }
         const double v = 
          (zLocalH2 - fField3DMapCurrentEqualizerZMinH2 
	            - iZ1*fField3DMapCurrentEqualizerZStepH2)/fField3DMapCurrentEqualizerZStepH2;
         Bfield[0] =  v*bxIz2;
         Bfield[1] =  v*byIz2; 
         if (fDebugIsOn) std::cerr << " ....    v " << v << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
     } else if (rSqZ2 < 0.) { 
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm1 = 
         fField3DMapCurrentEqualizerH2.find(ii1);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm2 = 
         fField3DMapCurrentEqualizerH2.find(ii2);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm3 = 
         fField3DMapCurrentEqualizerH2.find(ii3);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm4 = 
         fField3DMapCurrentEqualizerH2.find(ii4);
       const double bix1 = static_cast<double>(itm1->second.first);
       const double biy1 = static_cast<double>(itm1->second.second);     
       const double bix2 = static_cast<double>(itm2->second.first);
       const double biy2 = static_cast<double>(itm2->second.second);     
       const double bix3 = static_cast<double>(itm3->second.first);
       const double biy3 = static_cast<double>(itm3->second.second);     
       const double bix4 = static_cast<double>(itm4->second.first);
       const double biy4 = static_cast<double>(itm4->second.second);
       const double ti =  
        std::max(std::abs((rSqZ1 - radOCAtZ*radOCAtZ)/fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ1]) , 1.);
       bxIz1 = (1. - ui)*(1.0 - ti)*bix1 + (1.0 - ui)*ti*bix2 + ui*ti*bix3 + (1.0 - ti)*ui*bix4 ;
       byIz1 = (1. - ui)*(1.0 - ti)*biy1 + (1.0 - ui)*ti*biy2 + ui*ti*biy3 + (1.0 - ti)*ui*biy4 ;
       if (fDebugIsOn) { 
         std::cerr << " ui " << ui << " ti " << ti << std::endl;
         std::cerr << " bixs " << bix1 << " / " << bix2 << " / " << bix3 << " / " << bix4 << std::endl;
         std::cerr << " biys " << biy1 << " / " << biy2 << " / " << biy3 << " / " << biy4 << std::endl;
         std::cerr << " bxIz1 " << bxIz1 << " byIz1 " << byIz1 << std::endl;
       }
         const double v = 
          (zLocalH2 - fField3DMapCurrentEqualizerZMinH2 
	            - iZ1*fField3DMapCurrentEqualizerZStepH2)/fField3DMapCurrentEqualizerZStepH2;
       Bfield[0] = (1.0 - v)*bxIz1;
       Bfield[1] = (1.0 - v)*byIz1; 
       if (fDebugIsOn) std::cerr << " ....    v " << v << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
     
     } else {
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm1 = 
         fField3DMapCurrentEqualizerH2.find(ii1);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm2 = 
         fField3DMapCurrentEqualizerH2.find(ii2);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm3 = 
         fField3DMapCurrentEqualizerH2.find(ii3);
       std::map<unsigned int, std::pair<float, float> >::const_iterator itm4 = 
         fField3DMapCurrentEqualizerH2.find(ii4);
       const double bix1 = static_cast<double>(itm1->second.first);
       const double biy1 = static_cast<double>(itm1->second.second);     
       const double bix2 = static_cast<double>(itm2->second.first);
       const double biy2 = static_cast<double>(itm2->second.second);     
       const double bix3 = static_cast<double>(itm3->second.first);
       const double biy3 = static_cast<double>(itm3->second.second);     
       const double bix4 = static_cast<double>(itm4->second.first);
       const double biy4 = static_cast<double>(itm4->second.second);
       const double ti = 
        (rSqZ1 - iR1Z1*fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ1])/fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ1];
       bxIz1 = (1. - ui)*(1.0 - ti)*bix1 + (1.0 - ui)*ti*bix2 + ui*ti*bix3 + (1.0 - ti)*ui*bix4 ;
       byIz1 = (1. - ui)*(1.0 - ti)*biy1 + (1.0 - ui)*ti*biy2 + ui*ti*biy3 + (1.0 - ti)*ui*biy4 ;
       if (fDebugIsOn) { 
         std::cerr << " ui " << ui << " ti " << ti << std::endl;
         std::cerr << " bixs " << bix1 << " / " << bix2 << " / " << bix3 << " / " << bix4 << std::endl;
         std::cerr << " biys " << biy1 << " / " << biy2 << " / " << biy3 << " / " << biy4 << std::endl;
         std::cerr << " bxIz1 " << bxIz1 << " byIz1 " << byIz1 << std::endl;
       }
       if (iZ1 == iZ2) {
         Bfield[0] = bxIz1;
         Bfield[1] = byIz1;
       } else { 
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm1 = 
          fField3DMapCurrentEqualizerH2.find(jj1);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm2 = 
          fField3DMapCurrentEqualizerH2.find(jj2);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm3 = 
           fField3DMapCurrentEqualizerH2.find(jj3);
         std::map<unsigned int, std::pair<float, float> >::const_iterator jtm4 = 
           fField3DMapCurrentEqualizerH2.find(jj4);
         const double bjx1 = static_cast<double>(jtm1->second.first);
         const double bjy1 = static_cast<double>(jtm1->second.second);     
         const double bjx2 = static_cast<double>(jtm2->second.first);
         const double bjy2 = static_cast<double>(jtm2->second.second);     
         const double bjx3 = static_cast<double>(jtm3->second.first);
         const double bjy3 = static_cast<double>(jtm3->second.second);     
         const double bjx4 = static_cast<double>(jtm4->second.first);
         const double bjy4 = static_cast<double>(jtm4->second.second); 
         const double tj =
	  (rSqZ2 - iR1Z2*fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ1])/fField3DMapCurrentEqualizerRadSqRStepAtZH2[iZ2];
         bxIz2 = (1. - ui)*(1.0 - tj)*bjx1 + (1.0 - ui)*tj*bjx2 + ui*tj*bjx3 + (1.0 - tj)*ui*bjx4 ;
         byIz2 = (1. - ui)*(1.0 - tj)*bjy1 + (1.0 - ui)*tj*bjy2 + ui*tj*bjy3 + (1.0 - tj)*ui*bjy4 ;
         if (fDebugIsOn) {
           std::cerr << " tj " << tj << " bjxs " << bjx1 << " / " << bjx2 << " / " << bjx3 << " / " << bjx4 << std::endl;
           std::cerr << " bjys " << bjy1 << " / " << bjy2 << " / " << bjy3 << " / " << bjy4 << std::endl;
           std::cerr << " bxIz2 " << bxIz2 << " byIz2 " << byIz2 << std::endl;
         }
         const double v = 
          (zLocalH2 - fField3DMapCurrentEqualizerZMinH2 
	            - iZ1*fField3DMapCurrentEqualizerZStepH2)/fField3DMapCurrentEqualizerZStepH2;
         Bfield[0] = (1.0 - v)*bxIz1 + v*bxIz2;
         Bfield[1] = (1.0 - v)*byIz1 + v*byIz2;
         if (fDebugIsOn) std::cerr << " ....    v " << v << " Bfield " << Bfield[0] << " / " << Bfield[1] << std::endl;
       }
     } // both rSqZ1 and rSqZ2 positiv  
  }
  if (iQuadrant == 0) return;
  double bfQ[2]; bfQ[0] = Bfield[0]; bfQ[1] = Bfield[1];
  switch (iQuadrant ) {
    case 1:
       Bfield[0] = -bfQ[1]; Bfield[1] = bfQ[0]; break;
    case 2:
       Bfield[0] = -bfQ[0]; Bfield[1] = -bfQ[1]; break;
    case 3:
       Bfield[0] = bfQ[1]; Bfield[1] = -bfQ[0]; break;
  }     
  return;
}
void NumiMagneticField::fill3DMapCurrentEqualizerH2() const {
   //
   const char* ceqMapFileEnv = getenv("CEQMAPFILEH2");
   if (ceqMapFileEnv != 0) {
     this->readFieldMapCurrentEqualizerH2(ceqMapFileEnv);
     fDebugIsOn = false;
     return;
   }  
   fDebugIsOn = false;
   if (fDebugIsOn) std::cerr << " NumiMagneticField::fill3DMapCurrentEqualizer, starting .. " << std::endl;
   const G4double current = NumiData->HornCurrent/CLHEP::ampere/1000.;
   double magBField = current / (5./CLHEP::cm)/10*CLHEP::tesla; //B(kG)=i(kA)/[5*r(cm)], 1T=10kG 
  // The 1/r dependence is included in the map. 
   fField3DMapCurrentEqualizerRadSqAtZH2.clear();
   const double AQuad = fHorn2CurrentEqualizerQuadAmpl;
   const double AOct = fHorn2CurrentEqualizerOctAmpl;
   fNumZPtsForCEH2 = 200; // a bit arbitrary... 
   fNumRPtsForCEH2 = 400;
   fNumPhiPtsForCEH2 = 256;
   // Make first corase one.. 
   fField3DMapCurrentEqualizerZMinH2 = fEffectiveLengthHorn2 - 3.0*fHorn2CurrentEqualizerLongAbsLength;
   if (fField3DMapCurrentEqualizerZMinH2 < 0.) fField3DMapCurrentEqualizerZMinH2 = 0.;
   fField3DMapCurrentEqualizerZStepH2 = (3.0*fHorn2CurrentEqualizerLongAbsLength)/(fNumZPtsForCEH2-1);
   fField3DMapCurrentEqualizerPhiStepH2 = (M_PI/2.)/(fNumPhiPtsForCEH2-1); // Only one quadrant.. 
   const int numWires = 5000; // Really arbitrary.. 
   const double phiWStep = 2.0*M_PI/numWires; // Over two pi, full sources. 
   for (unsigned int iZ = 0; iZ != fNumZPtsForCEH2; iZ++) {
//     std::cerr << " At Z index " << iZ << std::endl;
     const double z = fField3DMapCurrentEqualizerZMinH2 + iZ * fField3DMapCurrentEqualizerZStepH2;
     const double radOC = this->fillHornPolygonRadiiH2(z);
     fField3DMapCurrentEqualizerRadSqAtZH2.push_back(radOC*radOC);
     double stepRadial = (fOuterRadiusHorn2*fOuterRadiusHorn2 - radOC*radOC)/(fNumRPtsForCEH2-1);
     fField3DMapCurrentEqualizerRadSqRStepAtZH2.push_back(stepRadial);
     double zFactAzi = (fEffectiveLengthHorn2 - z)/fHorn2CurrentEqualizerLongAbsLength;
     const double factCE = std::exp(-1.0*std::abs(zFactAzi));
     if (fDebugIsOn) {
         std::cerr << " At z = " << z << " radOC " 
                               << radOC << " zFactAzi " <<  factCE << " " << magBField << std::endl;
         std::cerr << " Size of the map " << fField3DMapCurrentEqualizer.size() << std::endl;
      }
     for (unsigned int iR=0; iR != fNumRPtsForCEH2; iR++) {
     // Choose a quadratic spacing... 
       const double r = 1.0e-12 + sqrt(radOC*radOC + iR*stepRadial); // 1 microns to avoid numerics 
       for (unsigned int iPhi=0; iPhi != fNumPhiPtsForCEH2; iPhi++) {
         const double phi = iPhi*fField3DMapCurrentEqualizerPhiStepH2;
         const double xP = r * std::cos(phi);
	 const double yP = r * std::sin(phi);
	 double bX = 0.; double bY = 0.;
	 // Discretize the current density on the conductor. Normalize to the unform current density, 
	 //  
	 double sumCurrent = 0.;
         for (int iW=0;  iW != numWires; iW++) {
           const double phiW =  1.0e-4 + phiWStep*(iW-1);
	   const double xW = radOC*std::cos(phiW);
	   const double yW = radOC*std::sin(phiW);
           const double xWf = xP - xW;
           const double yWf = yP - yW;
           const double phiff = atan2(yWf, xWf);
           const double radff = std::sqrt(xWf*xWf + yWf*yWf);
	   // Bug fixed on Oct 2 2017, after plotting the current.. 
//           const double iDens = 1.0 + AQuad*factCE*std::abs(sin(2.0*phiW));
           double iDens = 1.0 + AQuad*factCE*sin(4.0*phiW - M_PI/2.) + 
	                              AOct*factCE*sin(8.0*phiW - M_PI/2.);
	   if (iDens < 0.) iDens = 0.;			      
	   sumCurrent += iDens;
	   const double iDensByR = iDens/radff;
	   bX -= iDensByR*std::sin(phiff);
	   bY += iDensByR*std::cos(phiff);
	   if (std::isnan(bX) || std::isnan(bY) || std::isinf(bX) || std::isinf(bY)) {
	     std::cerr << " Numerics problem, xP " << xP << " yP " << yP << " iW " 
	               << iW << " xWf " << xWf << " yWf " << yWf << std::endl;
		std::cerr << " And quit here ! " << std::endl;       
	     exit(2);       
	   }
	   // Add the magentic field produce by the return current... On the Outer Conductor.. 
	   //
	   const double xWo = fOuterRadiusHorn2*std::cos(phiW);
	   const double yWo = fOuterRadiusHorn2*std::sin(phiW);
           const double xWfo = xP - xWo;
           const double yWfo = yP - yWo;
           const double phiffo = atan2(yWfo, xWfo);
           const double radffo = std::sqrt(xWfo*xWfo + yWfo*yWfo);
	   const double iDensByRo = iDens/radffo;
	   bX -= -1.0*iDensByRo*std::sin(phiffo);
	   bY += -1.0*iDensByRo*std::cos(phiffo);
	   if (std::isnan(bX) || std::isnan(bY) || std::isinf(bX) || std::isinf(bY)) {
	     std::cerr << " Numerics problem, xP " << xP << " yP " << yP << " iW " 
	               << iW << " xWfo " << xWfo << " yWfo " << yWfo << std::endl;
		std::cerr << " And quit here ! " << std::endl;       
	     exit(2);       
	   }
	   
	 }
	 if (sumCurrent < 1.0e-10) { bX = 0.; bY = 0.; } else { 
	    bX *=magBField/sumCurrent; bY *= magBField/sumCurrent; 
	 }
	 unsigned int ii = fNumZPtsForCEH2*(fNumRPtsForCEH2*iPhi + iR) + iZ;
	 fField3DMapCurrentEqualizerH2[ii] =  
	   std::pair<float, float>(static_cast<float>(bX), static_cast<float>(bY));
       } // On phi;
    } // on R 
  } // on Z
  /*
   std::cerr << " ... size of final Field map .. " << fField3DMapCurrentEqualizer.size() << std::endl;//
   std::map<unsigned int, std::pair<float, float> >::const_iterator itCheck0 = 
     	fField3DMapCurrentEqualizer.find(84029);
   std::cerr << " Check map at location 84029 " << itCheck0->second.first << " / " << itCheck0->second.second << std::endl;
   std::map<unsigned int, std::pair<float, float> >::const_iterator itCheck1 = 
     	fField3DMapCurrentEqualizer.find(84009);
   std::cerr << " Check map at location 84009 " << itCheck1->second.first << " / " << itCheck1->second.second << std::endl;
   std::map<unsigned int, std::pair<float, float> >::const_iterator itCheck2 = 
     	fField3DMapCurrentEqualizer.find(86009);
   std::cerr << " Check map at location 86009 " << itCheck2->second.first << " / " << itCheck2->second.second << std::endl;
     	
  */	
   fDebugIsOn = false;
//   this->dumpField();
   //
   // Write the map out .. 
   //
   this->writeFieldMapCurrentEqualizerH2();
   this->dumpField(); 
   std::cerr << " And quit !. " << std::endl; exit(2);
   
}

void NumiMagneticField::writeFieldMapCurrentEqualizerH2() const {

   std::ostringstream fNameOutStrStr; 
   fNameOutStrStr << "./FieldMapCEQ_Horn_2"
                  << "_LAbs_" << static_cast<int>(fHorn2CurrentEqualizerLongAbsLength) 
		  << "_Quad_" << static_cast<int>(static_cast<int>(100.*fHorn2CurrentEqualizerQuadAmpl)) 
		  << "_Oct_" << static_cast<int>(static_cast<int>(100.*fHorn2CurrentEqualizerOctAmpl)) 
		  << "_NZ_" << fNumZPtsForCEH2 << "_NR_" << fNumRPtsForCEH2 
		  << "_NP_" << fNumPhiPtsForCEH2 << ".dat";
   std::string fNameOutStr(fNameOutStrStr.str()); 
   std::ofstream fOutDat (fNameOutStr.c_str(), std::ios::out | std::ios::binary);
   const char *pTmp1 = (const char*) &fHorn2CurrentEqualizerLongAbsLength;
   fOutDat.write(pTmp1, sizeof(double));
   const char *pTmp2 = (const char*) &fHorn2CurrentEqualizerQuadAmpl;
   fOutDat.write(pTmp2, sizeof(double));
   char *pTmp = (char*) &fNumZPtsForCEH2;
   fOutDat.write(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumRPtsForCEH2;
   fOutDat.write(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumPhiPtsForCEH2;
   fOutDat.write(pTmp, sizeof(unsigned int));
   size_t lTotMap =   fField3DMapCurrentEqualizerH2.size(); 		  
   pTmp = (char*) &lTotMap;
   fOutDat.write(pTmp, sizeof(size_t));   		  
   for (std::map<unsigned int, std::pair<float, float> >::const_iterator it = fField3DMapCurrentEqualizerH2.begin(); 
                     it != fField3DMapCurrentEqualizerH2.end(); it++) {
	unsigned int ii = it->first;
        pTmp = (char*) &ii;
        fOutDat.write(pTmp, sizeof(unsigned int));   		  
	float bb[2];
	bb[0] = it->second.first;
	bb[1] = it->second.second;
        pTmp = (char*) &bb[0];
        fOutDat.write(pTmp, 2*sizeof(float));   		  
   }
   fOutDat.close();
   std::cerr << " FieldMapCurrentEqualizerH2 written .. " << std::endl;		     
   this->dumpFieldH2();
   std::cerr << " And quit !. " << std::endl; exit(2);
}

void NumiMagneticField::readFieldMapCurrentEqualizerH2(const char *fName) const {
   
   std::ifstream fInDat(fName, std::ios::in | std::ios::binary);
   if (!fInDat.is_open()) {
     std::string msg(" NumiMagneticField::readFieldMapCurrentEqualizerH2, could not open file ");
     msg +=  fName;
     G4Exception("NumiMagneticField::readFieldMapCurrentEqualizerH2", " ",  RunMustBeAborted, msg.c_str());		
   }
   char *pTmp1 = (char*) &fHorn2CurrentEqualizerLongAbsLength;
   fInDat.read(pTmp1, sizeof(double));
   char *pTmp2 = (char*) &fHorn2CurrentEqualizerQuadAmpl;
   fInDat.read(pTmp2, sizeof(double));
   char *pTmp = (char*) &fNumZPtsForCEH2;
   fInDat.read(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumRPtsForCEH2;
   fInDat.read(pTmp, sizeof(unsigned int));
   pTmp = (char*) &fNumPhiPtsForCEH2;
   fInDat.read(pTmp, sizeof(unsigned int));
   size_t lTotMap = 0;
   pTmp = (char*) &lTotMap;
   fInDat.read(pTmp, sizeof(size_t));
   std::cerr << " NumiMagneticField::readFieldMapCurrentEqualizerH2 " 
             << std::endl << " ...... Size of map in file " 
             <<  lTotMap << " Grid " << fNumZPtsForCEH2 << " /  " << fNumRPtsForCEH2 
	     << " / " << fNumPhiPtsForCEH2 << std::endl;  		  
   for (size_t i=0; i!= lTotMap; i++) {
	unsigned int ii = 0;
        pTmp = (char*) &ii;
        fInDat.read(pTmp, sizeof(unsigned int));   		  
	float bb[2];
        pTmp = (char*) &bb[0];
        fInDat.read(pTmp, 2*sizeof(float));
	fField3DMapCurrentEqualizerH2[ii] = std::pair<float, float>(bb[0], bb[1]);  		  
   }
   fField3DMapCurrentEqualizerZMinH2 = fEffectiveLengthHorn2 - 3.0*fHorn2CurrentEqualizerLongAbsLength;
   fField3DMapCurrentEqualizerZStepH2 = (3.0*fHorn2CurrentEqualizerLongAbsLength)/(fNumZPtsForCEH2-1);
   fField3DMapCurrentEqualizerPhiStepH2 = (M_PI/2.)/(fNumPhiPtsForCEH2-1); // Only one quadrant.. 
   fField3DMapCurrentEqualizerRadSqRStepAtZH2.clear();
   fField3DMapCurrentEqualizerRadSqAtZH2.clear();
   for (unsigned int iZ = 0; iZ != fNumZPtsForCEH2; iZ++) {
//     std::cerr << " At Z index " << iZ << std::endl;
     const double z = fField3DMapCurrentEqualizerZMinH2 + iZ * fField3DMapCurrentEqualizerZStepH2;
     double radOC = this->fillHornPolygonRadiiH2(z);
     fField3DMapCurrentEqualizerRadSqAtZH2.push_back(radOC*radOC);
     double stepRadial = (fOuterRadiusHorn2*fOuterRadiusHorn2 - radOC*radOC)/(fNumRPtsForCEH2-1);
     std::cerr << " At Z index " << iZ  << " .. radOC " << radOC << " " << " stepRadial " << stepRadial << std::endl;
     fField3DMapCurrentEqualizerRadSqRStepAtZH2.push_back(stepRadial);
  }
   fInDat.close();
//   this->testDivergence(0);
//   this->testDivergence(1);
//   this->dumpFieldH2();
//   std::cerr << " And quit after reading and dumping the H2 map..  " << std::endl;
//   exit(2);
   
} 

void NumiMagneticField::dumpFieldH2() const {
  
  std::ofstream fOut("./FieldMapHorn2CEQ_v1.txt");
  std::cerr << " NumiMagneticField::dumpFieldH2 " << std::endl;
  fOut << " z r phi x y z bx by bz br bphi " << std::endl;
  double aPtT1[3];
  aPtT1[2] = 21500.;
  const double phiT1 = 0.123;
  aPtT1[0] = 255.*std::cos(phiT1); 
  aPtT1[1] = 255.*std::sin(phiT1); 
  double aFieldT1[3];
  this->GetFieldValue(aPtT1, aFieldT1);
  const double brT1 = aFieldT1[0]*std::cos(phiT1) + aFieldT1[1]*std::sin(phiT1);
  const double bphiT1 = -aFieldT1[0]*std::sin(phiT1) + aFieldT1[1]*std::cos(phiT1);
  std::cerr << " out:  " << aPtT1[2] << " 255 " << phiT1 << " " << aPtT1[0] << " " << aPtT1[1] << " " << aPtT1[2] 
	     << " " << aFieldT1[0]/CLHEP::tesla << " " << aFieldT1[1]/CLHEP::tesla 
	     << " " << aFieldT1[2]/CLHEP::tesla << " " << brT1/CLHEP::tesla << " " << bphiT1/CLHEP::tesla <<  std::endl;
//  if (aPtT1[2] > 55) return; 
//  for (size_t iz = 0; iz != 150; iz++) {
//    const double z = 20900. + iz*10.; // For the me configuration. 
  for (size_t iz = 0; iz != 20; iz++) {
    const double z = 20900. + iz*100.; // For the me configuration. 
    double aPt[3];
    aPt[2] = z;
    double aField[3]; 
    std::cerr << " ..... at Z = " << z << std::endl;
    for (size_t ir =0; ir != 100; ir++) {
      const double r = 150. + ir*2.5; // temporary... 
//      const double r = 50. + ir*2.5;
      for (size_t iPhi = 0.; iPhi != 50; iPhi++) {
        const double phi = (iPhi + 0.5)*M_PI/25.;
	aPt[0] = r*std::cos(phi);
	aPt[1] = r*std::sin(phi);
	this->GetFieldValue(aPt, aField);
	const double br = aField[0]*std::cos(phi) + aField[1]*std::sin(phi);
	const double bphi = -aField[0]*std::sin(phi) + aField[1]*std::cos(phi);
	fOut << " " << z << " " << r << " " << phi << " " << aPt[0] << " " << aPt[1] << " " << aPt[2] 
	     << " " << aField[0]/CLHEP::tesla << " " << aField[1]/CLHEP::tesla 
	     << " " << aField[2]/CLHEP::tesla << " " << br/CLHEP::tesla << " " << bphi/CLHEP::tesla << std::endl;
      } 
    } 
  }
  fOut.close();
 std::ofstream fOut2("./FieldMapHorn2CEQ_fineV1.txt");
  std::cerr << " NumiMagneticField::dumpField " << std::endl;
  fOut2 << " z r phi x y z bx by bz br bphi " << std::endl;
  for (size_t iz = 0; iz != 2; iz++) {
    const double z = 21500. + iz*800.;
    double aPt[3];
    aPt[2] = z;
    double aField[3]; 
    std::cerr << " ..... at Z = " << z << std::endl;
    for (size_t ir =0; ir != 200; ir++) {
      const double r = 50. + ir*2.5;
      for (size_t iPhi = 0.; iPhi != 200; iPhi++) {
        const double phi = (iPhi + 0.5)*M_PI/400.;
	aPt[0] = r*std::cos(phi);
	aPt[1] = r*std::sin(phi);
	this->GetFieldValue(aPt, aField);
	const double br = aField[0]*std::cos(phi) + aField[1]*std::sin(phi);
	const double bphi = -aField[0]*std::sin(phi) + aField[1]*std::cos(phi);
	fOut2 << " " << z << " " << r << " " << phi << " " << aPt[0] << " " << aPt[1] << " " << aPt[2] 
	     << " " << aField[0]/CLHEP::tesla << " " << aField[1]/CLHEP::tesla 
	     << " " << aField[2]/CLHEP::tesla << " " << br/CLHEP::tesla << " " << bphi/CLHEP::tesla << std::endl;
      } 
    } 
  }
  fOut2.close();
}
 
//magnetic field in inner conductor ====================================================
NumiMagneticFieldIC::NumiMagneticFieldIC()
{
  NumiData=NumiDataInput::GetNumiDataInput();
  dumpHasBeenDump = false;
}

NumiMagneticFieldIC::~NumiMagneticFieldIC(){;}

void NumiMagneticFieldIC::rotateHorns() {
  //
  // Matrix rotation for the field itself. 
  if ((std::abs(NumiData->Horn1Phi) > 1.0e-6) || 
      (std::abs(NumiData->Horn1Theta) > 1.0e-6) ||
      (std::abs(NumiData->Horn1Psi) > 1.0e-6)) {
        fHorn1IsTilted = true;
	fRotMatrixHorn1Container = G4RotationMatrix(NumiData->Horn1Phi, NumiData->Horn1Theta, NumiData->Horn1Psi);
	fRotMatrixHorn1Inverse = fRotMatrixHorn1Container.inverse();
   }
  if ((std::abs(NumiData->Horn2Phi) > 1.0e-6) || 
      (std::abs(NumiData->Horn2Theta) > 1.0e-6) ||
      (std::abs(NumiData->Horn2Psi) > 1.0e-6)) {
        fHorn2IsTilted = true;
	fRotMatrixHorn2Container = G4RotationMatrix(NumiData->Horn2Phi, NumiData->Horn2Theta, NumiData->Horn2Psi);
	fRotMatrixHorn2Inverse = fRotMatrixHorn2Container.inverse();
   }
}

void NumiMagneticFieldIC::GetFieldValue(const double PointGlobal[3],double *Bfield) const
{
  static bool first = true;
  double Point[3];
  for (int kkk=0; kkk != 3; kkk++) Point[kkk] = PointGlobal[kkk];
  
  bool doTransform = (fHorn1IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] < 4500.)) ||
                     (fHorn2IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] > 4500.)) || 
		     (NumiData->usePosLocalCoordInMagField && 
		            ((std::abs(NumiData->Horn1X0) > 1.0e-6) || (std::abs(NumiData->Horn1X0) > 1.0e-6)));		       
  if (doTransform) {
    G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
    G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
    G4ThreeVector Position=G4ThreeVector(PointGlobal[0],PointGlobal[1],PointGlobal[2]);
    //G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
    G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
    G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position); 
    delete numinavigator;
//    std::cerr << " G4Transform, at Z = " << PointGlobal[2] << "  X= " << localPosition[0] << " approx " << Point[0] 
//               << " Y = " << localPosition[1] << " approx " << Point[1] << std::endl;
    Point[0] = localPosition[0];
    Point[1] = localPosition[1];
   
  }
  
  if (!dumpHasBeenDump && NumiData->GetDumpBFieldPlease()) {
    dumpHasBeenDump = true; 
    G4String aNameD("/scratch/minerva/lebrun/G4/BFieldDumpICTmp");
    if (NumiData->GetHorn1IsAlternate()) aNameD += G4String("Alt");
    else aNameD += G4String("Nom");
    aNameD += G4String(".txt");
    fSteppingStream.open(aNameD.c_str());
    fSteppingStream << " id x y z Bx By Bz " << std::endl;
  }

  G4double current = NumiData->HornCurrent/ampere/1000.;
  G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
  G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
//  G4ThreeVector Position=G4ThreeVector(Point[0],Point[1],Point[2]); 
  G4ThreeVector Position=G4ThreeVector(PointGlobal[0],PointGlobal[1],PointGlobal[2]); 
  G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
  G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
  G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position);
//  std::cerr << " Evaluating magnetic field in volume " << myVolume->GetName() 
//              << " R " << std::sqrt(Point[0]*Point[0] + Point[1]*Point[1]) << " z " << Point[2] << std::endl;

  delete numinavigator;


  G4double radius = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);
  G4double dOut=0.;
  G4double dIn=0.;
  G4double magBField = 0.; 
   
  if ((myVolume != 0) && (myVolume->GetLogicalVolume() != 0)) {
    
    G4VSolid * solid=myVolume->GetLogicalVolume()->GetSolid();
    if (solid != 0) {
      bool inOKVolume = false;
      if (Point[2] < 4500.) { // Ugly, but acceptable.. Horn1 here 
         if (!NumiData->GetHorn1IsAlternate()) {
	   inOKVolume = (myVolume->GetName().contains("IC") && (!myVolume->GetName().contains("ICWater")));
	 } else {  
            if (myVolume->GetName().contains("SubSect")) inOKVolume = true;
            if (myVolume->GetName().contains("Neck")) inOKVolume = true;
	    if (myVolume->GetName().contains("Water"))inOKVolume = false;
	 }
        //
        // Check that we are indeed in the field region.. 
       //
        const G4RunManager* pRunMgr = G4RunManager::GetRunManager();
        const NumiDetectorConstruction* pDet = reinterpret_cast<const NumiDetectorConstruction*>(pRunMgr->GetUserDetectorConstruction());
        const double ric = pDet->PHorn1ICRin((Point[2] - 30.)); // in Drawing coordinate system, MCZero = 30 mm
        if (radius < ric) inOKVolume = false; 
      } else { // Horn2  Same code as before Horn1 Alternate study. 
        inOKVolume = (myVolume->GetName().contains("IC") && (!myVolume->GetName().contains("ICWater")));
      }
      if (inOKVolume) {  
//        std::cerr << " Evaluating magnetic field in IC volume " << myVolume->GetName()
//                  << " R " << std::sqrt(Point[0]*Point[0] + Point[1]*Point[1]) 
//	          << " z " << Point[2] <<  " solid Ptr " << solid << std::endl;
        dOut=solid->DistanceToOut(localPosition,G4ThreeVector(Point[0]/radius,Point[1]/radius,0)); //distance to outer boundary
        dIn=solid->DistanceToOut(localPosition,G4ThreeVector(-Point[0]/radius,-Point[1]/radius,0));//distance to inner boundary
        if (dOut<1.*m&&dIn<1.*m&&(dOut!=0.&&dIn!=0.)) 
         {
	  magBField = current / (5.*radius/cm)/10*tesla; //B(kG)=i(kA)/[5*r(cm)], 1T=10kG
	
	  magBField=magBField*((radius*radius-(radius-dIn)*(radius-dIn))/((radius+dOut)*(radius+dOut)-(radius-dIn)*(radius-dIn)));// linear distribution of current
         }
      } 
    } // solid exists 
  }// volume Ptr o.k. 
  /*
  if (Point[2]>92*cm&&Point[2]<92.1*cm) {
     G4cout<<"ICMag: "<<myVolume->GetName()<<" "
	<<Point[0]<<" "
	<<Point[1]<<" "
	<<Point[2]<<" "
	<<sqrt(Point[0]*Point[0]+Point[1]*Point[1])<<" "
	<<magBField<<G4endl;
  }
  */
  if (radius!=0){
    Bfield[0] = -magBField*Point[1]/radius;
    Bfield[1] = magBField*Point[0]/radius;
    Bfield[2] = 0.;}
  else{
    Bfield[0] = 0.; 
    Bfield[1] = 0.; 
    Bfield[2] = 0.; 
  }

    if(NumiData->jCompare &&(localPosition.z()>3*m || localPosition.z()<0*m)) // Make gnumi like horns - this is for validation
    {  
      if (first) {
        G4cout << "Applying jCompare in NumiMagneticFieldIC." << G4endl;
        first = false;
      }      
      Bfield[0]=0;
      Bfield[1]=0;
      Bfield[2]=0;
    }
    if (fSteppingStream.is_open()) {
      const G4RunManager * pRunManager = G4RunManager::GetRunManager();
      fSteppingStream << " " << pRunManager->GetCurrentEvent()->GetEventID();
      fSteppingStream << " " << Point[0] << " " << Point[1] << " " << Point[2];
      fSteppingStream << " " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] << std::endl;;
    }

}


//magnetic field in outter conductor====================================================
NumiMagneticFieldOC::NumiMagneticFieldOC()
{
  NumiData=NumiDataInput::GetNumiDataInput();
  dumpHasBeenDump = false;
}
void NumiMagneticFieldOC::rotateHorns() {
  //
  // Matrix rotation for the field itself. 
  if ((std::abs(NumiData->Horn1Phi) > 1.0e-6) || 
      (std::abs(NumiData->Horn1Theta) > 1.0e-6) ||
      (std::abs(NumiData->Horn1Psi) > 1.0e-6)) {
        fHorn1IsTilted = true;
	fRotMatrixHorn1Container = G4RotationMatrix(NumiData->Horn1Phi, NumiData->Horn1Theta, NumiData->Horn1Psi);
	fRotMatrixHorn1Inverse = fRotMatrixHorn1Container.inverse();
   }
  if ((std::abs(NumiData->Horn2Phi) > 1.0e-6) || 
      (std::abs(NumiData->Horn2Theta) > 1.0e-6) ||
      (std::abs(NumiData->Horn2Psi) > 1.0e-6)) {
        fHorn2IsTilted = true;
	fRotMatrixHorn2Container = G4RotationMatrix(NumiData->Horn2Phi, NumiData->Horn2Theta, NumiData->Horn2Psi);
	fRotMatrixHorn2Inverse = fRotMatrixHorn2Container.inverse();
   }
}

NumiMagneticFieldOC::~NumiMagneticFieldOC(){;}

void NumiMagneticFieldOC::GetFieldValue(const double PointGlobal[3],double *Bfield) const
{
  double Point[3];
  for (int kkk=0; kkk != 3; kkk++) Point[kkk] = PointGlobal[kkk];
  
  bool doTransform = (fHorn1IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] < 4500.)) ||
                     (fHorn2IsTilted && NumiData->useRotLocalCoordInMagField && (PointGlobal[2] > 4500.)) || 
		     (NumiData->usePosLocalCoordInMagField && 
		            ((std::abs(NumiData->Horn1X0) > 1.0e-6) || (std::abs(NumiData->Horn1X0) > 1.0e-6)));		       
  if (doTransform) {
    G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
    G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
    G4ThreeVector Position=G4ThreeVector(PointGlobal[0],PointGlobal[1],PointGlobal[2]);
    //G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
    G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
    G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position); 
    delete numinavigator;
//    std::cerr << " G4Transform, at Z = " << PointGlobal[2] << "  X= " << localPosition[0] << " approx " << Point[0] 
//               << " Y = " << localPosition[1] << " approx " << Point[1] << std::endl;
    Point[0] = localPosition[0];
    Point[1] = localPosition[1];
   
  }
  
  
   static bool first = true;
  for (size_t k=0; k != 3; k++) Bfield[k] = 0.;
  if (!dumpHasBeenDump && NumiData->GetDumpBFieldPlease()) {
    dumpHasBeenDump = true; 
    G4String aNameD("/scratch/minerva/lebrun/G4/BFieldDumpOCTmp");
    if (NumiData->GetHorn1IsAlternate()) aNameD += G4String("Alt");
    else aNameD += G4String("Nom");
    aNameD += G4String(".txt");
    fSteppingStream.open(aNameD.c_str());
    fSteppingStream << " id x y z Bx By Bz " << std::endl;
  }

  G4double current = NumiData->HornCurrent/ampere/1000.;
  G4Navigator* numinavigator=new G4Navigator(); //geometry navigator
  G4Navigator* theNavigator=G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  numinavigator->SetWorldVolume(theNavigator->GetWorldVolume());
  G4ThreeVector Position=G4ThreeVector(PointGlobal[0],PointGlobal[1],PointGlobal[2]); 
  G4VPhysicalVolume* myVolume = numinavigator->LocateGlobalPointAndSetup(Position);
  if (myVolume == 0) return;
  G4TouchableHistoryHandle aTouchable = numinavigator->CreateTouchableHistoryHandle();
  G4ThreeVector localPosition = aTouchable->GetHistory()->GetTopTransform().TransformPoint(Position);

  delete numinavigator;
  if (myVolume->GetLogicalVolume() == 0) return;
  G4VSolid *solid=myVolume->GetLogicalVolume()->GetSolid();
  if (solid == 0) return;

  G4double radius = sqrt(Point[0]*Point[0]+Point[1]*Point[1]); // Now in local coodinate system. 
  G4double dOut=0.;
  G4double dIn=0.;
  G4double magBField = 0.;

  if (myVolume->GetName().contains("OC")){
  dOut=solid->DistanceToOut(localPosition,G4ThreeVector(Point[0]/radius,Point[1]/radius,0)); //distance to outer boundary
  dIn=solid->DistanceToOut(localPosition,G4ThreeVector(-Point[0]/radius,-Point[1]/radius,0));//distance to inner boundary
  if (dOut<1.*m&&dIn<1.*m&&(dOut!=0.&&dIn!=0.)) 
    {
      magBField = current / (5.*radius/cm)/10*tesla; //B(kG)=i(kA)/[5*r(cm)], 1T=10kG
      G4double rIn=radius-dIn;
      G4double rOut=radius+dOut;
      magBField=magBField*(1-(radius*radius-rIn*rIn)/(rOut*rOut-rIn*rIn)); // linear distribution of current
    }
  }

  if (radius!=0){
  Bfield[0] = -magBField*Point[1]/radius;
  Bfield[1] = magBField*Point[0]/radius;
  Bfield[2] = 0.;
  }
  else{
    Bfield[0] = 0.; 
    Bfield[1] = 0.; 
    Bfield[2] = 0.;
  }
  
  if(NumiData->jCompare &&(localPosition.z()>3*m || localPosition.z()<0*m)) // Make gnumi like horns - this is for validation
    {
      if (first) {
        G4cout << "Applying jCompare in NumiMagneticFieldOC." << G4endl;
        first = false;
      }      
      Bfield[0]=0;
      Bfield[1]=0;
      Bfield[2]=0;
    }
    if (fSteppingStream.is_open()) {
      const G4RunManager * pRunManager = G4RunManager::GetRunManager();
      fSteppingStream << " " << pRunManager->GetCurrentEvent()->GetEventID();
      fSteppingStream << " " << Point[0] << " " << Point[1] << " " << Point[2];
      fSteppingStream << " " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] << std::endl;;
    }

}
