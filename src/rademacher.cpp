#include <Rcpp.h>
using namespace Rcpp;

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
IntegerVector sample_0_1(int n)
{
  if (n < 0)
  {
    stop("n must be a positive integer");
  }
  if (n > 2147483647)
  {
    stop("n must be less than 2147483647");
  }

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 31.0);

  // 2^31 - 1 = 2147483647
  IntegerVector random_integer = sample(2147483647, num_integers, true);

  IntegerVector res = no_init(n);
  R_len_t k = 0;
  int J = 30;
  int bits;
  for (R_len_t i = 0; i < num_integers - 1; i++)
  {
    bits = random_integer[i];

    for (int j = 0; j <= J; ++j, ++k)
    {
      res[k] = ((bits >> j) & 1);
    }
  }

  bits = random_integer[num_integers - 1];
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
IntegerVector sample_rademacher(int n)
{
  if (n < 0)
  {
    stop("n must be a positive integer");
  }
  if (n > 2147483647)
  {
    stop("n must be less than 2147483647");
  }

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 31.0);

  // 2^31 - 1 = 2147483647
  IntegerVector random_integer = sample(2147483647, num_integers, true);

  IntegerVector res = no_init(n);
  R_len_t k = 0;
  int J = 30;
  int bits;
  for (R_len_t i = 0; i < num_integers - 1; i++)
  {
    bits = random_integer[i];

    for (int j = 0; j <= J; ++j, ++k)
    {
      res[k] = ((bits >> j) & 1) * 2 - 1;
    }
  }

  bits = random_integer[num_integers - 1];
  for (int j = 0; k < n; ++j, ++k)
  {
    res[k] = ((bits >> j) & 1) * 2 - 1;
  }

  return res;
}
