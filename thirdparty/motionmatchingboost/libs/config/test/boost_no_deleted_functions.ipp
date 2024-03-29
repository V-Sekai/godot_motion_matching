//  (C) Copyright Beman Dawes 2008

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX11_DELETED_FUNCTIONS 
//  TITLE:         C++0x =delete functions unavailable
//  DESCRIPTION:   The compiler does not support C++0x =delete functions

#if defined(__GNUC__) && !defined(__GXX_EXPERIMENTAL_CXX0X__) && !defined(BOOST_INTEL_STDCXX0X)
#  error Deleted functions are not supported in non-C++0x mode
#endif

namespace motionmatchingboost_no_cxx11_deleted_functions {

  struct foo {
    foo() = delete;
  };

int test()
{
  return 0;
}

}
