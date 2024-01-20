//================== How to use compare_flux.C ==================
// This script takes g4numi ntuples and produces plots of the 
// resulting neutrino flux (neutrino energy distribution at the
// front of MINERvA). It will two plots (actual flux and ratio) 
// with any number of inputs.
// You must set n_fluxes to be the number of inputs you wish to 
// compare. The input can have any number of ntuple files, 
// provided they are all in the same directory. You must also 
// put the full path to this directory in the list "directories".
// The first entry will be drawn in black and used as the 
// denominator for ratio plots. The "names" list is used to 
// make legends on the plots.
// Make sure that the included files for G4numiNtp are in the 
// same directory as this script.
//===============================================================

#include "G4numiNtp.hh"
#include "G4numiNtp.cxx"

const int nbins = 60;

static const int n_fluxes = 1;
char * names[n_fluxes] = { "FTFP_BERT" };
char * directories[n_fluxes] = { "/minerva/data/users/marshalc/flux/ftfp_bert/nominal" };

// For more than 3 variations, the colors start to be not so good
const int colors[8] = { kBlack, kRed, kBlue, kGreen+2, kOrange+1, kViolet-5, kCyan-4, kYellow+1 }; 

void drawRatio( TH1 * hist[n_fluxes], char * name )
{

  TCanvas * c1 = new TCanvas("c1","c1");
  for( int i = 0; i < n_fluxes; ++i ) {
    hist[i]->SetLineColor( colors[i] );
    hist[i]->GetYaxis()->SetRangeUser(0.0,2.0);
  }
  hist[0]->SetTitle(";E_{#nu} (GeV);Ratio to Nominal");
  hist[0]->Draw();
  for( int i = 1; i < n_fluxes; ++i ) hist[i]->Draw("same");
  c1->RedrawAxis();
  TLegend * leg = new TLegend(0.6,0.7,0.9,0.9);
  for( int i = 0; i < n_fluxes; ++i ) leg->AddEntry( hist[i], Form("%s",names[i]), "l" );
  leg->Draw();
  c1->Print(Form("%s.png",name));
  delete c1;

}

void drawFlux( TH1 * flux[n_fluxes], char * name )
{

  TCanvas * c1 = new TCanvas("c1","c1");
  for( int i = 0; i < n_fluxes; ++i ) flux[i]->SetLineColor( colors[i] );
  double max = flux[0]->GetMaximum();
  for( int i = 1; i < n_fluxes; ++i ) {
    if( flux[i]->GetMaximum() > max ) max = flux[i]->GetMaximum();
  }
  flux[0]->SetMaximum( 1.1*max );
  flux[0]->Draw();
  for( int i = 1; i < n_fluxes; ++i ) flux[i]->Draw("same");
  c1->RedrawAxis();
  TLegend * leg = new TLegend(0.6,0.7,0.9,0.9);
  for( int i = 0; i < n_fluxes; ++i ) leg->AddEntry( flux[i], Form("%s",names[i]), "l" );
  leg->Draw();
  c1->Print(Form("%s.png",name));
  delete c1;

}

// Plot neutrino energy at near detector from g4numi file
void compare_flux()
{

  TH1 * flux[n_fluxes];
  TH1 * ratio[n_fluxes];
  for( int i = 0; i < n_fluxes; ++i ) {
    flux[i] = new TH1D(Form("flux_%d",i),";E_{#nu} (GeV)",nbins,0.0,30.0);
    ratio[i] = new TH1D(Form("ratio_%d",i),";E_{#nu} (GeV)",nbins,0.0,30.0);
  }

  G4numiNtp * nt = new G4numiNtp();

  // loop over fluxes
  for( int f = 0; f < n_fluxes; ++f ) {

    TChain * chain = new TChain("nudata","nudata");
    chain->Add( Form("%s/*.root", directories[f]) );

    int NN = chain->GetEntries();
    for( int ii = 0; ii < NN; ++ii ) {

      chain->GetEntry(ii);
      nt->SetTree( *chain );

      if( nt->Ntype == 56 &&
          nt->NenergyN > 0.0 && nt->NenergyN < 1.E10 && 
          nt->Nimpwt > 0.0 && nt->Nimpwt < 1.E10 && 
          nt->NWtNear > 0.0 && nt->NWtNear < 1.E10 ) {
        flux[f]->Fill( nt->NenergyN, (nt->Nimpwt)*(nt->NWtNear) );
      }

    }

  }

  // Fill ratio plots - the first directory on the list is used as the denominator
  // If n_fluxes = 1, this will be pretty stupid
  for( int f = 0; f < n_fluxes; ++f ) {
    for( int b = 1; b <= nbins; ++b ) {
      if( flux[0]->GetBinContent(b) != 0.0 ) ratio[f]->SetBinContent( b, flux[f]->GetBinContent(b) / flux[0]->GetBinContent(b) );
      else if( flux[f]->GetBinContent(b) == 0.0 ) ratio[f]->SetBinContent( b, 1.0 ); // neither file has events in this bin
      else ratio[f]->SetBinContent( b, 0.0 ); // there are events for modified, but not nominal xsec
    }
  }

  drawFlux( flux, "comparison" );
  drawRatio( ratio, "ratio" );

}
