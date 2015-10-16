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

#include "SiconosConfig.h"

#include "BlockCSRMatrix.hpp"
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include "NonSmoothLaw.hpp"

#include "NewtonEulerDS.hpp"
#include "NewtonEulerR.hpp"
#include "SimulationGraphs.hpp"

#include "Tools.hpp"

// Default constructor: empty matrix
BlockCSRMatrix::BlockCSRMatrix():
  _nr(0), 
  _blockCSR(new CompressedRowMat()), 
  _sparseBlockStructuredMatrix(new SparseBlockStructuredMatrix()),
  _diagsize0(new IndexInt()),
  _diagsize1(new IndexInt()),
  rowPos(new IndexInt()),
  colPos(new IndexInt())
{}

// Constructor with dimensions
BlockCSRMatrix::BlockCSRMatrix(unsigned int nRow):
  _nr(nRow),
  // Only square-blocks matrices for the moment (ie nRow = nr = nrol)
  
  // Allocate memory and fill in the matrix rowPos, rowCol ... are
  // initialized with nr to reserve at first step the maximum possible
  // (according to given nr) space in memory.  Thus a future resize
  // will not require memory allocation or copy.
  _blockCSR(new CompressedRowMat(_nr, _nr)),
  _sparseBlockStructuredMatrix(new SparseBlockStructuredMatrix),
  _diagsize0(new IndexInt(_nr)),
  _diagsize1(new IndexInt(_nr)),
  rowPos(new IndexInt(_nr)),
  colPos(new IndexInt(_nr))
{}

// Basic constructor
BlockCSRMatrix::BlockCSRMatrix(SP::InteractionsGraph indexSet):
  _nr(indexSet->size()), 
  _blockCSR(new CompressedRowMat(_nr, _nr)),
  _sparseBlockStructuredMatrix(new SparseBlockStructuredMatrix()),
  _diagsize0(new IndexInt(_nr)),
  _diagsize1(new IndexInt(_nr)),
  rowPos(new IndexInt(_nr)),
  colPos(new IndexInt(_nr))
{
  fill(indexSet);
}

BlockCSRMatrix::~BlockCSRMatrix()
{}

// Fill the SparseMat
void BlockCSRMatrix::fill(SP::InteractionsGraph indexSet)
{
  // ======> Aim: find inter1 and inter2 both in indexSets[level] and which
  // have common DynamicalSystems.  Then get the corresponding matrix
  // from map blocks.

  assert(indexSet);

  // Number of blocks in a row = number of active constraints.
  _nr = indexSet->size();

  // (re)allocate memory for ublas matrix
  _blockCSR->resize(_nr, _nr, false);

  _diagsize0->resize(_nr);
  _diagsize1->resize(_nr);

  // === Loop through "active" Interactions (ie present in
  // indexSets[level]) ===


  int sizeV = 0;

  InteractionsGraph::VIterator vi, viend;
  for (std11::tie(vi, viend) = indexSet->vertices();
       vi != viend; ++vi)
  {
    SP::Interaction inter = indexSet->bundle(*vi);

    assert(inter->nonSmoothLaw()->size() > 0);

    sizeV  += inter->nonSmoothLaw()->size();
    (*_diagsize0)[indexSet->index(*vi)] = sizeV;
    (*_diagsize1)[indexSet->index(*vi)] = sizeV;
    assert((*_diagsize0)[indexSet->index(*vi)] > 0);
    assert((*_diagsize1)[indexSet->index(*vi)] > 0);

    (*_blockCSR)(indexSet->index(*vi), indexSet->index(*vi)) =
      indexSet->properties(*vi).block->getArray();
  }

  InteractionsGraph::EIterator ei, eiend;
  for (std11::tie(ei, eiend) = indexSet->edges();
       ei != eiend; ++ei)
  {
    InteractionsGraph::VDescriptor vd1 = indexSet->source(*ei);
    InteractionsGraph::VDescriptor vd2 = indexSet->target(*ei);
    SP::Interaction inter1 = indexSet->bundle(vd1);
    SP::Interaction inter2 = indexSet->bundle(vd2);

    assert(indexSet->index(vd1) < _nr);
    assert(indexSet->index(vd2) < _nr);

    assert(indexSet->is_vertex(inter2));

    assert(vd2 == indexSet->descriptor(inter2));
    assert(indexSet->index(vd2) == indexSet->index(indexSet->descriptor(inter2)));


    unsigned int pos = indexSet->index(vd1);
    unsigned int col = indexSet->index(vd2);

    assert(pos != col);

    (*_blockCSR)(std::min(pos, col), std::max(pos, col)) =
      indexSet->properties(*ei).upper_block->getArray();

    (*_blockCSR)(std::max(pos, col), std::min(pos, col)) =
      indexSet->properties(*ei).lower_block->getArray();
  }
}

