//----------------------------------------------------------------------
// Sets the relevant memebers of the data class for storing the
// MC data for the Hadron and Muon Monitors.
//
// $Id: draytupleSPB_t.cc,v 1.1.2.2 2014/01/22 22:31:08 kordosky Exp $
//----------------------------------------------------------------------

#include "draytupleSPB_t.hh"

ClassImp(draytupleSPB_t)

draytupleSPB_t::draytupleSPB_t()
  :muvx(-99999.),
   muvy(-99999.),
   muvz(-99999.),
   mupx(-99999.),
   mupy(-99999.),
   mupz(-99999.),
   ptype(-999)
     
{
  for(Int_t index=0;index<3;++index)
    {
      mmxpos[index]=-99999.;
      mmypos[index]=-99999.;
      //mmzpos[index]=-99999.;
      mmpx[index]=-99999.;
      mmpy[index]=-99999.;
      mmpz[index]=-99999.;
      cell[index]=-999;

      mu_edep[index] = 0.0;
      int_edep[index] = 0.0;
      ext_edep[index] = 0.0;

    }

}

//-----------------------------------------------------------------------------------
draytupleSPB_t::~draytupleSPB_t()
{
  // draytupleSPB_t destructor
}

//-----------------------------------------------------------------------------------
void draytupleSPB_t::Clear()
{
   muvx = -99999.;
   muvy = -99999.;
   //muvz = -99999.;
   mupx = -99999.;
   mupy = -99999.;
   mupz = -99999.;
   ptype = -999;
   
   for(Int_t index=0;index<3;++index)
   {
      mmxpos[index]=-99999.;
      mmypos[index]=-99999.;
      //mmzpos[index]=-99999.;
      mmpx[index]=-99999.;
      mmpy[index]=-99999.;
      mmpz[index]=-99999.;
      cell[index]=-999;
      
      mu_edep[index] = 0.0;
      int_edep[index] = 0.0;
      ext_edep[index] = 0.0;  
   }

}
