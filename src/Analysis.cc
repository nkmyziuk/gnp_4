/*
 * Analysis.cc
 *
 *  Created on: 9 Feb 2010
 *      Author: adotti
 */
#define _USE_MATH_DEFINES
#include <math.h>

#include "Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4ThreeVector.hh"
#include "G4Timer.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

//AIDA Stuff
#include "AIDA/AIDA.h"

Analysis* Analysis::singleton = NULL;

Analysis::Analysis() :
    aida(0),
	tree(0)
{
}


//------------------- Initialization and Cleanup -----------------//

void Analysis::PrepareNewSimulation()
{
	// global (simulation) variables: 
	totalRuns = 0;
	gnp_cnt = 0;
	gnp_scndry = 0;
	thisSimE = 0; 
	thisSimDose = 0; 

	timer.Start();
	G4cout<<"Starting Simulation... "<<G4endl;

	//Create t-r.y() he Analysis Factory
	aida = AIDA_createAnalysisFactory();
	if(!aida) { G4cout << " Problem creating the AIDA analysis factory."<< G4endl; return; }
	
	// Create an over-arching family of histograms that will be filled with multiple runs and deleted before the runmanager
	//Create an empty histogram
		
	//Create the Tree Factory
	AIDA::ITreeFactory* tf = aida->createTreeFactory();
	if(!tf) { G4cout << " Problem creating the AIDA tree factory."<< G4endl; return; }
	//Create the Tree
	tree = tf->create("Output.aida","xml",false,true);
	
	delete tf;
	if(!tree) { G4cout << " Problem creating the AIDA tree."<< G4endl; return; }
	
	//Create empty histograms
	//-----------------------
	//
	//Create histogram factory
	AIDA::IHistogramFactory* hf = aida->createHistogramFactory(*tree);
	if(!hf) { G4cout << " Problem creating the AIDA histogram factory."<< G4endl; return; }
	
	// create histograms in subdirectory "histograms"
	//
	tree->mkdir("histograms");tree->cd("histograms");

	hPrimarySpectrum = hf->createHistogram1D("EnergySpectrum","Incoming Primary Energy Spectrum",100,0,100);
	rProfile = hf->createHistogram1D("Radial Profile", "Radial Profile", 150 /*numBins*/, 0 /*lowerEdge*/, 300 /*upperEdge*/);
	
	// Radial Dose Profile, and scale dose profile per ionization 	
	gnpDoseProfile = hf->createHistogram1D("GNP Radial Dose Profile", "GNP Radial Dose Profile", 150 /*numBins*/, 0 /*lowerEdge*/, 300 /*upperEdge*/);
	gnpProfile = hf->createHistogram1D("GNP Radial Energy Profile", "GNP Radial Energy Profile", 150 /*numBins*/, 0 /*lowerEdge*/, 300 /*upperEdge*/);
	gnpScaledProfile = hf->createHistogram1D("GNP Radial Dose Profile per Ionization", "GNP Radial Dose Profile per Ionization", 150 /*numBins*/, 0 /*lowerEdge*/, 300 /*upperEdge*/);
	
	/// Monitor Step and Track length: 
	hStepLength = hf->createHistogram1D("StepLength", "Step Length of Secondary Electrons", 500/*numBins*/, 0/*lowerEdge*/, 1 /*upperEdge*/);
	hTrackLength = hf->createHistogram1D("TrackLength", "Track Length of Secondary Electrons", 500/*numBins*/, 0/*lowerEdge*/, 1 /*upperEdge*/);
	tree->cd("..");
	
    delete hf;
	
}

void Analysis::PrepareNewRun(const G4Run* /*aRun*/ )
{
	//Reset variables relative to the run
	totalRuns++;
	thisRunE = 0;
	n_gamma = 0;
	n_electron = 0;
	n_positron = 0;	
	thisRunTotSecondaries = 0; 	
}

void Analysis::PrepareNewEvent(const G4Event* /*anEvent*/)
{
	//Reset variables relative to this event
	thisEventSecondaries = 0;
	thisEventE = 0;
	thisEventEfromAu =0; 
}

void Analysis::EndOfEvent(const G4Event* anEvent)
{
	//Accumulate over the run
	thisRunE += thisEventE;
	thisSimE += thisEventE;
	thisSimDose += ((thisEventE/keV)*(0.0000001602));
	thisRunEfromAu += thisEventEfromAu; 

}

