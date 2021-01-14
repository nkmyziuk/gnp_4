/*
 * @file Analysis.hh
 *
 * @date: 9 Feb 2010
 * @author: adotti
 *
 * \brief Analysis class
 */

#ifndef ANALYSIS_HH_
#define ANALYSIS_HH_

#include "G4Event.hh"
#include "G4Run.hh"
#include "G4ThreeVector.hh"
#include "G4Timer.hh"

//AIDA Histograms
#include "AIDA/AIDA.h"

/*!
 * \brief Analysis class
 * This class contains the code to collect information from
 * the different UserActions.
 * The class is designed as a singleton.
 * To access it you need to use:
 * Analysis* analysis = Analysis::GetInstance()
 */

class Analysis {

private:
	//! Private construtor: part of singleton pattern
	Analysis();
	//! Singleton static instance
	static Analysis* singleton;
	
	// Timer
	G4Timer timer;
	G4int totalRuns;
	
	//! Number of secondaries 
	G4int thisEventSecondaries;
	G4int thisRunTotSecondaries;
	
	// Energy counters
	G4double thisRunE;
	G4double thisSimE;
	G4double thisSimDose;
	G4double thisEventE;
	G4double thisRunEfromAu;
	G4double thisEventEfromAu;
	
	// Particle counters
	G4int n_gamma;
	G4int n_electron;
	G4int n_positron;
	G4int gnp_cnt;
	G4int gnp_scndry;
	
	// beam and calorimeter geometry
	const G4ParticleDefinition* beamParticle;
	G4double beamEnergy;
	G4ThreeVector nanoPosition;
	//! AIDA Analysis factory
	AIDA::IAnalysisFactory *aida;
	//! AIDA Tree
	AIDA::ITree *tree;
	//! Primary spectrum to make sure we have the right thing
	AIDA::IHistogram1D* hPrimarySpectrum;
	//! Radial Energy Profile
	AIDA::IHistogram1D *gnpProfile;
	//Radial Dose Profile
	AIDA::IHistogram1D *gnpDoseProfile;
	//! Radial Dose Profile Scaled per ionization
	AIDA::IHistogram1D *gnpScaledProfile;

	//! Radial Profile
	AIDA::IHistogram1D* rProfile;
	//! Step Length / Track Length Histo
	AIDA::IHistogram1D* hStepLength;
	AIDA::IHistogram1D* hTrackLength;
	//! Step Length / Track Length Histo
	//AIDA::ITuple* fNtuple1;
	//AIDA::ITuple* fNtuple2;

public:
	//! Singleton pattern
	static Analysis* GetInstance() {
		if (singleton == NULL) singleton = new Analysis();
		return singleton;
	}
	//! destructor
	virtual ~Analysis() {};
	//! Should be called in main program after runmanager initialized
	void PrepareNewSimulation();
	//! Should be called at the end of all Runs in the main program
	void EndSimulation();
	//! Should be called at the beginning of an event
	void PrepareNewEvent(const G4Event *anEvent);
	//! Should be called at the end of an event
	void EndOfEvent(const G4Event *anEvent);
	//! Should be called at the beginning of a run
	void PrepareNewRun(const G4Run *aRun);
	//! Should be called at the end of a run
	void EndOfRun(const G4Run *aRun);
	//! Increase number of secondaries
	void AddSecondary(const G4ParticleDefinition *part);
	//! GNP counter
	void GNPcntr();
	void SetBeam( const G4ParticleDefinition* part, G4double energy );
	void AddEDepE(G4double edep, G4ThreeVector r, G4bool nanoStart, G4int dcopyno, const G4ParticleDefinition* part);
	void SetNanoPosition( G4ThreeVector input );
	void AddTLength(G4double trackLength);
	void AddStepLength(G4double stepLength);
	void AddSecondaries();
	
	//! Nutples are for exporting histogram data
	//void FillNtuple(const G4ParticleDefinition *part);
};

#endif /* ANALYSIS_HH_ */
