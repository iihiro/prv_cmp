#ifndef POLY_UTIL_HPP
#define POLY_UTIL_HPP

void ShowPolyInfo (const usint N, const Plaintext& poly) {
  for (size_t i = 0; i != N; ++i) {
    if (poly->GetCoefPackedValue().at(i) != 0) {
      cout << "Coeff on x^" << i << ": " << poly->GetCoefPackedValue().at(i) << endl;
    }
  }
}

void MakeMonomialCoeff (const usint d, const usint N, const bool is_neg, vector<int64_t>& coeff) {
  assert(d <= N);
  if (d == N) {
    coeff[0] = -1;
    return;
  }

  if (d == 0) {
    coeff[0] = 1;
    return;
  }

  if (is_neg) {
    coeff[N - d] = -1;
  } else {
    coeff[d] = 1;
  }
}

#endif // POLY_UTIL_HPP
