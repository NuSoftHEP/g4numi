
// C++
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>

// ROOT



// Local
#include "NtpMuon.hh"

using namespace std;

//------------------------------------------------------------------------------------------
NtpMuon::NtpMuon()
  :evtno(-999),
   muvx(-999.0),
   muvy(-999.0),
   muvz(-999.0),
   protonx(-999.0),
   protony(-999.0),
   protonz(-999.0),
   mupx(-999.0),
   mupy(-999.0),
   mupz(-999.0),
   muweight(-999.0),
   tvx(-999.0),
   tvy(-999.0),
   tvz(-999.0),
   tpx(-999.0),
   tpy(-999.0),
   tpz(-999.0),
   tptype(-999),
   nimpwt(-999.0),
   ppvx(-999.0),
   ppvy(-999.0),
   ppvz(-999.0),
   pdvx(-999.0),
   pdvy(-999.0),
   pdvz(-999.0),
   pdpx(-999.0),
   pdpy(-999.0),
   pdpz(-999.0),
   ptype(-999),
   ppmedium(-999),
   pgen(-999),
   evtnoS(65535),
   muvxF(-999.0),
   muvyF(-999.0),
   muvzF(-999.0),
   protonxF(-999.0),
   protonyF(-999.0),
   protonzF(-999.0),
   mupxF(-999.0),
   mupyF(-999.0),
   mupzF(-999.0),
   muweightD(-999.0),
   tvxF(-999.0),
   tvyF(-999.0),
   tvzF(-999.0),
   tpxF(-999.0),
   tpyF(-999.0),
   tpzF(-999.0),
   tptypeS(-999),
   nimpwtD(-999.0),
   ppvxF(-999.0),
   ppvyF(-999.0),
   ppvzF(-999.0),
   pdvxF(-999.0),
   pdvyF(-999.0),
   pdvzF(-999.0),
   pdpxF(-999.0),
   pdpyF(-999.0),
   pdpzF(-999.0),
   ptypeS(-999),
   ppmediumS(-999),
   pgenS(-999),
   ftree(0)
{
  for(Int_t loc = 0;loc<13;++loc){
    nuray_px[loc] = -9999.;
    nuray_py[loc] = -9999.;
    nuray_pz[loc] = -9999.;
    nuray_E[loc] = -9999.0;
    nuray_wgt[loc] = -9999.0;
  }
}

//------------------------------------------------------------------------------------------
NtpMuon::~NtpMuon()
{
}

//------------------------------------------------------------------------------------------
bool NtpMuon::SetTree(TTree* tree)
{
   cout << "NtpMuon::SetTree executed" << endl;
   
   ftree = tree;
   SetBranchesStatus();
   return SetBranches();
}

//------------------------------------------------------------------------------------------
void NtpMuon::SetBranchesStatus()
{
//   cout << "NtpMuon::SetBranchesStatus executed" << endl;

   ftree -> SetBranchStatus("evtno", 1, 0);
   ftree -> SetBranchStatus("muvx", 1, 0);
   ftree -> SetBranchStatus("muvy", 1, 0);
   ftree -> SetBranchStatus("muvz", 1, 0);
   ftree -> SetBranchStatus("mupx", 1, 0);
   ftree -> SetBranchStatus("mupy", 1, 0);
   ftree -> SetBranchStatus("mupz", 1, 0);
   ftree -> SetBranchStatus("muweight", 1, 0);
   ftree -> SetBranchStatus("tvx", 1, 0);
   ftree -> SetBranchStatus("tvy", 1, 0);
   ftree -> SetBranchStatus("tvz", 1, 0);
   ftree -> SetBranchStatus("tpx", 1, 0);
   ftree -> SetBranchStatus("tpy", 1, 0);
   ftree -> SetBranchStatus("tpz", 1, 0);
   ftree -> SetBranchStatus("tptype", 1, 0);
   ftree -> SetBranchStatus("nimpwt", 1, 0);
   ftree -> SetBranchStatus("ppvx", 1, 0);
   ftree -> SetBranchStatus("ppvy", 1, 0);
   ftree -> SetBranchStatus("ppvz", 1, 0);
   ftree -> SetBranchStatus("pdvx", 1, 0);
   ftree -> SetBranchStatus("pdvy", 1, 0);
   ftree -> SetBranchStatus("pdvz", 1, 0);
   ftree -> SetBranchStatus("pdpx", 1, 0);
   ftree -> SetBranchStatus("pdpy", 1, 0);
   ftree -> SetBranchStatus("pdpz", 1, 0);
   ftree -> SetBranchStatus("ptype", 1, 0);
   ftree -> SetBranchStatus("ppmedium", 1, 0);
   ftree -> SetBranchStatus("pgen", 1, 0);
   ftree -> SetBranchStatus("nuray_px", 1, 0);
   ftree -> SetBranchStatus("nuray_py", 1, 0);
   ftree -> SetBranchStatus("nuray_pz", 1, 0);
   ftree -> SetBranchStatus("nuray_E", 1, 0);
   ftree -> SetBranchStatus("nuray_wgt", 1, 0);
   

}



