//----------------------------------------------------------------------
//
// $Id: NumiMaterials.cc,v 1.11.4.7 2018/03/01 03:37:05 kordosky Exp $
//----------------------------------------------------------------------

#include "NumiDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "NumiDataInput.hh"

void NumiDetectorConstruction::DefineMaterials()
{  

   if(NumiData->GetDebugLevel() > 0)
   {
      std::cout << "NumiDetectorConstruction::DefineMaterials() Called." << std::endl;
   }

  //------------------------------------------------------ materials

  G4double A;  // atomic mass
  G4double Z;  // atomic number
  G4String symbol,name;
  G4double density, mass_percent;

  G4double rock_density_offset     = 0.04*g/cm3 * NumiData->GetMaterialSigma();
  G4double BluBlock_density_offset = 0.16*g/cm3 * NumiData->GetMaterialSigma();

  // Air and Vacuum
  A = 1.01*g/mole;
  G4Element* elH  = new G4Element(name="Hydrogen",symbol="H" , Z= 1, A);

  A = 4.003*g/mole; 
  G4Element* elHe  = new G4Element(name="Helium",symbol="He" , Z= 2, A);

  A = 12.01*g/mole;
  G4Element* elC  = new G4Element(name="Carbon"  ,symbol="C" , Z = 6, A);

  A = 14.01*g/mole;
  G4Element* elN = new G4Element(name="Nitrogen", symbol="N", Z=7.,A);

  A = 16.00*g/mole;
  G4Element* elO  = new G4Element(name="Oxygen"  ,symbol="O" , Z= 8, A);

  A = 22.99*g/mole; 
#ifdef FLUGG
  G4Element* elNa  = new G4Element(name="Sodium"  ,symbol="Na" , Z=11 , A);
#else
  G4Element* elNa  = new G4Element(name="Natrium"  ,symbol="Na" , Z=11 , A);
#endif
  
  A = 24.305*g/mole;  
  G4Element* elMg  = new G4Element(name="Magnesium"  ,symbol="Mg" , Z=12 , A); 

  A = 26.98*g/mole; 
  G4Element* elAl  = new G4Element(name="Aluminum"  ,symbol="Al" , Z=13, A);

  A = 28.09*g/mole;
  G4Element* elSi  = new G4Element(name="Silicon", symbol="Si", Z=14, A);

  A = 30.974*g/mole; 
#ifdef FLUGG
  G4Element* elP  = new G4Element(name="Phospho"  ,symbol="P" , Z=15 , A);
#else
  G4Element* elP  = new G4Element(name="Phosphorus"  ,symbol="P" , Z=15 , A);
#endif
  
  A = 32.065*g/mole; 
  G4Element* elS  = new G4Element(name="Sulfur"  ,symbol="S" , Z=16 , A);

  //A = 39.948*g/mole;
  //G4Element* elAr = new G4Element(name="Argon" , symbol="Ar", Z=18, A);

  A = 39.1*g/mole; 
  G4Element* elK  = new G4Element(name="Potassium"  ,symbol="K" , Z=19 , A);

  A = 40.09*g/mole; 
  G4Element* elCa  = new G4Element(name="Calcium"  ,symbol="Ca" , Z=20 , A);

  A = 47.867*g/mole; 
  G4Element* elTi  = new G4Element(name="Titanium"  ,symbol="Ti" , Z=22 , A);

  A = 51.9961*g/mole; 
  G4Element* elCr  = new G4Element(name="Chromium"  ,symbol="Cr" , Z=24 , A);

  A = 54.938*g/mole; 
  G4Element* elMn  = new G4Element(name="Manganese"  ,symbol="Mn" , Z=25 , A);

  A = 55.85*g/mole;
  G4Element* elFe = new G4Element(name="Iron"    ,symbol="Fe", Z=26, A);

  A = 58.6934*g/mole;
  G4Element* elNi = new G4Element(name="Nickel"    ,symbol="Ni", Z=28, A);

  A = 63.546*g/mole;
  G4Element* elCu = new G4Element(name="Copper"    ,symbol="Cu", Z=29, A);

  //A = 69.72*g/mole; 
  //G4Element* elGa  = new G4Element(name="Gallium"  ,symbol="Ga" , Z=31 , A);

  A = 200.59*g/mole; 
  G4Element* elHg  = new G4Element(name="Mercury"  ,symbol="Hg" , Z=80, A);

  density = 1.29*mg/cm3; 
  Air = new G4Material("Air", density, 2); // number of components =2
  Air->AddElement(elN, 70*perCent); // mass fraction =70%
  Air->AddElement(elO, 30*perCent); // 30%

  density = 7.75*g/cm3;  
  CT852 = new G4Material("CT852", density, 10); 
  CT852->AddElement(elC,  0.1*perCent); 
  CT852->AddElement(elSi, 0.8*perCent); 
  CT852->AddElement(elMn, 0.8*perCent); 
  CT852->AddElement(elCr, 13*perCent); 
  CT852->AddElement(elS,  0.025*perCent); 
  CT852->AddElement(elP,  0.03*perCent); 
  CT852->AddElement(elTi, 0.2*perCent); 
  CT852->AddElement(elCu, 0.3*perCent); 
  CT852->AddElement(elNi, 0.6*perCent); 
  CT852->AddElement(elFe, 84.145*perCent); 

  density = 7.8416*g/cm3;// ASTM836 steel is the closest
  Slab_Stl = new G4Material("Slab_Stl",density,6);
  Slab_Stl->AddElement(elC,  0.1*perCent);
  Slab_Stl->AddElement(elSi, 0.1*perCent);
  Slab_Stl->AddElement(elMn, 0.4*perCent);
  Slab_Stl->AddElement(elFe, 98.2*perCent);
  Slab_Stl->AddElement(elNi, 1.0*perCent);
  Slab_Stl->AddElement(elCu, 0.2*perCent);

  density = 7.25*g/cm3 + BluBlock_density_offset;// BluBlock steel
  Blu_Stl = new G4Material("Blu_Stl",density,1);
  Blu_Stl->AddElement(elFe, 100*perCent);

  density = 1.*g/cm3; 
  G4int natoms;
  G4int nel;
  Water = new G4Material("Water", density, 2); //number of components =2
  Water->AddElement(elH, natoms=2); 
  Water->AddElement(elO, natoms=1); 

  Chert = new G4Material("Chert",density,2);
  Chert->AddElement(elSi, natoms=1);
  Chert->AddElement(elO, natoms=2);

  Pyrite = new G4Material("Pyrite",density,2);
  Pyrite->AddElement(elFe, natoms=1);
  Pyrite->AddElement(elS, natoms=2);

  density = 2.376e-15*g/cm3;
  G4double temperature=300.*kelvin;
  G4double pressure=2.0e-7*bar;
  Vacuum = new G4Material("Vacuum", density, 1, kStateGas,temperature,pressure);
  Vacuum-> AddMaterial(Air, 1.);

  density = 1.29/760.*mg/cm3*0.4;
  temperature=300.*kelvin;
  pressure=atmosphere/760.*0.4; //Decay Pipe Vacuum is ~0.4Torr
  DecayPipeVacuum = new G4Material("DecayPipeVacuum", density, 1, kStateGas,temperature,pressure);
  DecayPipeVacuum-> AddMaterial(Air, 1.);

  //other materials  
  // M. Kordosky 10/14/16
  // Update helium densities according to advice from A. Holin
  // Suggested densities come from Jim Hylen and Phil Adamson
  SecMonitorHelium = new G4Material("SecMonitorHelium", Z=2., A=4.0026*g/mole, density= 0.1785*kg/m3,kStateGas,300*kelvin,2.55*atmosphere);
  // 0.1785*kg/m3 --> 0.1946 kg/m3.  2.55atm --> 1.27 atm
  TargetHelium = new G4Material("TargetHelium", Z=2., A=4.0026*g/mole, density= 0.1946*kg/m3,kStateGas,300*kelvin,1.27*atmosphere);
  // 0.145*kg/m3 --> 0.1498 kg/m3, 
  DecayPipeHelium = new G4Material("DecayPipeHelium", 0.1498*kg/m3, 1, 
				   kStateGas,300*kelvin,0.9*atmosphere);
  DecayPipeHelium->AddElement(elHe,1.0);
  

#ifdef FLUGG
  Be = new G4Material("Berylliu", Z=4.,A=9.01*g/mole, density=1.848*g/cm3);
  Target =  new G4Material("Carbon", Z=NumiData->TargetZ, A=NumiData->TargetA, density= NumiData->TargetDensity);
#else
  Be = new G4Material("Berillium", Z=4.,A=9.01*g/mole, density=1.848*g/cm3);
  C =  new G4Material("Carbon", Z=6., A=12.01*g/mole, density= 1.83*g/cm3);
  Target =  new G4Material("Target", Z=NumiData->TargetZ, A=NumiData->TargetA, density= NumiData->TargetDensity);
#endif
  Al = new G4Material("Aluminum", Z= 13., A= 26.98*g/mole, density= 2.7*g/cm3);
  Ar = new G4Material("Argon", Z= 18, A=39.948*g/mole,1.784*kg/m3,kStateGas,300*kelvin,atmosphere);
  Pb = new G4Material("Lead", Z= 82., A= 207.19*g/mole, density= 11.35*g/cm3);
  Fe = new G4Material("Iron", Z= 26., A=55.85*g/mole, density= 7.86999*g/cm3);


  density = 2.03*g/cm3;
  G4double fractionmass;
  Concrete = new G4Material("Concrete", density, 10);
  Concrete->AddElement( elH,  fractionmass = 0.01);
  Concrete->AddElement( elO,  fractionmass = 0.529);
  Concrete->AddElement( elNa, fractionmass = 0.016);
  Concrete->AddElement( elHg, fractionmass = 0.002);
  Concrete->AddElement( elAl, fractionmass = 0.034);
  Concrete->AddElement( elSi, fractionmass = 0.337);
  Concrete->AddElement( elK,  fractionmass = 0.013);
  Concrete->AddElement( elCa, fractionmass = 0.044);
  Concrete->AddElement( elFe, fractionmass = 0.014);
  Concrete->AddElement( elC,  fractionmass = 0.001);

  // This comes from the MARS simulation courtesy of 
  // Nancy Grossman
  density = 2.09*g/cm3;
  Shotcrete = new G4Material( "Shotcrete", density, 7 );
  Shotcrete->AddElement( elO,  fractionmass = 0.361747 );
  Shotcrete->AddElement( elSi, fractionmass = 0.100031 );
  Shotcrete->AddElement( elAl, fractionmass = 0.026992 );
  Shotcrete->AddElement( elS,  fractionmass = 0.013217 );
  Shotcrete->AddElement( elCa, fractionmass = 0.463835 );
  Shotcrete->AddElement( elFe, fractionmass = 0.016087 );
  Shotcrete->AddElement( elC,  fractionmass = 0.018091 );

  // This rebar value comes from "Concrete Shielding: Dimensions
  // & Weight" rev. 1988. Which puts the weight at 10150 +/- 50 
  // pounds

  density = 2.61*g/cm3;
  Rebar_Concrete = new G4Material("Rebar_Concrete", density, 10);
  Rebar_Concrete->AddElement( elH,  fractionmass = 0.01 );
  Rebar_Concrete->AddElement( elO,  fractionmass = 0.529 );
  Rebar_Concrete->AddElement( elNa, fractionmass = 0.016 );
  Rebar_Concrete->AddElement( elHg, fractionmass = 0.002 );
  Rebar_Concrete->AddElement( elAl, fractionmass = 0.034 );
  Rebar_Concrete->AddElement( elSi, fractionmass = 0.337 );
  Rebar_Concrete->AddElement( elK,  fractionmass = 0.013 );
  Rebar_Concrete->AddElement( elCa, fractionmass = 0.044 );
  Rebar_Concrete->AddElement( elFe, fractionmass = 0.014 );
  Rebar_Concrete->AddElement( elC,  fractionmass = 0.001 );

  // Dolomite

  density = 2.78*g/cm3 + rock_density_offset;
  G4Material *rockMat = new G4Material( "rockMat", density, 4 ); //CaMg(CO3)2
  rockMat->AddElement( elCa, natoms = 1 );
  rockMat->AddElement( elMg, natoms = 1 ); 
  rockMat->AddElement( elC,  natoms = 2 ); 
  rockMat->AddElement( elO,  natoms = 6 ); 
  
  // Maquoketa Shale

  density = 2.78*g/cm3 + rock_density_offset;
  MaqShale = new G4Material("MaqShale",density,5);
  mass_percent = 25.4/384.75;
  MaqShale->AddElement( elK, fractionmass = mass_percent );
  mass_percent =  71.55/384.75;
  MaqShale->AddElement( elAl, fractionmass = mass_percent );
  mass_percent =  93.8/384.75;
  MaqShale->AddElement( elSi, fractionmass = mass_percent );
  mass_percent =  192.0/384.75;
  MaqShale->AddElement( elO, fractionmass = mass_percent );
  mass_percent =  2.0/384.75;
  MaqShale->AddElement( elH, fractionmass = mass_percent );

  // Dolostone - a mixture of Dolomite and Maquoketa Shale
  // nominally 44.2/52/3.8 Dolomite/Shale/Water
  // for 3.8% water the density of water needs to be added

  density = 2.828*g/cm3 + rock_density_offset;
  DoloStone = new G4Material( "DoloStone", density, 3 );
  DoloStone->AddMaterial( rockMat, fractionmass = 0.30 );
  DoloStone->AddMaterial( MaqShale, fractionmass = 0.662 );
  DoloStone->AddMaterial( Water, fractionmass = 0.038 );

  // The variable density bits result from the water cooling 
  // pipes through the core. There is a nuclear Z 
  // difference between water and steel that is being ignored 
  // in these approximations. -DJK

  density = 2.45*g/cm3;
  var_Al = new G4Material( "VariableDensityAluminum", density, 3 );
  var_Al->AddElement( elAl, fractionmass = .891 );
  var_Al->AddMaterial( Water, fractionmass = .044 );
  var_Al->AddMaterial( Air, fractionmass = .065 );

  density = 6.836*g/cm3;
  var_Stl = new G4Material("VariableDensitySteel", density, 2);
  var_Stl->AddElement( elFe,   fractionmass = .853);
  var_Stl->AddMaterial( Water, fractionmass = .147);

  density = 7.8*g/cm3;
  n1018_Stl = new G4Material( "1018Steel", density, 3 );
  n1018_Stl->AddElement( elFe, fractionmass = .989 );
  n1018_Stl->AddElement( elMn, fractionmass = .009 );
  n1018_Stl->AddElement( elC,  fractionmass = .002 );

  density = 7.8*g/cm3;
  A500_Stl = new G4Material( "A500Steel", density, 3 );
  A500_Stl->AddElement( elFe, fractionmass = .995 );
  A500_Stl->AddElement( elCu, fractionmass = .002 );
  A500_Stl->AddElement( elC,  fractionmass = .003 );

  density = 7.8*g/cm3;
  M1018_Stl = new G4Material( "M1018Steel", density, 5 );
  M1018_Stl->AddElement( elFe, fractionmass = .9901 );
  M1018_Stl->AddElement( elMn, fractionmass = .007 );
  M1018_Stl->AddElement( elC,  fractionmass = .002 );
  M1018_Stl->AddElement( elP,  fractionmass = .00045 );
  M1018_Stl->AddElement( elS,  fractionmass = .00045 );

  //
  //Material for Muon Monitors
  //

  //
  //Casing/tubes for the muon monitors
  //Use Aluminum as defined above for this
  //

  //
  //Parallel plates of the ionization chamber
  //
  Alumina = new G4Material("Alumina",density= 3.97*g/cm3, nel=2);
  Alumina->AddElement(elAl, natoms=2);
  Alumina->AddElement(elO, natoms=3);

  //
  //Helium in muon chambers (at NTP)
  //
  HeGas = new G4Material("HeGas", Z=2., A=4.0026*g/mole, 0.1663*kg/m3,
                                     kStateGas, temperature= 293.15*kelvin,
                                     pressure= 1*atmosphere);

  //
  // Material for delta Ray absorbers 
  //
  
  //
  //Dry wall (CaSO4·2H2O)
  //
  Drywall = new G4Material("Drywall", density=2.32*g/cm3, nel=4);
  Drywall->AddElement(elCa, natoms=1);
  Drywall->AddElement(elS,  natoms=1);
  Drywall->AddElement(elO,  natoms=6);
  Drywall->AddElement(elH,  natoms=4);

  //
  //Paraffin Wax (assuming a composition, although that can vary)
  //
  Paraffin = new G4Material("Paraffin",density= 0.93*g/cm3,nel=2);
  Paraffin->AddElement(elC, natoms=25);
  Paraffin->AddElement(elH, natoms=52);

  //
  //wood?
  //
  /*
  G4Material* wood = new G4Material("Wood", density=0.9*g/cm3, nel=3);
  wood->AddElement(H , 4);
  wood->AddElement(O , 1);
  wood->AddElement(C , 2);
  */


  DefaultMaterial = Air;
  NumiData->SetDefaultMaterial(DefaultMaterial);
}

