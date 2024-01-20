//
// NumiTrackInformation.hh
//
#ifndef NumiTrackInformation_h
#define NumiTrackInformation_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"

class NumiTrackInformation : public G4VUserTrackInformation 
{
  public:
    NumiTrackInformation();
    NumiTrackInformation(const NumiTrackInformation* aTrackInfo);
    virtual ~NumiTrackInformation();
  
    inline void *operator new(size_t);
    inline void operator delete(void *aTrackInfo);
    inline int operator ==(const NumiTrackInformation& right) const
    {return (this==&right);}

    inline G4int GetDecayCode() const 
    {return decay_code;}
    inline void SetDecayCode(G4int decaycode)
    {decay_code=decaycode;} 
  
    inline G4int GetTgen() const 
    {return tgen;}
    inline void SetTgen(G4int tgeneration)
    {tgen=tgeneration;}

    inline G4double GetNImpWt() const 
    {return Nimpwt;}
    inline void SetNImpWt(G4double nimpweight)
    {Nimpwt=nimpweight;}
    
    void Print() const;

    void Print(const G4Track *aTrack) const;

    inline const G4ThreeVector& GetParentMomentumAtThisProduction() const {
        return fParentMomentumAtThisProduction;
    }

    inline void SetParentMomentumAtThisProduction(const G4ThreeVector& mom) {
        fParentMomentumAtThisProduction = mom;
    }

private:
    G4int              decay_code;
    G4int              tgen;
    G4double           Nimpwt;
    G4ThreeVector      fParentMomentumAtThisProduction;

};

extern G4Allocator<NumiTrackInformation> aTrackInformationAllocator;

inline void* NumiTrackInformation::operator new(size_t)
{ void* aTrackInfo;
 aTrackInfo = (void*)aTrackInformationAllocator.MallocSingle();
 return aTrackInfo;
}

inline void NumiTrackInformation::operator delete(void *aTrackInfo)
{ aTrackInformationAllocator.FreeSingle((NumiTrackInformation*)aTrackInfo);}

#endif

