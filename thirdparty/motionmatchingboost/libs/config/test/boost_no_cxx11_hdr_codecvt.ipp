//  (C) Copyright Beman Dawes 2009

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX11_HDR_CODECVT
//  TITLE:         C++0x header <codecvt> unavailable
//  DESCRIPTION:   The standard library does not supply C++0x header <codecvt>

#include <codecvt>
#include <locale> // for wstring_convert

namespace motionmatchingboost_no_cxx11_hdr_codecvt {

int test()
{
  using std::codecvt_utf8;
  using std::codecvt_utf16;
  using std::codecvt_utf8_utf16;
#ifndef BOOST_NO_CXX11_CHAR16_T
  std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> test;
#endif
  return 0;
}

}
