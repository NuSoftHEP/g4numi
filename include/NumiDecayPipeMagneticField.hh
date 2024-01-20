
// $Id: NumiDecayPipeMagneticField.hh,v 1.1.2.1 2017/05/11 10:51:23 lcremone Exp $
// --------------------------------------------------------------

#ifndef NumiDecayPipeMagneticField_H
#define NumiDecayPipeMagneticField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"
#include "NumiDataInput.hh"

class NumiDecayPipeMagneticField : public G4MagneticField
{
  public:
    NumiDecayPipeMagneticField();
    ~NumiDecayPipeMagneticField();

   virtual void GetFieldValue( const double Point[3],  double *Bfield ) const;
  //virtual void GetFieldValue( double *Bfield ) const;
  private:
    NumiDataInput* NumiData;
  //G4double current;
};


#endif

