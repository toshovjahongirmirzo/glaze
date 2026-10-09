#include "PrimaryGeneratorAction.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4GenericMessenger.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun(1);

  auto* particleTable = G4ParticleTable::GetParticleTable();
  auto* electron = particleTable->FindParticle("e-");
  fParticleGun->SetParticleDefinition(electron);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
  fParticleGun->SetParticleMomentum(fBeamMomentum);
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -10.0 * cm));

  fMessenger = new G4GenericMessenger(this, "/gun/", "Beam controls");
  fMessenger->DeclareMethodWithUnit("beamMomentum", "GeV", &PrimaryGeneratorAction::SetBeamMomentum)
    .SetGuidance("Beam momentum, e.g. ELSA tagged-photon-derived electron energy");
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  fParticleGun->SetParticleMomentum(fBeamMomentum);
  fParticleGun->GeneratePrimaryVertex(event);
}
