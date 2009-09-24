//----------------------------------------------------------------------
// Particle names should match the geant4 particle names since this is 
// currently used to translate particles from external ntuple to geant 
// (NumiPrimaryGeneratorAction.cc)
// $Id: NumiParticleCode.cc,v 1.2.2.4 2009/09/24 16:43:38 martens Exp $
//----------------------------------------------------------------------

#include "NumiParticleCode.hh"

G4String NumiParticleCode::AsString(NumiParticleCode_t pCode)
{
switch (pCode)
    {
    case kMuonPlus:             return "mu+";          break;
    case kMuonMinus:            return "mu-";          break;
    case kPion0:                return "pi0";          break;
    case kPionPlus:             return "pi+";          break;
    case kPionMinus:            return "pi-";          break;
    case kKaon0L:               return "kaon0L";       break;
    case kKaonPlus:             return "kaon+";        break;
    case kKaonMinus:            return "kaon-";        break;
    case kNeutron:              return "neutron";      break;
    case kProton:               return "proton";       break;
    case kAntiProton:           return "anti_proton";  break;
    case kKaon0S:               return "kaon0S";       break;
    case kEta:                  return "eta";          break;
    case kLambda:               return "lambda";       break;
    case kSigmaPlus:            return "sigma+";       break;
    case kSigma0:               return "sigma0";       break;
    case kSigmaMinus:           return "sigma-";       break;
    case kXi0:                  return "x0";           break;
    case kXiMinus:              return "xi-";          break;
    case kOmegaMinus:           return "omega-";       break;
    case kAntiNeutron:          return "anti_neutron"; break;
    case kAntiLambda:           return "anti_lambda";  break;
    case kAntiSigmaMinus:       return "anti_sigma-";  break;
    case kAntiSigma0:           return "anti_sigma0";  break;
    case kAntiSigmaPlus:        return "anti_sigma+";  break;
    case kAntiXi0:              return "anti_x0";      break;
    case kAntiXiMinus:          return "anti_xi-";     break;
    case kElectronAntiNeutrino: return "anti_nu_e";    break;
    case kElectronNeutrino:     return "nu_e";         break;
    case kMuonAntiNeutrino:     return "anti_nu_mu";   break;
    case kMuonNeutrino:         return "nu_mu";        break;
    case kOther:                return "other";        break;
    default:                    return "other";        break;
    }
}

G4int NumiParticleCode::AsInt(NumiParticleCode_t pCode)
{
  switch (pCode)
    {
    case kMuonPlus:             return 5;   break;
    case kMuonMinus:            return 6;   break;
    case kPion0:                return 7;   break;
    case kPionPlus:             return 8;   break;
    case kPionMinus:            return 9;   break;
    case kKaon0L:               return 10;  break;
    case kKaonPlus:             return 11;  break;
    case kKaonMinus:            return 12;  break;
    case kNeutron:              return 13;  break;
    case kProton:               return 14;  break;
    case kAntiProton:           return 15;  break;
    case kKaon0S:               return 16;  break;
    case kEta:                  return 17;  break;
    case kLambda:               return 18;  break;
    case kSigmaPlus:            return 19;  break;
    case kSigma0:               return 20;  break;
    case kSigmaMinus:           return 21;  break;
    case kXi0:                  return 22;  break;
    case kXiMinus:              return 23;  break;
    case kOmegaMinus:           return 24;  break;
    case kAntiNeutron:          return 25;  break;
    case kAntiLambda:           return 26;  break;
    case kAntiSigmaMinus:       return 27;  break;
    case kAntiSigma0:           return 28;  break;
    case kAntiSigmaPlus:        return 29;  break;
    case kAntiXi0:              return 30;  break;
    case kAntiXiMinus:          return 31;  break;
    case kElectronAntiNeutrino: return 52;  break;
    case kElectronNeutrino:     return 53;  break;
    case kMuonAntiNeutrino:     return 55;  break;
    case kMuonNeutrino:         return 56;  break;
    case kOther:                return 99;  break;
    default:                    return 99;  break;
    }
}

