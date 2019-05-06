//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestParticleCollection.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestParticleCollection
#define RestCore_TRestParticleCollection

#include <iostream>

#include "TObject.h"

#include <TRestParticle.h>

class TRestParticleCollection : public TObject {
 protected:
  std::vector<TRestParticle> fParticles;

 public:
  static TRestParticleCollection* instantiate(std::string model = "");

  virtual void SetParticleModel(std::string modelstring) {
    std::cout << "REST ERROR: SetParticleModel() called in base class "
                 "TRestParticleCollection"
              << std::endl;
    std::cout
        << "Package not properly installed? LD_LIBRARY_PATH not properly set?"
        << std::endl;
    exit(1);
  }
  virtual void VirtualUpdate() {}
  virtual Int_t GetNumberOfParticles() { return fParticles.size(); }
  virtual TRestParticle GetParticle(int i) { return fParticles[i]; }

  virtual void RemoveParticles() { fParticles.clear(); }
  virtual void AddParticle(TRestParticle ptcle) { fParticles.push_back(ptcle); }

  // Construtor
  TRestParticleCollection();
  // Destructor
  virtual ~TRestParticleCollection();

  ClassDef(TRestParticleCollection, 1);
};
#endif
