// $Id: PrimaryGeneratorAction.cc 94 2010-01-26 13:18:30Z adotti $
/**
 * @file
 * @brief implements mandatory user class PrimaryGeneratorAction
 */

#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include <math.h>
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction()
    : outfile(0)
{

  gun = InitializeGPS();      // set the spectrum of the source and the type of source (point).
  posX = 0*micrometer;        // start counter at lower left quadrant of desired square aperature
  posY = 0*micrometer;
  kX = 0*micrometer;
  kY = 0*micrometer;
  kZ = 100*micrometer;        // set distance from virtual source point to aperature plane (which is the z-component of the momentum vector)
  posZ = 200*micrometer;      // set position of the aperature plane w.r.t. the surface of the phantom (first number if total SSD)
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{

  //set the position of the starting point within the desired beam aperature
  gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(posX , posY , posZ));

  // set initial momentum such that the gammas originating from the aperature all have the same virtual origin behind the aperature plane
  gun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0, 0, -1));

  // generate event
  gun->GeneratePrimaryVertex(anEvent);
  
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete gun;
}


G4GeneralParticleSource* PrimaryGeneratorAction::InitializeGPS()
{
  G4GeneralParticleSource * gps = new G4GeneralParticleSource();
  // particle type
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* gamma = particleTable->FindParticle("gamma");  
  gps->GetCurrentSource()->SetParticleDefinition(gamma);

  // DEFINE A MONO-ENERGETIC SOURCE
  G4SPSEneDistribution *eneDist = gps->GetCurrentSource()->GetEneDist();
  eneDist->SetEnergyDisType("Mono"); eneDist->SetMonoEnergy(0.040*MeV); 
  
  // set position distribution
  G4SPSPosDistribution *posDist = gps->GetCurrentSource()->GetPosDist();
  posDist->SetPosDisType("Plane"); 
  posDist->SetPosDisShape("Square");
  posDist->SetHalfX(5*micrometer); // or Point,Plane,Volume,Beam
  posDist->SetHalfY(5*micrometer); // or Point,Plane,Volume,Beam
  return gps;
}


