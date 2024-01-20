
// This class implement the Kelvin Functions:
// Ber(x), Bei(x), Ker(x), Kei(x), and their first derivatives.
//These have been taken from:
// http://root.cern.ch/root/html516/src/ROOT__Math__KelvinFunctions.cxx.html

#include "NumiKelvinFunctions.hh"
#include <math.h>

double NumiKelvinFunctions::fgMin = 20;
double NumiKelvinFunctions::fgEpsilon = 1.e-20;

double kSqrt2 = 1.4142135623730950488016887242097;
double kPi    = 3.14159265358979323846;
double kEulerGamma = 0.577215664901532860606512090082402431042;

NumiKelvinFunctions* NumiKelvinFunctions::fNumiKelvinFunctions = 0;

NumiKelvinFunctions* NumiKelvinFunctions::GetNumiKelvinFunctions()
{
    if (!fNumiKelvinFunctions) {
        fNumiKelvinFunctions = new NumiKelvinFunctions();    
    }
    return fNumiKelvinFunctions;
}

NumiKelvinFunctions::NumiKelvinFunctions(){

}
NumiKelvinFunctions::~NumiKelvinFunctions()
{
}

double NumiKelvinFunctions::Ber(double x) 
{


   if (fabs(x) < fgEpsilon) return 1;
 
   if (fabs(x) < fgMin) {
      double sum, factorial = 1, n = 1;
      double term = 1, x_factor = x * x * x * x * 0.0625;
 
      sum = 1;
 
      do {
         factorial = 4 * n * n * (2 * n - 1) * (2 * n - 1);
         term *= (-1) / factorial * x_factor;
         sum += term;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term) > fgEpsilon * sum);
 
      return sum;
   } else {
      double alpha = x / kSqrt2 - kPi / 8;
      double value = F1(x) * cos(alpha) + G1(x) * sin(alpha);
      value *= exp(x / kSqrt2) / sqrt(2 * kPi * x);
      value -= Kei(x) / kPi;
      return value;
   }
}
 
