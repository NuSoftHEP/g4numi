#ifndef NTPMUON_H
#define NTPMUON_H


//C++
#include <map>

// ROOT
#include "Rtypes.h" // include Float_t, Double_t, etc definitions

#include "TTree.h"
class TTree;
class TBranch;
//class Rtypes;
const Int_t kMax=13;
//Local
class NtpMuon
{
public:
   
   NtpMuon();
   ~NtpMuon();

   void Clear();

   bool SetTree(TTree* tree);

   int GetNEntries();
   
   void GetEntry(unsigned int entry);

   void GetBranchEntry(std::string name, unsigned int entry);
   
public:

   Int_t evtno;
   Double_t muvx;
   Double_t muvy;
   Double_t muvz;
  Double_t protonx;
  Double_t protony;
  Double_t protonz;
   Double_t mupx;
   Double_t mupy;
   Double_t mupz;
  //Float_t horncurrent;
   Double_t muweight;
   Double_t tvx;
   Double_t tvy;
   Double_t tvz;
   Double_t tpx;
   Double_t tpy;
   Double_t tpz;
   Int_t tptype;
   Double_t nimpwt;
   Double_t ppvx;
   Double_t ppvy;
   Double_t ppvz;
   Double_t pdvx;
   Double_t pdvy;
   Double_t pdvz;
   Double_t pdpx;
   Double_t pdpy;
   Double_t pdpz;
   Int_t ptype;
   Int_t ppmedium;
   Int_t pgen;
   Int_t kSize;

   Double_t nuray_px[kMax];
   Double_t nuray_py[kMax];
   Double_t nuray_pz[kMax];
   Double_t nuray_E[kMax];
   Double_t nuray_wgt[kMax];


   UShort_t evtnoS;
   Float_t muvxF;
   Float_t muvyF;
   Float_t muvzF;
  Float_t protonxF;
  Float_t protonyF;
  Float_t protonzF;
   Float_t mupxF;
   Float_t mupyF;
   Float_t mupzF;
  //Float_t horncurrentF;
   Double_t muweightD;
   Float_t tvxF;
   Float_t tvyF;
   Float_t tvzF;
   Float_t tpxF;
   Float_t tpyF;
   Float_t tpzF;
   Short_t tptypeS;
   Double_t nimpwtD;
   Float_t ppvxF;
   Float_t ppvyF;
   Float_t ppvzF;
   Float_t pdvxF;
   Float_t pdvyF;
   Float_t pdvzF;
   Float_t pdpxF;
   Float_t pdpyF;
   Float_t pdpzF;
   Short_t ptypeS;
   Short_t ppmediumS;
   Short_t pgenS;
   Short_t kSizeS;
   Float_t nuray_pxF[kMax];
   Float_t nuray_pyF[kMax];
   Float_t nuray_pzF[kMax];
   Float_t nuray_EF[kMax];
   Float_t nuray_wgtF[kMax];
 
        
private:

   bool SetBranches();

   void SetBranchesStatus();

   bool SetBranch(void *var, const std::string &name);



   
   std::map<std::string, TBranch*> fBranch;
   
   TTree *ftree;
};

#endif
