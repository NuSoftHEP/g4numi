//----------------------------------------------------------------------
// Sets the relevant memebers of the data class for storing the
// MC data for the Hadron and Muon Monitors.
//
// $Id: draytupleMIB_t.cc,v 1.1.2.2 2014/01/22 22:31:08 kordosky Exp $
//----------------------------------------------------------------------


#include "globals.hh"
#include "G4ios.hh"

#include "draytupleMIB_t.hh"

ClassImp(draytupleMIB_t)
   
   
   draytupleMIB_t::draytupleMIB_t()
      :muvx(-99999.),
    muvy(-99999.),
    //muvz(-99999.),
    mupx(-99999.),
    mupy(-99999.),
    mupz(-99999.),
    muweight(-99999.),
    nimpwt(-99999.),
    ptype(-999),
    mm1_mu_edep(),
    mm2_mu_edep(),
    mm3_mu_edep(),
    mm1_int_edep(),
    mm2_int_edep(),
    mm3_int_edep(), 
    mm1_ext_edep(),
    mm2_ext_edep(),
    mm3_ext_edep()
   

   
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
   }


   
}

//-----------------------------------------------------------------------------------
draytupleMIB_t::~draytupleMIB_t()
{
  // draytupleMIB_t destructor
}

//-----------------------------------------------------------------------------------
void draytupleMIB_t::Clear()
{
   muvx = -99999.;
   muvy = -99999.;
   //muvz = -99999.;
   mupx = -99999.;
   mupy = -99999.;
   mupz = -99999.;
   muweight = -99999.;
   nimpwt = -99999.;
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
   }
   
   mm1_mu_edep.clear();
   mm2_mu_edep.clear();
   mm3_mu_edep.clear();

   mm1_int_edep.clear();
   mm2_int_edep.clear();
   mm3_int_edep.clear();

   mm1_ext_edep.clear();
   mm2_ext_edep.clear();
   mm3_ext_edep.clear();
   
   
   
   
}

//------------------------------------------------------------------------------------------
void draytupleMIB_t::ClearTrackIDVectors()
{
   draytupleMIB_t::ClearVectors(mm1_int_edep);
   draytupleMIB_t::ClearVectors(mm2_int_edep);
   draytupleMIB_t::ClearVectors(mm3_int_edep);
   draytupleMIB_t::ClearVectors(mm1_ext_edep);
   draytupleMIB_t::ClearVectors(mm2_ext_edep);
   draytupleMIB_t::ClearVectors(mm3_ext_edep);
}
//------------------------------------------------------------------------------------------
void draytupleMIB_t::ClearVectors(IEdepMap &Map)
{
   
   for(IEdepMap::iterator mit = Map.begin(); mit != Map.end(); ++mit)
   {
      (mit -> second).ClearVector();
   }
   
}

//------------------------------------------------------------------------------------------
const double draytupleMIB_t::GetMuEdep(const int mon, const int cell) const
{

   if( (cell < 0 || cell > 95) || (mon < 0 || mon > 2) )
   {
      G4cerr << "draytupleMIB_t::GetMuEdep - PROBLEM: index out of range for mon = " << mon << " and cell = " << cell << G4endl;
      return -999.0;
   }

   if(mon == 0)
   {
      IFMap::const_iterator mit = mm1_mu_edep.find(cell);
      if(mit == mm1_mu_edep.end())
         return -999.0;
      else
         return mit -> second;
   }
   else if(mon == 1)
   {
      IFMap::const_iterator mit = mm2_mu_edep.find(cell);
      if(mit == mm2_mu_edep.end())
         return -999.0;
      else
         return mit -> second;
   }
   else if(mon == 2)
   {
      IFMap::const_iterator mit = mm3_mu_edep.find(cell);
      if(mit == mm3_mu_edep.end())
         return -999.0;
      else
         return mit -> second;
   }

   G4cerr << "draytupleMIB_t::GetMuEdep - PROBLEM: Unknown problem  mon = " << mon << " and cell = " << cell << G4endl;
   return -999.0;
}







