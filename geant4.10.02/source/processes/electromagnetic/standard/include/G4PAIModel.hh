//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4PAIModel.hh 89821 2015-04-30 14:56:43Z gcosmo $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4PAIModel
//
// Author:        V. Grichine based on Vladimir Ivanchenko  code
//
// Creation date: 05.10.2003
//
// Modifications:
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
// 26-09-07 Fixed tmax computation (V.Ivantchenko)
// 19.08.13 V.Ivanchenko extract data handling to G4PAIModelData class 
//          added sharing of internal data between threads (MT migration)
//
//
// Class Description:
//
// Implementation of PAI model of energy loss and
// delta-electron production by heavy charged particles

// -------------------------------------------------------------------
//

#ifndef G4PAIModel_h
#define G4PAIModel_h 1

#include <CLHEP/Units/PhysicalConstants.h>

#include "G4VEmModel.hh"
#include "G4VEmFluctuationModel.hh"
#include "globals.hh"
#include <vector>

class G4Region;
class G4MaterialCutsCouple;
class G4ParticleChangeForLoss;
class G4PAIModelData;

class G4PAIModel : public G4VEmModel, public G4VEmFluctuationModel
{

public:

  G4PAIModel(const G4ParticleDefinition* p = 0, const G4String& nam = "PAI");

  virtual ~G4PAIModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual void InitialiseLocal(const G4ParticleDefinition*, 
                               G4VEmModel* masterModel);

  virtual G4double ComputeDEDXPerVolume(const G4Material*,
			       const G4ParticleDefinition*,
			       G4double kineticEnergy,
			       G4double cutEnergy);

  virtual G4double CrossSectionPerVolume(const G4Material*,
				const G4ParticleDefinition*,
				G4double kineticEnergy,
				G4double cutEnergy,
				G4double maxEnergy);

  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
				 const G4MaterialCutsCouple*,
				 const G4DynamicParticle*,
				 G4double tmin,
				 G4double maxEnergy);

  virtual G4double SampleFluctuations(const G4MaterialCutsCouple*,
				      const G4DynamicParticle*,
				      G4double,
				      G4double,
				      G4double);

  virtual G4double Dispersion(    const G4Material*,
				  const G4DynamicParticle*,
				  G4double,
				  G4double);

  void DefineForRegion(const G4Region* r);

  inline G4PAIModelData* GetPAIModelData();

  inline const std::vector<const G4MaterialCutsCouple*>& GetVectorOfCouples();

  inline G4double ComputeMaxEnergy(G4double scaledEnergy);

  inline void SetVerboseLevel(G4int verbose);

protected:

  G4double MaxSecondaryEnergy(const G4ParticleDefinition*,
			      G4double kinEnergy);

private:

  inline G4int FindCoupleIndex(const G4MaterialCutsCouple*);

  inline void SetParticle(const G4ParticleDefinition* p);

  // hide assignment operator 
  G4PAIModel & operator=(const  G4PAIModel &right);
  G4PAIModel(const  G4PAIModel&);

  G4int                       fVerbose; 

  G4PAIModelData*             fModelData; 

  std::vector<const G4MaterialCutsCouple*> fMaterialCutsCoupleVector;
  std::vector<const G4Region*>      fPAIRegionVector;

  const G4ParticleDefinition* fParticle;
  const G4ParticleDefinition* fElectron;
  const G4ParticleDefinition* fPositron;
  G4ParticleChangeForLoss*    fParticleChange;

  G4double fMass;
  G4double fRatio;
  G4double fChargeSquare;
};

inline G4PAIModelData* G4PAIModel::GetPAIModelData()
{
  return fModelData;
}

inline const std::vector<const G4MaterialCutsCouple*>& 
G4PAIModel::GetVectorOfCouples()
{
  return fMaterialCutsCoupleVector;
}

inline G4double G4PAIModel::ComputeMaxEnergy(G4double scaledEnergy)
{
  return MaxSecondaryEnergy(fParticle, scaledEnergy/fRatio);
}

inline void G4PAIModel::SetVerboseLevel(G4int verbose) 
{ 
  fVerbose=verbose; 
}

inline G4int G4PAIModel::FindCoupleIndex(const G4MaterialCutsCouple* couple)
{
  G4int idx = -1;
  size_t jMatMax = fMaterialCutsCoupleVector.size();
  for(size_t jMat = 0;jMat < jMatMax; ++jMat) { 
    if(couple == fMaterialCutsCoupleVector[jMat]) {
      idx = jMat; 
      break; 
    }
  }
  return idx;
}

inline void G4PAIModel::SetParticle(const G4ParticleDefinition* p)
{
  if(fParticle != p) {
    fParticle = p;
    fMass = fParticle->GetPDGMass();
    fRatio = CLHEP::proton_mass_c2/fMass;
    G4double q = fParticle->GetPDGCharge()/CLHEP::eplus;
    fChargeSquare = q*q;
  }
}

#endif







