//
//NumiEventAction.cc
//

//C++
#include <string>
 
#include "NumiEventAction.hh"
#include "NumiAnalysis.hh"
#include "NumiDataInput.hh"
#include "NumiTrajectory.hh"

#include "G4Event.hh"
#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"

//#define USEMODGEANT4
#ifdef USEMODGEANT4 

#include "MinervaElementInter.hh"

#endif

//------------------------------------------------------------------------------------- 
NumiEventAction::NumiEventAction()
   :fTrajectoryContainer(0),
    fSplitDRayIDcounter(1000000)
{
   NumiData = NumiDataInput::GetNumiDataInput();
}

//-------------------------------------------------------------------------------------
NumiEventAction::~NumiEventAction()
{
}

//-------------------------------------------------------------------------------------
G4TrajectoryContainer* NumiEventAction::GetTrajectoryContainer()
{
   return fTrajectoryContainer;
}

//-------------------------------------------------------------------------------------
void NumiEventAction::BeginOfEventAction(const G4Event* evt)
{
  if(NumiData->GetDebugLevel() > 1) 
    { 
      G4cout << "NumiEventAction::BeginOfEventAction called...Beginning Event #" << evt-> GetEventID() << G4endl;
    }

   if (NumiData->useMuonBeam && NumiData->GetSimDRays())
   {
      if (!fTrackMap.empty() || !fZPosMap.empty())
      {
         G4cerr << "NumiEventAction::BeginOfEventAction - Track Map or ZPos Map not empty" << G4endl;
      }
       
   }

   fSplitDRayIDcounter = 1000000;
      
}

//-------------------------------------------------------------------------------------
void NumiEventAction::AddTrack(const G4Track* aTrack)
{
   //
   //The muon has parentID = 0
   //and track ID = 1
   //
   
   G4int parentID = aTrack->GetParentID();
   G4int trackID = aTrack->GetTrackID();

   if(parentID == 0) return;
   if(trackID == 1)
   {
      G4cerr << "NumiEventAction::AddTrack - Problem: Logic Error Somewhere track should never be 1 here" << G4endl;
   }

   G4int IntExt;
   G4double zpos;
   
   IIMap::const_iterator mit = fTrackMap.find(trackID);
   if(mit != fTrackMap.end())
   {
      //if(!(aTrack->GetWeight() < 1.0))
      //{
         G4cerr << "NumiEventAction::AddTrack - Problem: Track " << trackID << " already exists in map" << G4endl;
         IntExt = 99;
         //}
         //else
         //{
         ////
         ////don't need to do anything, IntExt and zpos already exist for this track
         ////
         //}
         
   }
   else
   {
      //
      //if immediate parent is the muon
      //
      if(parentID == 1)
      {
         G4String volName = aTrack->GetVolume()->GetName();
         zpos = aTrack -> GetPosition()[2];
         IntExt = NumiEventAction::GetIntExt(volName);
      }
      else
      {
         IIMap::const_iterator vit = fTrackMap.find(parentID);
         if(vit == fTrackMap.end())
         {
            G4cerr << "NumiEventAction::AddTrack - Problem: No Entry with ID " << parentID << " exists in track map" << G4endl;
            IntExt = 99;
         }
         else
            IntExt = vit -> second;


         //
         //if the muon interation IntExt == 1 but the current
         //interaction IntExt == 0 then it is an ext dray
         //
         if(IntExt == 1)
         {  
            G4String volName = aTrack->GetVolume()->GetName();
            G4double currIntExt = NumiEventAction::GetIntExt(volName);
            if(currIntExt == 0)
            {
               IntExt = 0;
            }
         }

         
         IDMap::const_iterator zit = fZPosMap.find(parentID);
         if(zit == fZPosMap.end())
         {
            G4cerr << "NumiEventAction::AddTrack - Problem: No Entry with ID " << parentID << " exists in zpos map" << G4endl;
            zpos = -99999.0;
         }
         else
            zpos = zit -> second;
      }


      fTrackMap.insert(IIMap::value_type(trackID, IntExt));
      fZPosMap.insert(IDMap::value_type(trackID, zpos));
   }
   
   

   /*
   //if(aTrack->GetVolume()->GetName() == "MuCell")
   //{
      G4cout << "Track = " << aTrack->GetTrackID()
             << " ParentID = " << aTrack->GetParentID()
             << " Particle Name = " << aTrack->GetDefinition()->GetParticleName()
             << " pre step vol = " << aTrack->GetVolume()->GetName() 
             << " intext = " << IntExt << G4endl;
      //}

      G4int eid = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
      
      G4TrajectoryContainer* container = 
         G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetTrajectoryContainer();
      if(container==0)
      {
         G4cout << "Trajectory constiner is invalid " << G4endl;
      }

      G4cout << G4endl;
      
      TrajectoryVector* vect = container->GetVector();
      G4VTrajectory* tr;
      G4int i = 0; 
      while (i < G4int(vect->size()))
      {
         G4cout << "Trajectory " << i << ":";
         
         tr = (*vect)[i]; 
         NumiTrajectory* tr1 = (NumiTrajectory*)(tr);

         G4int npts = tr1 -> GetPointEntries();

         G4cout << " trk id = " << tr1 -> GetTrackID() << " par id = " << tr1 ->GetParentID() << " npts = " << npts;

         for(int p = 0; p < npts; ++p)
         {
            G4cout << " " << tr1 -> GetPreStepVolumeName(p);
         }
         G4cout << G4endl;

         ++i;
      }
      
      G4cout << "*************************"<< G4endl;
   */
}

