//     Copyright 2013, 2014, 2015 Ramon Diaz-Uriarte

//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.

//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <http://www.gnu.org/licenses/>.



#ifndef _NEW_RESTRICT_H__
#define _NEW_RESTRICT_H__

#include <Rcpp.h>
#include"debug_common.h"
#include <limits>
#include <random>

enum class Dependency {monotone, semimonotone, xmpn, single, NA}; 

struct Poset_struct {
  Dependency typeDep;
  int childNumID; //Not redundant
  double s;
  double sh;
  std::vector<int> parentsNumID;
  // The next two are clearly redundant but a triple check
  std::string child;
  std::vector<std::string> parents;
};

// We use same structure for epistasis and order effects. With order
// effects, NumID is NOT sorted, but reflects the order of the
// restriction. And checking is done using that fact.
struct epistasis {
  double s;
  std::vector<int> NumID; //a set instead? nope.using includes with epistasis
  std::vector<std::string> names; // will remove later
};


struct genesWithoutInt {
  int shift; // access the s as s[index of mutation or index of mutated
	     // gene in genome - shift]. shift is the min. of NumID, given
	     // how that is numbered from R. We assume mutations always
	     // indexed 1 to something. Not 0 to something.
  // If shift is -9, no elements The next first two are not really
  // needed. Will remove later. Nope! we use them to provide nice output.
  std::vector<int> NumID;
  std::vector<std::string> names;
  std::vector<double> s;
};

struct Gene_Module_struct {
  std::string GeneName;
  std::string ModuleName;
  int GeneNumID;
  int ModuleNumID;
};

struct fitnessEffectsAll {
  bool gMOneToOne;
  int genomeSize; 
  // We use allOrderG or allEpistRTG to place new mutations in their
  // correct place (orderEff or epistRtEff). Only one is needed.  Use the
  // one that is presumably always shorter which is allOrderG. And this is
  // sorted.
  std::vector<int> allOrderG; // Modules or genes if one-to-one.
  // std::vector<int> allEpistRTG;

  // This makes it faster to run evalPosetConstraints
  std::vector<int> allPosetG; //Modules or genes if one-to-one. Only
			      //poset. Not epist.
  std::vector<Poset_struct> Poset;
  std::vector<epistasis> Epistasis;
  std::vector<epistasis> orderE;
  // std::vector<Gene_Module_struct> Gene_Module_tabl;
  std::vector<Gene_Module_struct> Gene_Module_tabl;
  std::vector<int> allGenes; //used whenever a mutation created. Genes,
			     //not modules. Sorted.
  std::vector<int> drv; // Sorted.
  genesWithoutInt genesNoInt;
  
};

struct fitness_as_genes {
  // fitnessEffectsAll in terms of genes.  Useful for output
  // conversions. There could be genes that are both in orderG and
  // posetEpistG. In such a case, only in orderG.
  // We only use a small part for now.
  // All are ordered vectors.
  std::vector<int> orderG;
  std::vector<int> posetEpistG;
  std::vector<int> noInt;
};

// There are no shared genes in order and epist.  Any gene in orderEff can
// also be in the posets or general epistasis, but orderEff is only for
// those that have order effects.

// For all genes for which there are no order effects, any permutation of
// the same mutations is the same genotype, and has the same fitness. That
// is why we separate orderEff, which is strictly in the order in which
// mutations accumulate, and thus usorted, from the other effects, that
// are always kept sorted.

// rest are those genes that have no interactions. Evaluating their
// fitness is simple, and there can be no modules here.
struct Genotype {
  std::vector<int> orderEff;
  std::vector<int> epistRtEff; //always sorted
  std::vector<int> rest; // always sorted
};


inline Genotype wtGenotype() {
  // Not needed but to make it explicit
  Genotype g;
  g.orderEff.resize(0);
  g.epistRtEff.resize(0);
  g.rest.resize(0);
  return g;
}





std::vector<int> genotypeSingleVector(const Genotype& ge);

bool operator==(const Genotype& lhs, const Genotype& rhs);

bool operator<(const Genotype& lhs, const Genotype& rhs);


Dependency stringToDep(const std::string& dep);

std::string depToString(const Dependency dep);


void obtainMutations(const Genotype& parent,
		     const fitnessEffectsAll& fe,
		     int& numMutablePosParent,
		     std::vector<int>& newMutations,
		     std::mt19937& ran_gen);

Genotype createNewGenotype(const Genotype& parent,
			   const std::vector<int>& mutations,
			   const fitnessEffectsAll& fe,
			   std::mt19937& ran_gen);

std::vector<double> evalGenotypeFitness(const Genotype& ge,
					const fitnessEffectsAll& F);


fitnessEffectsAll convertFitnessEffects(Rcpp::List rFE);
std::vector<int> getGenotypeDrivers(const Genotype& ge, const std::vector<int>& drv);
void print_Genotype(const Genotype& ge);

fitness_as_genes fitnessAsGenes(const fitnessEffectsAll& fe);

std::map<int, std::string> mapGenesIntToNames(const fitnessEffectsAll& fe);

std::vector<int> getGenotypeDrivers(const Genotype& ge, const std::vector<int>& drv);

double prodFitness(std::vector<double> s);

double prodDeathFitness(std::vector<double> s);

#endif
