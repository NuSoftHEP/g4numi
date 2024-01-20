// $Header: /cvs/projects/numi-beam-sim/numi-beam-sim/g4numi/Attic/CreateInvXS.C,v 1.1.2.2 2014/02/13 21:12:03 kordosky Exp $

#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

// needed for invariant cross-section calculation
// these depend on the target geometry & material
const double rho=1.78; // g/cc graphite
const double A=12.01; // g/mol Carbon
const double NA=6.022e23; // atoms/mol
const double dx=0.7;//cm target thickness
const double cm2_per_mb=1e-27; // conversion factor
const double sigma_factor=1.0/(rho/A * NA * dx * cm2_per_mb); // mb
//Constant:  
// MAK note:  the code should be able to deduce most of these from the input files!!!!
const Int_t    Nhistos = 201; 
const Int_t    pT_bins = 80;
const Double_t fpT     = 0.0125;
const Double_t lpT     = 2.0125;
//const Double_t DpT     = (lpT-fpT)/Double_t(pT_bins);
const Double_t fxF     = -0.1025;
const Double_t lxF     = 0.9025;
const Double_t DxF     = (lxF- fxF)/Double_t(Nhistos);

//In MeVs: 
const Double_t mPion    = 0.13957; 
const Double_t mKaon    = 0.493667;
const Double_t mProton  = 0.938272;
const Double_t mNeutron = 0.939565;

const Int_t Npart = 6;
const std::string spart[Npart] = {"pip","pim","kap","kam","prt","neu"};

using std::cout;
using std::endl;

void CreateInvXS(Double_t mom, Int_t nincident, const char* infile, const char* outfile);
Double_t getxF(Int_t id);
Double_t getEnergy(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname);
Double_t getPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname);
Double_t getDPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname);

void CreateInvXS(Double_t mom, Int_t nincident, const char* infile, const char* outfile){

  //  Int_t ver =1;
  TFile* finput  = new TFile(infile,"READ");
  TFile* foutput = new TFile(outfile,"RECREATE");

  for(Int_t ii=0;ii<Npart;ii++){
    foutput->mkdir(spart[ii].c_str());
  }

  TH1D* hxF[Nhistos][Npart];
  TH1D* XShxF[Nhistos][Npart];

  TH2* yield_xFpT[Npart];
  TH2* sigma_xFpT[Npart];

  char  XShtitle[120];
  
 //Getting the yield histograms:

   for(Int_t ih=0;ih<Nhistos;ih++){ 
     for(Int_t ii=0;ii<Npart;ii++){
       hxF[ih][ii] = (TH1D*)finput->Get(Form("%s/xF%03d_%s",spart[ii].c_str(),ih,spart[ii].c_str()));
     }
   }
   
   for(Int_t ii=0;ii<Npart;ii++){
     yield_xFpT[ii] = (TH2*)finput->Get(Form("xFpT_%s",spart[ii].c_str()));
   }

   //Creating inv XS histograms:
   for(Int_t ih=0;ih<Nhistos;ih++){ 
     Double_t centralxF = getxF(ih); 
     Double_t leftxF    = centralxF-0.5*DxF;
     Double_t rightxF   = centralxF+0.5*DxF;

     sprintf(XShtitle,"f(x_{F},p_{T}: %f < x_{F} < %f;p_{T};f(x_{F},p_{T})",leftxF,rightxF); 

     for(Int_t ii=0;ii<Npart;ii++){
       foutput->cd(spart[ii].c_str());
       XShxF[ih][ii] = new TH1D(Form("XS_xF%03d_%s",ih,spart[ii].c_str()),XShtitle,pT_bins,fpT,lpT);
     }
   }

   // copy the yields histogram into the cross-section histogram
   // we convert to cross-section later
   foutput->cd(0);
   for(Int_t ii=0;ii<Npart;ii++){
     sigma_xFpT[ii] = (TH2*) yield_xFpT[ii]->Clone(Form("sigma_xFpT_%s",spart[ii].c_str()));
   }
   
   /*
   ///Looping over all histograms and pT values to convert 1D yields to cross-sections
   for(Int_t ih=0;ih<Nhistos;ih++){
     
    Double_t xfval   = getxF(ih);
    cout<<"Filling histo for xF = "<<xfval<<endl;
    for(Int_t ibin=1;ibin<=pT_bins;ibin++){
      //      cout<<"  ibin: "<<ibin<<endl;
      for(int ii=0;ii<Npart;ii++){
	//	cout<<"   ii: "<<ii<<endl;
	Double_t ptval  = hxF[ih][ii]->GetBinCenter(ibin); 
	Double_t DpT = hxF[ih][ii]->GetXaxis()->GetBinWidth(ibin);
	Double_t pTup   = ptval + DpT/2.0;
	Double_t pTdown = ptval - DpT/2.0;
	Double_t DpT2   = pow(pTup,2.)-pow(pTdown,2.);
	Double_t Ener   = getEnergy(mom,xfval, ptval,spart[ii]);  
	Double_t yield  = hxF[ih][ii]->GetBinContent(ibin);
	Double_t DPz    = getDPz(mom,xfval,ptval,spart[ii]);
	Double_t DP3    = TMath::Pi()*DPz*DpT2;
	Double_t invXS  = Ener/DP3 * (yield/nincident)*sigma_factor;
	XShxF[ih][ii]->Fill(ptval, invXS);
	
      }
    }
   }
   */

   // convert the 2D yields histogram into a 2D cross-section histogram
   for(Int_t ii=0;ii<Npart;ii++){
     TH2* hxs=sigma_xFpT[ii];
     hxs->Sumw2();
     for(Int_t ix=1; ix<=hxs->GetNbinsX(); ix++){
       for(Int_t ipt=1; ipt<=hxs->GetNbinsY(); ipt++){

	 Double_t ptval  = hxs->GetYaxis()->GetBinCenter(ipt); 
	 Double_t DpT = hxs->GetYaxis()->GetBinWidth(ipt);
	 Double_t pTup   = ptval + DpT/2.0;
	 Double_t pTdown = ptval - DpT/2.0;
	 Double_t DpT2   = pow(pTup,2.)-pow(pTdown,2.);
	 Double_t xfval = hxs->GetXaxis()->GetBinCenter(ix);

	 Double_t Ener   = getEnergy(mom,xfval, ptval,spart[ii]);  
	 Double_t yield  = hxs->GetBinContent(ix,ipt);
	 Double_t err_yield  = hxs->GetBinError(ix,ipt);

	 Double_t DPz    = getDPz(mom,xfval,ptval,spart[ii]);
	 Double_t DP3    = TMath::Pi()*DPz*DpT2;
	 Double_t invXS  = Ener/DP3 * (yield/nincident)*sigma_factor;
	 Double_t err_invXS = Ener/DP3 * (err_yield/nincident)*sigma_factor;
	 hxs->SetBinContent(ix,ipt,invXS);
	 hxs->SetBinError(ix,ipt,err_invXS);
       }
     }
   }


  foutput->Write();
  foutput->Close();
  cout<<"===>>>Running end"<<endl; 
   
}

