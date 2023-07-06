//  This file was automatically generated on Fri Jun 26 21:47:59 2015
//  by libs/config/tools/generate.cpp
//  Copyright John Maddock 2002-4.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for the most recent version.//
//  Revision $Id$
//


// Test file for macro BOOST_NO_CXX14_HDR_SHARED_MUTEX
// This file should not compile, if it does then
// BOOST_NO_CXX14_HDR_SHARED_MUTEX should not be defined.
// See file boost_no_cxx14_hdr_shared_mutex.ipp for details

// Must not have BOOST_ASSERT_CONFIG set; it defeats
// the objective of this file:
#ifdef BOOST_ASSERT_CONFIG
#  undef BOOST_ASSERT_CONFIG
#endif

#include <boost/config.hpp>
#include "test.hpp"

#ifdef BOOST_NO_CXX14_HDR_SHARED_MUTEX
#include "boost_no_cxx14_hdr_shared_mutex.ipp"
#else
#error "this file should not compile"
#endif

int main( int, char *[] )
{
   return motionmatchingboost_no_cxx14_hdr_shared_mutex::test();
}

