// Comments on variable description by Chris Marshall; many are based on 
// or taken from Zarko: http://www.hep.utexas.edu/~zarko/wwwgnumi/v19/
// or based on comments by Melissa Jerkins in various bits of code

// Note about decay and weights: "random decay" means you take the parent 
// (usually a pion) and let it decay randomly. Most of these will not hit
// Minerva. To avoid this, we force the decay to give the neutrino at the 
// center of the detector; basically you pick whatever decay kinematics 
// give you a neutrino that will actually hit Minerva. There is a weight that
// is basically a measure of how likely that forced decay is.

#include "G4numiNtp.hh"

G4numiNtp::G4numiNtp()
{}

G4numiNtp::~G4numiNtp()
{}

//void G4numiNtp::SetTree(TTree &t, Int_t entry)
void G4numiNtp::SetTree(TTree &t)
{
  entry=0;

  // Run number
  l_run=t.GetLeaf("run");
  run=Int_t(l_run->GetValue(entry));

  // Event number within run
  l_evtno = t.GetLeaf("evtno");
  evtno= Int_t(l_evtno->GetValue(entry));
  
  // Neutrino direction slopes for random decay
  l_Ndxdz = t.GetLeaf("Ndxdz");
  Ndxdz= l_Ndxdz->GetValue(entry);
  l_Ndydz = t.GetLeaf("Ndydz");
  Ndydz= l_Ndydz->GetValue(entry);

  // Neutrino momentum along beam axis in GeV
  l_Npz = t.GetLeaf("Npz");
  Npz= l_Npz->GetValue(entry);

  // Neutrino energy in GeV for random decay
  l_Nenergy = t.GetLeaf("Nenergy");
  Nenergy= l_Nenergy->GetValue(entry);

  // Enumerator for what decay produced neutrino (e.g. pi+ -> mu+ nu_mu); see the web site
  l_Ndecay = t.GetLeaf("Ndecay");
  Ndecay= Int_t(l_Ndecay->GetValue(entry));

  // Stupid convention for neutrino type: 56 is nu_mu, 55 is nu_mu_bar, 53 and 52 are nu_e and nu_e_bar
  l_Ntype = t.GetLeaf("Ntype");
  Ntype= Int_t(l_Ntype->GetValue(entry));

  // XYZ position of decay that produced neutrino
  l_Vx = t.GetLeaf("Vx");
  Vx= l_Vx->GetValue(entry);
  l_Vy = t.GetLeaf("Vy");
  Vy= l_Vy->GetValue(entry);
  l_Vz = t.GetLeaf("Vz");
  Vz= l_Vz->GetValue(entry);

  // XYZ momentum of parent particle at the decay point
  l_pdpx = t.GetLeaf("pdPx");
  pdpx= l_pdpx->GetValue(entry);
  l_pdpy = t.GetLeaf("pdPy");
  pdpy= l_pdpy->GetValue(entry);
  l_pdpz = t.GetLeaf("pdPz");
  pdpz= l_pdpz->GetValue(entry);

  // Directon and absolute z momentum (GeV) of parent at decay point
  l_ppdxdz = t.GetLeaf("ppdxdz");
  ppdxdz= l_ppdxdz->GetValue(entry);
  l_ppdydz = t.GetLeaf("ppdydz");
  ppdydz= l_ppdydz->GetValue(entry);
  l_pppz = t.GetLeaf("pppz");
  pppz= l_pppz->GetValue(entry);

  // Energy of parent at decay point (GeV)
  l_ppenergy = t.GetLeaf("ppenergy");
  ppenergy= l_ppenergy->GetValue(entry);

  // Stupid convention for parent particle: 8=pi+, 9=pi-, 11=K+, 12=K-, 10=K0L, 5 and 6 are muon
  l_ptype = t.GetLeaf("ptype");
  ptype= Int_t(l_ptype->GetValue(entry));

  // XYZ position where parent particle was produced
  l_ppvx = t.GetLeaf("ppvx");
  ppvx= l_ppvx->GetValue(entry);
  l_ppvy = t.GetLeaf("ppvy");
  ppvy= l_ppvy->GetValue(entry);
  l_ppvz = t.GetLeaf("ppvz");
  ppvz= l_ppvz->GetValue(entry);

  // I think these are the momentum of the particle that produced the muon for neutrinos with muon parents
  // Most of the time these get value of -1 million, meaning parent is not muon
  l_muparpx = t.GetLeaf("muparpx");
  muparpx= l_muparpx->GetValue(entry);
  l_muparpy = t.GetLeaf("muparpy");
  muparpy= l_muparpy->GetValue(entry);
  l_muparpz = t.GetLeaf("muparpz");
  muparpz= l_muparpz->GetValue(entry);

  // Neutrino energy in CoM frame of initial proton-carbon interaction
  l_Necm = t.GetLeaf("Necm");
  Necm= l_Necm->GetValue(entry);

  // "Importance weight" of neutrino parent
  l_Nimpwt = t.GetLeaf("Nimpwt");
  Nimpwt= l_Nimpwt->GetValue(entry);

  // XYZ position (v) and momentum (p) of parent as it exited target
  // Get silly negative values if parent was not produced in target
  l_tvx = t.GetLeaf("tvx");
  tvx= l_tvx->GetValue(entry);
  l_tvy = t.GetLeaf("tvy");
  tvy= l_tvy->GetValue(entry);
  l_tvz = t.GetLeaf("tvz");
  tvz= l_tvz->GetValue(entry);
  l_tpx = t.GetLeaf("tpx");
  tpx= l_tpx->GetValue(entry);
  l_tpy = t.GetLeaf("tpy");
  tpy= l_tpy->GetValue(entry);
  l_tpz = t.GetLeaf("tpz");
  tpz= l_tpz->GetValue(entry);

  // Stupid enumerator for parent particle type exiting target, not sure why this is useful
  l_tptype = t.GetLeaf("tptype");
  tptype= Int_t(l_tptype->GetValue(entry));

  // Number of generations: 1 means parent is primary proton, 2 means parent was produced by primary proton, etc.
  // For Minerva purposes, as of March 2013 anything where this is > 2 gets labeled as "tertiary"
  l_tgen = t.GetLeaf("tgen");
  tgen= Int_t(l_tgen->GetValue(entry));

  // These variables are actually arrays
  // The elements in the array correspond to different near detectors
  // 0 is MINOS near, we'll use that. Nova has a bunch of different ones, etc.
  entry=0;

  // Slopes for neutrino when you force the decay to be in the center of the detector  
  l_NdxdzNear=t.GetLeaf("NdxdzNear");
  NdxdzNear=l_NdxdzNear->GetValue(entry);
  l_NdydzNear=t.GetLeaf("NdydzNear");
  NdydzNear=l_NdydzNear->GetValue(entry);

  // Neutrino energy forced to hit center of detector
  l_NenergyN=t.GetLeaf("NenergyN");
  NenergyN=l_NenergyN->GetValue(entry);  

  // Weight that deals with how likely the decay is to produce a neutrino that hits detector
  l_NWtNear = t.GetLeaf("NWtNear");
  NWtNear= l_NWtNear->GetValue(entry);

  // Trying to get at the full ancestry
  l_pdg = t.GetLeaf("pdg");
  l_startpx = t.GetLeaf("startpx");
  l_startpy = t.GetLeaf("startpy");
  l_startpz = t.GetLeaf("startpz");
  l_stoppx = t.GetLeaf("stoppx");
  l_stoppy = t.GetLeaf("stoppy");
  l_stoppz = t.GetLeaf("stoppz");
  l_startx = t.GetLeaf("startx");
  l_starty = t.GetLeaf("starty");
  l_startz = t.GetLeaf("startz");
  l_stopx = t.GetLeaf("stopx");
  l_stopy = t.GetLeaf("stopy");
  l_stopz = t.GetLeaf("stopz");

  for( int i = 0; i < 10; ++i ) {
    pdg[i] = l_pdg->GetValue(i);
    startpx[i] = l_startpx->GetValue(i);
    startpy[i] = l_startpy->GetValue(i);
    startpz[i] = l_startpz->GetValue(i);
    stoppx[i] = l_stoppx->GetValue(i);
    stoppy[i] = l_stoppy->GetValue(i);
    stoppz[i] = l_stoppz->GetValue(i);
    startx[i] = l_startx->GetValue(i);
    starty[i] = l_starty->GetValue(i);
    startz[i] = l_startz->GetValue(i);
    stopx[i] = l_stopx->GetValue(i);
    stopy[i] = l_stopy->GetValue(i);
    stopz[i] = l_stopz->GetValue(i);
  }

}