NumiParticleCode::NumiParticleCode_t NumiParticleCode::StringToEnum(G4String pName)
{
 //returns particle enum
  
  
  
  if (pName ==  "nu_tau")             return kOther;                     
  else if (pName ==  "anti_nu_tau")   return kOther;                    
  else if (pName ==  "eta_prime")     return kOther;                     //?

  else if (pName ==  "mu+")           return kMuonPlus;                 
  else if (pName ==  "mu-")           return kMuonMinus;                
  else if (pName ==  "pi0")           return kPion0;                    
  else if (pName ==  "pi+")           return kPionPlus;                 
  else if (pName ==  "pi-")           return kPionMinus;                
  else if (pName ==  "kaon0L")        return kKaon0L;                   
  else if (pName ==  "kaon+")         return kKaonPlus;                 
  else if (pName ==  "kaon-")         return kKaonMinus;                
  else if (pName ==  "neutron")       return kNeutron;                  
  else if (pName ==  "proton")        return kProton;                   
  else if (pName ==  "anti_proton")   return kAntiProton;                
  else if (pName ==  "kaon0S")        return kKaon0S;                   
  else if (pName ==  "eta")           return kEta;                       
  else if (pName ==  "lambda")        return kLambda;                   
  else if (pName ==  "sigma+")        return kSigmaPlus;                 
  else if (pName ==  "sigma0")        return kSigma0;                   
  else if (pName ==  "sigma-")        return kSigmaMinus;               
  else if (pName ==  "x0")            return kXi0;                       
  else if (pName ==  "xi-")           return kXiMinus;                  
  else if (pName ==  "omega-")        return kOmegaMinus;                
  else if (pName ==  "anti_neutron")  return kAntiNeutron;               
  else if (pName ==  "anti_lambda")   return kAntiLambda;               
  else if (pName ==  "anti_sigma-")   return kAntiSigmaMinus;            
  else if (pName ==  "anti_sigma0")   return kAntiSigma0;               
  else if (pName ==  "anti_sigma+")   return kAntiSigmaPlus;            
  else if (pName ==  "anti_xi0")      return kAntiXi0;                   
  else if (pName ==  "anti_xi-")      return kAntiXiMinus;              //?
  else if (pName ==  "anti_nu_e")     return kElectronAntiNeutrino;     
  else if (pName ==  "nu_e")          return kElectronNeutrino;         
  else if (pName ==  "anti_nu_mu")    return kMuonAntiNeutrino;         
  else if (pName ==  "nu_mu")         return kMuonNeutrino;             
  else                                return kOther;                    
} 


NumiParticleCode::NumiParticleCode_t NumiParticleCode::IntToEnum(G4int particleInt)
{
//returns particle enum
  switch (particleInt)
    {
    case 5:                return kMuonPlus;                break;
    case 6:                return kMuonMinus;               break;
    case 7:                return kPion0;                   break;
    case 8:                return kPionPlus;                break;
    case 9:                return kPionMinus;               break;
    case 10:               return kKaon0L;                  break;
    case 11:               return kKaonPlus;                break;
    case 12:               return kKaonMinus;               break;
    case 13:               return kNeutron;                 break;
    case 14:               return kProton;                  break;
    case 15:               return kAntiProton;              break; 
    case 16:               return kKaon0S;                  break;
    case 17:               return kEta;                     break; 
    case 18:               return kLambda;                  break;
    case 19:               return kSigmaPlus;               break; 
    case 20:               return kSigma0;                  break;
    case 21:               return kSigmaMinus;              break;
    case 22:               return kXi0;                     break; 
    case 23:               return kXiMinus;                 break;
    case 24:               return kOmegaMinus;              break; 
    case 25:               return kAntiNeutron;             break; 
    case 26:               return kAntiLambda;              break;
    case 27:               return kAntiSigmaMinus;          break; 
    case 28:               return kAntiSigma0;              break;
    case 29:               return kAntiSigmaPlus;           break;
    case 30:               return kAntiXi0;                 break; 
    case 31:               return kAntiXiMinus;             break;//?
    case 52:               return kElectronAntiNeutrino;    break;
    case 53:               return kElectronNeutrino;        break;
    case 55:               return kMuonAntiNeutrino;        break;
    case 56:               return kMuonNeutrino;            break;
    default:               return kOther;                   break;
    } 
}
