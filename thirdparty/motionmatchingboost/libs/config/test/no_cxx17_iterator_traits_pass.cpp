//  This file was automatically generated on Sun Jul  9 15:26:23 2017
//  by libs/config/tools/generate.cpp
//  Copyright John Maddock 2002-4.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for the most recent version.//
//  Revision $Id$
//


// Test file for macro BOOST_NO_CXX17_ITERATOR_TRAITS
// This file should compile, if it does not then
// BOOST_NO_CXX17_ITERATOR_TRAITS should be defined.
// See file boost_no_cxx17_iterator_traits.ipp for details

// Must not have BOOST_ASSERT_CONFIG set; it defeats
// the objective of this file:
#ifdef BOOST_ASSERT_CONFIG
#  undef BOOST_ASSERT_CONFIG
#endif

#include <boost/config.hpp>
#include "test.hpp"

#ifndef BOOST_NO_CXX17_ITERATOR_TRAITS
#include "boost_no_cxx17_iterator_traits.ipp"
#else
namespace motionmatchingboost_no_cxx17_iterator_traits = empty_boost;
#endif

int main( int, char *[] )
{
   return motionmatchingboost_no_cxx17_iterator_traits::test();
}

