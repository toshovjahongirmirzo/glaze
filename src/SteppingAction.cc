#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4LogicalVolume.hh"

SteppingAction::SteppingAction(const DetectorConstruction* detector, RunAction* runAction)
  : fDetector(detector), fRunAction(runAction)
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (step->GetTrack()->GetTrackID() != 1) return;
  if (step->GetTrack()->GetParticleDefinition()->GetParticleName() != "e-") return;

  auto* preVolume = step->GetPreStepPoint()->GetPhysicalVolume();
  if (!preVolume) return;

  auto* scoringVolume = fDetector->GetScoringVolume();
  if (!scoringVolume) return;

  if (preVolume->GetLogicalVolume() == scoringVolume &&
      step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {

    G4ThreeVector dir = step->GetPostStepPoint()->GetMomentumDirection();
    G4double theta = std::atan2(dir.x(), dir.z()); 
    fRunAction->RecordAngle(theta);
  }
}
