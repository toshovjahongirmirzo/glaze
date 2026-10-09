#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4GenericMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;

  void SetBeamMomentum(G4double p) { fBeamMomentum = p; }

private:
  G4ParticleGun* fParticleGun = nullptr;
  G4GenericMessenger* fMessenger = nullptr;
  G4double fBeamMomentum = 2.0 * CLHEP::GeV; // set to your actual ELSA tag energy, e.g. /gun/momentum
};

#endif
