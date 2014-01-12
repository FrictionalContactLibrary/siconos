/* Siconos-Kernel, Copyright INRIA 2005-2012.
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
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
*/
#include "LinearSensor.hpp"

#include "SensorFactory.hpp"
#include "SiconosVector.hpp"
#include "DynamicalSystem.hpp"
#include "Model.hpp"
#include "TimeDiscretisation.hpp"


LinearSensor::LinearSensor(SP::DynamicalSystem ds): ControlSensor(LINEAR_SENSOR, ds), _k(0), _nSteps(0)
{}

LinearSensor::LinearSensor(SP::DynamicalSystem ds, SP::SimpleMatrix matC, SP::SimpleMatrix matD):
  ControlSensor(LINEAR_SENSOR, ds), _k(0), _matC(matC), _matD(matD), _nSteps(0)
{}

LinearSensor::~LinearSensor()
{
  std::cout << "LinearSensor destructor called" <<std::endl;
}

void LinearSensor::initialize(const Model& m)
{
  // Call initialize of base class
  ControlSensor::initialize(m);

  // consistency checks
  unsigned int colC = _matC->size(1);
  unsigned int rowC = _matC->size(0);
  // What happen here if we have more than one DS ?
  // This may be unlikely to happen.
  //  _DS = _model->nonSmoothDynamicalSystem()->dynamicalSystemNumber(0);
  if (colC != _DS->getN())
  {
    RuntimeException::selfThrow(" LinearSensor::initialize - The number of column of the C matrix must be equal to the length of x");
  }
  if (_matD)
  {
    unsigned int rowD = _matD->size(0);
    if (rowC != rowD)
    {
      RuntimeException::selfThrow("C and D must have the same number of rows");
    }
  }

  // --- Get the values ---
  // -> saved in a matrix data
  // -> event
  _storedY.reset(new SiconosVector(rowC));
  //  (_data[_eSensor])["StoredY"] = storedY;
  // set the dimension of the output
  *_storedY = prod(*_matC, *_DSx);
}

void LinearSensor::capture()
{
  *_storedY = prod(*_matC, *_DSx);
  // untested
  if (_matD)
//    *_storedY += prod(*_matD, *_DS->z());
  //  _dataPlot->setSubRow(_k, 1, _storedY);
  _k++;

  if (_delay > 0)
  {
    _bufferY.push_back(_storedY);
  }
}
void  LinearSensor::setC(const SimpleMatrix& C)
{
  *_matC = C;
}

void  LinearSensor::setD(const SimpleMatrix& D)
{
  *_matD = D;
}

AUTO_REGISTER_SENSOR(LINEAR_SENSOR, LinearSensor)
