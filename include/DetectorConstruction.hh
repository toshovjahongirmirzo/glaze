#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"
#include "G4RunManager.hh"

class G4GenericMessenger;

// Configurations you can select at runtime with:
//   /tile/config <n>
enum TileConfig {
  kAlReference   = 0,  // Aluminium slab (method validation)
  kPMMAReference = 1,  // PMMA slab (method validation)
  kClayOnly      = 2,  // unglazed clay body only
  kClayIshkor    = 3,  // clay body + lead-free Rishtan ishkor glaze
  kClayGijduvan  = 4,  // clay body + leaded Gijduvan glaze
  kIshkorOnly    = 5,  // pure Rishtan ishkor glaze alone (calibration)
  kGijduvanOnly  = 6,  // pure Gijduvan glaze alone (calibration)
  kNoSample      = 7,  // background: air only, same total thickness as the stack
  kGijduvanFlipped = 8 // clay+Gijduvan stack, order reversed (rotation/orientation check)
};

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  ~DetectorConstruction() override;

  G4VPhysicalVolume* Construct() override;

  // Messenger callbacks
  void SetConfig(G4int config)          { fConfig = config; fGeomChanged = true; G4RunManager::GetRunManager()->ReinitializeGeometry(); }
  void SetGlazeThickness(G4double t)    { fGlazeThickness_mm = t; fGeomChanged = true; G4RunManager::GetRunManager()->ReinitializeGeometry(); }
  void SetClayThickness(G4double t)     { fClayThickness_mm = t; fGeomChanged = true; G4RunManager::GetRunManager()->ReinitializeGeometry(); }
  G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

private:
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();

  G4GenericMessenger* fMessenger = nullptr;
  G4LogicalVolume* fScoringVolume = nullptr;

  G4int fConfig = kClayGijduvan;
  G4double fGlazeThickness_mm = 1.0;   // "thick test tile" default from the proposal
  G4double fClayThickness_mm  = 5.0;   // set this to your real tile thickness
  G4bool fGeomChanged = true;

  // Materials, built once
  G4Material* fMatClayBody   = nullptr;
  G4Material* fMatIshkor     = nullptr;   // lead-free alkali glaze
  G4Material* fMatGijduvan   = nullptr;   // leaded glaze (~51 wt% PbO, Bukhara-measured)
  G4Material* fMatAluminium  = nullptr;
  G4Material* fMatPMMA       = nullptr;
};

#endif
