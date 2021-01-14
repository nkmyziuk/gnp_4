// $Id: DetectorConstruction.hh 33 2010-01-14 17:08:18Z adotti $
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

/**
 * @file
 * @brief Defines mandatory user class DetectorConstruction.
 */

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4Region;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  //! Constructor
  DetectorConstruction();
  //! Destructor
  ~DetectorConstruction();
public:
  //! Construct geometry of the setup
  virtual G4VPhysicalVolume* Construct();

  G4Region *GetTargetRegion() { return target;}
  G4Region *target;

private :
  
  //! define needed materials
  void DefineMaterials();
  
  //! initialize geometry parameters
  G4VPhysicalVolume *DefineVolume();

  //data members
  G4Material *vacuum;  
  G4Material *air;
  G4Material *h20;
  G4Material *gold;
};

#endif
