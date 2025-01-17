/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2022 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <stdio.h>                        // for printf, NULL
#include <stdlib.h>                       // for free, malloc, calloc
#include "MCP_Solvers.h"                  // for mcp_old_driver_init, mcp_ol...
#include "MCP_cst.h"                      // for SICONOS_MCP_OLD_FB
#include "MixedComplementarityProblem.h"  // for MixedComplementarityProblem...
#include "NonSmoothDrivers.h"             // for mcp_old_driver
#include "NumericsFwd.h"                  // for MixedComplementarityProblem...
#include "NumericsVerbose.h"              // for numerics_set_verbose
#include "SolverOptions.h"                // for SolverOptions, solver_optio...
static double * M;
static double * q;

void testF(int size, double *z, double * F);
void testF(int size, double *z, double * F)
{
  /* printf("call to MCP function F(z) ...\n");   */
  /* for (int i =0 ; i <size; i++) */
  /* { */
  /*   printf("z[%i]= %lf\t",i,z[i]); */
  /* } */
  /* printf("\n"); */
  for(int i = 0 ; i < size; i++)
  {
    F[i] = q[i];
    for(int j = 0 ; j < size ;  j++)
    {
      F[i] += M[i + j * size] * z[j] ;
    }
  }
  /* for (int i =0 ; i <size; i++) */
  /* { */
  /*   printf("F[%i]= %lf\t",i,F[i]); */
  /* } */
  /* printf("\n"); */
  /* printf("End call to MCP function F(z) ...\n");   */
}

void testNablaF(int size, double *z, double *nablaF);
void testNablaF(int size, double *z, double *nablaF)
{
  /* printf("call to MCP function nablaF(z) ...\n"); */

  for(int i = 0 ; i < size; i++)
  {
    for(int j = 0 ; j < size ;  j++)
    {
      nablaF[i + j * size] = M[i + j * size];
    }
  }


}

int main(void)
{
  printf(" Start tests for MCP solvers.\n");

  int info = 0 ;
  int n=10;

  /* Set solver options */
  SolverOptions * options = solver_options_create(SICONOS_MCP_OLD_FB);
  /* FB solver */
  /* Create a MixedComplementarityProblem */
  MixedComplementarityProblem_old* problem = (MixedComplementarityProblem_old *)malloc(sizeof(MixedComplementarityProblem_old));

  problem->sizeEqualities = n-5;
  problem->sizeInequalities = 5;
  problem->computeFmcp = &testF ;
  problem->computeNablaFmcp = &testNablaF ;
  problem->Fmcp = NULL;
  problem->nablaFmcp = NULL;

  M = (double *) calloc(n*n,sizeof(double));
  q = (double *) calloc(n,sizeof(double));

  for(int i =0; i< n; i++)
  {
    q[i] = - i + 7.;
    M[i+i*n] = 2.0 ;
    if(i < n-1)
      M[i+(i+1)*n] =1.0;
    if(i >0)
      M[i+(i-1)*n] =1.0;
  }
  //NM_dense_display(M,n,n,n);

  numerics_set_verbose(1);

  int size = problem->sizeEqualities + problem->sizeInequalities ;
  double * z = (double *)malloc(size * sizeof(double));
  double * w = (double *)malloc(size * sizeof(double));

  for(int i = 0 ; i < size; i++)
  {
    z[i] = 0.0;
    w[i] = 0.0;
  }



  /* Initialize the solver */
  mcp_old_driver_init(problem, options) ;
  info = mcp_old_driver(problem, z, w,  options);
  mcp_old_driver_reset(problem, options) ;
  /// TODO : write a real test ... ////


  for(int i = 0 ; i < size; i++)
  {
    printf("z[%i]= %lf\t", i, z[i]);
  }
  printf("\n");
  for(int i = 0 ; i < size; i++)
  {
    printf("w[%i]= %lf\t", i, w[i]);
  }
  printf("\n");
  solver_options_delete(options);
  options = NULL;

  free(z);
  free(w);
  free(problem);
  printf("End of MCP solvers test. \n");

  return info;
}
