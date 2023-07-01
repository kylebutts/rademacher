
<!-- README.md is generated from README.Rmd. Please edit that file -->

# rademacher

<!-- badges: start -->
<!-- badges: end -->

The goal of `rademacher` is to help generate rademacher random variables
as quickly as possible using low-level C and bit operations.

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
  runif_rademacher  = (runif(n) > 0.5) * 2 - 1,
  sample_rademacher = sample_rademacher(n),
  samp = sample(x = c(1, -1), size = n, replace = TRUE),
  dqsamp = dqsample(x = c(1,-1), size = n, replace = TRUE),
  check = FALSE, 
  iterations = 1000
)
#> Warning: Some expressions had a GC in every iteration; so filtering is
#> disabled.
#> # A tibble: 4 × 6
#>   expression             min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>        <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 runif_rademacher   85.16ms  90.58ms     10.8    190.7MB    16.0 
#> 2 sample_rademacher   6.04ms   6.79ms    132.      39.4MB    23.1 
#> 3 samp               106.4ms 109.78ms      8.98   114.5MB     8.97
#> 4 dqsamp              44.4ms  47.57ms     20.4    114.5MB    20.4

# 1000 draws is ~ 4.5x as fast
n = 1000
bench::mark(
  runif_rademacher  = (runif(n) > 0.5) * 2 - 1,
  sample_rademacher = sample_rademacher(n),
  check = FALSE
)
#> # A tibble: 2 × 6
#>   expression             min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>        <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 runif_rademacher    8.16µs  10.17µs    92925.   22.16KB     9.29
#> 2 sample_rademacher   1.15µs   1.56µs   540471.    6.62KB     0
```

This is totally open source. You can copy and past to your package
without any credit needed.

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
IntegerVector sample_rademacher(int n) {
  if (n < 0) {
    stop("n must be a positive integer");
  }
  if (n > 2147483647) {
    stop("n must be less than 2147483647");
  }

  IntegerVector result(n);

  // Calculate the number of integers needed based on N
  int num_integers = ceil(n / 31.0);

  // 2^31 - 1 = 2147483647
  IntegerVector random_integer = Rcpp::sample(2147483647, num_integers, true);
  
  int k = 0;
  int J = 30;
  for (int i = 0; i < num_integers; i++) {
    int curr = random_integer[i];

    // Make sure not to overfill the result vector
    if (i == num_integers - 1) {
      J = (n % 31) - 1;
    } 
    
    for (int j = J; j >= 0; j--) {
      
      result[k] = ((curr >> j) & 1) * 2 - 1;
      k = k + 1;
    }
  }

  return result;
}

```
