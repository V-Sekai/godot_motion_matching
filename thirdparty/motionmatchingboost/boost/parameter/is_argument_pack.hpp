// Copyright Cromwell D. Enage 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PARAMETER_IS_ARGUMENT_PACK_HPP
#define BOOST_PARAMETER_IS_ARGUMENT_PACK_HPP

#include <boost/parameter/aux_/is_tagged_argument.hpp>
#include <boost/parameter/aux_/arg_list.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace motionmatchingboost { namespace parameter {

    template <typename T>
    struct is_argument_pack
      : ::motionmatchingboost::mpl::if_<
            ::motionmatchingboost::is_base_of< ::motionmatchingboost::parameter::aux::empty_arg_list,T>
          , ::motionmatchingboost::mpl::true_
          , ::motionmatchingboost::parameter::aux::is_tagged_argument<T>
        >::type
    {
    };
}}

#endif  // include guard

