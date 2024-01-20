//
// zpdata_t.cc
//
//  Jasmine, July 2005
//  This is a class that defines the hadmmtuple_t object that is used to store the g4numi output
//   in a root tree.
//------------------

#include "zptuple_t.hh"

ClassImp(zptuple_t)

  zptuple_t::zptuple_t():
  run(-1),
  evtno(-10),
  xposatz(-10000),
  yposatz(-10000),
  zposatz(-10000),
  xmomatz(-10000),
  ymomatz(-10000),
  zmomatz(-10000),
  matilen(-10000),
  field(-1000),
  pathlength(-10000),
  ptypeatz(-10),
  pidtype(-10),
  zpoint(-10)
{
  /*
  Int_t run;
  Int_t evtno;
  Double_t xposatz;
  Double_t yposatz;
  Double_t zposatz;
  Double_t xmomatz;
  Double_t ymomatz;
  Double_t zmomatz;
  Double_t matilen;
  Double_t field;
  Double_t pathlength;
  Int_t ptypeatz;
  Double_t zpoint;
  */
  
}
  
    

  //-----------------------------------------------------------------------------------
zptuple_t::~zptuple_t()
{
  // zptuple_t destructor
}

