// $Id: PrimaryGeneratorAction.hh 94 2010-01-26 13:18:30Z adotti $

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

/**
 * @file
 * @brief Defines mandatory user class PrimaryGeneratorAction.
 */

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include <fstream>


class G4VPrimaryGenerator;
 
/*!
\brief This mandatory user class provides the primary particle generator

Geant4 provides a number of predefined primary particle generator, to be utilised by the user.
 - G4ParticleGun
 - G4GeneralParticleSource

\sa GeneratePrimaries()
 */
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  //! constructor
  PrimaryGeneratorAction();
  //! destructor
  ~PrimaryGeneratorAction();
  //! defines primary particles (mandatory)
  void GeneratePrimaries(G4Event*);
private:  
    G4GeneralParticleSource* InitializeGPS();
private:
  G4GeneralParticleSource* gun;
  std::ofstream * outfile;
	double posX;
	double posY;
	double posZ;
	double kX;
	double kY;
	double kZ;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