G4Material* NumiDetectorConstruction::GetMaterial(G4int matcode)
{

  if (matcode==5) return Be; 
  if (matcode==6) return C;
  if (matcode==9) return Al;
  if (matcode==10) return Fe;
  if (matcode==11) return Slab_Stl;
  if (matcode==12) return Blu_Stl;
  if (matcode==15) return Air;
  if (matcode==16) return Vacuum;
  if (matcode==17) return Concrete;
  if (matcode==18) return Target;
  if (matcode==19) return Rebar_Concrete;
  if (matcode==20) {G4cout << "*** Requesting mat=20.  Behavior may have changed!!!" << G4endl; return Shotcrete;}
  if (matcode==21) return var_Al;
  if (matcode==22) return var_Stl;
  if (matcode==23) return n1018_Stl;
  if (matcode==24) return A500_Stl;
  if (matcode==25) return Water;
  if (matcode==26) return M1018_Stl;
  if (matcode==28) return DecayPipeVacuum;
  if (matcode==31) return CT852;
  if (matcode==32) return Alumina;
  if (matcode==33) return HeGas;
  if (matcode==34) return Drywall;
  if (matcode==35) return Paraffin;

  G4cout << G4endl << " **** Wrong material code **** " << matcode << G4endl << G4endl;
  return Vacuum;
}

