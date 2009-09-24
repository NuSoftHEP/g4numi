
// $Id: NumiMagneticField.hh,v 1.2.2.4 2009/09/24 16:43:38 martens Exp $
// --------------------------------------------------------------
// NumiMagneticField.hh modified by Yuki 2004/7/16
// modified by Yuki 8/2/04

#ifndef NumiMagneticField_H
#define NumiMagneticField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"
#include "NumiDataInput.hh"

class NumiMagneticField : public G4MagneticField
{
  public:
    NumiMagneticField();
    ~NumiMagneticField();

  virtual void GetFieldValue( const double Point[3],
                               double *Bfield ) const;
  
  private:
    NumiDataInput* NumiData;
  //G4double current;
};

class NumiMagneticFieldIC : public G4MagneticField
{
  public:
    NumiMagneticFieldIC();
    ~NumiMagneticFieldIC();

  virtual void GetFieldValue( const double Point[3], double *Bfield ) const;

  private:
    NumiDataInput* NumiData;
  //G4double current;
};

class NumiMagneticFieldOC : public G4MagneticField
{
  public:
    NumiMagneticFieldOC();
    ~NumiMagneticFieldOC();

  virtual void GetFieldValue( const double Point[3], double *Bfield ) const;

  private:
    NumiDataInput* NumiData;
  //G4double current;
};

#endif

