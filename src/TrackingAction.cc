// $Id: RunAction.cc 102 2010-01-26 16:45:56Z adotti $
/**
 * @file   RunAction.cc
 *
 * @date   17 Dec 2009
 * @author adotti
 *
 * @brief  Implements user class RunAction.
 */

#include "TrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "TrackInformation.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"

TrackingAction::TrackingAction():counter(0)
{;}

TrackingAction::~TrackingAction()
{;}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{

	// Check if the track already has track information
	if(aTrack->GetUserInformation()!=0) return;
	
	// Check if the track has the creator process (not the case for primaries)
	if(aTrack->GetParentID()==0) return;
	
	// Create a track information and attach it to the track if its originated from the nanolayer
	const G4VTouchable* oTouchable = aTrack->GetOriginTouchable();
	if(oTouchable->GetVolume()->GetCopyNo()==2)
	{
		TrackInformation* anInfo = new TrackInformation(aTrack);
		G4Track* theTrack = (G4Track*)aTrack;
		theTrack->SetUserInformation(anInfo);
	}

}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
	// Check if the track already has track information
	TrackInformation* info = (TrackInformation*)(aTrack->GetUserInformation());
	if(info==0) return;
	
	// Copy nt's track information to its daughters
	G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
	if(secondaries)
	{
		size_t nSeco = secondaries->size();
		for(size_t i=0;i<nSeco;i++)
		{ 
			TrackInformation* infoNew = new TrackInformation(info);
			(*secondaries)[i]->SetUserInformation(infoNew);
			counter++;
			//G4cout << counter << G4endl; 
		}
	}
	
}
