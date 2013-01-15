/*
 * Network.h
 *
 *  Created on: Nov 19, 2012
 *      Author: cws
 */

#ifndef ONEMODENETWORK_H_
#define ONEMODENETWORK_H_

#include "Network.h"
#include <Vector.h>



/**
 * Basic implementation of a one-mode (quadratic) network
 * with no computational optimizations.
 *
 * As a default, the network is handled as a binary and directed network.
 */
class OneModeNetwork : public Network{

public:

	OneModeNetwork(int size, bool directed = true, bool reflexive = false);
	OneModeNetwork(std::vector<std::vector<double> > graph,
			bool directed = true, bool reflexive = false);
	virtual ~OneModeNetwork();

	/*
	 * Adds a tie to the network. This equals
	 * setting a tie to the the default value
	 */
	bool addTie(int i, int j);

	/**
	 * Sets a tie to a particular value. Indexes are checked.
	 * In case of undirected networks the tie is automatically symmetrized
	 */
	bool setTie(int i, int j, double value);

	/*
	 * Checks whether a tie exists
	 */
	bool hasTie(int i, int j);

	/**
	 * Get value of tie from i to j
	 */
	double getTieValue(int i, int j);

	/*
	 * Removes a tie (set to zero)
	 */
	bool removeTie(int i, int j);

	/*
	 * Accessor 2d vector representation of the network
	 */
	std::vector<std::vector<double> > getGraph() const;

	/*
	 * Get size of the network which resembles the number of actors
	 */
	int getSize();

	/**
	 * Directed networks can have directed ties i -> j without
	 * the existence if a tie j -> i
	 */
	bool isDirected() const;

	/**
	 * Reflexive networks can have reflexive ties i -> i
	 */
	bool isReflexive() const;

private:
	// a binary network is the default network
	static const double DefaultTieValue = 1.0;

	/*
	 * checks whether an index i is in the range [0, size-1]
	 */
	bool isIndexValid(int i);

	/*
	 * checks whether both  indexes i, j are in the range [0, size-1]
	 */
	bool isIndexesValid(int i, int j);

protected:

	std::vector<std::vector<double> > _graph;
	int _size;
	bool _directed;
	bool _reflexive;

	/*
	 * Updates all look up maps. To to be called after a tie change.
	 */
	virtual void updateInternalRepresentation(int i, int j, double oldValue, double newValue);

	/*
	 * Initialization of variables and lookup maps. To be called by constructors.
	 */
	virtual void init();

};

#endif /* ONEMODENETWORK_H_ */