//------------------------------------------------------------------------------------------
const double draytupleMIB_t::GetIntEdep(const int mon, const int cell, const std::string &varname) const
{

   if( (cell < 0 || cell > 95) || (mon < 0 || mon > 2) )
   {
      G4cerr << "draytupleMIB_t::GetIntEdep - PROBLEM: index out of range for mon = " << mon
             << " and cell = " << cell << G4endl;
      return -999.0;
   }

   IEdepMap::const_iterator mit;



   if(mon == 0)
   {
      mit = mm1_int_edep.find(cell);
      if(mit == mm1_int_edep.end())
      {
         return -999.0;
      }
   }
   else if(mon == 1)
   {
      mit = mm2_int_edep.find(cell);
      if(mit == mm2_int_edep.end())
      {
         return -999.0;
      }
   }
   else if(mon == 2)
   {
      mit = mm3_int_edep.find(cell);
      if(mit == mm3_int_edep.end())
      {
         return -999.0;
      }
   }
   else
   {
      G4cerr << "draytupleMIB_t::GetIntEdep - PROBLEM: UNKNOWN mon = " << mon
             << " and cell = " << cell << G4endl;
      return -999.0;
   }
   
   const Edep_t &edep = (mit -> second);
   
   if(varname == "ntracks" ||
      varname == "n diff particles" ||
      varname == "n particles" )
   {
      return (double)(edep.nTracks);
   }
   else if(varname == "wghtedNTracks" ||
           varname == "n wghted diff particles" ||
           varname == "n wghted particles" )
   {
      return (edep.wghtedNTracks);
   }
   else if(varname == "sumEdepWghts" ||
           varname == "sum of edep wghts" ||
           varname == "sum of edep weights")
   {
      return (edep.sumEdepWghts);
   }
   else if(varname == "edep" ||
           varname == "sumWghtdEdep" ||
           varname == "total wghtd edep" ||
           varname == "total weighted edep")
   {
      return (edep.sumWghtdEdep);
   }
   else if(varname == "sumWghtdEdep2" ||
           varname == "sum of wghtd edep squared" ||
           varname == "sum squared of wghtd edep" ||
           varname == "sum of weighted edep squared" ||
           varname == "sum squared of weighted edep")
   {
      return (edep.sumWghtdEdep2);
   }
   else
   {
      G4cerr << "draytupleMIB_t::GetIntEdep - PROBLEM: INVALID varname = " << varname << G4endl;
      return -999.0;
   }
   
}
//------------------------------------------------------------------------------------------
const double draytupleMIB_t::GetExtEdep(const int mon, const int cell, const std::string &varname) const
{

   if( (cell < 0 || cell > 95) || (mon < 0 || mon > 2) )
   {
      G4cerr << "draytupleMIB_t::GetExtEdep - PROBLEM: index out of range for mon = " << mon
             << " and cell = " << cell << G4endl;
      return -999.0;
   }

   IEdepMap::const_iterator mit;



   if(mon == 0)
   {
      mit = mm1_ext_edep.find(cell);
      if(mit == mm1_ext_edep.end())
      {
         return -999.0;
      }
   }
   else if(mon == 1)
   {
      mit = mm2_ext_edep.find(cell);
      if(mit == mm2_ext_edep.end())
      {
         return -999.0;
      }
   }
   else if(mon == 2)
   {
      mit = mm3_ext_edep.find(cell);
      if(mit == mm3_ext_edep.end())
      {
         return -999.0;
      }
   }
   else
   {
      G4cerr << "draytupleMIB_t::GetExtEdep - PROBLEM: UNKNOWN mon = " << mon
             << " and cell = " << cell << G4endl;
      return -999.0;
   }
   
   const Edep_t &edep = (mit -> second);
   
   if(varname == "ntracks" ||
      varname == "n diff particles" ||
      varname == "n particles" )
   {
      return (double)(edep.nTracks);
   }
   else if(varname == "wghtedNTracks" ||
           varname == "n wghted diff particles" ||
           varname == "n wghted particles" )
   {
      return (edep.wghtedNTracks);
   }
   else if(varname == "sumEdepWghts" ||
           varname == "sum of edep wghts" ||
           varname == "sum of edep weights")
   {
      return (edep.sumEdepWghts);
   }
   else if(varname == "edep" ||
           varname == "sumWghtdEdep" ||
           varname == "total wghtd edep" ||
           varname == "total weighted edep")
   {
      return (edep.sumWghtdEdep);
   }
   else if(varname == "sumWghtdEdep2" ||
           varname == "sum of wghtd edep squared" ||
           varname == "sum squared of wghtd edep" ||
           varname == "sum of weighted edep squared" ||
           varname == "sum squared of weighted edep")
   {
      return (edep.sumWghtdEdep2);
   }
   else
   {
      G4cerr << "draytupleMIB_t::GetExtEdep - PROBLEM: INVALID varname = " << varname << G4endl;
      return -999.0;
   }
   
}




