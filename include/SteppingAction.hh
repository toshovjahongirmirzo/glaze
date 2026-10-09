#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class DetectorConstruction;
class RunAction;

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(const DetectorConstruction* detector, RunAction* runAction);
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step* step) override;

private:
  const DetectorConstruction* fDetector;
  RunAction* fRunAction;
};

#endif
