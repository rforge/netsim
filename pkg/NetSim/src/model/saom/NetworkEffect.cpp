/*
 * NetworkEffect.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: cws
 */

#include "NetworkEffect.h"

OneModeNetworkEffect::OneModeNetworkEffect(size_t networkIndex) {
	_networkIndex = networkIndex;
}

DensityEffect::DensityEffect(size_t networkIndex) : OneModeNetworkEffect(networkIndex){
}

double DensityEffect::getEffect(ProcessState* processState, int actorIndex) {
	MemoryOneModeNetwork * net =
			dynamic_cast<MemoryOneModeNetwork *>(processState->getNetwork(_networkIndex));
	return net->getOutDegree(actorIndex);
}

std::string DensityEffect::getName() {
	return "Density";// +_networkIndex;
}

ReciprocityEffect::ReciprocityEffect(size_t networkIndex) : OneModeNetworkEffect(networkIndex){
}

double ReciprocityEffect::getEffect(ProcessState* processState,
		int actorIndex) {
	MemoryOneModeNetwork * net =
			dynamic_cast<MemoryOneModeNetwork *>(processState->getNetwork(_networkIndex));
	return net->getReciprocalNeighbors(actorIndex).size();
}


std::string ReciprocityEffect::getName() {
	return "Reciprocity";
}

TransitivityEffect::TransitivityEffect(size_t networkIndex)  : OneModeNetworkEffect(networkIndex){
}

double TransitivityEffect::getEffect(ProcessState* processState,
		int actorIndex) {
	double value = 0;
	MemoryOneModeNetwork * net =
			dynamic_cast<MemoryOneModeNetwork *>(processState->getNetwork(_networkIndex));

	// get all neighbors
	std::set<int> neighbors = net->getOutgoingNeighbors(actorIndex);
	// for each neighbor count the number of two-paths also leading there
	for (std::set<int>::iterator itNeighbors = neighbors.begin(); itNeighbors != neighbors.end(); ++itNeighbors){
		int nTransitiveTriads = net->getIntermediateNodesForward(actorIndex, (*itNeighbors)).size();
		value += (double) nTransitiveTriads;
	}

	return value;
}

std::string TransitivityEffect::getName() {
	return "Transitivity";
}

ThreeCycleEffect::ThreeCycleEffect(size_t networkIndex) : OneModeNetworkEffect(networkIndex){
}

double ThreeCycleEffect::getEffect(ProcessState* processState,
		int actorIndex) {
	double value = 0;
	MemoryOneModeNetwork * net =
			dynamic_cast<MemoryOneModeNetwork *>(processState->getNetwork(_networkIndex));

	// get all incoming neighbors
	std::set<int> neighbors = net->getIncomingNeighbors(actorIndex);
	// for each neighbor count the number of two-paths also leading there
	for (std::set<int>::iterator itNeighbors = neighbors.begin(); itNeighbors != neighbors.end(); ++itNeighbors){
		int nTransitiveTriads = net->getIntermediateNodesForward(actorIndex, (*itNeighbors)).size();
		value += (double) nTransitiveTriads;
	}

	return value;
}

std::string ThreeCycleEffect::getName() {
	return "Three-circle";
}

TwoPathEffect::TwoPathEffect(size_t networkIndex) : OneModeNetworkEffect(networkIndex){
}

double TwoPathEffect::getEffect(ProcessState* processState, int actorIndex) {
	MemoryOneModeNetwork * net =
			dynamic_cast<MemoryOneModeNetwork *>(processState->getNetwork(_networkIndex));

	std::set<int> distanceTwo = net->getNodesInDistanceTwo(actorIndex);
	std::set<int> neighboredNodes = net->getOutgoingNeighbors(actorIndex);

	// delete all neighbored nodes
	// TODO: Is there a faster way?
	for (std::set<int>::iterator it = neighboredNodes.begin();
			it != neighboredNodes.end();
			++it){
		if (distanceTwo.find(*it) != distanceTwo.end())
			distanceTwo.erase(*it);
	}

	return (double) distanceTwo.size();
}

std::string TwoPathEffect::getName() {
	return "Two Path";
}

InPopularityEffect::InPopularityEffect(size_t networkIndex)  : OneModeNetworkEffect(networkIndex){
}

double InPopularityEffect::getEffect(ProcessState* processState, int actorIndex) {
	double value = 0;
	MemoryOneModeNetwork * net =
			dynamic_cast<MemoryOneModeNetwork *>(processState->getNetwork(_networkIndex));

	std::set<int> neighbors = net->getOutgoingNeighbors(actorIndex);

	// sum the incoming ties of all neighbors
	for (std::set<int>::iterator it = neighbors.begin(); it != neighbors.end(); ++it){
		value += (double) net->getIncomingNeighbors(*it).size();
	}

	return value;

}

std::string InPopularityEffect::getName() {
	return "In-popularity";
}

OutPopularityEffect::OutPopularityEffect(size_t networkIndex)  : OneModeNetworkEffect(networkIndex){
}

double OutPopularityEffect::getEffect(ProcessState* processState, int actorIndex) {
	double value = 0;
	MemoryOneModeNetwork * net =
			dynamic_cast<MemoryOneModeNetwork *>(processState->getNetwork(_networkIndex));

	std::set<int> neighbors = net->getOutgoingNeighbors(actorIndex);

	// sum the incoming ties of all neighbors
	for (std::set<int>::iterator it = neighbors.begin(); it != neighbors.end(); ++it){
		value += (double) net->getOutgoingNeighbors(*it).size();
	}

	return value;

}

std::string OutPopularityEffect::getName() {
	return "Out-popularity";
}
