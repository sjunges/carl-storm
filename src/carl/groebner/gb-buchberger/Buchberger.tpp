/*
 * GiNaCRA - GiNaC Real Algebra package
 * Copyright (C) 2010-2012  Ulrich Loup, Joachim Redies, Sebastian Junges
 *
 * This file is part of GiNaCRA.
 *
 * GiNaCRA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GiNaCRA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GiNaCRA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once
#include "Buchberger.h"
//
//
namespace carl
{
//
///**
// * Calculate the Groebner basis
// * @return An ideal object with generators which equal the Groebner basis.
// */

template<class Polynomial, template<typename> class AddingPolicy>
void Buchberger<Polynomial, AddingPolicy>::calculate(const std::list<Polynomial>& scheduledForAdding)
{
	LOGMSG_INFO("carl.gb.buchberger", "Calculate gb");
	for(unsigned i = 0; i < pGb->getGenerators().size(); ++i)
	{
		mGbElementsIndices.push_back(i);
	}

	bool foundGB = false;
	for(Polynomial newPol : scheduledForAdding)
	{
		if(addToGb(newPol))
		{
			LOGMSG_INFO("carl.gb.buchberger", "Added a constant polynomial.");
			foundGB = true;
			break;
		}
	}


	//As long as unprocessed pairs exist..
	if(!foundGB)
	{
		while(!mCritPairs.empty())
		{
			// Takes the next pair scheduled
			SPolPair critPair = mCritPairs.pop();
			LOGMSG_DEBUG("carl.gb.buchberger", "Calculate SPol for: " << pGb->getGenerators()[critPair.mP1] << ", " << pGb->getGenerators()[critPair.mP2]);
			// Calculates the S-Polynomial
			Polynomial spol = Polynomial::SPolynomial(pGb->getGenerators()[critPair.mP1], pGb->getGenerators()[critPair.mP2]);
			LOGMSG_DEBUG("carl.gb.buchberger", "SPol: " << spol);
			// Schedules the S-polynomial for reduction
			Reductor<Polynomial, Polynomial, void> reductor(*pGb, spol);
			// Does a full reduction on this
			Polynomial remainder = reductor.fullReduce();
			LOGMSG_DEBUG("carl.gb.buchberger", "Remainder of SPol: " << remainder);
			// If it is not zero, we should add this one to our GB
			if(!remainder.isZero())
			{
				// If it is constant, we are done and can return {1} as GB.
				if(remainder.isConstant())
				{
					pGb->clear();
					pGb->addGenerator(remainder.normalize());
					break;
				}
				else
				{

					// divide the polynomial through the leading coefficient.

					if(addToGb(remainder.normalize())) break;
				}
			}
		}
	}
	mGbElementsIndices.clear();

	return;
}


//

template<class Polynomial, template<typename> class AddingPolicy>
void Buchberger<Polynomial, AddingPolicy>::update(const size_t index)
{
	std::vector<Polynomial>& generators = pGb->getGenerators();
	std::vector<size_t>::const_iterator jEnd = mGbElementsIndices.end();

	std::unordered_map<size_t, SPolPair> spairs;
	std::vector<size_t> primelist;
	for(std::vector<size_t>::const_iterator jt = mGbElementsIndices.begin(); jt != jEnd; ++jt)
	{
		size_t otherIndex = *jt;
		SPolPair sp(otherIndex, index, Monomial::lcm(*generators[index].lmon(), *generators[otherIndex].lmon()));
		if(sp.mLcm.tdeg() == generators[index].lmon()->tdeg() + generators[otherIndex].lmon()->tdeg())
		{
			// *generators[index].lmon( ), *generators[otherIndex].lmon( ) are prime.
			primelist.push_back(otherIndex);
		}
		spairs.emplace(otherIndex, sp);
	}

	mCritPairs.elimMultiples(*generators[index].lmon(), index, spairs);

	removeBuchbergerTriples(spairs, primelist);

	// Pairs which are primes don't have to be added according to Buchbergers first criterion
	for(std::vector<size_t>::const_iterator pt = primelist.begin(); pt != primelist.end(); ++pt)
	{
		spairs.erase(*pt);
	}

	// We add the critical pairs to our tree of pairs
	std::list<SPolPair> critPairsList;

	std::transform(spairs.begin(), spairs.end(), std::back_inserter(critPairsList), [](std::pair<size_t, SPolPair> val)
	{
		return val.second;
	});
	mCritPairs.push(critPairsList);

	std::vector<size_t> tempIndices;
	jEnd = mGbElementsIndices.end();
	for(std::vector<size_t>::const_iterator jt = mGbElementsIndices.begin(); jt != jEnd; ++jt)
	{
		if(!generators[*jt].lmon()->dividableBy(*generators[index].lmon()))
		{
			tempIndices.push_back(*jt);
		}
		else
		{
			pGb->eliminateGenerator(*jt);
		}
	}

	mGbElementsIndices.swap(tempIndices);
	// We add the currently added polynomial to our GB.
	mGbElementsIndices.push_back(index);
}

template<class Polynomial, template<typename> class AddingPolicy>
void Buchberger<Polynomial, AddingPolicy>::removeBuchbergerTriples(std::unordered_map<size_t, SPolPair>& spairs, std::vector<size_t>& primelist)
{
	std::unordered_map<size_t, SPolPair>::iterator it = spairs.begin();

	if(primelist.size() != 0)
	{
		std::vector<size_t>::const_iterator primes = primelist.begin();
		while(it != spairs.end())
		{
			if(it->first == *primes)
			{
				++primes;
				//if there are no primes left, we can stop this check
				if(primes == primelist.end())
				{
					break;
					++it;
				}
			}

			bool elim = false;
			for(std::unordered_map<size_t, SPolPair>::const_iterator jt = spairs.begin(); jt != it; ++jt)
			{
				if(it->second.mLcm.dividableBy(jt->second.mLcm))
				{
					it = spairs.erase(it);
					elim = true;
					break;
				}
			}

			if(elim) continue;

			std::unordered_map<size_t, SPolPair>::const_iterator jt = it;
			for(++jt; jt != spairs.end(); ++jt)
			{
				if(it->second.mLcm.dividableBy(jt->second.mLcm))
				{
					it = spairs.erase(it);
					elim = true;
					break;
				}
			}

			if(elim) continue;
			++it;
		}
	}
	//TODO function
	// same as above, but now without prime-skipping.
	while(it != spairs.end())
	{
		bool elim = false; //critPair.print(std::cout);
		for(std::unordered_map<size_t, SPolPair>::const_iterator jt = spairs.begin(); jt != it; ++jt)
		{
			if(it->second.mLcm.dividableBy(jt->second.mLcm))
			{
				it = spairs.erase(it);
				elim = true;
				break;
			}
		}
		if(elim) continue;

		std::unordered_map<size_t, SPolPair>::const_iterator jt = it;
		for(++jt; jt != spairs.end(); ++jt)
		{
			if(it->second.mLcm.dividableBy(jt->second.mLcm))
			{
				it = spairs.erase(it);
				elim = true;
				break;
			}
		}
		if(elim)
		{
			continue;
		}
		else
		{
			++it;
		}
	}
}
}
