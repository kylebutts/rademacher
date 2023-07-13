
<!-- README.md is generated from README.Rmd. Please edit that file -->

# rademacher

<!-- badges: start -->
<!-- badges: end -->

The goal of `rademacher` is to help generate rademacher random variables
as quickly as possible using low-level C++ and bit operations. Note that
this function is now implemented in the `dqrng` and is even faster than
this version (see benchmark below). Use that if you don’t mind adding
dependencies to your package (a few C++ libraries are used in dqrng).

The Rademacher distribution draws -1 and 1 with probability 0.5 each.
The way I generate $N$ is (approximately) as follows:

1.  Draw `N / 31` integers from 1:(2^31 - 1).
2.  Each integer has 31 bits (0/1) variables and each bit is randomly
    generated. I grab all the bits and do $b * 2 - 1$ to convert to
    $-1, 1$.

This is totally open source. You can copy and past to your package
without any credit needed.

## Installation

You can install the development version of rademacher like so:

``` r
devtools::install_github("kylebutts/rademacher")
```

## Benchmark

``` r
library(rademacher)
library(dqrng)
library(bench)

# 1,000,000 draws is ~ 13x as fast as stats::runif, 25x as fast as stats::sample, 3x as fast as dqrng::dqsample
n = 1e7
bench::mark(
  sample_rademacher = rademacher::sample_rademacher(n),
  runif_rademacher  = (runif(n) > 0.5) * 2 - 1,
  samp = sample(x = c(1, -1), size = n, replace = TRUE),
  dqsamp = dqrng::dqsample(x = c(1,-1), size = n, replace = TRUE),
  dqrrademacher = dqrng::dqrrademacher(n),
  check = FALSE, 
  iterations = 250
)
#> Warning: Some expressions had a GC in every iteration; so filtering is
#> disabled.
#> # A tibble: 5 × 6
#>   expression             min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>        <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 sample_rademacher   4.51ms    5.3ms    162.      39.4MB    43.4 
#> 2 runif_rademacher   87.92ms   90.9ms     10.8    190.7MB    12.9 
#> 3 samp              106.05ms 110.15ms      9.13   114.5MB     4.71
#> 4 dqsamp             44.54ms  47.35ms     20.5    114.5MB    20.2 
#> 5 dqrrademacher       1.33ms   2.31ms    362.      38.1MB    91.3

# 1000 draws is ~ 4.5x as fast
n = 1000
bench::mark(
  runif_rademacher  = (runif(n) > 0.5) * 2 - 1,
  sample_rademacher = sample_rademacher(n),
  dqrrademacher = dqrng::dqrrademacher(n),
  check = FALSE
)
#> # A tibble: 3 × 6
#>   expression             min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>        <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 runif_rademacher    8.24µs  10.41µs    90629.   22.16KB     9.06
#> 2 sample_rademacher   1.02µs   1.39µs   519037.    6.62KB     0   
#> 3 dqrrademacher     410.01ns    656ns  1143065.    3.95KB   114.
```

This is totally open source (CC0 license). You can copy and past to your
package without any credit needed.

``` cpp
#include <Rcpp.h>
using namespace Rcpp;

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
```
