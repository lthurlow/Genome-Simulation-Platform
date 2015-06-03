#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "randvars2.hpp"

/* randsvars2 cc and hpp are from Brad Smiths CAR simulations */
/* They use GSL (Gnu Scientific Library) for random distribution functions*/

double Exponential(const double a)
{
  return(gsl_ran_exponential(r,a));
}

unsigned int Geometric(const double p)
{
  return(gsl_ran_geometric(r,p));
}

unsigned int Binomial(const int    n,
		      const double p )
{
  return(gsl_ran_binomial(r,p,n));
}

unsigned int Poisson(const double a)
{
  return(gsl_ran_poisson(r,a));
}

double Erlang(int    m,
	      double a )
{
  return(gsl_ran_gamma(r,m,a));
}

double Gaussian(double m,
		double s )
{
  return(m + gsl_ran_gaussian(r,s));
}

double Lognormal(double m,
		 double s )
{
  return(gsl_ran_lognormal(r,m,s));
}

static double doubleRandom()
{
  return((double)gsl_rng_get(r)/(double)(gsl_rng_max(r)));
}

double UniformC(const double min,
		const double max )
{
  return(min + (max - min) * doubleRandom());
}

unsigned int UniformD(const unsigned int min,
		      const unsigned int max )
{
  return(min + gsl_rng_uniform_int(r,max-min));
}
