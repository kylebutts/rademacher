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

// 64-bit version
class Splitmix64
{
public:
  Splitmix64() { state = 0; }
  Splitmix64(const uint64_t seed) : state(seed) {}

  void seed(const uint64_t seed)
  {
    state = seed;
  }

  uint64_t next_int()
  {
    uint64_t z = (state += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
  }

private:
  uint64_t state;
  const double twoPower64 = pow(2.0, 64);
};

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
//' @export
// [[Rcpp::export]]
Rcpp::IntegerVector sample_0_1_64bit(int n, int seed = R_random_bits())
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
  Splitmix64 random;
  random.seed(seed);

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 63.0);

  // Do RNG
  R_len_t k = 0;
  int J = 62;
  uint64_t bits;
  for (R_len_t i = 0; i < num_integers - 1; i++)
  {
    bits = random.next_int();
    for (int j = 0; j <= J; ++j, ++k)
    {
      res[k] = ((bits >> j) & 1);
    }
  }

  // Avoid needing if statement through all of this
  // https://github.com/daqana/dqrng/pull/50#discussion_r1249571555
  bits = random.next_int();
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
Rcpp::IntegerVector sample_rademacher_64bit(int n, int seed = R_random_bits())
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
  Splitmix64 random;
  random.seed(seed);

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 31.0);

  // Do RNG
  R_len_t k = 0;
  int J = 30;
  uint64_t bits;
  for (R_len_t i = 0; i < num_integers - 1; i++)
  {
    bits = random.next_int();

    for (int j = 0; j <= J; ++j, ++k)
    {
      res[k] = ((bits >> j) & 1) * 2 - 1;
    }
  }

  bits = random.next_int();
  for (int j = 0; k < n; ++j, ++k)
  {
    res[k] = ((bits >> j) & 1) * 2 - 1;
  }

  return res;
}
