//
// NumiTrajectory.hh
//

#ifndef NumiTrajectory_h
#define NumiTrajectory_h 1

#include "G4VTrajectory.hh"
#include "G4Allocator.hh"
#include <stdlib.h>
#include "G4ThreeVector.hh"
#include "G4ios.hh"     
#include <vector>
#include <iostream>
#include "globals.hh" 
#include "G4ParticleDefinition.hh" 
#include "G4TrajectoryPoint.hh"   
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4UserSteppingAction.hh"
#include "G4Colour.hh"

class NumiTrajectory;
class G4Polyline;
class NumiDataInput;

typedef std::vector<G4VTrajectoryPoint*> NumiTrajectoryPointContainer;
typedef std::vector<G4ThreeVector> NumiTrajectoryMomentumContainer;
typedef std::vector<G4String> NumiTrajectoryVolumeName;
typedef std::vector<G4double> DVec;

class NumiTrajectory : public G4VTrajectory
{
 public:
   NumiTrajectory();
   NumiTrajectory(const G4Track* aTrack);
   NumiTrajectory(NumiTrajectory &);
   virtual ~NumiTrajectory();

   inline void* operator new(size_t);
   inline void  operator delete(void*);
   inline int operator == (const NumiTrajectory& right) const
   {return (this==&right);} 

   inline G4int GetTrackID() const
   { return fTrackID; }
   inline G4int GetParentID() const
   { return fParentID; }
   inline G4String GetParticleName() const
   { return fParticleName; }
   inline G4double GetCharge() const
   { return fPDGCharge; }
   inline G4double GetMass() const
   { return fParticleMass; }
   inline G4int GetPDGEncoding() const
   { return fPDGEncoding; }
   inline const G4ThreeVector& GetVertexPosition() const
   { return fVertexPosition; }
   virtual int GetPointEntries() const
   { return fPositionRecord->size(); }
   virtual G4VTrajectoryPoint* GetPoint(G4int i) const 
   { return (*fPositionRecord)[i]; }
   virtual G4ThreeVector GetMomentum(G4int i) const
   { return (*fMomentumRecord)[i]; }
   virtual G4String GetPreStepVolumeName(G4int i) const
   { return (*fPreStepVolume)[i]; }
   inline G4ThreeVector GetInitialMomentum() const 
   { return fMomentum; }                            
   virtual G4int GetTgen() const
   { return fTgen;}
   inline G4int GetDecayCode() const
   { return fDecayCode;}
   virtual G4double GetNImpWt() const
   { return fNImpWt;}
   virtual G4double GetStepLength(G4int i) const
      {return (*fStepLength)[i];}

    const G4String& GetProcessName() const { return fProcessName;}
    void SetProcessName(const G4String& name) { fProcessName = name;}

    inline const G4ThreeVector& GetParentMomentumAtThisProduction() const {
        return fParentMomentumAtThisProduction;
    }

    inline void SetParentMomentumAtThisProduction(const G4ThreeVector& mom) {
        fParentMomentumAtThisProduction = mom;
    }
  //for dk2nu:
  inline G4double GetTime() const
   { return fTime; }
  
   virtual void ShowTrajectory() const;
   virtual void ShowTrajectory(std::ostream& o) const;
   virtual void DrawTrajectory(G4int i_mode=0) const;
   virtual void AppendStep(const G4Step* aStep);
   virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

   G4ParticleDefinition* GetParticleDefinition();

 private:
   NumiTrajectoryMomentumContainer* fMomentumRecord;
   NumiTrajectoryPointContainer*    fPositionRecord;
   G4int                            fDecayCode;
   G4int                            fEventNo;
   G4int                            fTgen;
   G4double                         fNImpWt;
   G4int                            fTrackID;
   G4int                            fParentID;
   G4ParticleDefinition*            fParticleDefinition;
   G4String                         fParticleName;
   G4double                         fPDGCharge;
   G4int                            fPDGEncoding;
   G4ThreeVector                    fMomentum;
   G4ThreeVector                    fVertexPosition;
   G4double                         fParticleMass;
   NumiTrajectoryVolumeName*        fPreStepVolume;
   DVec*                            fStepLength;
   G4String                         fProcessName;
  /// Momentum of the parent particle when producing this particle
   G4ThreeVector      fParentMomentumAtThisProduction; 
  //New for dk2nu:
  G4double                         fTime;
    

   NumiDataInput* fND;
};
extern G4Allocator<NumiTrajectory> myTrajectoryAllocator;

inline void* NumiTrajectory::operator new(size_t)
{
  void* aTrajectory;
  aTrajectory = (void*)myTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void NumiTrajectory::operator delete(void* aTrajectory)
{
  myTrajectoryAllocator.FreeSingle((NumiTrajectory*)aTrajectory);
}
#endif