//-------------------------------------------------------------------------------------
G4int NumiEventAction::GetIntExt(const G4String &volName)
{
   G4int IntExt = 0; // 0 is ext
   
   if(volName == "FrontTubePlate" ||
      volName == "LeftSidePlate" ||
      volName == "FrontIonChamberPlate" ||
      volName == "ChamberLayer" ||
      volName == "MuCell" ||
      volName == "BackIonChamberPlate" ||
      volName == "RightSidePlate" ||
      volName == "BackTubePlate" ||
      volName.find("MuMon_") != std::string::npos)
   {
      //
      //identify this particle as and internal dRay
      //
      IntExt = 1;
   }

   return IntExt;
}

//-------------------------------------------------------------------------------------
G4int NumiEventAction::GetIntExt(const G4Track* aTrack)
{
   G4int IntExt = 0; // 0 is ext

   G4int trackID = aTrack->GetTrackID();
   
   IIMap::const_iterator vit = fTrackMap.find(trackID);
   if(vit == fTrackMap.end())
   {
      G4cerr << "NumiEventAction::GetIntExt - Problem: No Entry with track ID " << trackID << " exists in map" << G4endl;
      IntExt = 99;
   }
   else
      IntExt = vit -> second;
   
   
   return IntExt;
}

//-------------------------------------------------------------------------------------
G4double NumiEventAction::GetZPos(const G4Track* aTrack)
{
   G4double zpos = -99999.0; // 0 is ext

   G4int trackID = aTrack->GetTrackID();
   
   IDMap::const_iterator zit = fZPosMap.find(trackID);
   if(zit == fZPosMap.end())
   {
      G4cerr << "NumiEventAction::GetZPos - Problem: No Entry with track ID " << trackID << " exists in map" << G4endl;
      zpos = -99999.0;
   }
   else
      zpos = zit -> second;
   
   
   return zpos;
}

//-------------------------------------------------------------------------------------
void NumiEventAction::EndOfEventAction(const G4Event* evt)
{
  if(NumiData->GetDebugLevel() > 1) 
    { 
      G4cout << "NumiEventAction::EndOfEventAction called...Ending Event # " << evt-> GetEventID() << G4endl;
    }

   
   if(NumiData->createHadmmNtuple)
   {
      NumiAnalysis* analysis = NumiAnalysis::getInstance();
      analysis->WriteHadmmNtuple();
   }
   
   if (NumiData->useMuonBeam && NumiData->GetSimDRays())
   {
      fTrackMap.clear();
      fZPosMap.clear();
   }

#ifdef USEMODGEANT4 
   
   MinervaElementInter*  this_elementinter =  MinervaElementInter::getInstance();
   this_elementinter->CleanInfo();
   
#endif

}

