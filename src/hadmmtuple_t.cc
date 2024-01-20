//----------------------------------------------------------------------
// Sets the relevant memebers of the data class for storing the
// MC data for the Hadron and Muon Monitors.
//
// $Id: hadmmtuple_t.cc,v 1.8.4.2 2014/01/22 22:31:08 kordosky Exp $
//----------------------------------------------------------------------

#include "hadmmtuple_t.hh"

ClassImp(hadmmtuple_t)

hadmmtuple_t::hadmmtuple_t()
  :evtno(65535), 
   muvx(-99999.),
   muvy(-99999.),
   muvz(-99999.),
   protonx(-99999.),
   protony(-99999.),
   protonz(-99999.),
   mupx(-99999.),
   mupy(-99999.),
   mupz(-99999.),
   muweight(-99999.),
   tvx(-99999.),
   tvy(-99999.),
   tvz(-99999.),
   tpx(-99999.),
   tpy(-99999.),
   tpz(-99999.),
   tpptype(-999),
   nimpwt(-99999.),
   ppvx(-99999.0),
   ppvy(-99999.0),
   ppvz(-99999.0),
   pdvx(-99999.0),
   pdvy(-99999.0),
   pdvz(-99999.0),
   pdpx(-99999.0),
   pdpy(-99999.0),
   pdpz(-99999.0),
   pptype(-999),
   ptype(-999),
   ppmedium(-999),
   pgen(-999),
   kSize(-999),

   z1xpos(-99999.),
   z1ypos(-99999.),
   z1zpos(-99999.),
   z1px(-99999.),
   z1py(-99999.),
   z1pz(-99999.),
   z2xpos(-99999.),
   z2ypos(-99999.),
   z2zpos(-99999.),
   z2px(-99999.),
   z2py(-99999.),
   z2pz(-99999.),
   z3xpos(-99999.),
   z3ypos(-99999.),
   z3zpos(-99999.),
   z3px(-99999.),
   z3py(-99999.),
   z3pz(-99999.),
   z4xpos(-99999.),
   z4ypos(-99999.),
   z4zpos(-99999.),
   z4px(-99999.),
   z4py(-99999.),
   z4pz(-99999.),
   z5xpos(-99999.),
   z5ypos(-99999.),
   z5zpos(-99999.),
   z5px(-99999.),
   z5py(-99999.),
   z5pz(-99999.),
   z6xpos(-99999.),
   z6ypos(-99999.),
   z6zpos(-99999.),
   z6px(-99999.),
   z6py(-99999.),
   z6pz(-99999.),
   z7xpos(-99999.),
   z7ypos(-99999.),
   z7zpos(-99999.),
   z7px(-99999.),
   z7py(-99999.),
   z7pz(-99999.)

   //hmmenergy(-99999.),

    //run(-999),
    //mtgthsig(-99999.),
    //mtgtvsig(-99999.),
    //mtgthpos(-99999.),
    //mtgtvpos(-99999.),
   
  
{
   for(Int_t index=0;index<3;++index)
   {
      mmxpos[index]=-99999.;
      mmypos[index]=-99999.;
      mmzpos[index]=-99999.;
      mmpx[index]=-99999.;
      mmpy[index]=-99999.;
      mmpz[index]=-99999.;
      cell[index]=-999;
      
      //mmxpos_Edep[index]=-99999.;
      //mmypos_Edep[index]=-99999.;
      //mmzpos_Edep[index]=-99999.;
      mmpx_Edep[index]=-99999.;
      mmpy_Edep[index]=-99999.;
      mmpz_Edep[index]=-99999.;

      /////////////////////////////////////////////////
      /*
      mu_edep[index] = 0.0;
      int_edep[index] = 0.0;
      ext_edep[index] = 0.0;
      */
      ////////////////////////////////////////////////

      //zpos_edep[index] = 0.0;
   }
   for(Int_t loc = 0;loc<13;++loc){
      nuray_px[loc]=-9999;
      nuray_py[loc]=-9999; 
      nuray_pz[loc]=-9999;
      nuray_E[loc]=-9999;
      nuray_wgt[loc]=-9999;
   }

}

