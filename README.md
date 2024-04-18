
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

There is a developmental 64-bit version, but I have not figured out a
good way to handle seeds, yet (currently just requires a seed be given).
It was primarily written for testing purposes, but if you want the 
fastest version, use the package `dqrng`.

## Installation

You can install the development version of rademacher like so:

``` r
devtools::install_github("kylebutts/rademacher")
```

## Benchmark

``` r
library(rademacher)
#> Loading required package: Rcpp
library(dqrng)
library(bench)

# 1,000,000 draws is ~ 15x as fast as stats::runif and 20x as fast as stats::sample
n = 1e7
bench::mark(
  sample_rademacher = rademacher::sample_rademacher(n),
  runif_rademacher  = (runif(n) > 0.5) * 2 - 1,
  samp = sample(x = c(1, -1), size = n, replace = TRUE),
  dqrrademacher = dqrng::dqrrademacher(n),
  check = FALSE, 
  iterations = 250
)
#> Warning: Some expressions had a GC in every iteration; so filtering is
#> disabled.
#> # A tibble: 5 × 6
#>   expression             min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>        <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 sample_rademacher  39.08ms   40.5ms     24.1     38.2MB     7.53
#> 2 runif_rademacher   88.17ms  92.96ms     10.6    190.7MB    13.6 
#> 3 samp               106.6ms 111.96ms      8.89   114.5MB     4.91
#> 4 dqsamp             44.48ms  48.94ms     19.8    114.5MB    17.9 
#> 5 dqrrademacher       3.71ms   4.28ms    196.      38.1MB    50.8

# 1000 draws is ~ 10x as fast
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
#> 1 runif_rademacher     8.4µs  10.25µs    94284.   22.16KB     9.43
#> 2 sample_rademacher   4.51µs   4.88µs   189422.    6.45KB    18.9 
#> 3 dqrrademacher     492.03ns 696.98ns  1194761.    3.95KB     0
```

This is totally open source (CC0 license). You can copy and paste to
your package without any credit needed.
