/* Siconos-sample , Copyright INRIA 2005-2011.
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


// =============================== Robot arm sample  ===============================
//
// Keywords: LagrangianDS, LagrangianLinear relation, Moreau TimeStepping, LCP.
//
// =============================================================================================

#include "SiconosKernel.hpp"
#include <math.h>

#define PI 3.14159265

using namespace std;

int main(int argc, char* argv[])
{
  boost::timer time;
  time.restart();
  try
  {

    // ================= Creation of the model =======================

    // User-defined main parameters
    unsigned int nDof = 6;           // degrees of freedom for robot arm
    double t0 = 0;                   // initial computation time
    double T = 6.0;                   // final computation time
    double h = 1e-2;                // time step
    double criterion = 1e-8;
    unsigned int maxIter = 200000;
    double e = 0.0;                  // nslaw

    // -> mind to set the initial conditions below.

    // -------------------------
    // --- Dynamical systems ---
    // -------------------------

    // unsigned int i;
    // --- DS: manipulator arm ---

    // Initial position (angles in radian)
    SP::SiconosVector q0(new SiconosVector(nDof)), v0(new SiconosVector(nDof));
    q0->zero();
    v0->zero();
    (*q0)(1) = PI / 3;
    (*q0)(2) = -PI / 6;
    (*q0)(4) = PI / 6;
    (*v0)(0) = -0.34;
    (*v0)(3) = 0.59;
    (*v0)(5) = -0.34;

    SP::LagrangianDS arm(new LagrangianDS(q0, v0, "RX90Plugin:mass"));

    // external plug-in
    arm->setComputeNNLFunction("RX90Plugin.so", "NNL");
    //arm->setComputeJacobianNNLFunction(1,"RX90Plugin.so","jacobianVNNL");
    arm->setComputeJacobianNNLqDotFunction("RX90Plugin.so", "jacobianVNNL");
    //arm->setComputeJacobianNNLFunction(0,"RX90Plugin.so","jacobianNNLq");
    arm->setComputeJacobianNNLqFunction("RX90Plugin.so", "jacobianNNLq");
    arm->setComputeFIntFunction("RX90Plugin.so", "U");
    //arm->setComputeJacobianFIntFunction(1,"RX90Plugin.so","jacobFintV");
    arm->setComputeJacobianFIntqDotFunction("RX90Plugin.so", "jacobFintV");
    //arm->setComputeJacobianFIntFunction(0,"RX90Plugin.so","jacobFintQ");
    arm->setComputeJacobianFIntqFunction("RX90Plugin.so", "jacobFintQ");
    // -------------------
    // --- Interactions---
    // -------------------

    //  - one with Lagrangian non linear relation to define contact with ground
    //  Both with newton impact nslaw.

    // -- relations --

    SP::NonSmoothLaw nslaw(new NewtonImpactNSL(e));

    SimpleMatrix H(12, 6);
    SiconosVector b(12);
    H.zero();
    for (unsigned int i = 0; i < nDof; i++)
    {
      H(2 * i, i) = 1;
      H(2 * i + 1, i) = -1;
    }
    b(0) = PI * 167.0 / 180.0;
    b(1) = b(0);
    b(2) = PI * 137.5 / 180.0;
    b(3) = b(2);
    b(4) = PI * 142.5 / 180.0;
    b(5) = b(4);
    b(6) = PI * 270.0 / 180.0;
    b(7) = b(6);
    b(8) = PI * 112.5 / 180.0;
    b(9) = b(8);
    b(10) = PI * 270.0 / 180.0;
    b(11) = b(10);
    SP::Relation relation(new LagrangianLinearTIR(H, b));
    SP::Interaction inter(new Interaction(12, nslaw, relation));

    // -------------
    // --- Model ---
    // -------------

    SP::Model RX90(new Model(t0, T));
    RX90->nonSmoothDynamicalSystem()->insertDynamicalSystem(arm);
    RX90->nonSmoothDynamicalSystem()->link(inter, arm);
        
    // ----------------
    // --- Simulation ---
    // ----------------

    // -- Time discretisation --
    SP::TimeDiscretisation t(new TimeDiscretisation(t0, h));

    SP::TimeStepping s(new TimeStepping(t));

    // -- OneStepIntegrators --
    SP::OneStepIntegrator OSI(new Moreau(arm, 0.5));
    s->insertIntegrator(OSI);


    // -- OneStepNsProblem --
    SP::OneStepNSProblem osnsp(new LCP());
    s->insertNonSmoothProblem(osnsp);

    cout << "=== End of model loading === " << endl;

    // =========================== End of model definition ===========================  dataPlot(k,7) = (*inter->y(0))(0);


    // ================================= Computation
    // --- Simulation initialization ---


    RX90->initialize(s);
    cout << "End of simulation initialisation" << endl;

    int k = 0;
    int N = ceil((T - t0) / h) + 1; // Number of time steps

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    unsigned int outputSize = 13;
    SimpleMatrix dataPlot(N + 1, outputSize);
    // For the initial time step:

    SP::SiconosVector q = arm->q();
    SP::SiconosVector v = arm->velocity();
    SP::EventsManager eventsManager = s->eventsManager();

    dataPlot(k, 0) =  RX90->t0();
    dataPlot(k, 1) = (*q)(0);
    dataPlot(k, 2) = (*q)(1);
    dataPlot(k, 3) = (*q)(2);
    dataPlot(k, 4) = (*q)(3);
    dataPlot(k, 5) = (*q)(4);
    dataPlot(k, 6) = (*q)(5);
    dataPlot(k, 7) = (*v)(0);
    dataPlot(k, 8) = (*v)(1);
    dataPlot(k, 9) = (*v)(2);
    dataPlot(k, 10) = (*v)(3);
    dataPlot(k, 11) = (*v)(4);
    dataPlot(k, 12) = (*v)(5);


    time.restart();
    while (s->hasNextEvent())
    {
      s->advanceToEvent();
      s->processEvents();

      k++;
      dataPlot(k, 0) =  s->startingTime();
      dataPlot(k, 1) = (*q)(0);
      dataPlot(k, 2) = (*q)(1);
      dataPlot(k, 3) = (*q)(2);
      dataPlot(k, 4) = (*q)(3);
      dataPlot(k, 5) = (*q)(4);
      dataPlot(k, 6) = (*q)(5);
      dataPlot(k, 7) = (*v)(0);
      dataPlot(k, 8) = (*v)(1);
      dataPlot(k, 9) = (*v)(2);
      dataPlot(k, 10) = (*v)(3);
      dataPlot(k, 11) = (*v)(4);
      dataPlot(k, 12) = (*v)(5);
    }
    cout << "Computation Time " << time.elapsed()  << endl;

    // --- Output files ---
    ioMatrix::write("result.dat", "ascii", dataPlot, "noDim");

  }

  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught in RX90 manipulator " << endl;
  }
}
