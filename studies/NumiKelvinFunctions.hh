
// This class implement the Kelvin Functions:
// Ber(x), Bei(x), Ker(x), Kei(x), and their first derivatives.
//These have been taken from:
// http://root.cern.ch/root/html516/src/ROOT__Math__KelvinFunctions.cxx.html

#ifndef NumiKelvinFunctions_h
#define NumiKelvinFunctions_h 1

  
class NumiKelvinFunctions
{
  
public:

  NumiKelvinFunctions();
  ~NumiKelvinFunctions();
  
  static NumiKelvinFunctions* GetNumiKelvinFunctions();

  static NumiKelvinFunctions* fNumiKelvinFunctions;

  static double Ber(double x);
  static double Bei(double x);
  static double Ker(double x);
  static double Kei(double x);
  static double DBer(double x);
  static double DBei(double x);
  static double DKer(double x);
  static double DKei(double x);
  
  // Utilities:
  static double F1(double x);
  static double F2(double x);
  static double G1(double x);
  static double G2(double x);
  static double M(double x);
  static double Theta(double x);
  static double N(double x);
  static double Phi(double x);

protected:
  
  static double fgMin;     
  static double fgEpsilon;
  
};
 
#endif 
 
