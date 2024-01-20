// $Id: NumiNuWeight.cc,v 1.1.4.2 2014/01/22 22:31:07 kordosky Exp $

#include <iostream>
#include <cmath>
#include "Rtypes.h"

#include "G4ParticleTable.hh"

#include "data_t.hh"
#include "NumiNuWeight.hh"
#include "NumiParticleCode.hh"

const double rdet = 100.; //in cm

using namespace std;

NumiNuWeight::NumiNuWeight()
{
}

NumiNuWeight::~NumiNuWeight()
{
}

double NumiNuWeight::GetWeight(const data_t* nudata, const vector<double> xdet,
			       double& nu_wght, double& nu_energy)
{
  //assumes units are GeV and cm

  double parent_mass=0.;
  G4ParticleTable* pTable=G4ParticleTable::GetParticleTable();
  string parent_name = 
    NumiParticleCode::AsString(NumiParticleCode::IntToEnum(nudata->ptype));
  parent_mass=pTable->FindParticle(parent_name)->GetPDGMass()/GeV;

  double parent_energy = sqrt(nudata->pdPx*nudata->pdPx +
			      nudata->pdPy*nudata->pdPy +
			      nudata->pdPz*nudata->pdPz + 
			      parent_mass*parent_mass);
  double gamma = parent_energy / parent_mass;
  double gamma_sqr = gamma*gamma;
  double beta_mag = sqrt((gamma_sqr-1.)/gamma_sqr);

  double enuzr = nudata->Necm;

  double rad = sqrt((xdet[0]-nudata->Vx)*(xdet[0]-nudata->Vx) +
		    (xdet[1]-nudata->Vy)*(xdet[1]-nudata->Vy) +
		    (xdet[2]-nudata->Vz)*(xdet[2]-nudata->Vz));

  double parentp = sqrt((nudata->pdPx*nudata->pdPx)+
			(nudata->pdPy*nudata->pdPy)+
			(nudata->pdPz*nudata->pdPz));
  double costh_pardet = (nudata->pdPx*(xdet[0]-nudata->Vx) +
			 nudata->pdPy*(xdet[1]-nudata->Vy) +
			 nudata->pdPz*(xdet[2]-nudata->Vz))/(parentp*rad);

  if (costh_pardet>1.) costh_pardet = 1.;
  else if (costh_pardet<-1.) costh_pardet = -1.;
  double theta_pardet = acos(costh_pardet);

  double emrat = 1./(gamma*(1. - beta_mag * cos(theta_pardet)));

  nu_energy = emrat*enuzr;

  double sangdet = (rdet*rdet /(rad*rad)/ 4.); 

  nu_wght = sangdet * emrat * emrat;

  //done for all except polarized muon
  // in which case need to modify weight
  if (nudata->ptype == NumiParticleCode::kMuonPlus || 
      nudata->ptype == NumiParticleCode::kMuonMinus)
    {
      //boost new neutrino to mu decay cm
      double beta[3];
      double p_nu[3]; //nu momentum
      beta[0]=nudata->pdPx / parent_energy;
      beta[1]=nudata->pdPy / parent_energy;
      beta[2]=nudata->pdPz / parent_energy;

      p_nu[0] = (xdet[0]- nudata->Vx) * nu_energy / rad;
      p_nu[1] = (xdet[1]- nudata->Vy) * nu_energy / rad;
      p_nu[2] = (xdet[2]- nudata->Vz) * nu_energy / rad;

      double partial = gamma*(beta[0]*p_nu[0]+
			      beta[1]*p_nu[1]+
			      beta[2]*p_nu[2]);
      partial = nu_energy-partial / (gamma+1.);
      double p_dcm_nu[4];
      for (int i=0;i<3;i++) p_dcm_nu[i]=p_nu[i]-beta[i]*gamma*partial;
      p_dcm_nu[3]=0.;
      for (int i=0;i<3;i++) p_dcm_nu[3]+=p_dcm_nu[i]*p_dcm_nu[i];
      p_dcm_nu[3]=sqrt(p_dcm_nu[3]);
      
      //boost parent of mu to mu production cm
      gamma=nudata->ppenergy / parent_mass;
      beta[0] = nudata->ppdxdz * nudata->pppz / nudata->ppenergy;
      beta[1] = nudata->ppdydz * nudata->pppz / nudata->ppenergy;
      beta[2] =                  nudata->pppz / nudata->ppenergy;
      partial = gamma*(beta[0]*nudata->muparpx+
		       beta[1]*nudata->muparpy+
		       beta[2]*nudata->muparpz);
      partial = nudata->mupare - partial / (gamma+1.);
      double p_pcm_mp[4];
      p_pcm_mp[0]=nudata->muparpx-beta[0]*gamma*partial;
      p_pcm_mp[1]=nudata->muparpy-beta[1]*gamma*partial;
      p_pcm_mp[2]=nudata->muparpz-beta[2]*gamma*partial;
      p_pcm_mp[3]=0.;
      for (int i=0;i<3;i++) p_pcm_mp[3]+=p_pcm_mp[i]*p_pcm_mp[i];
      p_pcm_mp[3]=sqrt(p_pcm_mp[3]);
      
      double wt_ratio = 1.;
      //have to check p_pcm_mp
      //it can be 0 if mupar..=0. (I guess muons created in target??)
      if (p_pcm_mp[3] != 0. ) {
	//calc new decay angle w.r.t. (anti)spin direction
	double costh = (p_dcm_nu[0]*p_pcm_mp[0]+ 
			p_dcm_nu[1]*p_pcm_mp[1]+ 
			p_dcm_nu[2]*p_pcm_mp[2])/(p_dcm_nu[3]*p_pcm_mp[3]);
	
	if (costh>1.) costh = 1.;
	else if (costh<-1.) costh = -1.;
	
	//calc relative weight due to angle difference
	if (nudata->Ntype == NumiParticleCode::kElectronNeutrino || 
	    nudata->Ntype == NumiParticleCode::kElectronAntiNeutrino)
	  wt_ratio = 1.-costh;
	else if (nudata->Ntype == NumiParticleCode::kMuonNeutrino || 
		 nudata->Ntype == NumiParticleCode::kMuonAntiNeutrino) {
	  double mumass = pTable->FindParticle("mu+")->GetPDGMass()/GeV;
	  double xnu = 2.* enuzr / mumass;
	  wt_ratio = ( (3.-2.*xnu) - (1.-2.*xnu)*costh ) / (3.-2.*xnu);
	} else {
	  cout << "NuWeight:: Bad neutrino type"<<endl;
	}
      }
      nu_wght *= wt_ratio;
    }
  
  return nu_wght;
}
