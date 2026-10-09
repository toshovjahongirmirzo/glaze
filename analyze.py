"""
analyze.py — turn a Geant4 angles.csv into a measured radiation length X0.

Usage:
    python3 analyze.py angles.csv <thickness_mm> <momentum_GeV>
"""
import sys
import numpy as np
from scipy.optimize import curve_fit

def highland_theta0(x_over_X0, beta_c_p_MeV):
    return (13.6 / beta_c_p_MeV) * np.sqrt(x_over_X0) * (1 + 0.038 * np.log(x_over_X0))

def invert_highland_for_X0(theta0, x_mm, p_GeV, beta=1.0):
    p_MeV = p_GeV * 1000.0
    beta_c_p = beta * p_MeV
    from scipy.optimize import brentq
    def f(X0_mm):
        x_over_X0 = x_mm / X0_mm
        return highland_theta0(x_over_X0, beta_c_p) - theta0
    return brentq(f, 0.1, 5000.0)

def gaussian(x, amp, mu, sigma):
    return amp * np.exp(-0.5 * ((x - mu) / sigma) ** 2)

def fit_core_sigma(angles, core_fraction=0.9):
    angles = np.asarray(angles)
    lo, hi = np.percentile(angles, [(100 - core_fraction * 100) / 2,
                                     100 - (100 - core_fraction * 100) / 2])
    core = angles[(angles >= lo) & (angles <= hi)]
    counts, edges = np.histogram(core, bins=40)
    centers = 0.5 * (edges[1:] + edges[:-1])
    p0 = [counts.max(), np.mean(core), np.std(core)]
    popt, pcov = curve_fit(gaussian, centers, counts, p0=p0)
    sigma = abs(popt[2])
    sigma_err = np.sqrt(pcov[2, 2])
    return sigma, sigma_err

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(__doc__)
        sys.exit(1)
    csv_path, thickness_mm, momentum_GeV = sys.argv[1], float(sys.argv[2]), float(sys.argv[3])
    angles = np.loadtxt(csv_path, skiprows=1)
    theta0, theta0_err = fit_core_sigma(angles)
    X0_measured_mm = invert_highland_for_X0(theta0, thickness_mm, momentum_GeV)
    X0_hi_mm = invert_highland_for_X0(theta0 - theta0_err, thickness_mm, momentum_GeV)
    X0_lo_mm = invert_highland_for_X0(theta0 + theta0_err, thickness_mm, momentum_GeV)
    X0_err_mm = 0.5 * (X0_hi_mm - X0_lo_mm)
    print(f"N events:              {len(angles)}")
    print(f"Fitted core theta0:    {theta0*1000:.3f} +/- {theta0_err*1000:.4f} mrad")
    print(f"Slab thickness:        {thickness_mm} mm")
    print(f"Beam momentum:         {momentum_GeV} GeV")
    print(f"--> Measured X0:       {X0_measured_mm/10:.3f} +/- {X0_err_mm/10:.3f} cm")
    print("Compare this to your Bragg-additivity prediction (Fig. 1 of the proposal).")