void Analysis::EndOfRun(const G4Run* aRun)
{
	//Some print outs
	G4int numEvents = aRun->GetNumberOfEvent();

	G4cout<<G4endl<<G4endl<<"Summary for run: "<<aRun->GetRunID()<<G4endl;
	G4cout<<"\t Event processed: "<<numEvents<<G4endl;
	G4cout<<"\t Number of secondaries: "<<thisRunTotSecondaries<<G4endl;
	G4cout<<"\t Average number of secondaries: "<< double(thisRunTotSecondaries)/double(numEvents)<<G4endl;
    G4cout<<"\t Total energy deposited: "<<G4BestUnit(thisRunE,"Energy")<<G4endl;
	G4cout << "\t Number ionizations of the GNP: " << gnp_cnt << G4endl;
	G4cout << "\t Number secondaries generated within the GNP: " << gnp_scndry << G4endl;
}

void Analysis::EndSimulation()
{
	G4double histo_scalar = (1/(double)gnp_cnt);
	G4cout << histo_scalar << G4endl; 
	gnpScaledProfile->scale(histo_scalar);

	// Write out histograms to AIDA file then delete them and associated tree and factory
	tree->commit();
	tree->close();

	delete hPrimarySpectrum;
	delete rProfile;
	delete gnpScaledProfile;
	delete gnpDoseProfile;
	delete hTrackLength;
	delete hStepLength;
	//delete fNtuple1;

	delete tree;
	delete aida;

	hPrimarySpectrum = 0;
	rProfile = 0;
	gnpScaledProfile = 0;
	gnpDoseProfile = 0;
	hTrackLength = 0;
	hStepLength = 0;
	//fNtuple1 =0;

	tree = 0;
	aida = 0;

	timer.Stop();

	G4cout << "Combined results of interest for the entire simulation: " << G4endl; 
	G4cout << "\t Total energy deposited: " << G4BestUnit(thisSimE, "Energy") << G4endl;
	G4cout << "\t Total dose deposited: " << thisSimDose << "Gy" << G4endl;
	G4cout << "\t Total Number ionizations of the GNP: " << gnp_cnt << G4endl;
	G4cout << "\t Total Number secondaries generated from within the GNP: " << gnp_scndry << G4endl;
	G4cout << "Total number of Runs: " << totalRuns << G4endl;
	G4cout << "Total time for simulation: " << timer << G4endl;
}

// ---------------- Functions for Analysis -------------------//

void Analysis::AddSecondary(const G4ParticleDefinition* part)
{
    thisRunTotSecondaries++;
	if(part == G4Gamma::Gamma()) { ++n_gamma; }
	else if(part == G4Electron::Electron()) { ++n_electron; }  
	else if(part == G4Positron::Positron()) { ++n_positron; }
}

void Analysis::GNPcntr()
{++gnp_cnt;}

void Analysis::AddSecondaries()
{++gnp_scndry;}

void Analysis::AddEDepE(G4double edep, G4ThreeVector r, G4bool nanoStart, G4int dcopyno, const G4ParticleDefinition* part)
{
	// add energy counter
	thisEventE += edep;
	
	// Poor mans spherical shell calc
	G4int rInnerShell = floor(r.mag()/nm);
	G4int rOuterShell = rInnerShell +2; 
	G4double rShellVol = (4/3) * M_PI * (pow(rOuterShell,3) - pow(rInnerShell,3));


	//Convert keV -> J and cc -> nm to arrive a j/kg
	G4double eConversion = 160200000;
	G4double dcf = (eConversion / rShellVol);	
	// Radial dose profile for all particles 
	
	if(dcopyno==2) 
		{rProfile->fill(r.mag()/nm, (edep/19.30)/keV);}
	else 
		{rProfile->fill(r.mag()/nm, edep/keV);}

	
	// Radial profile of the particles created by the GNP:
	if (nanoStart==true)
	{	
	//	if (r.mag()/nm <= 300)
	//		{G4cout << "Inner Shell: " << rInnerShell << " Outer Shell :" << rOuterShell << G4endl;}
		if(dcopyno==2) 
		{
			gnpProfile->fill(r.mag()/nm, (edep/19.30)/keV);
			gnpDoseProfile->fill(r.mag() / nm, ((edep / 19.30) / keV)*dcf);
			gnpScaledProfile->fill(r.mag() / nm, ((edep / 19.30) / keV) * dcf);
		}
		else 
		{
			gnpProfile->fill(r.mag()/nm, edep/keV);
			gnpDoseProfile->fill(r.mag() / nm, (edep/ keV) * dcf);
			gnpScaledProfile->fill(r.mag() / nm, (edep / keV) * dcf);
		}
		
		thisEventEfromAu += edep;
	}
}

void Analysis::AddStepLength(G4double stepLength)
{
	hStepLength->fill(stepLength /um);
}

void Analysis::AddTLength(G4double tLength)
{
	hTrackLength->fill( tLength/um );	
}


void Analysis::SetBeam(const G4ParticleDefinition *part, G4double energy)
{
	beamParticle = part;
	beamEnergy = energy;
	hPrimarySpectrum->fill(energy/keV);
}