//------------------------------------------------------------------------------------------
bool NtpMuon::SetBranches()
{
//   cout << "NtpMuon::SetBranches executed" << endl;

   SetBranch(&evtnoS, "evtno");
   SetBranch(&muvxF, "muvx");
   SetBranch(&muvyF, "muvy");
   SetBranch(&muvzF, "muvz");
   SetBranch(&protonxF, "protonx");
   SetBranch(&protonyF, "protony");
   SetBranch(&protonzF, "protonz");
   SetBranch(&mupxF, "mupx");
   SetBranch(&mupyF, "mupy");
   SetBranch(&mupzF, "mupz");
   SetBranch(&muweightD, "muweight");
   SetBranch(&tvxF, "tvx");
   SetBranch(&tvyF, "tvy");
   SetBranch(&tvzF, "tvz");
   SetBranch(&tpxF, "tpx");
   SetBranch(&tpyF, "tpy");
   SetBranch(&tpzF, "tpz");
   SetBranch(&tptypeS, "tptype");
   SetBranch(&nimpwtD, "nimpwt");
   SetBranch(&ppvxF, "ppvx");
   SetBranch(&ppvyF, "ppvy");
   SetBranch(&ppvzF, "ppvz");
   SetBranch(&pdvxF, "pdvx");
   SetBranch(&pdvyF, "pdvy");
   SetBranch(&pdvzF, "pdvz");
   SetBranch(&pdpxF, "pdpx");
   SetBranch(&pdpyF, "pdpy");
   SetBranch(&pdpzF, "pdpz");
   SetBranch(&ptypeS, "ptype");
   SetBranch(&ppmediumS, "ppmedium");
   SetBranch(&pgenS, "pgen");
   SetBranch(&nuray_pxF,"nuray_px");
   SetBranch(&nuray_pyF,"nuray_py");
   SetBranch(&nuray_pzF,"nuray_pz");
   SetBranch(&nuray_EF,"nuray_E");
   SetBranch(&nuray_wgtF,"nuray_wgt");
 
   return true;
}

//------------------------------------------------------------------------------------------
bool NtpMuon::SetBranch(void *var, const std::string &name)
{
//   cout << "NtpMuon::SetBranch executed" << endl;

   
   if (!ftree)
   {
      return false;
   }
   
   /* TBranch *branch = ftree -> GetBranch(name.c_str());
   
   if(!branch)
   {
      return false;
   }

   if(fBranch.find(name) != fBranch.end())
   {
      cerr << "NtpMuon::SetBranch - Branch " << name << " already set" << endl;
      return false;
   }

   //cout<<"branch = "<<branch<<endl;
    fBranch[name] = branch;
    if (fBranch.find(name)!= fBranch.end())
    {
       //cout<< "found branch "<< name<<endl;
    }
    //cout<< "1no of leaves = " << fBranch[name] -> GetNleaves() << endl;
    */

    ftree -> SetBranchAddress(name.c_str(), var);

/*    char* str = ftree -> GetTree() -> GetBranch(name.c_str()) -> GetAddress();
    
    stringstream straddress;
    straddress << str << 0;
    cout<< "0 branch "<<name <<" address= "<< straddress.str() <<endl;
   cout<< name << "  = "<< var << endl;
*/
   //cout<< "no of leaves = " << fBranch[name] -> GetNleaves() << endl;
   
  

   //cout<< "1 branch Ntype address= "<< fBranch["run"] -> GetAddress() <<endl;

   

   return true;
}

//------------------------------------------------------------------------------------------
int NtpMuon::GetNEntries()
{
   cout << "NtpMuon::GetNEnetries executed" << endl;

   return ftree -> GetEntries();
}

