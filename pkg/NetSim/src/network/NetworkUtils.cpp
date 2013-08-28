/*
 * NetworkUtils.cpp
 *
 *  Created on: Nov 21, 2012
 *      Author: cws
 */

#include "NetworkUtils.h"


/**
 *
 */
void NetworkUtils::dumpNetwork(MemoryOneModeNetwork* network, int round) {
	std::vector<std::vector<double> > graph = network->getGraph();
	int size = network->getSize();

	// print
	std::cout << "MemoryOneModeNetwork, " << size << " nodes" << std::endl;
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			printf("%.2lf", graph[i][j]);
			// double digit = round( graph[i][j] * (1 + round * 10) ) / (1 + round * 10);
			// std::cout << digit;
			if (j == (size - 1)) // last element in a row
				std::cout << std::endl;
			else std::cout << " ";
		}
	}
}

double NetworkUtils::getDensity(MemoryOneModeNetwork* network) {
	bool isDirected = network->isDirected();
	bool isReflexive = network->isReflexive();
	int size = network->getSize();
	double nTies = 0;
	int possibleTies = 0;

	// this first case is weird
	if (isDirected && isReflexive) possibleTies = size * size;
	if (isDirected && !isReflexive) possibleTies = size * (size -1);
	if (!isDirected && isReflexive) possibleTies = (size * (size -1) / 2) + size;
	if (!isDirected && !isReflexive) possibleTies = size * (size -1) / 2;

	nTies = countTies(network, false);

	return nTies / (double) possibleTies;

}

int NetworkUtils::getNumberOfTies(MemoryOneModeNetwork* network) {
	return countTies(network, false);
}


int NetworkUtils::getNumberOfReciprocalTies(MemoryOneModeNetwork* network) {
	return countTies(network, true);
}

int NetworkUtils::getHammingDistance(MemoryOneModeNetwork* network1,
		MemoryOneModeNetwork* network2) {
	int nActors = network1->getSize();
	if (network2->getSize() != nActors) return -1;

	int distance = 0;

	for (int i = 0; i < nActors; i++){
		for (int j = 0; j< nActors; j++){
			if(network1->getTieValue(i, j) !=
					network2->getTieValue(i,j))
				distance++;
		}
	}

	return distance;

}

void NetworkUtils::dumpInternalObjects(MemoryOneModeNetwork* network) {

	std::cout << "-------------------" << std::endl;
	std::cout << "Dumping internal objects of a MemoryOneModeNetwork:" << std::endl;
	std::cout << "Size: " << network->_size << std::endl;
	std::cout << "Directed: " << network->_directed << std::endl;
	std::cout << "Reflexive: " << network->_reflexive << std::endl;

	std::cout << "Node : in-degree" << std::endl;
	std::map<int, int>::iterator itInDeg = network->_inDegreeMap.begin();
	for (; itInDeg != network->_inDegreeMap.end(); ++itInDeg){
		std::cout << (*itInDeg).first << " : " << (*itInDeg).second << std::endl;
	}
	std::cout << "Node : out-degree" << std::endl;
	std::map<int, int>::iterator itOutDeg = network->_outDegreeMap.begin();
	for (; itOutDeg != network->_outDegreeMap.end(); ++itOutDeg){
		std::cout << (*itOutDeg).first << " : " << (*itOutDeg).second << std::endl;
	}

	std::cout << "Node : incoming neighbors" << std::endl;
	std::map<int,std::set<int>* >::iterator itIncNeigh =
			network->_incomingNeighborsMap.begin();
	for (; itIncNeigh != network->_incomingNeighborsMap.end(); ++itIncNeigh){
		std::cout << (*itIncNeigh).first << " : ";
		std::set<int>::iterator itNeighbors =  (*itIncNeigh).second->begin();
		for(; itNeighbors !=  (*itIncNeigh).second->end(); ++itNeighbors){
			std::cout << (*itNeighbors) << ", ";
		}
		std::cout << std::endl;
	}

	std::cout << "Node : outgoing neighbors" << std::endl;
	std::map<int,std::set<int>* >::iterator itOutNeigh =
			network->_outgoingNeighborsMap.begin();
	for (; itOutNeigh != network->_outgoingNeighborsMap.end(); ++itOutNeigh){
		std::cout << (*itOutNeigh).first << " : ";
		std::set<int>::iterator itNeighbors =  (*itOutNeigh).second->begin();
		for(; itNeighbors !=  (*itOutNeigh).second->end(); ++itNeighbors){
			std::cout << (*itNeighbors) << ", ";
		}
		std::cout << std::endl;
	}


	std::cout << "-------------------" << std::endl;
}