//------------------------------------------------------------------------------------------
void draytupleMIB_t::SetMuEdep(const int mon, const int cell, const double value)
{
   if( (cell < 0 || cell > 95) || (mon < 0 || mon > 2) )
   {
      G4cerr << "draytupleMIB_t::SetMuEdep - PROBLEM: index out of range for mon = " << mon << " and cell = " << cell << G4endl;
      return;
   }

   if(mon == 0)
   {
      IFMap::iterator mit = mm1_mu_edep.find(cell);
      if(mit == mm1_mu_edep.end())
      {
         mm1_mu_edep.insert(IFMap::value_type(cell, value));
      }
      else
      {
         (mit -> second) += value;
      }
      
      return;
   }
   else if(mon == 1)
   {
      IFMap::iterator mit = mm2_mu_edep.find(cell);
      if(mit == mm2_mu_edep.end())
      {
         mm2_mu_edep.insert(IFMap::value_type(cell, value));
      }
      else
      {
         (mit -> second) += value;
      }
      
      return;
   }
   else if(mon == 2)
   {      
      IFMap::iterator mit = mm3_mu_edep.find(cell);
      if(mit == mm3_mu_edep.end())
      {
         mm3_mu_edep.insert(IFMap::value_type(cell, value));
      }
      else
      {
         (mit -> second) += value;
      }

      return;
   }

   G4cerr << "draytupleMIB_t::SetMuEdep - PROBLEM: Unknown problem  mon = " << mon << " and cell = " << cell << G4endl;
   
}
//------------------------------------------------------------------------------------------
void draytupleMIB_t::SetIntEdep(const int mon, const int cell, const double value,
                                const int trackID, const double weight)
{

   if( (cell < 0 || cell > 95) || (mon < 0 || mon > 2) )
   {
      G4cerr << "draytupleMIB_t::SetIntEdep - PROBLEM: index out of range for mon = " << mon << " and cell = " << cell << G4endl;
      return;
   }

   IEdepMap::iterator mit;

   if(mon == 0)
   {
      mit = mm1_int_edep.find(cell);
      if(mit == mm1_int_edep.end())
      {
         Edep_t edep;
         mit = (mm1_int_edep.insert(IEdepMap::value_type(cell, edep))).first;
      }
   }
   else if(mon == 1)
   {
      mit = mm2_int_edep.find(cell);
      if(mit == mm2_int_edep.end())
      {
         Edep_t edep;
         mit = (mm2_int_edep.insert(IEdepMap::value_type(cell, edep))).first;
      }
   }
   else if(mon == 2)
   {
      mit = mm3_int_edep.find(cell);
      if(mit == mm3_int_edep.end())
      {
         Edep_t edep;
         mit = (mm3_int_edep.insert(IEdepMap::value_type(cell, edep))).first;
      }
   }
   else
   {
      G4cerr << "draytupleMIB_t::SetIntEdep - PROBLEM: UNKNOWN mon = " << mon
             << " and cell = " << cell << G4endl;
      return;
   }
      
   Edep_t &edep = (mit -> second);

   std::vector<int>::iterator it = std::find((edep.trackVec).begin(), (edep.trackVec).end(), trackID);
   if(it == (edep.trackVec).end())
   {
      (edep.trackVec).push_back(trackID);
      ++edep.nTracks;
      edep.wghtedNTracks += weight;
   }
   
   edep.sumEdepWghts  += weight;
   edep.sumWghtdEdep  += (weight*value);
   edep.sumWghtdEdep2 += ( (weight*value) * (weight*value) );
   
}


//------------------------------------------------------------------------------------------
void draytupleMIB_t::SetExtEdep(const int mon, const int cell, const double value,
                                const int trackID, const double weight)
{

   if( (cell < 0 || cell > 95) || (mon < 0 || mon > 2) )
   {
      G4cerr << "draytupleMIB_t::SetExtEdep - PROBLEM: index out of range for mon = " << mon << " and cell = " << cell << G4endl;
      return;
   }

   IEdepMap::iterator mit;

   if(mon == 0)
   {
      mit = mm1_ext_edep.find(cell);
      if(mit == mm1_ext_edep.end())
      {
         Edep_t edep;
         mit = (mm1_ext_edep.insert(IEdepMap::value_type(cell, edep))).first;
      }
   }
   else if(mon == 1)
   {
      mit = mm2_ext_edep.find(cell);
      if(mit == mm2_ext_edep.end())
      {
         Edep_t edep;
         mit = (mm2_ext_edep.insert(IEdepMap::value_type(cell, edep))).first;
      }
   }
   else if(mon == 2)
   {
      mit = mm3_ext_edep.find(cell);
      if(mit == mm3_ext_edep.end())
      {
         Edep_t edep;
         mit = (mm3_ext_edep.insert(IEdepMap::value_type(cell, edep))).first;
      }
   }
   else
   {
      G4cerr << "draytupleMIB_t::SetExtEdep - PROBLEM: UNKNOWN mon = " << mon
             << " and cell = " << cell << G4endl;
      return;
   }
      
   Edep_t &edep = (mit -> second);

   std::vector<int>::iterator it = std::find((edep.trackVec).begin(), (edep.trackVec).end(), trackID);
   if(it == (edep.trackVec).end())
   {
      (edep.trackVec).push_back(trackID);
      ++edep.nTracks;
      edep.wghtedNTracks += weight;
   }
   
   edep.sumEdepWghts  += weight;
   edep.sumWghtdEdep  += (weight*value);
   edep.sumWghtdEdep2 += ( (weight*value) * (weight*value) );
   
}

