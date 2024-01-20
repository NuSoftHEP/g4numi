
// $Id: NumiMagneticField.hh,v 1.2.4.6 2017/11/02 21:43:27 lebrun Exp $
// --------------------------------------------------------------
// NumiMagneticField.hh modified by Yuki 2004/7/16
// modified by Yuki 8/2/04

#ifndef NumiMagneticField_H
#define NumiMagneticField_H 1

#include <fstream>
#include "globals.hh"
#include "G4MagneticField.hh"
#include "G4RotationMatrix.hh"
#include "NumiDataInput.hh"
class NumiMagneticField : public G4MagneticField
{
  public:
    NumiMagneticField();
    ~NumiMagneticField();

  virtual void GetFieldValue( const double Point[3],  double *Bfield ) const;  
  void DumpItToAsciiFile(); // Not implemented, but could be useful..
  void rotateHorns();   // Using NuMI Data data... Done as the Horns are constructed.. 
  
  private:
    NumiDataInput* NumiData;
    mutable bool fDebugIsOn;
    mutable bool dumpHasBeenDump;
    mutable std::ofstream fSteppingStream;
  //G4double current;
    double fHorn1FieldZCutUpstream;
    double fHorn1FieldZCutDwnstream;
    double fHorn2FieldZCutDwnstream; 
    double fEffectiveLengthHorn1; 
    double fEffectiveLengthHorn2;
    double fOuterRadiusHorn1;
    double fOuterRadiusHorn2;
    bool fHorn1IsTilted;
    bool fHorn2IsTilted;
    G4RotationMatrix fRotMatrixHorn1Container;
    G4RotationMatrix fRotMatrixHorn2Container;
    G4RotationMatrix fRotMatrixHorn1Inverse;
    G4RotationMatrix fRotMatrixHorn2Inverse;
    double fHorn1CurrentEqualizerLongAbsLength;
    double fHorn1CurrentEqualizerQuadAmpl;
    double fHorn1CurrentEqualizerOctAmpl;
    double fHorn2CurrentEqualizerLongAbsLength;
    double fHorn2CurrentEqualizerQuadAmpl;
    double fHorn2CurrentEqualizerOctAmpl;
   // a simple Cartesian grid in Z, R square, and phi. 
    mutable unsigned int fNumZPtsForCE; 
    mutable unsigned int fNumRPtsForCE; 
    mutable unsigned int fNumPhiPtsForCE;
    mutable double fField3DMapCurrentEqualizerZMin; 
    mutable double fField3DMapCurrentEqualizerZStep;
    mutable double fField3DMapCurrentEqualizerPhiStep;
    mutable unsigned int fNumZPtsForCEH2; 
    mutable unsigned int fNumRPtsForCEH2; 
    mutable unsigned int fNumPhiPtsForCEH2;
    mutable double fField3DMapCurrentEqualizerZMinH2; 
    mutable double fField3DMapCurrentEqualizerZOffsetH2; // global to Local to H2 (configuration dependent) 
    mutable double fField3DMapCurrentEqualizerZStepH2;
    mutable double fField3DMapCurrentEqualizerPhiStepH2;
    //
    // Imperfect Current equalizer section support. 
    // To generate the RZ map, one needs a the accurate position of the IC, + thickness. 
    // Define a simple array.. This array is filled/coded up in NumiHorn1.cc 
    //
    bool fIgnoreCEQBr; // treat the CEQ only as a perturbation of the main componenet of the field.
    std::vector<double> fEffectiveRadiiForFieldMap; 
    mutable std::map<unsigned int, std::pair<float, float> > fField3DMapCurrentEqualizer;
    mutable std::vector<double> fField3DMapCurrentEqualizerRadSqAtZ;   
    mutable std::vector<double> fField3DMapCurrentEqualizerRadSqRStepAtZ;   
   // In global coodinate system. ( but rotation and misalignment taken out.  ) 
    void getFieldFrom3DMapCurrentEqualizer(const double Point[3], double *Bfield) const;
   // No skin depth effect using this map.. 
    void fill3DMapCurrentEqualizer() const ; // pseudo const. 
    // Same for Horn2.  This calls service both Horn1 and Horn2.  
    std::vector<double> fEffectiveRadiiForFieldMapH2; 
    mutable std::map<unsigned int, std::pair<float, float> > fField3DMapCurrentEqualizerH2;
    mutable std::vector<double> fField3DMapCurrentEqualizerRadSqAtZH2;   
    mutable std::vector<double> fField3DMapCurrentEqualizerRadSqRStepAtZH2;   
   // In global coodinate system. ( but rotation and misalignment taken out.  ) 
    void getFieldFrom3DMapCurrentEqualizerH2(const double Point[3], double *Bfield) const;
   // No skin depth effect using this map.. 
    void fill3DMapCurrentEqualizerH2() const ; // pseudo const. 
    //
    
