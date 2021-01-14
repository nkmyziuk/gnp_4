#include "SteppingAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"
#include "Analysis.hh"
#include "G4ThreeVector.hh"
#include "G4DynamicParticle.hh"
#include "G4SystemOfUnits.hh"
//#include "TrackInformation.hh"

//note
SteppingAction::SteppingAction()
{
}

SteppingAction::~SteppingAction()
{
}

void SteppingAction::UserSteppingAction( const G4Step * theStep ) 
{
	//Get the track to which the step belongs
	G4Track * theTrack = theStep->GetTrack();

	//check if the track is a secondary
	if(theTrack->GetParentID()==0) { return; }
	
	//check if there is track information attached, if so the track originated in the nanoparticle and was labeled in TrackingAction
	G4bool nanoStart = false;
	if(theTrack->GetUserInformation()!=0) nanoStart = true;
	
	//get the particle type
	const G4ParticleDefinition* part = theTrack->GetDefinition();

	//get the total energy
	G4double edep = theStep->GetTotalEnergyDeposit();

	//get the location where the energy was deposited 
	G4ThreeVector r1 = theStep->GetPreStepPoint()->GetPosition();
	G4ThreeVector r2 = theStep->GetPostStepPoint()->GetPosition();
	G4ThreeVector r  = r1 + G4UniformRand()*(r2 - r1);
	G4VPhysicalVolume *vol = theStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
	
	//get current volume
	const G4VTouchable *cTouchable = theStep->GetPreStepPoint()->GetTouchable();
	G4int cVolCopyNum = cTouchable->GetVolume()->GetCopyNo();
	if (cVolCopyNum >= 1)
	{
		Analysis::GetInstance()->AddEDepE(edep, r, nanoStart, cVolCopyNum, part);
	}

	//check if the track is dead
	if(theTrack->GetTrackStatus()!=fAlive) //track is dead, get its final length
	 {
		if(part != G4Gamma::Gamma()) {   // only track length of secondary electrons/positrons
			G4double trackLength = theTrack->GetTrackLength();
			Analysis::GetInstance()->AddTLength(trackLength);
			//Analysis::GetInstance()->FillNtuple(part);
			}
	 }

	// Check step length - turn off for production mode
	 G4double steplen = theStep->GetStepLength();
	 if (theTrack->GetDefinition()->GetPDGCharge() != 0.)
	 {
		//G4cout << steplen/nanometer << G4endl;
		Analysis::GetInstance()->AddStepLength(steplen);
	 }
	 // ---------------- ///
}

