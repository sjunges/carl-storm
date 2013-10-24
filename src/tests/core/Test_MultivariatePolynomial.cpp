#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include <gmpxx.h>


using namespace carl;

TEST(MultivariatePolynomial, Constructor)
{
    Variable v0(0);
    Term<mpz_class> t0(v0);
    MultivariatePolynomial<mpz_class> p0(t0);
    
    MultivariatePolynomial<mpz_class> p1(3);
    EXPECT_EQ(1, p1.nrTerms());
    EXPECT_TRUE(p1.isLinear());
}

TEST(MultivariatePolynomial, Operators)
{
    Variable v0(0);
    Term<mpz_class> t0(v0);
    MultivariatePolynomial<mpz_class> p0a(t0);
    MultivariatePolynomial<mpz_class> p0b(v0);
    EXPECT_EQ(p0a, p0b);
    
    EXPECT_TRUE(p0a.isUnivariate());
}

TEST(MultivariatePolynomial, Addition)
{
    Variable v0(0);
    Term<mpz_class> t0(v0);
    MultivariatePolynomial<mpz_class> p0(v0);
    p0 += 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 += 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 += -6;
    EXPECT_EQ(1, p0.nrTerms());
    
    Variable v1(1);
    Variable v2(2);
    p0 += v1;
    p0 += Monomial(v2);
    EXPECT_EQ(3,p0.nrTerms());
    p0 += Monomial(v2);
    EXPECT_EQ(3,p0.nrTerms());
    p0 += Term<mpz_class>(-2,v2);
    EXPECT_EQ(2,p0.nrTerms());
    
    MultivariatePolynomial<mpz_class> p1(v0);
    p1 += v1;
    p0 += p1;
    EXPECT_EQ(2,p0.nrTerms());   
    MultivariatePolynomial<mpz_class> mp2(v0);
    mp2 += (mpz_class)2 * v1;
    EXPECT_EQ(v0, *mp2.lterm());
    MultivariatePolynomial<mpz_class> p10a;
    p10a += v0*v0;
    p10a += v1;
    EXPECT_EQ(v0*v0, *p10a.lterm());
    MultivariatePolynomial<mpz_class> p10b;
    p10b += v1;
    p10b += v0*v0;
    EXPECT_EQ(v0*v0, *p10b.lterm());
	EXPECT_EQ(p10a, p10b);
    p10a += p10b;
	p10a += mp2;
	p10a += p0;
	p10b += mp2;
	p10b += p0;
	p10b += v0 * v0;
	p10b += v1;
	EXPECT_EQ(p10a, p10b);
	
	
	
    
	
	
}

TEST(MultivariatePolynomial, Substraction)
{
    Variable v0(0);
    MultivariatePolynomial<mpz_class> p0(v0);
    p0 -= 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 -= 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 -= -6;
    EXPECT_EQ(1, p0.nrTerms());
    Variable v1(1);
    Variable v2(2);
    p0 -= v1;
    EXPECT_EQ(2,p0.nrTerms());
    p0 -= Monomial(v2);
    EXPECT_EQ(3,p0.nrTerms());
    p0 -= Monomial(v2);
    
    EXPECT_EQ(3,p0.nrTerms());
    p0 -= Term<mpz_class>(-2,v2);
    EXPECT_EQ(2,p0.nrTerms());
    
    MultivariatePolynomial<mpz_class> p1(v0);
    p1 -= v1;
    p0 -= p1;
    EXPECT_EQ(0,p0.nrTerms());
}

TEST(MultivariatePolynomial, Multiplication)
{
    Variable v0(0);
    Variable v1(1);
    MultivariatePolynomial<mpz_class> p0(v0);
    
    p0 *= v0;
    EXPECT_EQ(Term<mpz_class>(1,v0,2), *(p0.lterm()));
    
    MultivariatePolynomial<mpz_class> q({mpz_class(2)*v0*v0, mpz_class(1)*v0*v1});
    Term<mpz_class> t(1, v0,2);
    MultivariatePolynomial<mpz_class> res = q * t;
    EXPECT_EQ(MultivariatePolynomial<mpz_class>({mpz_class(2)*v0*v0*v0*v0, mpz_class(1)*v0*v0*v0*v1}), res);
    
    p0 += v1;
    p0 += 1;
    //std::cout << p0 << std::endl;
    p0 *= p0;
    //std::cout << p0 << std::endl;
}