  public:
    inline void rotateHorn1Field(double bf[3]) const {
      // Silly copy.. 
      CLHEP::Hep3Vector bLocal(bf[0], bf[1], bf[2]);
//      std::cerr << " Bfield in Local coordinate " << bf[0] 
//                << " / " << bf[1] << " / "  << bf[2] << std::endl;
      CLHEP::Hep3Vector bGlobal = fRotMatrixHorn1Container(bLocal);
      bf[0] = bGlobal.x();  bf[1] = bGlobal.y(); bf[2] = bGlobal.z();
//      std::cerr << " Bfield in Global coordinate " << bf[0] 
//                << " / " << bf[1] << " / "  << bf[2] << std::endl;
//		exit(2);
    }
    inline void rotateHorn2Field(double bf[3]) const {
      CLHEP::Hep3Vector bLocal(bf[0], bf[1], bf[2]);
      CLHEP::Hep3Vector bGlobal = fRotMatrixHorn2Container(bLocal);
      bf[0] = bGlobal.x();  bf[1] = bGlobal.y(); bf[2] = bGlobal.z();
    }
    inline void SetIgnoreCEQBr(bool t) { fIgnoreCEQBr=t; }
    inline void SetHorn1FieldZCutUpstream(double p) { fHorn1FieldZCutUpstream= p; }
    inline void SetHorn1FieldZCutDwnstream(double p) { fHorn1FieldZCutDwnstream= p; }
    inline void SetHorn1CurrentEqualizerLongAbsLength(double p) { fHorn1CurrentEqualizerLongAbsLength= p; }
    inline void SetHorn1CurrentEqualizerQuadAmpl(double p) { fHorn1CurrentEqualizerQuadAmpl= p; }
    inline void SetHorn1CurrentEqualizerOctAmpl(double p) { fHorn1CurrentEqualizerOctAmpl= p; }
    inline void SetEffectiveRadiiForFieldMap (const std::vector<double> &rr) { 
         fEffectiveRadiiForFieldMap = rr; 
    }
    inline void SetField3DMapCurrentEqualizerZOffsetH2(double z) {  
         fField3DMapCurrentEqualizerZOffsetH2 = z;  
         fHorn2FieldZCutDwnstream = fField3DMapCurrentEqualizerZOffsetH2 + fEffectiveLengthHorn2; 
    }
    inline double fillHornPolygonRadii(double z) const {
       if (z < 1.) return fEffectiveRadiiForFieldMap[0]; 
       size_t izz = static_cast<size_t>(z);
       if (izz >= fEffectiveRadiiForFieldMap.size()) 
          return fEffectiveRadiiForFieldMap[fEffectiveRadiiForFieldMap.size()-1];
       return fEffectiveRadiiForFieldMap[izz];
    }
    inline void SetHorn2CurrentEqualizerLongAbsLength(double p) { fHorn2CurrentEqualizerLongAbsLength= p; }
    inline void SetHorn2CurrentEqualizerQuadAmpl(double p) { fHorn2CurrentEqualizerQuadAmpl= p; }
    inline void SetHorn2CurrentEqualizerOctAmpl(double p) { fHorn2CurrentEqualizerOctAmpl= p; }
    inline void SetEffectiveRadiiForFieldMapH2 (const std::vector<double> &rr) 
       { fEffectiveRadiiForFieldMapH2 = rr; }
       // Unlike for Horn1, the reference frame for this method is still local. 
       // z = 0 start of the IC .
    inline double fillHornPolygonRadiiH2(double z) const {
       if (z < 0) return fEffectiveRadiiForFieldMapH2[0]; 
       size_t izz = static_cast<size_t>(z);
       if (izz >= fEffectiveRadiiForFieldMapH2.size()) 
          return fEffectiveRadiiForFieldMapH2[fEffectiveRadiiForFieldMapH2.size()-1];
       return fEffectiveRadiiForFieldMapH2[izz];
    }
    void writeFieldMapCurrentEqualizer() const;
    void readFieldMapCurrentEqualizer(const char* aName) const; // not really, evidently.. 
    void dumpField() const;
    // Same for Horn2 
    void writeFieldMapCurrentEqualizerH2() const;
    void readFieldMapCurrentEqualizerH2(const char* aName) const; // not really, evidently.. 
    void dumpFieldH2() const;
    void testDivergence(int iOpt) const;
    
};

