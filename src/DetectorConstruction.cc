#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"

DetectorConstruction::DetectorConstruction()
{
  fMessenger = new G4GenericMessenger(this, "/tile/", "Tile-stack geometry controls");

  fMessenger->DeclareMethod("config", &DetectorConstruction::SetConfig)
    .SetGuidance("0=Al ref, 1=PMMA ref, 2=clay only, 3=clay+ishkor, 4=clay+gijduvan");

  fMessenger->DeclareMethodWithUnit("glazeThickness", "mm", &DetectorConstruction::SetGlazeThickness)
    .SetGuidance("Glaze layer thickness");

  fMessenger->DeclareMethodWithUnit("clayThickness", "mm", &DetectorConstruction::SetClayThickness)
    .SetGuidance("Clay body thickness");

  DefineMaterials();
}

DetectorConstruction::~DetectorConstruction()
{
  delete fMessenger;
}

void DetectorConstruction::DefineMaterials()
{
  auto* nist = G4NistManager::Instance();

  fMatAluminium = nist->FindOrBuildMaterial("G4_Al");
  fMatPMMA      = nist->FindOrBuildMaterial("G4_PLEXIGLASS");

  fMatClayBody = new G4Material("ClayBody", 2.2 * g / cm3, 5);
  fMatClayBody->AddMaterial(nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"), 0.60);
  fMatClayBody->AddMaterial(nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE"), 0.20);
  fMatClayBody->AddElement(nist->FindOrBuildElement("Fe"), 0.06);
  fMatClayBody->AddElement(nist->FindOrBuildElement("Ca"), 0.06);
  fMatClayBody->AddElement(nist->FindOrBuildElement("K"),  0.08);

    // REAL composition: Rishtan ishkor glaze, directly measured via SEM-EDS
  // (Henshaw PhD thesis, Tables 5.39-5.40, shared via personal communication
  // with Prof. Thilo Rehren). Oxide wt% (renormalized to 100): SiO2 68.98,
  // Na2O 7.05, K2O 5.44, CaO 5.44, Al2O3 4.73, CuO 3.52, MgO 2.52, MnO 1.21,
  // Fe2O3 1.11. Elemental fractions converted from this oxide composition
  // (see proposal Sec. 2.3 for the same renormalization method used for the
  // Gijduvan material below). Density rho=2.6 g/cm3 per the proposal's
  // established working value for this glaze.
  fMatIshkor = new G4Material("IshkorGlaze", 2.6 * g / cm3, 10);
  fMatIshkor->AddElement(nist->FindOrBuildElement("O"),  0.45573);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Si"), 0.32247);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Na"), 0.05230);
  fMatIshkor->AddElement(nist->FindOrBuildElement("K"),  0.04514);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Ca"), 0.03887);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Cu"), 0.02816);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Al"), 0.02505);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Mg"), 0.01518);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Mn"), 0.00936);
  fMatIshkor->AddElement(nist->FindOrBuildElement("Fe"), 0.00774);

  // Real composition: mean of the n=9 leaded Bukhara glaze samples from
  // Gradmann et al. 2015 Supplementary Table S1 (samples KhZ029, KhZ029.1,
  // KhZ134, KhZ163, KhZ164, KhZ200, KhZ201oc, MMA4, BM0001; PbO>30wt%
  // threshold used to isolate the leaded subgroup). Elemental fractions
  // converted from the mean oxide wt%: SiO2 39.48, Al2O3 1.41, PbO 52.13,
  // MgO 0.59, CaO 1.40, Na2O 0.77, K2O 0.75, Fe2O3 2.94, MnO 0.01,
  // CuO 0.45, ZnO 0.01, TiO2 0.06 (renormalized to 100%).
  // Density: NOT fit to any target X0. Estimated via the standard
  // oxide-additive reciprocal rule (1/rho = sum(w_i/rho_i) using literature
  // pure-oxide densities), giving 3.90 g/cm3, consistent with published
  // lead-silicate glass density measurements at ~50 wt% PbO (~3.8-4.6 g/cm3
  // across independent sources).
  fMatGijduvan = new G4Material("GijduvanGlaze", 3.75 * g / cm3, 13);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Pb"), 0.48390);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("O"),  0.27390);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Si"), 0.18457);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Fe"), 0.02056);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Ca"), 0.00999);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Al"), 0.00748);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("K"),  0.00626);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Na"), 0.00570);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Cu"), 0.00357);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Mg"), 0.00356);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Ti"), 0.00038);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Zn"), 0.00006);
  fMatGijduvan->AddElement(nist->FindOrBuildElement("Mn"), 0.00005);
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return DefineVolumes();
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  auto* nist = G4NistManager::Instance();
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");

  G4double worldSize = 1.0 * m;
  auto* solidWorld = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
  auto* logicWorld  = new G4LogicalVolume(solidWorld, worldMat, "World");
  auto* physWorld   = new G4PVPlacement(nullptr, {}, logicWorld, "World", nullptr, false, 0);
  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  logicWorld->SetUserLimits(new G4UserLimits(2.0 * mm));

  G4double halfXY = 25.0 * mm;
  G4double z = 0.0;

  auto placeSlab = [&](G4Material* mat, G4double thickness_mm, const G4String& name) {
    G4double halfZ = 0.5 * thickness_mm * mm;
    auto* solid = new G4Box(name, halfXY, halfXY, halfZ);
    auto* logic = new G4LogicalVolume(solid, mat, name);
    z += halfZ;
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, z), logic, name, logicWorld, false, 0);
    z += halfZ + 0.001 * mm;
    return logic;
  };

  G4LogicalVolume* lastLogic = nullptr;

  switch (fConfig) {
    case kAlReference:
      lastLogic = placeSlab(fMatAluminium, fClayThickness_mm, "AlReference");
      break;
    case kPMMAReference:
      lastLogic = placeSlab(fMatPMMA, fClayThickness_mm, "PMMAReference");
      break;
    case kClayOnly:
      lastLogic = placeSlab(fMatClayBody, fClayThickness_mm, "ClayBody");
      break;
    case kClayIshkor:
      placeSlab(fMatClayBody, fClayThickness_mm, "ClayBody");
      lastLogic = placeSlab(fMatIshkor, fGlazeThickness_mm, "IshkorGlaze");
      break;
    case kClayGijduvan:
      placeSlab(fMatClayBody, fClayThickness_mm, "ClayBody");
      lastLogic = placeSlab(fMatGijduvan, fGlazeThickness_mm, "GijduvanGlaze");
      break;
    case kIshkorOnly:
      lastLogic = placeSlab(fMatIshkor, fGlazeThickness_mm, "IshkorGlaze");
      break;
    case kGijduvanOnly:
      lastLogic = placeSlab(fMatGijduvan, fGlazeThickness_mm, "GijduvanGlaze");
      break;
    case kNoSample:
      lastLogic = placeSlab(worldMat, fClayThickness_mm + fGlazeThickness_mm, "NoSampleAir");
      break;
    case kGijduvanFlipped:
      placeSlab(fMatGijduvan, fGlazeThickness_mm, "GijduvanGlaze");
      lastLogic = placeSlab(fMatClayBody, fClayThickness_mm, "ClayBody");
      break;
    default:
      lastLogic = placeSlab(fMatGijduvan, fGlazeThickness_mm, "GijduvanGlaze");
      break;
  }

  fScoringVolume = lastLogic;
  fGeomChanged = false;

  return physWorld;
}