TEST(MultivariatePolynomial, toString)
{
    
}

TEST(MultivariatePolynomial, Normalize)
{
    Variable v0(0);
    Variable v1(1);
    Variable v2(2);
    MultivariatePolynomial<cln::cl_RA> mp;
    mp += v0;
    MultivariatePolynomial<cln::cl_RA> mp2 = mp * (cln::cl_RA)2;
    EXPECT_EQ(mp, mp.normalize());
    EXPECT_EQ(mp, mp2.normalize());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)2 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += v1;
    EXPECT_EQ(mp2, mp.normalize());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += (cln::cl_RA)2 * v1;
    EXPECT_EQ(mp2, mp.normalize());
}

TEST(MultivariatePolynomial, Coprime)
{
    Variable v0(0);
    Variable v1(1);
    MultivariatePolynomial<cln::cl_RA> mp;
    mp += v0;
    MultivariatePolynomial<cln::cl_RA> mp2 = mp * (cln::cl_RA)2;
    EXPECT_EQ(mp, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += (cln::cl_RA)2 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1 * v0);
    mp2 += (cln::cl_RA)24 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)1/4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp2 += (cln::cl_RA)3 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    MultivariatePolynomial<mpq_class> mpG = MultivariatePolynomial<mpq_class>(mpq_class(1,6) * v0);
    mpG += mpq_class(1,4) * v1;
    MultivariatePolynomial<mpq_class> mp2G = MultivariatePolynomial<mpq_class>(mpq_class(2) * v0);
    mp2G += mpq_class(3) * v1;
    EXPECT_EQ(mp2G, mpG.coprimeCoefficients());
    
    
}
        
TEST(MultivariatePolynomial, Substitute)
{
    Variable v0(0);
    Variable v1(1);
    MultivariatePolynomial<cln::cl_RA> mp((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)4 * v1;
    MultivariatePolynomial<cln::cl_RA> mp2((cln::cl_RA)2);
    mp2 += (cln::cl_RA)4 * v1;
    std::map<Variable, cln::cl_RA> substitutions;
    substitutions[v0] = (cln::cl_RA)12;
    EXPECT_EQ(mp2, mp.substitute(substitutions));
    substitutions[v0] = (cln::cl_RA)0;
    EXPECT_EQ(MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)4 * v1), mp.substitute(substitutions));
}

TEST(MultivariatePolynomial, SPolynomial)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> g1({(cln::cl_RA)3*x*x*x*x*y, (cln::cl_RA)1*y*y});
    EXPECT_EQ(3,MultivariatePolynomial<cln::cl_RA>::SPolynomial(f1.normalize(), g1.normalize()).nrTerms());
    //MultivariatePolynomial<cln::cl_RA> s1({(cln::cl_RA)-1*x*x});
    //EXPECT_EQ(s1, MultivariatePolynomial::SPolynomial(f1.normalize(), g1.normalize()));
    
    
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)1*x*x*x, (cln::cl_RA)-2*x*y} );
    MultivariatePolynomial<cln::cl_RA> g2({(cln::cl_RA)1*x*x*y, (cln::cl_RA)-2*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> s2({(cln::cl_RA)-1*x*x});
    EXPECT_EQ(s2, MultivariatePolynomial<cln::cl_RA>::SPolynomial(f2, g2));
}

TEST(MultivariatePolynomial, GatherVariables)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    std::set<Variable> vars;
    f1.gatherVariables(vars);
    EXPECT_EQ(x, *vars.begin());
    EXPECT_EQ(2, vars.size());
}

