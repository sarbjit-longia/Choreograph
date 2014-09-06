/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#include "Motion.h"

namespace choreograph
{
  //! Remove all elements from \a container that match \a compare
  template<class CONTAINER_TYPE, class COMPARATOR>
  void erase_if( CONTAINER_TYPE *container, COMPARATOR compare )
  {
    container->erase( std::remove_if( container->begin(),
                                     container->end(),
                                     compare ),
                     container->end() );
  }

  //! Remove all copies of \a element from \a vec
  template<class ELEMENT_TYPE>
  void vector_remove( std::vector<ELEMENT_TYPE> *vec, const ELEMENT_TYPE &element )
  {
    vec->erase( std::remove_if( vec->begin()
                               , vec->end()
                               , [element](const ELEMENT_TYPE &e){ return e == element; } )
               , vec->end() );
  }

/*
 Holds a collection of Motions and updates them through time.
 */
class Timeline
{
public:

  //! Create a Sequence that is connected out to \a output.
  template<typename T>
  Motion<T>& move( T *output )
  {
    auto sequence = std::make_shared<Sequence<T>>();
    sequence->_initial_value = *output;
    return move( output, sequence );
  }

  //! Create a Motion that plays \a sequence into \a output.
  template<typename T>
  Motion<T>& move( T *output, std::shared_ptr<Sequence<T>> sequence )
  { // remove any existing motions that affect the same variable (because that doesn't make sense within a single timeline)
    remove( output );

    auto c = std::make_shared<Motion<T>>( output );
    c->_sequence = sequence;
    _motions.push_back( c );
    return *c;
  }

  //! Create a Motion that plays \a sequence into \a output. Copies the input sequence.
  template<typename T>
  Motion<T>& move( T *output, const Sequence<T> &sequence )
  { // remove any existing motions that affect the same variable (because that doesn't make sense within a single timeline)
    remove( output );

    auto c = std::make_shared<Motion<T>>( output );
    c->_sequence = std::make_shared<Sequence<T>>( sequence );
    _motions.push_back( c );
    return *c;
  }

  //! Add phrases to the end of the Sequence currently connected to \a output.
  template<typename T>
  Sequence<T>& queue( T *output )
  {
    for( auto &motion : _motions ) {
      if( motion->_target == output ) {
      //
        return std::static_pointer_cast<Motion<T>>( motion )->getSequence();
      }
    }
    return move( output ).getSequence();
  }

  //! Advance all current motions.
  void step( float dt );
  //! Remove specific motion.
  void remove( const std::shared_ptr<MotionBase> &motion );

  //! Remove motion associated with specific output.
  template<typename T>
  void remove( T *output )
  {
    erase_if( &_motions, [=] (std::shared_ptr<MotionBase> m) { return m->_target == output; } );
  }

private:
  bool                                      _auto_clear = true;
  std::vector<std::shared_ptr<MotionBase>>  _motions;
};

} // namespace choreograph