//------------------------------------------------------------------------------------------
void NtpMuon::GetEntry(unsigned int entry)
{
   //cout << "NtpMuon::FillTree executed" << endl;

   ftree -> GetEntry(entry);


   evtno = (Int_t)evtnoS;
   muvx = (Double_t)muvxF;
   muvy = (Double_t)muvyF;
   muvz = (Double_t)muvzF;
   protonx = (Double_t)protonxF;
   protony = (Double_t)protonyF;
   protonz = (Double_t)protonzF;
   mupx = (Double_t)mupxF;
   mupy = (Double_t)mupyF;
   mupz = (Double_t)mupzF;
   muweight = muweightD;
   tvx = (Double_t)tvxF;
   tvy = (Double_t)tvyF;
   tvz = (Double_t)tvzF;
   tpx = (Double_t)tpxF;
   tpy = (Double_t)tpyF;
   tpz = (Double_t)tpzF;
   tptype = (Int_t)tptypeS;
   nimpwt = nimpwtD;
   ppvx = (Double_t)ppvxF;
   ppvy = (Double_t)ppvyF;
   ppvz = (Double_t)ppvzF;
   pdvx = (Double_t)pdvxF;
   pdvy = (Double_t)pdvyF;
   pdvz = (Double_t)pdvzF;
   pdpx = (Double_t)pdpxF;
   pdpy = (Double_t)pdpyF;
   pdpz = (Double_t)pdpzF;
   ptype = (Int_t)ptypeS;
   ppmedium = (Int_t)ppmediumS;
   pgen = (Int_t)pgenS;
   for(int i = 0;i<kMax;i++){
     nuray_px[i] = (Double_t)nuray_pxF[i];
     nuray_py[i] = (Double_t)nuray_pyF[i];
     nuray_pz[i] = (Double_t)nuray_pzF[i];
     nuray_E[i] = (Double_t)nuray_EF[i];
     nuray_wgt[i] = (Double_t)nuray_wgtF[i];

   }
}

//------------------------------------------------------------------------------------------
void NtpMuon::GetBranchEntry(std::string name, unsigned int entry)
{
   cout << "NtpMuon::FillBranch executed" << endl;


  ftree -> GetBranch(name.c_str()) -> GetEntry(entry);
  
}




//------------------------------------------------------------------------------------------
void NtpMuon::Clear()
{
   //cout << "NtpMuon::Clear executed" << endl;

   evtno = -999;
   muvx = -999.0;
   muvy = -999.0;
   muvz = -999.0;
   mupx = -999.0;
   mupy = -999.0;
   mupz = -999.0;
   muweight = -999.0;
   tvx = -999.0;
   tvy = -999.0;
   tvz = -999.0;
   tpx = -999.0;
   tpy = -999.0;
   tpz = -999.0;
   tptype = -999;
   nimpwt = -999.0;
   ppvx = -999.0;
   ppvy = -999.0;
   ppvz = -999.0;
   pdvx = -999.0;
   pdvy = -999.0;
   pdvz = -999.0;
   pdpx = -999.0;
   pdpy = -999.0;
   pdpz = -999.0;
   ptype = -999;
   ppmedium = -999;
   pgen = -999;
   evtnoS = 65535;
   muvxF = -999.0;
   muvyF = -999.0;
   muvzF = -999.0;
   protonxF = -999.0;
   protonyF = -999.0;
   protonzF = -999.0;
   mupxF = -999.0;
   mupyF = -999.0;
   mupzF = -999.0;
   muweightD = -999.0;
   tvxF = -999.0;
   tvyF = -999.0;
   tvzF = -999.0;
   tpxF = -999.0;
   tpyF = -999.0;
   tpzF = -999.0;
   tptypeS = -999;
   nimpwtD = -999.0;
   ppvxF = -999.0;
   ppvyF = -999.0;
   ppvzF = -999.0;
   pdvxF = -999.0;
   pdvyF = -999.0;
   pdvzF = -999.0;
   pdpxF = -999.0;
   pdpyF = -999.0;
   pdpzF = -999.0;
   ptypeS = -999;
   ppmediumS = -999;
   pgenS = -999;
   for(int i = 0;i<kMax;i++){
     nuray_pxF[i] = -999.;
     nuray_pyF[i] = -999.;
     nuray_pzF[i] = -999.;
     nuray_EF[i] = -999.;
     nuray_wgtF[i] = -999.;

   }


}













































































