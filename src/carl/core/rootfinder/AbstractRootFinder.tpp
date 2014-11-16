/**
 * @file AbstractRootFinder.tpp
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#include "AbstractRootFinder.h"
#include "../logging.h"

#pragma once

namespace carl {
namespace rootfinder {

template<typename Number>
AbstractRootFinder<Number>::AbstractRootFinder(
		const UnivariatePolynomial<Number>& polynomial,
		const Interval<Number>& interval,
		bool tryTrivialSolver
	) :
		originalPolynomial(polynomial),
		polynomial(polynomial.squareFreePart()),
		//polynomial(polynomial),
		interval(interval),
		finished(false)
{
#ifdef ROOTFINDER_CACHE
	if (this->inCache()) {
		LOGMSG_TRACE("carl.core.rootfinder", "Hit cache: " << this->originalPolynomial);
		this->roots = cache[this->originalPolynomial];
		this->finished = true;
		return;
	}
#endif
	LOGMSG_TRACE("carl.core.rootfinder", "Creating abstract rootfinder for " << polynomial << " with interval " << this->interval);
	if (this->polynomial.zeroIsRoot()) {
		this->addRoot(RealAlgebraicNumberNR<Number>::create(0));
	}
	if (this->polynomial.isZero()) {
		this->setFinished();
		return;
	}
	if (tryTrivialSolver && this->solveTrivial()) {
		LOGMSG_TRACE("carl.core.rootfinder", "Polynomial was solved trivially.");
		this->setFinished();
		return;
	}
	if ((this->interval.lowerBoundType() == BoundType::INFTY) || (this->interval.upperBoundType() == BoundType::INFTY)) {
		Number bound = this->polynomial.cauchyBound();

		if (this->interval.lowerBoundType() == BoundType::INFTY) {
			this->interval.setLowerBoundType(BoundType::STRICT);
			if (-bound < this->interval.upper()) {
				this->interval.setLower(-bound);
			} else {
				this->interval.setLower(this->interval.upper());
			}
		}
		if (this->interval.upperBoundType() == BoundType::INFTY) {
			this->interval.setUpperBoundType(BoundType::STRICT);
			if (this->interval.lower() < bound) {
				this->interval.setUpper(bound);
			} else {
				this->interval.setUpper(this->interval.lower());
			}
		}
	}
	
	if (this->interval.lowerBoundType() == BoundType::WEAK) {
		this->interval.setLowerBoundType(BoundType::STRICT);
		if (this->polynomial.isRoot(this->interval.lower())) {
			this->addRoot(RealAlgebraicNumberNR<Number>::create(this->interval.lower()));
		}
	}
	if (this->interval.upperBoundType() == BoundType::WEAK) {
		this->interval.setUpperBoundType(BoundType::STRICT);
		if (this->polynomial.isRoot(this->interval.upper())) {
			this->addRoot(RealAlgebraicNumberNR<Number>::create(this->interval.upper()));
		}
	}
}

template<typename Number>
std::list<RealAlgebraicNumberPtr<Number>> AbstractRootFinder<Number>::getAllRoots() {
	if (! this->isFinished()) {
		this->findRoots();
		this->setFinished();
	}
	this->roots.sort(std::less<RealAlgebraicNumberPtr<Number>>());
	return this->roots;
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(RealAlgebraicNumberPtr<Number> root, bool reducePolynomial) {
	if (root->isNumeric()) {
		if (reducePolynomial) {
			this->polynomial.eliminateRoot(root->value());
		}
	} else {
		RealAlgebraicNumberIRPtr<Number> r = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(root);
		if (r->getInterval().diameter() == 0) {
			root = RealAlgebraicNumberNR<Number>::create(r->getInterval().lower());
		}
	}
	this->roots.push_back(root);
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(const Interval<Number>& interval) {
	this->addRoot(RealAlgebraicNumberIR<Number>::create(this->polynomial, interval));
}

template<typename Number>
bool AbstractRootFinder<Number>::solveTrivial() {
	switch (this->polynomial.degree()) {
		case 0: break;
		case 1: {
			Number a = this->polynomial.coefficients()[1], b = this->polynomial.coefficients()[0];
			assert(a != Number(0));
			this->addRoot(RealAlgebraicNumberNR<Number>::create(-b / a), false);
			break;
		}
		case 2: {
			Number a = this->polynomial.coefficients()[2], b = this->polynomial.coefficients()[1], c = this->polynomial.coefficients()[0];
			assert(a != Number(0));
			/* Use this formulation of p-q-formula:
			 * x = ( -b +- \sqrt{ b*b - 4*a*c } ) / (2*a)
			 */
			Number rad = b*b - 4*a*c;
			if (rad == 0) {
				this->addRoot(RealAlgebraicNumberNR<Number>::create(-b / (2*a)), false);
			} else if (rad > 0) {
				std::pair<Number, Number> res = carl::sqrt_fast(rad);
				if (res.first == res.second) {
					// Root could be calculated exactly
					this->addRoot(RealAlgebraicNumberNR<Number>::create((-b - res.first) / (2*a)), false);
					this->addRoot(RealAlgebraicNumberNR<Number>::create((-b + res.first) / (2*a)), false);
				} else {
					// Root is within interval (res.first, res.second)
					Interval<Number> r(res.first, BoundType::STRICT, res.second, BoundType::STRICT);
					this->addRoot(RealAlgebraicNumberIR<Number>::create(this->polynomial, (-b - r) / (2*a)), false);
					this->addRoot(RealAlgebraicNumberIR<Number>::create(this->polynomial, (-b + r) / (2*a)), false);
				}
			}
			break;
		}
		default:
			return false;
	}
	return true;
}

}
}
