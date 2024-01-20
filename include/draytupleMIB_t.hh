//----------------------------------------------------------------------
// draytupleMIB_t.hh
//
// ADM, July 2005
// This is a class that defines the draytupleMIB_t object that is used to store 
// the g4numi output in a root tree.
//
// $Id: draytupleMIB_t.hh,v 1.1.2.2 2014/01/22 22:31:06 kordosky Exp $
//----------------------------------------------------------------------

#ifndef DRAYTUPLEMIB_T_HH
#define DRAYTUPLEMIB_T_HH

//C++
#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "Rtypes.h" // include Float_t, Double_t, etc definitions
#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"

#include "Edep_t.hh"

class  draytupleMIB_t 
{
  
public:
   // a constructor and a destructor
   draytupleMIB_t();
   virtual ~draytupleMIB_t();
   
   void Clear();

   const double GetMuEdep (const int mon, const int cell) const;
   const double GetIntEdep(const int mon, const int cell,
                           const std::string &varname = "edep") const;
   const double GetExtEdep(const int mon, const int cell,
                           const std::string &varname = "edep") const;
   
  
   void SetMuEdep (const int mon, const int cell, const double value);
   void SetIntEdep(const int mon, const int cell, const double value,
                   const int trackID, const double weight);
   void SetExtEdep(const int mon, const int cell, const double value,
                   const int trackID, const double weight);

   void ClearTrackIDVectors();



   typedef std::map<int, Float_t> IFMap;
   typedef std::vector<int> IVec;
   typedef std::map<int, Edep_t> IEdepMap;

private:

   void ClearVectors(IEdepMap &Map);


public:
   

   // the following variables are placed in the root tree
      
   // x y and z position of muon at EODP
   Float_t muvx;
   Float_t muvy;
   //Float_t muvz;

   // x y and z momentum of muon at EODP
   Float_t mupx;
   Float_t mupy;
   Float_t mupz;

   //muon weight to reach EODP
   Double_t muweight;

   //importance weight of parent particle from gnumi
   Double_t nimpwt;

   //particle type = muon
   Short_t ptype;

   //
   //For the following index 0, 1 and 2 correspond
   //to Alcove 1, Alcove 2 and Alcove 3 respectively
   //
   
   //x y and z position of muon at the muon monitor
   Float_t mmxpos[3];
   Float_t mmypos[3];
   //Float_t mmzpos[3];

   //x y and z momentum of muon at the muon monitor
   Float_t mmpx[3];
   Float_t mmpy[3];
   Float_t mmpz[3];

   // the monitor pixel that the muon intercepts
   Short_t cell[3];

private:

   

      
   //total energy deposited by muon in the cell(s)
   IFMap mm1_mu_edep;
   IFMap mm2_mu_edep;
   IFMap mm3_mu_edep;

   //total energy deposited by electrons/positrons
   //that are generated from interactions of the
   //muon with the monitor
   IEdepMap mm1_int_edep;
   IEdepMap mm2_int_edep;
   IEdepMap mm3_int_edep;

   //total energy deposited by electrons/positrons
   //that are generated from interactions of the
   //muon with the elements external to the monitor
   IEdepMap mm1_ext_edep;
   IEdepMap mm2_ext_edep;
   IEdepMap mm3_ext_edep;
   
   
private:
   ClassDef(draytupleMIB_t , 1)
      
      
      };
#endif 

