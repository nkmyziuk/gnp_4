// $Id: task2.cc 94 2010-01-26 13:18:30Z adotti $
/**
 * @file
 * @brief Main program.
 */

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4Version.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "StackingAction.hh"
#include "EventAction.hh"
#include "QGSP_BERT.hh"
#include "PhysicsList.hh"
#include "Analysis.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"
//#include "G4PhysListFactory.hh"
//#include "G4RadioactiveDecayPhysics.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


/*!
\brief Main program

\callgraph

*/
int main(int argc,char** argv)
{
    // stuff to set the seed for the run so we can do statistical uncertainty.  
	unsigned int startingSeed;

	// set seed using internal random stuff
	std::ifstream f("/dev/urandom");
	f.read(reinterpret_cast<char*>(&startingSeed), sizeof(startingSeed));
	
	G4cout << "Starting seed : " << startingSeed << G4endl;
	f.close();

  // Run manager
  G4RunManager * runManager = new G4RunManager();

  // Convert the starting seed to integer and feed it to the random engine
  CLHEP::HepRandom::setTheSeed(startingSeed);

  // mandatory Initialization classes 
  G4VUserDetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  G4VUserPhysicsList* physics = new PhysicsList(); //new PhysicsList();
  runManager->SetUserInitialization(physics);

  // mandatory User Action classes
  G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction();
  runManager->SetUserAction(gen_action);

  //Optional User Action classes:

  //Stacking Action
  StackingAction* aStackingAction = new StackingAction();
  runManager->SetUserAction(aStackingAction);

  //Event action (handles for beginning / end of event)
  EventAction* anEventAction = new EventAction();
  runManager->SetUserAction( anEventAction );
  
  
  //Stepping Action
  SteppingAction* aSteppingAction = new SteppingAction();
  runManager->SetUserAction(aSteppingAction);

  //Tracking Action
  TrackingAction* aTrackingAction = new TrackingAction();
  runManager->SetUserAction(aTrackingAction);

  //Run action (handles for beginning / end of event):
  RunAction* aRunAction = new RunAction();
  runManager->SetUserAction( aRunAction );


  // Initialize G4 kernel
  //runManager->Initialize();

  // Set up the histograms, start keeping track of simulation time, etc...	
  Analysis* analysis = Analysis::GetInstance();
  analysis->PrepareNewSimulation();
      
  //Initilize the visualization manager
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();
     
  // Get the pointer to the User Interface manager
  //
  G4UImanager * UImanager = G4UImanager::GetUIpointer();  

  if (argc!=1) {  // batch mode  
	  //command line contains name of the macro to execute
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
  }
  else {           // interactive mode : define UI session
     
	  G4UIExecutive * ui = new G4UIExecutive(argc,argv);
	  //If UI has graphics execute special macro: opens OpenGL Qt driver
		UImanager->ApplyCommand("/control/execute visQt.mac");

	  ui->SessionStart();
	  delete ui;
  }


	// Write out histograms, etc...
	analysis->EndSimulation();
	
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete runManager;

  return 0;
}