//Get xF value from xF histo number:
Double_t getxF(Int_t id){
  return fxF + (Double_t(id)+0.5)*DxF; 
}

//
Double_t getEnergy(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname){

  Double_t mPart = 0; 
  if(partname=="pip" || partname=="pim")mPart = mPion;
  else if(partname=="kap" || partname=="kam")mPart = mKaon;
  else if(partname=="prt")mPart = mProton;
  else if(partname=="neu")mPart = mNeutron;
  else {
    cout<<"Error here... no particle under study (p, n, K+/-, pi+/-)"<<endl;
    return 1000.;
  }

  Double_t beam_ener = sqrt(pow(beam_mom,2.0)+pow(mProton,2.0));
  Double_t beta= sqrt(pow(beam_ener,2.0)-pow(mProton,2.0))/(beam_ener+mProton);
  Double_t gamma  = 1./sqrt(1.-pow(beta,2.0));
  Double_t Ecm = sqrt(2.*pow(mProton,2.0)+2.*beam_ener*mProton);
  Double_t pl=xfey*Ecm/2.;
  Double_t EnerPartCM = sqrt(pow(pl,2.0)+pow(ptrans,2.0)+pow(mPart,2.0));
  return gamma*(EnerPartCM+beta*pl);  
}
///
Double_t getPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname){
  Double_t mPart = 0; 
  if(partname=="pip" || partname=="pim")mPart = mPion;
  else if(partname=="kap" || partname=="kam")mPart = mKaon;
  else if(partname=="prt")mPart = mProton;
  else if(partname=="neu")mPart = mNeutron;
  else {
    cout<<"Error here... no particle under study (p, n, K+/-, pi+/-)"<<endl;
    return 1000.;
  }

  Double_t beam_ener = sqrt(pow(beam_mom,2.0)+pow(mProton,2.0));
  Double_t Ecm = sqrt(2.*pow(mProton,2.0)+2.*beam_ener*mProton);
  Double_t beta= sqrt(pow(beam_ener,2.0)-pow(mProton,2.0))/(beam_ener+mProton);
  Double_t gamma  = 1./sqrt(1.0-pow(beta,2.0));
  Double_t pl=xfey*Ecm/2.;
  Double_t EnerPartCM = sqrt(pow(pl,2.0)+pow(ptrans,2.0)+pow(mPart,2.0));
  return gamma*(beta*EnerPartCM+pl);
}

///
Double_t getDPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname){

  Double_t xF_l  = xfey-0.5*DxF;
  Double_t xF_r =  xfey+0.5*DxF;
  
  Double_t Pzup   = getPz(beam_mom,xF_r,ptrans,partname);
  Double_t Pzdown = getPz(beam_mom,xF_l,ptrans,partname);
  return   Pzup - Pzdown;
}
