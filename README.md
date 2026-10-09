# glaze

Geant4 simulation of electron multiple scattering in glazed ceramic tiles, with analysis scripts that extract the radiation length X₀ and compare it with the Bragg-additivity prediction.

## Configurations

Selected with `/tile/config N`:

| N | Configuration |
|---|---|
| 0 | Aluminium reference |
| 1 | PMMA reference |
| 2 | Clay body only |
| 3 | Clay + Rishtan ishkor glaze (lead-free) |
| 4 | Clay + Gijduvan glaze (leaded) |
| 5 | Ishkor glaze only |
| 6 | Gijduvan glaze only |
| 7 | No sample (air) |
| 8 | Clay + Gijduvan, order reversed |

Macro commands:

```
/tile/config 3
/tile/clayThickness 9.2 mm
/tile/glazeThickness 1.0 mm
/gun/beamMomentum 2.9        # GeV/c
/run/beamOn 15000
```

Defaults: 5.0 mm clay, 1.0 mm glaze.

## Materials

Defined in `src/DetectorConstruction.cc`.

- Ishkor glaze: ρ = 2.6 g/cm³, from SEM-EDS oxide composition.
- Gijduvan glaze: ρ = 3.75 g/cm³, mean of 9 leaded Bukhara glaze samples (Gradmann et al. 2015), PbO = 50.55 ± 6.9 wt%.
- Clay body: ρ = 2.2 g/cm³, generic assumed composition (not measured).

## Build and run

Requires Geant4 and CMake.

```bash
mkdir build && cd build
cmake ..
make -j4
./tileScatter ../run_ishkor_final_2p9.mac
mv angles.csv angles_ishkor_final_2p9.csv
```

Each run overwrites `angles.csv`, so rename it after every run. Final macros: `run_{ishkor,gijduvan}_final_{2p9,3p2}.mac`.

## Analysis

Needs Python 3, `numpy`, `scipy`.

```bash
python3 analyze.py results/angles_ishkor_final_2p9.csv 10.2 2.9
# arguments: CSV, total stack thickness in mm, beam momentum in GeV
```

`analyze.py` fits the core of the exit-angle distribution with a Gaussian and inverts the Highland formula for X₀. `check.py` reproduces the table below from `results/`, including the analytic Bragg prediction and a bootstrap of the statistical significance.

## Results

9.2 mm clay + 1.0 mm glaze (10.2 mm total), 15,000 events per run.

| | 2.9 GeV | 3.2 GeV |
|---|---|---|
| Ishkor stack X₀ | 9.891 ± 0.212 cm | 9.803 ± 0.207 cm |
| Gijduvan stack X₀ | 7.772 ± 0.146 cm | 7.495 ± 0.160 cm |
| Separation | 21.4% (8.2σ) | 23.5% (8.8σ) |
| Bragg prediction (Ishkor / Gijduvan) | 10.69 / 8.43 cm | same |

Validation against exact X₀: Al 5 mm −5.1%, PMMA 5 mm −1.0%, Al 10.2 mm −9.2%. The Highland extraction reads low and the bias grows with x/X₀, so ceramic stacks should be compared with an aluminium reference at matched thickness.

## Limitations

- Ideal beam: no detector resolution, divergence or background.
- Clay composition and glaze densities are assumptions.
- Gijduvan composition is Bukhara-regional, not workshop-specific.
