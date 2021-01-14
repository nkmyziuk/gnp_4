#include "StackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4Track.hh"
#include "Analysis.hh"
//#include "G4SystemofUnits.hh"
//#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
//#include "G4ParticleTypes.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
//#include "G4VProcess.hh"
//#include <iostream>
//#include <string>
//#include <cmath>


StackingAction::StackingAction()
{
}


StackingAction::~StackingAction()
{
}


G4ClassificationOfNewTrack 
StackingAction::ClassifyNewTrack( const G4Track * aTrack ) 
{
  G4ClassificationOfNewTrack result( fUrgent );
  if ( aTrack->GetParentID() > 0 )
  {
		Analysis::GetInstance()->AddSecondary(aTrack->GetDefinition());
    
    // Simply count the number of particles originating in the GNP //
    const G4int trackOrigin = aTrack->GetOriginTouchable()->GetVolume()->GetCopyNo();
    //G4cout << aTrack->GetTrackID() << G4endl; 
    if ((trackOrigin == 2) && (aTrack->GetTrackID() == 2)){Analysis::GetInstance()->GNPcntr();}
    if (trackOrigin == 2){Analysis::GetInstance()->AddSecondaries();}
  }
  else
  {
    Analysis::GetInstance()->SetBeam(aTrack->GetDefinition(),aTrack->GetKineticEnergy());
  }
		
  return result;

}


