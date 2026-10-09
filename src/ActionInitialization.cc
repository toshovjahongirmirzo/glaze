#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction());

  auto* runAction = new RunAction();
  SetUserAction(runAction);

  const auto* detector = static_cast<const DetectorConstruction*>(
      G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  SetUserAction(new SteppingAction(detector, runAction));
}
