#ifndef BOOST_SMART_PTR_DETAIL_SP_ACCESS_HPP_INCLUDED
#define BOOST_SMART_PTR_DETAIL_SP_ACCESS_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  detail/sp_caccess.hpp
//
//  Copyright 2008 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>

namespace boost
{

namespace detail
{

// sp_element, element_type

template< class T > struct sp_element
{
    typedef T type;
};

#if !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< class T > struct sp_element< T[] >
{
    typedef T type;
};

#if !defined( BOOST_BORLANDC ) || !BOOST_WORKAROUND( BOOST_BORLANDC, < 0x600 )

template< class T, std::size_t N > struct sp_element< T[N] >
{
    typedef T type;
};

#endif

#endif // !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

// sp_dereference, return type of operator*

template< class T > struct sp_dereference
{
    typedef T & type;
};

template<> struct sp_dereference< void >
{
    typedef void type;
};

#if !defined(BOOST_NO_CV_VOID_SPECIALIZATIONS)

template<> struct sp_dereference< void const >
{
    typedef void type;
};

template<> struct sp_dereference< void volatile >
{
    typedef void type;
};

template<> struct sp_dereference< void const volatile >
{
    typedef void type;
};

#endif // !defined(BOOST_NO_CV_VOID_SPECIALIZATIONS)

#if !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< class T > struct sp_dereference< T[] >
{
    typedef void type;
};

#if !defined( BOOST_BORLANDC ) || !BOOST_WORKAROUND( BOOST_BORLANDC, < 0x600 )

template< class T, std::size_t N > struct sp_dereference< T[N] >
{
    typedef void type;
};

#endif

#endif // !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

// sp_member_access, return type of operator->

template< class T > struct sp_member_access
{
    typedef T * type;
};

#if !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< class T > struct sp_member_access< T[] >
{
    typedef void type;
};

#if !defined( BOOST_BORLANDC ) || !BOOST_WORKAROUND( BOOST_BORLANDC, < 0x600 )

template< class T, std::size_t N > struct sp_member_access< T[N] >
{
    typedef void type;
};

#endif

#endif // !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

// sp_array_access, return type of operator[]

template< class T > struct sp_array_access
{
    typedef void type;
};

#if !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< class T > struct sp_array_access< T[] >
{
    typedef T & type;
};

#if !defined( BOOST_BORLANDC ) || !BOOST_WORKAROUND( BOOST_BORLANDC, < 0x600 )

template< class T, std::size_t N > struct sp_array_access< T[N] >
{
    typedef T & type;
};

#endif

#endif // !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

// sp_extent, for operator[] index check

template< class T > struct sp_extent
{
    enum _vt { value = 0 };
};

#if !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< class T, std::size_t N > struct sp_extent< T[N] >
{
    enum _vt { value = N };
};

#endif // !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

// sp_assert_convertible

template< class Y, class T > inline void sp_assert_convertible() BOOST_SP_NOEXCEPT
{
#if !defined( BOOST_SP_NO_SP_CONVERTIBLE )

    // static_assert( sp_convertible< Y, T >::value );
    typedef char tmp[ sp_convertible< Y, T >::value? 1: -1 ];
    (void)sizeof( tmp );

#else

    T* p = static_cast< Y* >( 0 );
    (void)p;

#endif
}

struct sp_internal_constructor_tag
{
};

} // namespace detail

} // namespace boost

#endif  // #ifndef BOOST_SMART_PTR_DETAIL_SP_ACCESS_HPP_INCLUDED
