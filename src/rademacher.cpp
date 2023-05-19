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
IntegerVector sample_0_1(int n) {
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
      result[k] = ((curr >> j) & 1);
      k = k + 1;
    }
  }

  return result;
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

