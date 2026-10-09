#include "RunAction.hh"
#include "G4Run.hh"

void RunAction::BeginOfRunAction(const G4Run*)
{
  fOut.open("angles.csv", std::ios::out | std::ios::trunc);
  fOut << "exit_polar_angle_rad\n";
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  fOut.close();
  G4cout << "\n=== Run finished: " << run->GetNumberOfEvent()
         << " primaries. Exit angles written to angles.csv ===\n"
         << "Next step: load angles.csv (e.g. with numpy/pandas), fit the\n"
         << "core of the angular distribution (a Gaussian fit to the central\n"
         << "part, ignoring large-angle tails from rare hard scatters), take\n"
         << "the fitted sigma as theta_0, and invert the Highland formula\n"
         << "for X0. Compare against your Bragg-additivity prediction.\n" << G4endl;
}

void RunAction::RecordAngle(G4double theta_rad)
{
  fOut << theta_rad << "\n";
}