TEST(MultivariatePolynomial, Derivative)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    MultivariatePolynomial<cln::cl_RA> fx({x});
    EXPECT_EQ((cln::cl_RA)1, fx.derivative(x));
    EXPECT_EQ((cln::cl_RA)0, fx.derivative(y));
    MultivariatePolynomial<cln::cl_RA> f2x({(cln::cl_RA)2*x});
    EXPECT_EQ((cln::cl_RA)2, f2x.derivative(x));
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> f1dx({(cln::cl_RA)3*x*x*y*y, (cln::cl_RA)-2*x*y*y*y, Term<cln::cl_RA>((cln::cl_RA)1)});
    MultivariatePolynomial<cln::cl_RA> f1dy({(cln::cl_RA)2*x*x*x*y, (cln::cl_RA)-3*x*x*y*y});
    EXPECT_EQ(f1dx, f1.derivative(x));
    EXPECT_EQ(f1dy, f1.derivative(y));
    EXPECT_EQ(f1.derivative(x).derivative(y), f1.derivative(y).derivative(x));
}

TEST(MultivariatePolynomial, varInfo)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    
    VariablesInformation<false, MultivariatePolynomial<cln::cl_RA>> vi = f1.getVarInfo<false>();
    EXPECT_EQ(3, vi.getVarInfo(x)->maxDegree());
    EXPECT_EQ(1, vi.getVarInfo(x)->minDegree());
    EXPECT_EQ(3, vi.getVarInfo(x)->occurence());
    EXPECT_EQ(3, vi.getVarInfo(y)->maxDegree());
    EXPECT_EQ(2, vi.getVarInfo(y)->minDegree());
    EXPECT_EQ(2, vi.getVarInfo(y)->occurence());
    EXPECT_EQ(nullptr, vi.getVarInfo(z));
    
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)1*x*x*x*x*y*y, (cln::cl_RA)1*x*x*x*x*z*z ,(cln::cl_RA)-1*x*y, (cln::cl_RA)1*z});
    VariablesInformation<true, MultivariatePolynomial<cln::cl_RA>> vi2 = f2.getVarInfo<true>();
    EXPECT_EQ(4, vi2.getVarInfo(x)->maxDegree());
    EXPECT_EQ(1, vi2.getVarInfo(x)->minDegree());
    EXPECT_EQ(3, vi2.getVarInfo(x)->occurence());
    EXPECT_EQ(0, vi2.getVarInfo(x)->coeffs().count(0));
    EXPECT_EQ(0, vi2.getVarInfo(x)->coeffs().count(2));
    EXPECT_EQ(0, vi2.getVarInfo(x)->coeffs().count(3));
    EXPECT_EQ(MultivariatePolynomial<cln::cl_RA>({(cln::cl_RA)1*y*y, (cln::cl_RA)1*z*z}), vi2.getVarInfo(x)->coeffs().at(4)); 
    EXPECT_EQ(2, vi2.getVarInfo(y)->maxDegree());
    EXPECT_EQ(1, vi2.getVarInfo(y)->minDegree());
    EXPECT_EQ(2, vi2.getVarInfo(y)->occurence());
    EXPECT_EQ((cln::cl_RA)-1*x, vi2.getVarInfo(y)->coeffs().at(1)); 
    EXPECT_EQ((cln::cl_RA)1*x*x*x*x, vi2.getVarInfo(y)->coeffs().at(2)); 
    EXPECT_EQ(2, vi2.getVarInfo(z)->maxDegree());
    EXPECT_EQ(1, vi2.getVarInfo(z)->minDegree());
    EXPECT_EQ(2, vi2.getVarInfo(z)->occurence());
    EXPECT_EQ((cln::cl_RA)1, vi2.getVarInfo(z)->coeffs().at(1));
}

TEST(MultivariatePolynomial, cauchyBounds)
{
    EXPECT_TRUE(false);
}

TEST(MultivariatePolyonomial, factorization)
{
    EXPECT_TRUE(false);
}

TEST(MultivariatePolynomial, toUnivariatePolynomial)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*x, (cln::cl_RA)-1*x*x, (cln::cl_RA)3*x});   
    std::cout << f1.toUnivariatePolynomial() << std::endl;
}
