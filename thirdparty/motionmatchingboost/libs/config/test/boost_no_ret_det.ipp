//  (C) Copyright John Maddock 2001. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for the most recent version.

//  MACRO:         BOOST_NO_UNREACHABLE_RETURN_DETECTION
//  TITLE:         detection of unreachable returns
//  DESCRIPTION:   If a return is unreachable, then no return
//                 statement should be required, however some
//                 compilers insist on it, while other issue a
//                 bunch of warnings if it is in fact present.

#if defined( BOOST_NO_EXCEPTIONS )
# include <stdlib.h>
#endif

namespace motionmatchingboost_no_unreachable_return_detection{

int checker()
{
#if defined( BOOST_NO_EXCEPTIONS ) && (!defined( _MSC_VER ) || defined(__clang__))
   abort();
#else
   throw 0;
#endif
   // no return statement: we don't ever get here...
}

int check = 0;

int test()
{
   if(check)
      return checker();
   return 0;
}

}