//-----------------------------------------------------------------------------------
hadmmtuple_t::~hadmmtuple_t()
{
  // hadmmtuple_t destructor
}

//-----------------------------------------------------------------------------------
void hadmmtuple_t::Clear()
{
  
  evtno = 65535; 
  muvx = -99999.;
  muvy = -99999.;
  muvz = -99999.;
  protonx = -99999.;
  protony = -99999.;
  protonz = -99999.;
  mupx = -99999.;
  mupy = -99999.;
  mupz = -99999.;
  muweight = -99999.;
  tvx = -99999.;
  tvy = -99999.;
  tvz = -99999.;
  tpx = -99999.;
  tpy = -99999.;
  tpz = -99999.;
  tpptype = -999;
  nimpwt = -99999.;
  ppvx = -99999.0;
  ppvy = -99999.0;
  ppvz = -99999.0;
  pdvx = -99999.0;
  pdvy = -99999.0;
  pdvz = -99999.0;
  pdpx = -99999.0;
  pdpy = -99999.0;
  pdpz = -99999.0;
  pptype = -999;
  ptype = -999;
  ppmedium = -999;
  pgen = -999;
  kSize = -999;
  

  //hmmenergy = -99999.;

  z1xpos = -99999.;
  z1ypos = -99999.;
  z1zpos = -99999.;
  z1px = -99999.;
  z1py = -99999.;
  z1pz = -99999.;
  z2xpos = -99999.;
  z2ypos = -99999.;
  z2zpos = -99999.;
  z2px = -99999.;
  z2py = -99999.;
  z2pz = -99999.;
  z3xpos = -99999.;
  z3ypos = -99999.;
  z3zpos = -99999.;
  z3px = -99999.;
  z3py = -99999.;
  z3pz = -99999.;
  z4xpos = -99999.;
  z4ypos = -99999.;
  z4zpos = -99999.;
  z4px = -99999.;
  z4py = -99999.;
  z4pz = -99999.;
  z5xpos = -99999.;
  z5ypos = -99999.;
  z5zpos = -99999.;
  z5px = -99999.;
  z5py = -99999.;
  z5pz = -99999.;
  z6xpos = -99999.;
  z6ypos = -99999.;
  z6zpos = -99999.;
  z6px = -99999.;
  z6py = -99999.;
  z6pz = -99999.;
  z7xpos = -99999.;
  z7ypos = -99999.;
  z7zpos = -99999.;
  z7px = -99999.;
  z7py = -99999.;
  z7pz = -99999.;

  //run = -999;
  //mtgthsig = -99999.;
  //mtgtvsig = -99999.;
  //mtgthpos = -99999.;
  //mtgtvpos = -99999.;
   
    for(Int_t loc = 0;loc<13;++loc){
      nuray_px[loc]=-9999;
      nuray_py[loc]=-9999; 
      nuray_pz[loc]=-9999;
      nuray_E[loc]=-9999;
      nuray_wgt[loc]=-9999;
   }
  
  for(Int_t index=0;index<3;++index)
    {
      mmxpos[index]=-99999.;
      mmypos[index]=-99999.;
      mmzpos[index]=-99999.;
      mmpx[index]=-99999.;
      mmpy[index]=-99999.;
      mmpz[index]=-99999.;
      cell[index]=-999;

      //mmxpos_Edep[index]=-99999.;
      //mmypos_Edep[index]=-99999.;
      //mmzpos_Edep[index]=-99999.;
      mmpx_Edep[index]=-99999.;
      mmpy_Edep[index]=-99999.;
      mmpz_Edep[index]=-99999.;

      //////////////////////////////////////////////////
      /*
      mu_edep[index] = 0.0;
      int_edep[index] = 0.0;
      ext_edep[index] = 0.0;
      */
         /////////////////////////////////////////////////////

      //zpos_edep[index] = 0.0;

    }

}
