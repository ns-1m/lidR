/*
===============================================================================

PROGRAMMERS:

jean-romain.roussel.1@ulaval.ca  -  https://github.com/Jean-Romain/lidR

COPYRIGHT:

Copyright 2016 Jean-Romain Roussel

This file is part of lidR R package.

lidR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

===============================================================================
*/

#include <Rcpp.h>
// [[Rcpp::depends(RcppProgress)]]
#include <progress.hpp>

using namespace Rcpp;

// Defined in cxx_utils.cpp
NumericVector distance(NumericVector, NumericVector, double, double);

// [[Rcpp::export]]
IntegerVector algo_li2012(NumericVector X, NumericVector Y, const NumericVector Z, const double dt1, const double dt2, const double R)
{
  bool end = false;
<<<<<<< HEAD
=======

  double dt1 = dt(0);
  double dt2 = dt(1);
>>>>>>> 7c8b4401a5a7ebde68b6e7581993fc5922176992

  int ni = X.length();
  int n  = ni;
  int k  = 1;

  Progress p(ni, true);

  IntegerVector idpoint = seq_len(ni)-1;
  IntegerVector idtree(ni);

  while(!end)
  {
    // Intial step not point in P or N
    LogicalVector N(n);
    NumericVector XP,XN,YP,YN;

    if (Progress::check_abort() )
      return  IntegerVector::create(0);
    else
      p.update(ni-n);

    // element 0 is the current highest points and is in P
    XP.push_back(X(0));
    YP.push_back(Y(0));

    // Add dummy point in N
    XN.push_back(X(0)+100);
    YN.push_back(Y(0)+100);

    // Compute the distance between the local max u and all the other point
    NumericVector d = distance(X, Y, X(0), Y(0));

    for (int i = 1 ; i < n ; ++i)
    {
      if(d[i] > R)            // If d > R those points are not the current segmented tree
      {
        N[i] = true;
      }
      else                    // If d <= R classify point base on Li et al. rules
      {
        double dmin1 = min(distance(XP, YP, X[i], Y[i]));
        double dmin2 = min(distance(XN, YN, X[i], Y[i]));
<<<<<<< HEAD
        double dt    = (Z[idpoint[i]] > 15) ? dt2 : dt1;
=======
        double dt    = (Z[i] > 15) ? dt2 : dt1;
>>>>>>> 7c8b4401a5a7ebde68b6e7581993fc5922176992

        if ( (dmin1 > dt) || (dmin1 <= dt & dmin1 > dmin2) )
        {
          N[i] = true;
          XN.push_back(X(i));
          YN.push_back(Y(i));
        }
        else if (dmin1 <= dt & dmin1 <= dmin2)
        {
          XP.push_back(X(i));
          YP.push_back(Y(i));

<<<<<<< HEAD
          idtree[idpoint[i]] = k;
=======
          int id = idpoint[i];
          idtree[id] = k;
>>>>>>> 7c8b4401a5a7ebde68b6e7581993fc5922176992
        }
      }
    }

    // Increase current tree id
    k++;

    // Keep the point in N and redo the loop with remining points
    X = X[N];
    Y = Y[N];
<<<<<<< HEAD
=======
    Z = Z[N];
>>>>>>> 7c8b4401a5a7ebde68b6e7581993fc5922176992
    idpoint = idpoint[N];

    n = X.length();

    if(n == 0)
      end = true;
  }

  return idtree;
}
