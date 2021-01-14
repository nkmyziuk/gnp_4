//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// This example is provided by the Geant4-DNA collaboration
// Any report or published results obtained using the Geant4-DNA software 
// shall cite the following Geant4-DNA collaboration publication:
// Med. Phys. 37 (2010) 4692-4708
// The Geant4-DNA web site is available at http://geant4-dna.org
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "Analysis.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4ThreeVector.hh"
#include "G4GeometryManager.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:G4VUserDetectorConstruction(),
 fpWaterMaterial(0),fpRegion(0)
{}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()

{
  DefineMaterials();
  return ConstructDetector();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{ 

  // Water is defined from NIST material database
  G4NistManager * man = G4NistManager::Instance();
  G4Material * H2O = man->FindOrBuildMaterial("G4_WATER");
  air = man->FindOrBuildMaterial("G4_AIR");
  gold = man->FindOrBuildMaterial("G4_Au");

  // Default materials in setup.
  fpWaterMaterial = H2O;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructDetector()
{
  // WORLD VOLUME
  
  G4double worldSizeX  = 200*um;
  G4double worldSizeY  = 200*um;
  G4double worldSizeZ  = 200*um;

  G4VSolid* solidWorld = new G4Box("World",         //its name
                          worldSizeX/2,
                          worldSizeY/2,
                          worldSizeZ/2);  //its size

  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,  //its solid
                                    air,  //its material
                                    "World");    //its name

  G4VPhysicalVolume* physiWorld = new G4PVPlacement(0,      //no rotation
                                  G4ThreeVector(),  //at (0,0,0)
                                  "World",    //its name
                                  logicWorld,    //its logical volume
                                  0,      //its mother  volume
                                  false,      //no boolean operation
                                  0);      //copy number

  // Water Tank Definition //
  G4Box *waterTankSV = new G4Box("waterTankSolid", 5*um, 5 *um, 5* um);
  G4LogicalVolume *waterTankLV = new G4LogicalVolume(waterTankSV, fpWaterMaterial, "waterTankLogic");
  G4VPhysicalVolume *waterTankPV = new G4PVPlacement(0, -G4ThreeVector(0, 0, 0),"waterTank", waterTankLV, physiWorld, false, 1);

  // Surrounding Sphere of H2O for analysis /Mteria
  G4Sphere *H20SV = new G4Sphere("Target", 0.01* um, 0.5 * um, 0 * deg, 360 * deg, 0 * deg, 360 * deg);
  G4LogicalVolume *h2oLV = new G4LogicalVolume(H20SV, fpWaterMaterial, "LogicSphere");
  G4VPhysicalVolume *h2oPV = new G4PVPlacement(0, -G4ThreeVector(0, 0, 0), "TargetROI", h2oLV, waterTankPV, false, 3);

  // Gold Sphere Definition //
  G4Sphere *gnpSV = new G4Sphere("GNP", 0, 0.01 * um, 0 * deg, 360 * deg, 0 * deg, 360 * deg);
  G4LogicalVolume *gnpLV = new G4LogicalVolume(gnpSV,gold, "GNP");
  G4VPhysicalVolume *gnpPV = new G4PVPlacement(0, -G4ThreeVector(0, 0, 0),"GNP",gnpLV, waterTankPV, false, 2);


  // Visualization attributes

  G4Color water(0, 0, 1);
  waterTankLV->SetVisAttributes(new G4VisAttributes(water));
  h2oLV->SetVisAttributes(new G4VisAttributes(water));

  G4Color goldColor(0.906, 0.773, 0.424);
  gnpLV->SetVisAttributes(new G4VisAttributes(goldColor));


  // Create Target G4Region and add logical volume

  fpRegion = new G4Region("TargetROI");

  G4ProductionCuts* cuts = new G4ProductionCuts();

  G4double defCut = 1*nanometer;
  cuts->SetProductionCut(defCut,"gamma");
  cuts->SetProductionCut(defCut,"e-");
  cuts->SetProductionCut(defCut,"e+");
  cuts->SetProductionCut(defCut,"proton");

  fpRegion->SetProductionCuts(cuts);
  fpRegion->AddRootLogicalVolume(h2oLV); 

  return physiWorld;
}
