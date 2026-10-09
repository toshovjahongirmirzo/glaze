// tileScatter.cc
//
// Minimal Geant4 application for the BL4S "Bragg additivity" experiment.
// Fires an electron beam through a stack of clay + glaze slabs (or Al/PMMA
// reference slabs) and records the exit polar angle of each primary
// electron, so you can compare the RMS scattering angle to:
//   1) the Highland formula prediction, and
//   2) the Bragg-additivity X0 prediction built from layer composition.
//
// Usage:
//   ./tileScatter run.mac
//
// Output: "angles.csv" — one exit polar angle (radians) per primary electron.

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
  G4UIExecutive* ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new PhysicsList());
  runManager->SetUserInitialization(new ActionInitialization());

  runManager->Initialize();

  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();

  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (ui) {
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  delete visManager;
  delete runManager;
  return 0;
}
