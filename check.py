import numpy as np, sys
sys.path.insert(0,'.')
from analyze import fit_core_sigma, invert_highland_for_X0

alpha=1/137.035999; re=2.8179403e-13; NA=6.02214076e23
Amass={'H':1.008,'C':12.011,'O':15.999,'Na':22.990,'Mg':24.305,'Al':26.982,'Si':28.086,'K':39.098,'Ca':40.078,
       'Ti':47.867,'Mn':54.938,'Fe':55.845,'Cu':63.546,'Zn':65.38,'Pb':207.2}
Zn={'H':1,'C':6,'O':8,'Na':11,'Mg':12,'Al':13,'Si':14,'K':19,'Ca':20,'Ti':22,'Mn':25,'Fe':26,'Cu':29,'Zn':30,'Pb':82}
def inv_rho_X0(el):
    Z=Zn[el]; A=Amass[el]
    if Z<=4:
        Lr={1:5.31,2:4.79,3:4.74,4:4.71}[Z]; Lp={1:6.144,2:5.621,3:5.805,4:5.924}[Z]
    else:
        Lr=np.log(184.15*Z**(-1/3)); Lp=np.log(1194*Z**(-2/3))
    a2=(alpha*Z)**2
    fc=a2*(1/(1+a2)+0.20206-0.0369*a2+0.0083*a2**2-0.002*a2**3)
    return 4*alpha*re**2*NA/A*(Z*Z*(Lr-fc)+Z*Lp)
def X0cm(comp,rho):
    s=sum(w*inv_rho_X0(e) for e,w in comp.items())/sum(comp.values())
    return 1/(s*rho)

print("Al   X0 = %.3f cm (PDG 8.897)"%X0cm({'Al':1},2.699))
pmma={'H':0.080538,'C':0.599848,'O':0.319614}
print("PMMA X0 = %.2f cm (PDG 34.07-34.2)"%X0cm(pmma,1.19))

clay={'Si':0.60*28.086/60.084,'O':0.60*2*15.999/60.084+0.20*3*15.999/101.961,'Al':0.20*2*26.982/101.961,'Fe':0.06,'Ca':0.06,'K':0.08}
ish ={'O':.45573,'Si':.32247,'Na':.05230,'K':.04514,'Ca':.03887,'Cu':.02816,'Al':.02505,'Mg':.01518,'Mn':.00936,'Fe':.00774}
gij ={'Pb':.48390,'O':.27390,'Si':.18457,'Fe':.02056,'Ca':.00999,'Al':.00748,'K':.00626,'Na':.00570,'Cu':.00357,'Mg':.00356,'Ti':.00038,'Zn':.00006,'Mn':.00005}
RHO_GIJ=3.75   # change to 3.75 once you decide
X={'clay':X0cm(clay,2.2),'ish':X0cm(ish,2.6),'gij':X0cm(gij,RHO_GIJ)}
print("Layer X0 (cm):",{k:round(float(v),3) for k,v in X.items()})

def bragg_stack(tc,tg,Xg): return (tc+tg)/(tc/X['clay']+tg/Xg)
res={'ish':bragg_stack(0.92,0.10,X['ish']),'gij':bragg_stack(0.92,0.10,X['gij'])}
print("Bragg stack X0: ish %.3f gij %.3f cm -> separation %.1f%%"%(res['ish'],res['gij'],100*(res['ish']-res['gij'])/res['ish']))

sims={}
for mom,tag in [(2.9,'2p9'),(3.2,'3p2')]:
    for m,k in [('ishkor','ish'),('gijduvan','gij')]:
        a=np.loadtxt(f'build/angles_{m}_final_{tag}.csv',skiprows=1); th,e=fit_core_sigma(a)
        x0=invert_highland_for_X0(th,10.2,mom)/10; sims[(m,mom)]=a
        print(f"{m} {mom} GeV: sim X0 {x0:.3f} cm, vs Bragg {100*(x0/res[k]-1):+.1f}%")

for f,comp,rho,lab in [('build/angles_al_5k_v2.csv',{'Al':1},2.699,'Al'),('build/angles_pmma_v2.csv',pmma,1.19,'PMMA')]:
    a=np.loadtxt(f,skiprows=1); th,e=fit_core_sigma(a); xs=invert_highland_for_X0(th,5.0,2.0)/10
    ex=X0cm(comp,rho); print(f"{lab} 5mm 2GeV: sim {xs:.3f} vs exact {ex:.3f} -> {100*(xs/ex-1):+.1f}%")

rng=np.random.default_rng(1)
def boot(a,mom,n,B=300):
    out=[]
    for _ in range(B):
        s=rng.choice(a,n,replace=True)
        try:
            th,_=fit_core_sigma(s); out.append(invert_highland_for_X0(th,10.2,mom)/10)
        except Exception: pass
    return np.array(out)
for n in (15000,3500,1000):
    bi=boot(sims[('ishkor',2.9)],2.9,n); bg=boot(sims[('gijduvan',2.9)],2.9,n)
    d=bi.mean()-bg.mean(); sd=np.hypot(bi.std(),bg.std())
    print(f"n={n:5d}: Ishkor {bi.mean():.2f}+-{bi.std():.2f}  Gij {bg.mean():.2f}+-{bg.std():.2f}  sep {100*d/bi.mean():.1f}%  {d/sd:.1f} sigma")
