#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "FTFP_BERT.hh"
#include "G4StepLimiterPhysics.hh"

class PhysicsList : public FTFP_BERT
{
public:
  PhysicsList() : FTFP_BERT()
  {
    RegisterPhysics(new G4StepLimiterPhysics());
  }
  ~PhysicsList() override = default;
};

#endif
