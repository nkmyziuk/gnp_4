// Detector construction from scratch (even though I borrowed stuff). 


// ------------- //
//#include "DetectorMessenger.hh"
//#include "G4UserLimits.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4RegionStore.hh"

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

DetectorConstruction::DetectorConstruction()
{
    DefineMaterials();
    DefineVolume();
}

DetectorConstruction::~DetectorConstruction()
{

}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    // Define materials
    DefineMaterials();

    // Define volumes
    return DefineVolume();
}

// -------------- //

void DetectorConstruction::DefineMaterials()
    {
        G4NistManager *nistManager = G4NistManager::Instance();

        vacuum = nistManager->FindOrBuildMaterial("G4_Galactic");
        air = nistManager->FindOrBuildMaterial("G4_AIR");
        h20 = nistManager->FindOrBuildMaterial("G4_WATER");
        gold = nistManager->FindOrBuildMaterial("G4_Au");
        // Print a material Table //
        G4cout << *(G4Material::GetMaterialTable()) << G4endl;
    }

G4VPhysicalVolume* DetectorConstruction::DefineVolume()
{
    G4Box *worldSV = new G4Box("world", 200*micrometer, 200*micrometer, 200*micrometer);
    G4LogicalVolume *worldLV = new G4LogicalVolume(worldSV,air,"world");

    G4VPhysicalVolume *worldPV = new G4PVPlacement(0,                   // no rotation
                                                      G4ThreeVector(),  // at (0,0,0)
                                                      worldLV,          // its logical volume
                                                      "World",          // its name
                                                      0,                // its mother  volume
                                                      false,            // no boolean operations
                                                      0);               // copy number

    // Water Tank Definition //
    G4Box *waterTankSV = new G4Box("waterTankSolid", 100*micrometer, 100*micrometer, 100*micrometer);
    G4LogicalVolume *waterTankLV= new G4LogicalVolume(waterTankSV,h20,"waterTankLogic");
    G4VPhysicalVolume *waterTankPV = new G4PVPlacement(0,-G4ThreeVector(0, 0, 0),waterTankLV,"waterTank",worldLV,false, 1); 

    // Surrounding Sphere of H2O for analysis //
    G4Sphere *H20SV = new G4Sphere("Target", 0.01*micrometer, 0.5 * micrometer, 0 * deg, 360 * deg, 0 * deg, 360 * deg);
    G4LogicalVolume *h2oLV = new G4LogicalVolume(H20SV, h20, "LogicSphere");
    G4VPhysicalVolume *h2oPV = new G4PVPlacement(0, -G4ThreeVector(0, 0, 0), h2oLV, "Target", waterTankLV, false, 3);
    
    // Gold Sphere Definition //
    G4Sphere *gnpSV = new G4Sphere("GNP", 0 , 0.01*micrometer, 0 * deg, 360 * deg, 0 * deg, 360 * deg);
    G4LogicalVolume *gnpLV = new G4LogicalVolume(gnpSV,gold,"GNP");
    G4VPhysicalVolume *gnpV = new G4PVPlacement(0,-G4ThreeVector(0, 0,0), gnpLV,"GNP",waterTankLV,false,2);

    // Colors for visual display //
    worldLV->SetVisAttributes(G4VisAttributes::Invisible);
    
    G4Color water(0, 0, 1);
    waterTankLV->SetVisAttributes(new G4VisAttributes(water));
    h2oLV->SetVisAttributes(new G4VisAttributes(water));
    
    G4Color goldColor(0.906, 0.773, 0.424);
    gnpLV->SetVisAttributes(new G4VisAttributes(goldColor));
    
    
    // Create region(s) and add logical vols
    G4Region* target = new G4Region("Target");
    target->AddRootLogicalVolume(waterTankLV);

    //G4String regName = "Target";
    //G4Region* region = G4RegionStore::GetInstance()->GetRegion(regName);

    G4ProductionCuts *cuts = new G4ProductionCuts();
    G4double defCut = 1 * nanometer;
    cuts->SetProductionCut(defCut, "gamma");
    cuts->SetProductionCut(defCut, "e-");
    cuts->SetProductionCut(defCut, "e+");
    cuts->SetProductionCut(defCut, "proton");
    
    target->SetProductionCuts(cuts);

    return worldPV;
}

