#ifndef _RANDVARS2_HPP_
#define _RANDVARS2_HPP_

#include <limits.h>
#include <gsl/gsl_rng.h>

extern gsl_rng *r;

extern double Exponential(const double a);

extern double UniformC(const double min, const double max);

extern unsigned int UniformD(const unsigned int min, const unsigned int max);

extern unsigned int Geometric(const double p);

extern unsigned int Binomial(const int n, const double p);

// Poisson random variable with mean a.
extern unsigned int Poisson(const double a);

// m-Erlang random variable with mean a.
extern double Erlang(int m, double a);

// Gaussian random variable with mean m and standard deviation s.
extern double Gaussian(double m, double s);

// Lognormal random variable with mean m and standard deviation s.
extern double Lognormal(double m, double s);

#endif
