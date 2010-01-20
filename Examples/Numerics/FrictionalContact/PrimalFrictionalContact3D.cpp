/* Siconos-sample version 3.0.0, Copyright INRIA 2005-2008.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */

/*!\file FrictionalContact3D.cpp
  A very simple example to chow how to use SiconosNumerics to solve the time--discretized FrictionalContact3D problem with a dense storage.
  The problem: Find \f$(reaction,velocity)\f$ such that:\n

  \f$
  \left\lbrace
  \begin{array}{l}
  M \ reaction + q = velocity \\
  0 \le reaction_n \perp velocity_n \ge 0 \\
  -velocity_t \in \partial\psi_{D_(\mu reaction_n)}(reaction_t)\\
  D_(\mu reaction_n) = \{ reaction_t \mid  \|reaction_t\| \leq \mu reaction_n  \}
  \end{array}
  \right.
  \f$

  \f$ reaction, velocity, q\f$ are vectors of size n and \f$ M \f$ is a nXn matrix, with \f$ n = 2*nc or 3*nc \f$, nc being the number of contacts. \n
  \f$ reaction_n\f$ represents the normal part of the reaction while \f$ reaction_t\f$ is its tangential part.

  \f$ \mu \f$ is the friction coefficient (it may be different for each contact).

  Use the generic function frictionContact3D_driver() to call one the the specific solvers listed below:

  - frictionContact3D_nsgs() : non-smooth Gauss-Seidel solver

  (see the functions/solvers list in FrictionContact3D_Solvers.h)

  FrictionContact3D problems needs some specific parameters, given to the FrictionContact3D_driver() function thanks to a Solver_Options structure. \n
  They are:\n
     - the name of the solver (ex: NSGS), used to switch to the right solver function
     - iparam[0]: max. number of iterations allowed
     - iparam[1]:
     - dparam[0]: tolerance
     - isStorageSparse: 1 if a SparseBlockStructuredMatrix is used for M, else 0 (double* storage)


  \brief
*/

#include "SiconosNumerics.h"

int main(int argc, char* argv[])
{


  // Problem Definition
  int info = -1;



  int NC = 1;//Number of contacts
  int Ndof = 9;//Number of DOF
  int m = 3;
  int n = 9;
  double M[81] = {1, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 1, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 1, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 1, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 1, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 1, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 1, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 1, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 1
                 };

  double H[27] = {1, 0, 0, 0, 0, 0, -1, 0, 0,
                  0, 1, 0, 0, 0, 0, 0, -1, 0,
                  0, 0, 1, 0, 0, 0, 0, 0, -1
                 };

  double q[9] = { -3, -3, -3, -1, 1, 3, -1, 1, 3};
  double b[3] = {0, 0, 0};
  double mu[1] = {0.1};

  /*     int NC = 3;//Number of contacts  */
  /*     int Ndof = 9;//Number of DOF  */
  /*     double M[81] = {1, 0, 0, 0, 0, 0, 0, 0, 0,  */
  /*        0, 1, 0, 0, 0, 0, 0, 0, 0,  */
  /*        0, 0, 1, 0, 0, 0, 0, 0, 0,  */
  /*        0, 0, 0, 1, 0, 0, 0, 0, 0,  */
  /*        0, 0, 0, 0, 1, 0, 0, 0, 0,  */
  /*        0, 0, 0, 0, 0, 1, 0, 0, 0,  */
  /*        0, 0, 0, 0, 0, 0, 1, 0, 0,  */
  /*        0, 0, 0, 0, 0, 0, 0, 1, 0,  */
  /*        0, 0, 0, 0, 0, 0, 0, 0, 1}; */
  /*     double H[81] = {1, 0, 0, 0, 0, 0, 0, 0, 0,  */
  /*        0, 1, 0, 0, 0, 0, 0, 0, 0,  */
  /*        0, 0, 1, 0, 0, 0, 0, 0, 0,  */
  /*        0, 0, 0, 1, 0, 0, 0, 0, 0,  */
  /*        0, 0, 0, 0, 1, 0, 0, 0, 0,  */
  /*        0, 0, 0, 0, 0, 1, 0, 0, 0,  */
  /*        0, 0, 0, 0, 0, 0, 1, 0, 0,  */
  /*        0, 0, 0, 0, 0, 0, 0, 1, 0,  */
  /*        0, 0, 0, 0, 0, 0, 0, 0, 1}; */


  /*     double q[9] = {-1, 1, 3, -1, 1, 3, -1, 1, 3}; */
  /*     double b[9] = {0, 0, 0,0, 0, 0,0, 0, 0 }; */
  /*     double mu[3] = {0.1,0.1,0.1};    */


  int i = 0, j = 0, k = 0;

  PrimalFrictionContact_Problem numericsProblem;
  numericsProblem.numberOfContacts = NC;
  numericsProblem.dimension = 3;
  numericsProblem.isComplete = 0;
  numericsProblem.mu = mu;
  numericsProblem.q = q;
  numericsProblem.b = b;


  numericsProblem.M = (NumericsMatrix*)malloc(sizeof(NumericsMatrix));
  NumericsMatrix *MM = numericsProblem.M ;
  MM->storageType = 0;
  MM->matrix0 = M;
  MM->size0 = n;
  MM->size1 = n;


  numericsProblem.H  = (NumericsMatrix*)malloc(sizeof(NumericsMatrix));
  NumericsMatrix *HH = numericsProblem.H;
  HH->storageType = 0;
  HH->matrix0 = H;
  HH->size0 = n;
  HH->size1 = m;


  /*     FILE * foutput = fopen("Example_PrimalFrictionContact.dat", "w"); */
  /*     primalFrictionContact_printInFile(&numericsProblem,  foutput ); */
  /*     fclose(foutput); */



  // Unknown Declaration

  double *reaction = (double*)malloc(m * sizeof(double));
  double *velocity = (double*)malloc(m * sizeof(double));
  double *globalVelocity = (double*)malloc(n * sizeof(double));
  for (k = 0 ; k < m; k++)
  {
    velocity[k] = 0.0;
    reaction[k] = 0.0;
  };
  for (k = 0 ; k < n; k++)
  {
    globalVelocity[k] = 0.0;
  };

  // Numerics and Solver Options

  Numerics_Options numerics_options;
  numerics_options.verboseMode = 1; // turn verbose mode to off by default


  Solver_Options * numerics_solver_options;
  char solvername[10] = "NSGS";
  /*\warning Must be adpated  for future primalFrictionContact3D_setDefaultSolverOptions*/
  frictionContact3D_setDefaultSolverOptions(&numerics_solver_options, solvername);
  numerics_solver_options->dparam[0] = 1e-14;
  numerics_solver_options->iparam[0] = 100000;
  //Driver call
  i = 0;
  info = primalFrictionContact3D_driver(&numericsProblem,
                                        reaction , velocity, globalVelocity,
                                        numerics_solver_options, &numerics_options);


  /*\warning Must be adpated  for future primalFrictionContact3D_setDefaultSolverOptions*/
  frictionContact3D_deleteDefaultSolverOptions(&numerics_solver_options, solvername);
  // Solver output
  printf("\n");
  for (k = 0 ; k < m; k++) printf("velocity[%i] = %12.8e \t \t reaction[%i] = %12.8e \n ", k, velocity[k], k , reaction[k]);
  for (k = 0 ; k < n; k++) printf("globalVelocity[%i] = %12.8e \t \n ", k, globalVelocity[k]);
  printf("\n");

  free(reaction);
  free(velocity);
  free(globalVelocity);
  free(numericsProblem.M);
  free(numericsProblem.H);
  return info;


}
