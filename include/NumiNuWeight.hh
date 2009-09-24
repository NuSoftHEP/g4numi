// $Id: NumiNuWeight.hh,v 1.1.2.4 2009/09/24 16:43:38 martens Exp $

#ifndef NUMINUWEIGHT_H
#define NUMINUWEIGHT_H 1

#include "Rtypes.h"
#include "data_t.hh"
#include <vector>

class NumiNuWeight
{
  public:
    NumiNuWeight();
    ~NumiNuWeight();
 
    double GetWeight(const data_t* nudata, 
		     const std::vector<double> xdet,
		     double& nu_wght, double& nu_energy);
 
};
 
#endif
