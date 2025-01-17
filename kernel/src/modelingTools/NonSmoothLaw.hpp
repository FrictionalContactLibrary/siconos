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

/*! \file NonSmoothLaw.hpp
        \brief Base (abstract) class for a nonsmooth law
*/

#ifndef NSLAW_H
#define NSLAW_H

#include "SiconosConst.hpp"
#include "SiconosFwd.hpp"
#include "SiconosPointers.hpp"
#include "SiconosSerialization.hpp" // For ACCEPT_SERIALIZATION
#include "SiconosVisitor.hpp"

/** Non Smooth Laws (NSL) Base Class
 *
 * This class is the base class for all nonsmooth laws in Siconos.
 * A nonsmooth law characterize the (nonsmooth) relationship between 2
 * variables, usually designated by \f$y\f$ and \f$\lambda\f$. \f$y\f$ is most
 * of time seen as the "input" from DynamicalSystems and is given by a Relation
 * linked to this nonsmoothlaw. \f$\lambda\f$ is then the "output" and through
 * the same Relation is fed back to one or more DynamicalSystem.
 *
 * classical examples of nonsmooth law include:
 * - RelayNSL: \f$-y \in \mathcal{N}_{[-1,1]}(\lambda)\quad
 * \Longleftrightarrow\quad -\lambda \in \mbox{sgn} (y)\f$
 * - NormalConeNSL: given a polytope $K$, \f$-\lambda \in \partial
 * \sigma_{-K}(y)\quad\Longleftrightarrow\quad y\in\mathcal{N}_{-K}(-\lambda)\f$
 * - ComplementarityConditionNSL: \f$0\leq y \perp \lambda \geq 0\f$
 * - NewtonImpactNSL and NewtonImpactFrictionNSL for impact, without or with
 * friction
 * - MultipleImpactNSL for a multiple impact law
 * - MixedComplementarityConditionNSL
 *
 * The computation of both \f$y\f$ and \f$\lambda\f$ is carried on by a solver
 * in Numerics through a OneStepNSProblem object.
 */
class NonSmoothLaw {
protected:
  // serialization hooks
  ACCEPT_SERIALIZATION(NonSmoothLaw);

  /** "size" of the NonSmoothLaw */
  unsigned int _size{0};

  // Rule of five ...
  NonSmoothLaw() = default;
  NonSmoothLaw(const NonSmoothLaw &) = delete;
  NonSmoothLaw(NonSmoothLaw &&) = delete;
  NonSmoothLaw &operator=(const NonSmoothLaw &) = delete;
  NonSmoothLaw &operator=(const NonSmoothLaw &&) = delete;

public:
  /** basic constructor
   * \param size the nonsmooth law size
   */
  NonSmoothLaw(unsigned int size) : _size(size) {}

  /** destructor */
  virtual ~NonSmoothLaw() noexcept = default;

  /** \return a boolean value which determines if the NS Law is verified.
      Not implemented for the moment.
  */
  virtual bool isVerified() const
  {
    THROW_EXCEPTION("NonSmoothLaw::isVerified, not yet implemented!");
    return false;
  }

  /** \return the size of the NS law */
  inline unsigned int size() const { return _size; }

  /** display the data of the NonSmoothLaw on the standard output */
  virtual void display() const = 0;

  /// visitors hook
  VIRTUAL_ACCEPT_VISITORS(NonSmoothLaw);
};
#endif