class NumiMagneticFieldIC : public G4MagneticField
{
  public:
    NumiMagneticFieldIC();
    ~NumiMagneticFieldIC();

  virtual void GetFieldValue( const double Point[3], double *Bfield ) const;
  void DumpItToAsciiFile();
  void rotateHorns();

  private:
    NumiDataInput* NumiData;
    mutable bool dumpHasBeenDump;
    mutable std::ofstream fSteppingStream;
  //G4double current;
    bool fHorn1IsTilted;
    bool fHorn2IsTilted;
    G4RotationMatrix fRotMatrixHorn1Container;
    G4RotationMatrix fRotMatrixHorn2Container;
    G4RotationMatrix fRotMatrixHorn1Inverse;
    G4RotationMatrix fRotMatrixHorn2Inverse;
    inline void rotateHorn1Field(double bf[3]) const {
      // Silly copy.. 
      CLHEP::Hep3Vector bLocal(bf[0], bf[1], bf[2]);
//      std::cerr << " Bfield in Local coordinate " << bf[0] 
//                << " / " << bf[1] << " / "  << bf[2] << std::endl;
      CLHEP::Hep3Vector bGlobal = fRotMatrixHorn1Container(bLocal);
      bf[0] = bGlobal.x();  bf[1] = bGlobal.y(); bf[2] = bGlobal.z();
//      std::cerr << " Bfield in Global coordinate " << bf[0] 
//                << " / " << bf[1] << " / "  << bf[2] << std::endl;
//		exit(2);
    }
    inline void rotateHorn2Field(double bf[3]) const {
      CLHEP::Hep3Vector bLocal(bf[0], bf[1], bf[2]);
      CLHEP::Hep3Vector bGlobal = fRotMatrixHorn2Container(bLocal);
      bf[0] = bGlobal.x();  bf[1] = bGlobal.y(); bf[2] = bGlobal.z();
    }
};

class NumiMagneticFieldOC : public G4MagneticField
{
  public:
    NumiMagneticFieldOC();
    ~NumiMagneticFieldOC();

  virtual void GetFieldValue( const double Point[3], double *Bfield ) const;
  void rotateHorns();

  private:
    NumiDataInput* NumiData;
    mutable bool dumpHasBeenDump;
    mutable std::ofstream fSteppingStream;
  //G4double current;
    bool fHorn1IsTilted;
    bool fHorn2IsTilted;
    G4RotationMatrix fRotMatrixHorn1Container;
    G4RotationMatrix fRotMatrixHorn2Container;
    G4RotationMatrix fRotMatrixHorn1Inverse;
    G4RotationMatrix fRotMatrixHorn2Inverse;
    inline void rotateHorn1Field(double bf[3]) const {
      // Silly copy.. 
      CLHEP::Hep3Vector bLocal(bf[0], bf[1], bf[2]);
//      std::cerr << " Bfield in Local coordinate " << bf[0] 
//                << " / " << bf[1] << " / "  << bf[2] << std::endl;
      CLHEP::Hep3Vector bGlobal = fRotMatrixHorn1Container(bLocal);
      bf[0] = bGlobal.x();  bf[1] = bGlobal.y(); bf[2] = bGlobal.z();
//      std::cerr << " Bfield in Global coordinate " << bf[0] 
//                << " / " << bf[1] << " / "  << bf[2] << std::endl;
//		exit(2);
    }
    inline void rotateHorn2Field(double bf[3]) const {
      CLHEP::Hep3Vector bLocal(bf[0], bf[1], bf[2]);
      CLHEP::Hep3Vector bGlobal = fRotMatrixHorn2Container(bLocal);
      bf[0] = bGlobal.x();  bf[1] = bGlobal.y(); bf[2] = bGlobal.z();
    }
};

#endif

