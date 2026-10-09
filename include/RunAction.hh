#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include <fstream>

class RunAction : public G4UserRunAction
{
public:
  RunAction() = default;
  ~RunAction() override = default;

  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

  void RecordAngle(G4double theta_rad);

private:
  std::ofstream fOut;
};

#endif