void BlockCSRMatrix::fillM(SP::InteractionsGraph indexSet)
{
  assert(indexSet);

  /* on adjoint graph a dynamical system may be on several edges */
  std::map<SP::DynamicalSystem, bool> involvedDS;
  InteractionsGraph::EIterator ei, eiend;
  for(std11::tie(ei, eiend) = indexSet->edges();
      ei != eiend; ++ei)
  {
    if (Type::value(*indexSet->bundle(*ei)) != Type::NewtonEulerDS)
    {
      RuntimeException::selfThrow("BlockCSRMatrix::fillM only for Newton EulerDS");
    }

    _nr = 0;
    
    if (involvedDS.find(indexSet->bundle(*ei)) == involvedDS.end())
    {
      _nr++;
      involvedDS[indexSet->bundle(*ei)] = true;
      _blockCSR->resize(_nr, _nr, false);

      (*_blockCSR)(_nr-1, _nr-1) = std11::static_pointer_cast<NewtonEulerDS>
        (indexSet->bundle(*ei))->mass()->getArray();
    }
  }
  
  _diagsize0->resize(involvedDS.size());
  _diagsize1->resize(involvedDS.size());

  /* here we suppose NewtonEuler with 6 dofs */
  /* it cannot be another case at this point */
  unsigned int index, ac;
  for (index = 0, ac = 6; 
       index < involvedDS.size();
       ++index, ac+=6)
  {
    (*_diagsize0)[index] = ac;
    (*_diagsize1)[index] = ac;
  }
  
}

void BlockCSRMatrix::fillH(SP::InteractionsGraph indexSet)
{
  assert(indexSet);

  /* on adjoint graph a dynamical system may be on several edges */
  std::map<SP::DynamicalSystem, unsigned int> involvedDS;
  InteractionsGraph::EIterator ei, eiend;
  {
    unsigned int index;
    for(std11::tie(ei, eiend) = indexSet->edges(), index=0;
        ei != eiend; ++ei, ++index)
    {
      if (involvedDS.find(indexSet->bundle(*ei)) == involvedDS.end())
      {
        if (Type::value(*indexSet->bundle(*ei)) != Type::NewtonEulerDS)
        {
          RuntimeException::selfThrow("BlockCSRMatrix::fillH only for Newton EulerDS");
        }
        involvedDS[indexSet->bundle(*ei)] = index;
      }
    }
  }

  _nr = involvedDS.size();

  _blockCSR->resize(_nr, _nr, false);

  InteractionsGraph::VIterator vi, viend;
  for(std11::tie(vi, viend) = indexSet->vertices();
      vi != viend; ++vi)
  {

    SP::DynamicalSystem first = SP::DynamicalSystem();
    unsigned int pos=0, col=0;
    InteractionsGraph::EDescriptor ed1, ed2;
    InteractionsGraph::OEIterator oei, oeiend;
    for(std11::tie(oei, oeiend) = indexSet->out_edges(*vi);
        oei != oeiend; ++oei)
    {
      if (!first)
      {
        first = indexSet->bundle(*oei);
        col = involvedDS[first];
        pos = involvedDS[first];
      }
      else
      {
        if (indexSet->bundle(*oei) != first)
        {
          pos = involvedDS[indexSet->bundle(*oei)];
        }
      }
    }

    (*_blockCSR)(std::min(pos, col), std::max(pos, col)) = 
      std11::static_pointer_cast<NewtonEulerR>(indexSet->bundle(*vi)->relation())->jachqT()->getArray();
    
    (*_blockCSR)(std::max(pos, col), std::min(pos, col)) = 
      std11::static_pointer_cast<NewtonEulerR>(indexSet->bundle(*vi)->relation())->jachqT()->getArray();
    
  }
  
  _diagsize0->resize(involvedDS.size());
  _diagsize1->resize(involvedDS.size());
  
  /* only NewtonEulerFrom3DLocalFrameR */
  unsigned int index, ac0, ac1;
  for (index= 0, ac0 = 6, ac1 = 3; 
       index < involvedDS.size();
       ++index, ac0 +=6, ac1 +=3)
  {
    (*_diagsize0)[index] = ac0;
    (*_diagsize1)[index] = ac1;
  }
  
}


