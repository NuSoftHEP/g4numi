//
// absbkgtuple_t.cc
//
//  ADM, July 2005
//  This is a class that defines the absbkgtuple_t object that is used to 
//  store the g4numi output in a root tree.
// $Id: absbkgtuple_t.cc,v 1.1.2.2 2014/01/22 22:31:08 kordosky Exp $
//------------------

#include "absbkgtuple_t.hh"
ClassImp(absbkgtuple_t)

//-----------------------------------------------------------------------------------
absbkgtuple_t::absbkgtuple_t():
   ihorn(-999.0),
   tgtz(-999),
   ptype(-999),
   x(-99999.0),
   y(-99999.0),
   z(-99999.0),
   px(-99999.0),
   py(-99999.0),
   pz(-99999.0),
   KE(-99999.0),
   impwt(-99999.0),
   tgen(-999)
{
}

//-----------------------------------------------------------------------------------
void absbkgtuple_t::Clear()
{
   ihorn = -999.0;
   tgtz = -999;
   ptype = -999;
   x = -99999.0;
   y = -99999.0;
   z = -99999.0;
   px = -99999.0;
   py = -99999.0;
   pz = -99999.0;
   KE = -99999.0;
   impwt = -99999.0;
   tgen = -999;
}

//----------------------------------------------------------------------
absbkgtuple_t::~absbkgtuple_t()
{
  // absbkgtuple_t destructor
}

