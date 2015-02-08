/**
 * @file Common.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <tuple>
#include <vector>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>


#include "../../core/MonomialPool.h"
#include "../../core/MultivariatePolynomial.h"
#include "../../core/RationalFunction.h"
#include "../../core/Variable.h"
#include "../../core/VariablePool.h"
#include "../../io/streamingOperators.h"

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace px = boost::phoenix;

namespace carl {
namespace parser {

using carl::operator<<;

typedef std::string::const_iterator Iterator;
typedef boost::spirit::qi::space_type Skipper;

template<typename Coeff>
using Poly = MultivariatePolynomial<Coeff>;

template<typename Coeff>
using RatFun = RationalFunction<Poly<Coeff>>;

template<typename Coeff>
struct RationalPolicies : qi::ureal_policies<Coeff> {
    template <typename It, typename Attr>
    static bool parse_nan(It&, It const&, Attr&) { return false; }
    template <typename It, typename Attr>
    static bool parse_inf(It&, It const&, Attr&) { return false; }
};

}
}