//______________________________________________________________________________
double NumiKelvinFunctions::Bei(double x) 
{
  
   if (fabs(x) < fgEpsilon) return 0;
 
   if (fabs(x) < fgMin) {
      double sum, factorial = 1, n = 1;
      double term = x * x * 0.25, x_factor = term * term;
 
      sum = term;
 
      do {
         factorial = 4 * n * n * (2 * n + 1) * (2 * n + 1);
         term *= (-1) / factorial * x_factor;
         sum += term;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term) > fgEpsilon * sum);
 
      return sum;
   } else {
      double alpha = x / kSqrt2 - kPi / 8;
      double value = F1(x) * sin(alpha) + G1(x) * cos(alpha);
      value *= exp(x / kSqrt2) / sqrt(2 * kPi * x);
      value += Ker(x) / kPi;
      return value;
   }
}
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::Ker(double x) 
{

   if (fabs(x) < fgEpsilon) return 1E+100;
 
   if (fabs(x) < fgMin) {
      double term = 1, x_factor = x * x * x * x * 0.0625;
      double factorial = 1, harmonic = 0, n = 1, sum;
      double delta = 0;
      if(x < 0) delta = kPi;

      sum  = - (log(fabs(x) * 0.5) + kEulerGamma) * Ber(x) + (kPi * 0.25 - delta) * Bei(x);
 
      do {
         factorial = 4 * n * n * (2 * n - 1) * (2 * n - 1);
         term *= (-1) / factorial * x_factor;
         harmonic += 1 / (2 * n - 1 ) + 1 / (2 * n);
         sum += term * harmonic;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term * harmonic) > fgEpsilon * sum);
 
      return sum;
   } else {
      double beta = x / kSqrt2 + kPi / 8;
      double value = F2(x) * cos(beta) - G2(x) * sin(beta);
      value *= sqrt(kPi / (2 * x)) * exp(- x / kSqrt2);
      return value;
   }
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::Kei(double x) 
{

   if (fabs(x) < fgEpsilon) return (-0.25 * kPi);
 
   if (fabs(x) < fgMin) {
      double term = x * x * 0.25, x_factor = term * term;
      double factorial = 1, harmonic = 1, n = 1, sum;
      double delta = 0;
      if(x < 0) delta = kPi;

      sum  = term - (log(fabs(x) * 0.5) + kEulerGamma) * Bei(x) - (kPi * 0.25 - delta) * Ber(x);
 
      do {
         factorial = 4 * n * n * (2 * n + 1) * (2 * n + 1);
         term *= (-1) / factorial * x_factor;
         harmonic += 1 / (2 * n) + 1 / (2 * n + 1);
         sum += term * harmonic;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term * harmonic) > fgEpsilon * sum);
 
      return sum;
   } else {
      double beta = x / kSqrt2 + kPi / 8;
      double value = - F2(x) * sin(beta) - G2(x) * cos(beta);
      value *= sqrt(kPi / (2 * x)) * exp(- x / kSqrt2);
      return value;
   }
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::DBer(double x) 
{

   if (fabs(x) < fgEpsilon) return 0;
 
   if (fabs(x) < fgMin) {
      double sum, factorial = 1, n = 1;
      double term = - x * x * x * 0.0625, x_factor = - term * x;
 
      sum = term;
 
      do {
         factorial = 4 * n * (n + 1) * (2 * n + 1) * (2 * n + 1);
         term *= (-1) / factorial * x_factor;
         sum += term;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term) > fgEpsilon * sum);
 
      return sum;
   }
   else return (M(x) * sin(Theta(x) - kPi / 4));
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::DBei(double x) 
{

   if (fabs(x) < fgEpsilon) return 0;
  
   if (fabs(x) < fgMin) {
      double sum, factorial = 1, n = 1;
      double term = x * 0.5, x_factor = x * x * x * x * 0.0625;
 
      sum = term;
 
      do {
         factorial = 4 * n * n * (2 * n - 1) * (2 * n + 1);
         term *= (-1) * x_factor / factorial;
         sum += term;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term) > fgEpsilon * sum);
 
      return sum;
   }
   else return (M(x) * cos(Theta(x) - kPi / 4));
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::DKer(double x) 
{

   if (fabs(x) < fgEpsilon) return -1E+100;
 
   if (fabs(x) < fgMin) {
      double term = - x * x * x * 0.0625, x_factor = - term * x;
      double factorial = 1, harmonic = 1.5, n = 1, sum;
      double delta = 0;
      if(x < 0) delta = kPi;
 
      sum = 1.5 * term - Ber(x) / x - (log(fabs(x) * 0.5) + kEulerGamma) * DBer(x) + (0.25 * kPi - delta) * DBei(x);
 
      do {
         factorial = 4 * n * (n + 1) * (2 * n + 1) * (2 * n + 1);
         term *= (-1) / factorial * x_factor;
         harmonic += 1 / (2 * n + 1 ) + 1 / (2 * n + 2);
         sum += term * harmonic;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term * harmonic) > fgEpsilon * sum);
 
      return sum;
   }
   else return N(x) * sin(Phi(x) - kPi / 4);
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::DKei(double x) 
{

   if (fabs(x) < fgEpsilon) return 0;
 
   if (fabs(x) < fgMin) {
      double term = 0.5 * x, x_factor = x * x * x * x * 0.0625;
      double factorial = 1, harmonic = 1, n = 1, sum;
      double delta = 0;
      if(x < 0) delta = kPi;
 
      sum  = term - Bei(x) / x - (log(fabs(x) * 0.5) + kEulerGamma) * DBei(x) - (kPi * 0.25 - delta) * DBer(x);
 
      do {
         factorial = 4 * n * n * (2 * n - 1) * (2 * n + 1);
         term *= (-1) / factorial * x_factor;
         harmonic += 1 / (2 * n) + 1 / (2 * n + 1);
         sum += term * harmonic;
         n += 1;
         if (n > 1000) break;
      } while (fabs(term * harmonic) > fgEpsilon * sum);
 
      return sum;
   }
   else return N(x) * cos(Phi(x) - kPi / 4);
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::F1(double x) 
{
   double sum, term;
   double prod = 1, x_factor = 8 * x, factorial = 1, n = 2;
  
   sum = kSqrt2 / (16 * x);
 
   do {
      factorial *= n;
      prod *= (2 * n - 1) * (2 * n - 1);
      x_factor *= 8 * x;
      term = prod / (factorial * x_factor) * cos(0.25 * n * kPi);
      sum += term;
      n += 1;
      if (n > 1000) break;
   } while (fabs(term) > fgEpsilon * sum);

   sum += 1;
 
   return sum;
}
 
//______________________________________________________________________________
double NumiKelvinFunctions::F2(double x) 
{
   double sum, term;
   double prod = 1, x_factor = 8 * x, factorial = 1, n = 2;
 
   sum = kSqrt2 / (16 * x);
 
   do {
      factorial *= - n;
      prod *= (2 * n - 1) * (2 * n - 1);
      x_factor *= 8 * x;
      term = (prod / (factorial * x_factor)) * cos(0.25 * n * kPi);
      sum += term;
      n += 1;
      if (n > 1000) break;
   } while (fabs(term) > fgEpsilon * sum);

   sum += 1;
 
   return sum;
}
 
  
//______________________________________________________________________________
double NumiKelvinFunctions::G1(double x) 
{

   double sum, term;
   double prod = 1, x_factor = 8 * x, factorial = 1, n = 2;
 
   sum = kSqrt2 / (16 * x);
 
   do {
      factorial *= n;
      prod *= (2 * n - 1) * (2 * n - 1);
      x_factor *= 8 * x;
      term = prod / (factorial * x_factor) * sin(0.25 * n * kPi);
      sum += term;
      n += 1;
      if (n > 1000) break;
   } while (fabs(term) > fgEpsilon * sum);
 
   return sum;
}
 
//______________________________________________________________________________
double NumiKelvinFunctions::G2(double x) 
{

   double sum, term;
   double prod = 1, x_factor = 8 * x, factorial = 1, n = 2;
 
   sum = kSqrt2 / (16 * x);
 
   do {
      factorial *= - n;
      prod *= (2 * n - 1) * (2 * n - 1);
      x_factor *= 8 * x;
      term = prod / (factorial * x_factor) * sin(0.25 * n * kPi);
      sum += term;
      n += 1;
      if (n > 1000) break;
   } while (fabs(term) > fgEpsilon * sum);
 
   return sum;
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::M(double x) 
{
   double value = 1 + 1 / (8 * kSqrt2 * x) + 1 / (256 * x * x) - 399 / (6144 * kSqrt2 * x * x * x);
   value *= exp(x / kSqrt2) / sqrt(2 * kPi * x);
   return value;
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::Theta(double x) 
{
   double value = x / kSqrt2 - kPi / 8;
   value -= 1 / (8 * kSqrt2 * x) + 1 / (16 * x * x) + 25 / (384 * kSqrt2 * x * x * x);
   return value;
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::N(double x) 
{
   double value = 1 - 1 / (8 * kSqrt2 * x) + 1 / (256 * x * x) + 399 / (6144 * kSqrt2 * x * x * x);
   value *= exp(- x / kSqrt2) * sqrt(kPi / (2 * x));
   return value;
}
 
 
 
//______________________________________________________________________________
double NumiKelvinFunctions::Phi(double x) 
{
   double value = - x / kSqrt2 - kPi / 8;
   value += 1 / (8 * kSqrt2 * x) - 1 / (16 * x * x) + 25 / (384 * kSqrt2 * x * x * x);
   return value;
}
