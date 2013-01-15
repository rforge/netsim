/*
 * MultinomialChoiceChangeModel.h
 *
 *  Created on: Dec 9, 2012
 *      Author: cws
 */

#ifndef MULTINOMIALCHOICECHANGEMODEL_H_
#define MULTINOMIALCHOICECHANGEMODEL_H_

#include "ChangeModel.h"
#include "saom/NetworkEffect.h"
#include "Updater.h"
#include "ModelResult.h"
#include "../utils/Random.h"

class MultinomialChoiceNetworkChangeModel : public ChangeModel{

public:
	/**
	 * Create a simple model in which choices are based on how
	 * the network would look like after a tie swap update
	 */
	MultinomialChoiceNetworkChangeModel(int actorIndex,
			size_t dependentNetworkIndex,
			std::set<std::pair<NetworkEffect*, double> *> * effectParameterPairs,
			std::vector<Updater*>* updaters){

		init(actorIndex, dependentNetworkIndex, effectParameterPairs, updaters);
	}

	/**
	 * create a multinomial choice model in which the choice
	 * dependes on a process state without changes
	 */
	MultinomialChoiceNetworkChangeModel(int actorIndex,
			size_t dependentNetworkIndex,
			std::set<std::pair<NetworkEffect*, double> *> * effectParameterPairs){

		std::vector<Updater*>* updaters = new std::vector<Updater*>(1, new NullUpdater());
		init(actorIndex, dependentNetworkIndex, effectParameterPairs, updaters);
	}



	ModelResult * getChange(ProcessState * processState){

		int nActors = processState->getNumberOfActors();
		std::vector<double> objectiveFunctions(nActors, 0);
		double denominator = 0;

		// iterate over all actors
		for (int j = 0; j < nActors; j++){

			int i = _actorIndex;

			// update tie.
 			for (std::vector<Updater*>::iterator it = _updaters->begin(); it != _updaters->end(); ++it){
 				(*it)->update(processState, new TieModelResult(i,j)); // TODO check i, j
 			}

			// calculate and save objective function
 			objectiveFunctions[j] = getValueObjectiveFunction(processState, i); // processState has been updated previously

			// increase sum (denominator)
 			denominator += exp(objectiveFunctions[j]);

 			if (_debug) std::cout << "" << objectiveFunctions[j] << std::endl;

			// reset process state
			// So far, this is implemented as a swap back
			// Performance can be improved by implementing a
 			// Memento mechanism in the process state
 			for (std::vector<Updater*>::iterator it = _updaters->begin(); it != _updaters->end(); ++it){
 				(*it)->undoUpdate(processState, new TieModelResult(i,j)); // TODO check i, j
 			}
		}

		if (_debug) std::cout << "Active actor: " << _actorIndex << std::endl;
		if (_debug) std::cout << "calculated denominator: " << denominator << std::endl;

		// draw random number
		double randomNumber = Random::getInstance().getRandom();

		// iterate over objective functions until actor is found
		double cumProbability = 0;
		for (int j = 0; j < nActors; j++){
			double probability = exp(objectiveFunctions[j]) / denominator;
			cumProbability += probability;

			// j is the randomly chosen actor
			// create and return ActorModelResult
			if (cumProbability >= randomNumber){
				return new TieModelResult(_actorIndex,j);
			}
		}

		// Should never be reached except an error occurs
		std::cout << "Should not be here MultinomialChoiceModel wrong return type";
		return new TieModelResult(-1,-1);


	}

	void setDebugMode(bool verbose){
		_debug = verbose;
	}


private:
	int _actorIndex;
	size_t _dependentNetworkIndex;
	std::set<std::pair<NetworkEffect*, double> *> * _effectParameterPairs;
	// std::vector<NetworkEffect*> * _effects; // Effect container
	// std::vector<double> * _parameters;
	std::vector<Updater*> * _updaters;
	bool _debug;

	void init(int actorIndex,
			size_t dependentNetworkIndex,
			std::set<std::pair<NetworkEffect*, double> *> * effectParameterPairs,
			std::vector<Updater*>* updaters){

		_actorIndex = actorIndex;
		_dependentNetworkIndex = dependentNetworkIndex;
		_effectParameterPairs = effectParameterPairs;
		_updaters = updaters;
		_debug = false;

	}

	double getValueObjectiveFunction(ProcessState * processState, int i){

		double valueObjectiveFunction = 0.0;

		std::set<std::pair<NetworkEffect*, double> *>::iterator itEffects = _effectParameterPairs->begin();

		for (; itEffects != _effectParameterPairs->end(); ++itEffects){
			double effectValue = (*itEffects)->first->getEffect(processState, i);
			double parameter = (*itEffects)->second;

			if(_debug) std::cout << "  param=" << parameter << " value=" << effectValue << std::endl;

			valueObjectiveFunction += ( effectValue * parameter );
		}


		return valueObjectiveFunction;
	}

};


#endif /* MULTINOMIALCHOICECHANGEMODEL_H_ */