G4VisAttributes* NumiDetectorConstruction::GetMaterialVisAttrib(G4int matCode)
{ 
  G4VisAttributes* visAttrib=new G4VisAttributes(G4Color(1., 0., 0.));
  if (matCode==5) visAttrib=new G4VisAttributes(G4Color(0.1, 0.2, 0.95));//Be 
  if (matCode==6) visAttrib=new G4VisAttributes(G4Color(0.7, 0.7, 0.7)); //C
  if (matCode==9 || matCode==20) visAttrib=new G4VisAttributes(G4Color(0.2, 0.8, 1.));//Al
  if (matCode==10 || matCode==11) visAttrib=new G4VisAttributes(G4Color(0.5, 0.3, 0.0));//Fe
  if (matCode==12) visAttrib=new G4VisAttributes(G4Color(0.0, 0.4, .9));//BluBlocks
  if (matCode==15) visAttrib=new G4VisAttributes(false); //Air
  if (matCode==16) visAttrib=new G4VisAttributes(false);//Vacuum
  if (matCode==17 || matCode==19 || matCode==20) visAttrib=new G4VisAttributes(G4Color(0.75, 0.85, 0.95));//Concrete
  if (matCode==18) visAttrib=new G4VisAttributes(G4Color(0.6, 0.6, 0.7));//Target
  if (matCode==25) visAttrib=new G4VisAttributes(G4Color(0., 0., 1.));//Water
  if (matCode==28) visAttrib=new G4VisAttributes(false);//Vacuum
  if (matCode==31) visAttrib=new G4VisAttributes(G4Color(1., 1., 1.));//CT852

  return visAttrib;
}
G4VisAttributes* NumiDetectorConstruction::GetMaterialVisAttrib(G4String matName){
  G4VisAttributes* visAttrib=new G4VisAttributes(G4Color(1.,0.,0.));
  if (matName=="Berillium") return GetMaterialVisAttrib(5);
  if (matName=="Carbon") return GetMaterialVisAttrib(6);
  if (matName=="Aluminum") return GetMaterialVisAttrib(9);
  if (matName=="Iron") return GetMaterialVisAttrib(10);
  if (matName=="Slab_Stl") return GetMaterialVisAttrib(11);
  if (matName=="Blu_Stl") return GetMaterialVisAttrib(12);
  if (matName=="Air") return GetMaterialVisAttrib(15);
  if (matName=="Argon") return GetMaterialVisAttrib(16);
  if (matName=="Vacuum") return GetMaterialVisAttrib(16);
  if (matName=="DecayPipeVacuum") return GetMaterialVisAttrib(16);
  if (matName=="Concrete") return GetMaterialVisAttrib(17);
  if (matName=="Target") return GetMaterialVisAttrib(18);
  if (matName=="Rebar_Concrete") return GetMaterialVisAttrib(19);
  if (matName=="Water") return GetMaterialVisAttrib(25);
  if (matName=="CT852") return GetMaterialVisAttrib(31);
  return visAttrib;
}

void NumiDetectorConstruction::DestroyMaterials()
{
  // Destroy all allocated elements and materials
  size_t i;
  G4MaterialTable* matTable = (G4MaterialTable*)G4Material::GetMaterialTable();
  for(i=0;i<matTable->size();i++)
  { delete (*(matTable))[i]; }
  matTable->clear();
  G4ElementTable* elemTable = (G4ElementTable*)G4Element::GetElementTable();
  for(i=0;i<elemTable->size();i++)
  { delete (*(elemTable))[i]; }
  elemTable->clear();
}
