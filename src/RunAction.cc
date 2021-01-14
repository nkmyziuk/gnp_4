// $Id: RunAction.cc 102 2010-01-26 16:45:56Z adotti $
/**
 * @file   RunAction.cc
 *
 * @date   17 Dec 2009
 * @author adotti
 *
 * @brief  Implements user class RunAction.
 */

#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "Analysis.hh"
#include "G4Timer.hh"

RunAction::RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run* aRun )
{
	timer.Start();
	
	G4cout<<"Starting Run: "<<aRun->GetRunID()<<G4endl;
	G4cout<<"--------------------------------"<<G4endl;
	Analysis::GetInstance()->PrepareNewRun(aRun);
}

void RunAction::EndOfRunAction( const G4Run* aRun )
{
	Analysis::GetInstance()->EndOfRun(aRun);
	timer.Stop();
	
	G4cout<<"Total Time for Run: " <<timer<<G4endl;
	G4cout<<"--------------------------------"<<G4endl<<G4endl;
}
