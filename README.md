# BL4S Tile-Stack Geant4 Starter (Experiment 1)

A minimal, runnable Geant4 model of your electron-beam-through-tile-stack
measurement, built to mirror the design in your proposal exactly:

- Aluminium and PMMA reference slabs for method validation (`/tile/config 0` / `1`)
- Unglazed clay body only (`/tile/config 2`)
- Clay + lead-free Rishtan ishkor glaze (`/tile/config 3`)
- Clay + leaded Gijduvan glaze (`/tile/config 4`)
- Adjustable glaze thickness (`/tile/glazeThickness`) — set to 1 mm by default,
  matching your redesigned "thicker test tile" that predicts a 20–23% split
- Adjustable beam momentum (`/gun/momentum`) — for your two-momentum
  1/p-scaling cross-check

## What it does NOT do yet (things you should add)

1. **Real compositions.** `DetectorConstruction.cc` has clearly marked
   PLACEHOLDER oxide weight fractions for the clay body, ishkor glaze, and
   Gijduvan glaze. Replace these with:
   - Rishtan's directly-measured glaze composition
   - The n=9 Bukhara/Gijduvan-analogue mean from Gradmann et al. 2015
   - Prof. Rehren's SEM-EDS numbers once he runs them on your own tiles
   This is the single highest-value fix — the physics conclusions are only
   as good as these numbers.

2. **Upstream/downstream telescope planes.** Right now the code scores the
   exit angle of the primary track the instant it leaves the last slab —
   a reasonable proxy, but ELSA's real telescope has finite planes with
   their own resolution (you quote ~27 microradians). You can add two thin
   "virtual" tracking-plane volumes upstream and downstream of the tile
   stack and score position there instead, to convolve in realistic
   telescope resolution and reproduce your actual track-fitting method
   more faithfully.

3. **Background/no-sample subtraction run.** Add `/tile/config` for an
   empty stack (air only) as your background configuration, exactly as
   your proposal specifies.

4. **Two-beam-momentum 1/p check.** Loop `run_gijduvan.mac`-style macros at
   two momenta and check that fitted theta0 scales as Highland predicts.

## Build

```bash
mkdir build && cd build
cmake ..
make -j4
```

## Run

```bash
./tileScatter run_gijduvan.mac   # writes angles.csv
mv angles.csv angles_gijduvan.csv
./tileScatter run_ishkor.mac
mv angles.csv angles_ishkor.csv
```

## Analyze

```bash
pip install numpy scipy --break-system-packages
python3 analyze.py angles_gijduvan.csv 1.0 2.0
python3 analyze.py angles_ishkor.csv 1.0 2.0
```

This applies your own pre-registered decision rule: compare the two
measured X0 values, and check whether the separation exceeds 2 standard
deviations (using the statistical uncertainty from your ~1000-event
sample, propagated through the Gaussian fit) and whether each measured
X0 is within 5%/15% of your Bragg-additivity prediction.

## A note on realism

This is a genuine, physically correct Geant4 model of your measurement —
FTFP_BERT is the real reference physics list, Highland's formula is
inverted exactly as in your proposal, and the geometry matches your
described stack. It is *not* a substitute for validating it yourself: run
the Al and PMMA reference slabs first and check the measured X0 against
the well-known literature values (X0(Al) ≈ 8.9 cm, X0(PMMA) ≈ 34.4 cm)
before trusting any ceramic result, exactly as your proposal's validation
step requires.
