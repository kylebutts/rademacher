#include <Rcpp.h>

// Create a random 32-bit int using R's PRNGs.
// inspiration:
// https://github.com/daqana/dqrng/blob/9e83c4d035a2ef41c42171e38adeb47a4f9d2003/inst/include/R_randgen.h#L18
// https://github.com/wch/r-source/blob/trunk/src/main/RNG.c
inline int R_random_bits()
{
  int v = 65536 * (int)floor(R::unif_rand() * 65536);
  v += (int)floor(R::unif_rand() * 65536);
  return v;
}

//' Sample random 32-bit integers fast
//'
//' @param n Integer, number of random variables to draw
//'
//' @return integer vector of length n
//'
//' @export
// [[Rcpp::export]]
Rcpp::IntegerVector sample_random_bits(int n)
{
  if (n < 0)
  {
    Rcpp::stop("n must be a positive integer");
  }
  if (n > 2147483647)
  {
    Rcpp::stop("n must be less than 2147483647");
  }
  
  Rcpp::IntegerVector res = Rcpp::no_init(n);
  for (int j = 0; j < n; ++j)
  {
    res[j] = R_random_bits();
  }
  return res;
}

//' Sample Bernoulli(0.5) distribution really fast
//'
//' @description This uses a fancy trick to draw Bernoulli(0.5) r.v.s
//'   quickly. To do so, the function draws from 1:(2^31 - 1), and then
//'   uses each bit of the integer to determine 31 values of 0/1. This
//'   allows for 31 0/1 random variables to be drawn per random draw.
//'
//' @param n Integer, number of random variables to draw
//'
//' @return integer vector of length n with values -1 or 1
//'
//' @export
// [[Rcpp::export]]
Rcpp::IntegerVector sample_0_1(int n)
{
  if (n < 0)
  {
    Rcpp::stop("n must be a positive integer");
  }
  if (n > 2147483647)
  {
    Rcpp::stop("n must be less than 2147483647");
  }

  Rcpp::IntegerVector res = Rcpp::no_init(n);

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 31.0);

  // Do RNG
  R_len_t k = 0;
  int J = 30;
  int bits;
  for (R_len_t i = 0; i < num_integers - 1; i++)
  {
    bits = R_random_bits();

    for (int j = 0; j <= J; ++j, ++k)
    {
      res[k] = ((bits >> j) & 1);
    }
  }

  bits = R_random_bits();
  for (int j = 0; k < n; ++j, ++k)
  {
    res[k] = ((bits >> j) & 1);
  }

  return res;
}

//' Sample TRUE/FALSE with equal probability distribution really fast
//'
//' @description This uses a fancy trick to draw Bernoulli(0.5) r.v.s
//'   quickly. To do so, the function draws from 1:(2^31 - 1), and then
//'   uses each bit of the integer to determine 31 values of 0/1. This
//'   allows for 31 0/1 random variables to be drawn per random draw.
//'
//' @param n Integer, number of random variables to draw
//'
//' @return logical vector of length n with values -1 or 1
//'
//' @export
// [[Rcpp::export]]
Rcpp::LogicalVector sample_logical(int n)
{
  if (n < 0)
  {
    Rcpp::stop("n must be a positive integer");
  }
  if (n > 2147483647)
  {
    Rcpp::stop("n must be less than 2147483647");
  }

  Rcpp::LogicalVector res = Rcpp::no_init(n);

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 31.0);

  // Do RNG
  R_len_t k = 0;
  int J = 30;
  int bits;
  for (R_len_t i = 0; i < num_integers - 1; i++)
  {
    bits = R_random_bits();

    for (int j = 0; j <= J; ++j, ++k)
    {
      res[k] = ((bits >> j) & 1);
    }
  }

  bits = R_random_bits();
  for (int j = 0; k < n; ++j, ++k)
  {
    res[k] = ((bits >> j) & 1);
  }

  return res;
}

//' Sample Rademacher distribution really fast
//'
//' @description This uses a fancy trick to draw Rademacher weights very
//'   quickly. To do so, the function draws from 1:(2^31 - 1), and then
//'   uses each bit of the integer to determine 31 values of 0/1. This
//'   allows for 31 Rademacher random variables to be drawn per random draw.
//'   Taking those bits * 2 - 1 gives the Rademacher random variables.
//'
//' @param n Integer, number of random variables to draw
//'
//' @return integer vector of length n with values -1 or 1
//'
//' @export
// [[Rcpp::export]]
Rcpp::IntegerVector sample_rademacher(int n)
{
  if (n < 0)
  {
    Rcpp::stop("n must be a positive integer");
  }
  if (n > 2147483647)
  {
    Rcpp::stop("n must be less than 2147483647");
  }

  Rcpp::IntegerVector res = Rcpp::no_init(n);

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 31.0);

  // Do RNG
  R_len_t k = 0;
  int J = 30;
  int bits;
  for (R_len_t i = 0; i < num_integers - 1; i++)
  {
    bits = R_random_bits();

    for (int j = 0; j <= J; ++j, ++k)
    {
      res[k] = ((bits >> j) & 1) * 2 - 1;
    }
  }

  bits = R_random_bits();
  for (int j = 0; k < n; ++j, ++k)
  {
    res[k] = ((bits >> j) & 1) * 2 - 1;
  }

  return res;
}
