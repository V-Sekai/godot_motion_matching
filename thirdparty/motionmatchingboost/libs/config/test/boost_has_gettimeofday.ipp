//  (C) Copyright John Maddock 2001. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for most recent version.

//  MACRO:         BOOST_HAS_GETTIMEOFDAY
//  TITLE:         gettimeofday
//  DESCRIPTION:   The platform supports POSIX standard API gettimeofday.

#include <sys/time.h>


namespace motionmatchingboost_has_gettimeofday{

void f()
{
    // this is never called, it just has to compile:
    timeval tp;
    int res = gettimeofday(&tp, 0);
    (void) &res;
}

int test()
{
   return 0;
}

}





