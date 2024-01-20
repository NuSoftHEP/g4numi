//----------------------------------------------------------------------
// draytupleMIB_t.hh
//
// ADM, July 2005
// This is a class that defines the draytupleMIB_t object that is used to store 
// the g4numi output in a root tree.
//
// $Id: Edep_t.hh,v 1.1.2.2 2014/01/22 22:31:06 kordosky Exp $
//----------------------------------------------------------------------

#ifndef EDEP_T_HH
#define EDEP_T_HH

//C++
#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "Rtypes.h" // include Float_t, Double_t, etc definitions
#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"



class  Edep_t 
{
  
public:
   // a constructor and a destructor
   Edep_t();
   virtual ~Edep_t();
   

   Short_t nTracks;
   Float_t wghtedNTracks;
   Float_t sumEdepWghts;
   Float_t sumWghtdEdep;
   Float_t sumWghtdEdep2;
   std::vector<int> trackVec;
   
   void ClearVector();

      
private:
   ClassDef(Edep_t , 1)
      
      
      };
#endif 