std::set<int> NetworkUtils::getNRandomNodes(Network* network, int n) {

	std::set<int> nodes;

	if (network->getSize() < n) return nodes;

	int nNodes = network->getSize();

	while (nodes.size() < n){

		double rand = Random::getInstance().getRandom();
		double interval = 1.0 / (double) nNodes;

		// the compiler truncates when casting double to int
		nodes.insert(rand / interval);

	}

	return nodes;


}

void NetworkUtils::addRandomTiesToNetwork(Network* network, double p) {
	int size = network->getSize();

	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			double rand = Random::getInstance().getRandom();

			if (rand < p)
				network->addTie(i, j);

		}
	}
}

std::pair<int, int> NetworkUtils::getRandomTie(MemoryOneModeNetwork* network) {

	std::set<int> actorIDs = network->getActorIDs();
	double randDissolve = Random::getInstance().getRandom();

	int nTies = getNumberOfTies(network);
	int chosenTieNumber = randDissolve * (double) nTies; // automatically rounding down
	int tieNumber = 0;

	std::set<int>::iterator itActor1 = actorIDs.begin();
	for(; itActor1 != actorIDs.end(); ++itActor1){
		int i = *itActor1;
		std::set<int>::iterator itActor2 = actorIDs.begin();
		for(; itActor2 != actorIDs.end(); ++itActor2){
			int j = *itActor2;
			if (network->hasTie(i, j)){
				if (tieNumber == chosenTieNumber){

					return std::make_pair(i, j);

				}
				tieNumber++;
			}

		}
	}

	// error
	return std::make_pair(-1, -1);
}

void NetworkUtils::addRingLatticeTiesToNetwork(MemoryOneModeNetwork * network,
		int nReciprocalEdges) {
	// TODO check correctness of this exception statement
	if (nReciprocalEdges % 2 != 0)
		throw std::invalid_argument("A ring lattice needs an even number of reciprocal ties");

	int tiesToAddFromNode = nReciprocalEdges / 2;
	int nSteps = network->getSize() + tiesToAddFromNode - 1;
	std::set<int> ids = network->getActorIDs();
	std::set<int>::iterator itNodes = ids.begin();

	std::vector<int> lastNodes(tiesToAddFromNode, -1);
	for (size_t i = 0; i < lastNodes.size(); i++){
		if (itNodes == ids.end())
			throw std::invalid_argument("Too many edges to be included");
		// the closest node has to be on [0]
		lastNodes[lastNodes.size() - 1 - i] = *itNodes;
		++itNodes;
	}


	while(nSteps > 0){
		if (itNodes == ids.end()) itNodes = ids.begin();

		// add ties
		for (size_t i = 0; i < lastNodes.size(); i++){
			network->addTie(*itNodes, lastNodes[i]);
			network->addTie(lastNodes[i], *itNodes);
		}

		// update list of previous nodes
		for (size_t i = 0 ; i < lastNodes.size() - 1; i++){
			lastNodes[i+1] = lastNodes[i];
		}
		lastNodes[0] = *itNodes;

		itNodes++;
		nSteps--;
	}


}

// WARNING: Was earlier marked as deprecated but seems fine.
// ignores reflexive ties
// reciprocal ties are counted "twice"
int NetworkUtils::countTies(MemoryOneModeNetwork * network,
		bool reciprocal) {

	int nTies = 0;
	int nReciprocalTies = 0;
	std::set<int> actorIDs = network->getActorIDs();

	std::set<int>::iterator itActorI = actorIDs.begin();
	for (; itActorI != actorIDs.end(); ++itActorI){
		int i = *itActorI;
		std::set<int>::iterator itActorJ = actorIDs.begin();
		for (; itActorJ != actorIDs.end(); ++itActorJ){
			int j = *itActorJ;
			if ( i < j ){
				if (network->hasTie(i,j))
					nTies++;
				if (network->hasTie(j,i))
					nTies++;
				if (network->hasTie(i,j) && network->hasTie(j,i))
					nReciprocalTies += 2;
			}
		}
	}

	if (reciprocal) return nReciprocalTies;
	else return nTies;
}