// Fill the SparseMat
void BlockCSRMatrix::fill(SP::DynamicalSystemsSet DSSet,
                          MapOfDSMatrices& DSblocks)
{
  RuntimeException::selfThrow
  (" BlockCSRMatrix::fill(DynamicalSystemsSet* DSSet, MapOfDSMatrices& DSblocks), Not Yet Implemented");
}
// Fill the SparseMat
void BlockCSRMatrix::fill(SP::InteractionsGraph indexSet,
                          SP::DynamicalSystemsSet DSSet,
                          MapOfInteractionMapOfDSMatrices& interactionDSBlocks)
{
  RuntimeException::selfThrow
  (" BlockCSRMatrix::fill(DynamicalSystemsSet* DSSet, MapOfDSMatrices& DSblocks), Not Yet Implemented");
}


// convert _blockCSR to numerics structure
void BlockCSRMatrix::convert()
{
  _sparseBlockStructuredMatrix->blocknumber0 = _nr;
  _sparseBlockStructuredMatrix->blocknumber1 = _nr;  // nc not always set
  _sparseBlockStructuredMatrix->nbblocks = (*_blockCSR).nnz();
  // Next copies: pointer links!!
  _sparseBlockStructuredMatrix->blocksize0 =  &((*_diagsize0)[0]);
  _sparseBlockStructuredMatrix->blocksize1 =  &((*_diagsize1)[0]);  // nr = nc

  // boost
  _sparseBlockStructuredMatrix->filled1 = (*_blockCSR).filled1();
  _sparseBlockStructuredMatrix->filled2 = (*_blockCSR).filled2();
  _sparseBlockStructuredMatrix->index1_data = &((*_blockCSR).index1_data()[0]);
  if (_nr > 0)
  {
    _sparseBlockStructuredMatrix->index2_data = &((*_blockCSR).index2_data()[0]);
    _sparseBlockStructuredMatrix->block =  &((*_blockCSR).value_data()[0]);
  };

  //   // Loop through the non-null blocks
  //   for (SpMatIt1 i1 = _blockCSR->begin1(); i1 != _blockCSR->end1(); ++i1)
  //     {
  //       for (SpMatIt2 i2 = i1.begin(); i2 != i1.end(); ++i2)
  //  {
  //    block[i] = *i2;
  //  }
  //     }
}

// Display data
void BlockCSRMatrix::display() const
{
  std::cout << "----- Sparse Block Matrix with "
            << _nr << " blocks in a row/col and "
            << _blockCSR->nnz()
            << " non-null blocks" <<std::endl;
  std::cout << "filled1:" << _blockCSR->filled1() <<std::endl;
  std::cout << "filled2:" << _blockCSR->filled2() <<std::endl;
  std::cout << "index1_data:\n";
  print(_blockCSR->index1_data().begin(), _blockCSR->index1_data().end());
  std::cout <<std::endl;
  std::cout << "index2_data:\n";
  print(_blockCSR->index2_data().begin(), _blockCSR->index2_data().end());
  std::cout <<std::endl;
  std::cout << "Sum of sizes of the diagonal blocks:"
            <<std::endl;
  print(_diagsize0->begin(), _diagsize0->end());
  print(_diagsize1->begin(), _diagsize1->end());
}

unsigned int BlockCSRMatrix::getNbNonNullBlocks() const
{
  return _blockCSR->nnz();
};
