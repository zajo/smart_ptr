#ifndef BOOST_SMART_PTR_UNSHARED_PTR_HPP_INCLUDED
#define BOOST_SMART_PTR_UNSHARED_PTR_HPP_INCLUDED

//
//  unshared_ptr.hpp
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001-2008 Peter Dimov
//  Copyright (c) 2020 Emil Dotchevski
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/ for documentation.
//

#include <boost/smart_ptr/detail/shared_count.hpp>
#include <boost/smart_ptr/detail/sp_convertible.hpp>
#include <boost/smart_ptr/detail/sp_access.hpp>
#include <boost/smart_ptr/detail/sp_nullptr_t.hpp>
#include <boost/smart_ptr/detail/sp_disable_deprecated.hpp>
#include <boost/smart_ptr/detail/sp_noexcept.hpp>
#include <boost/checked_delete.hpp>
#include <boost/throw_exception.hpp>
#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/config/workaround.hpp>

#if defined( BOOST_NO_CXX11_RVALUE_REFERENCES )
#error unshared_ptr requires rvalue references
#endif

#if !defined(BOOST_SP_NO_ATOMIC_ACCESS)
#include <boost/smart_ptr/detail/spinlock_pool.hpp>
#endif

#include <algorithm>            // for std::swap
#include <functional>           // for std::less
#include <typeinfo>             // for std::bad_cast
#include <cstddef>              // for std::size_t
#include <memory>               // for std::auto_ptr

#if !defined(BOOST_NO_IOSTREAM)
#if !defined(BOOST_NO_IOSFWD)
#include <iosfwd>               // for std::basic_ostream
#else
#include <ostream>
#endif
#endif

#if defined( BOOST_SP_DISABLE_DEPRECATED )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace boost
{

template<class T> class shared_ptr;
template<class T> class unshared_ptr;
template<class T> class weak_ptr;

namespace movelib
{

    template< class T, class D > class unique_ptr;

} // namespace movelib

namespace detail
{

// pointer constructor helper

template< class T, class Y > inline void sp_pointer_construct_unshared( boost::unshared_ptr< T > * ppx, Y * p, boost::detail::unshared_count & pn )
{
    boost::detail::unshared_count( p ).swap( pn );
}

#if !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< class T, class Y > inline void sp_pointer_construct_unshared( boost::unshared_ptr< T[] > * /*ppx*/, Y * p, boost::detail::unshared_count & pn )
{
    sp_assert_convertible< Y[], T[] >();
    boost::detail::unshared_count( p, boost::checked_array_deleter< T >() ).swap( pn );
}

template< class T, std::size_t N, class Y > inline void sp_pointer_construct_unshared( boost::unshared_ptr< T[N] > * /*ppx*/, Y * p, boost::detail::unshared_count & pn )
{
    sp_assert_convertible< Y[N], T[N] >();
    boost::detail::unshared_count( p, boost::checked_array_deleter< T >() ).swap( pn );
}

#endif // !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

// deleter constructor helper

template< class T, class Y > inline void sp_deleter_construct_unshared( boost::unshared_ptr< T > * /*ppx*/, Y * /*p*/ )
{
}

#if !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< class T, class Y > inline void sp_deleter_construct_unshared( boost::unshared_ptr< T[] > * /*ppx*/, Y * /*p*/ )
{
    sp_assert_convertible< Y[], T[] >();
}

template< class T, std::size_t N, class Y > inline void sp_deleter_construct_unshared( boost::unshared_ptr< T[N] > * /*ppx*/, Y * /*p*/ )
{
    sp_assert_convertible< Y[N], T[N] >();
}

#endif // !defined( BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

struct sp_unshared_internal_constructor_tag
{
};

} // namespace detail


//
//  unshared_ptr
//
//  A unique_ptr-like relative of shared_ptr with reference counted ownership
//  semantics. The object pointed to is deleted by the destructor (or on reset)
//  except if there are shared_ptr object(s) keeping it alive.
//

template<class T> class unshared_ptr
{
private:

    // Borland 5.5.1 specific workaround
    typedef unshared_ptr<T> this_type;

    unshared_ptr( unshared_ptr const & );
    unshared_ptr & operator= ( unshared_ptr const & );

public:

    typedef typename boost::detail::sp_element< T >::type element_type;

    BOOST_CONSTEXPR unshared_ptr() BOOST_SP_NOEXCEPT : px( 0 ), pn()
    {
    }

#if !defined( BOOST_NO_CXX11_NULLPTR )

    BOOST_CONSTEXPR unshared_ptr( boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT : px( 0 ), pn()
    {
    }

#endif

    template<class Y>
    explicit unshared_ptr( Y * p ): px( p ), pn() // Y must be complete
    {
        boost::detail::sp_pointer_construct_unshared( this, p, pn );
    }

    //
    // Requirements: D's copy constructor must not throw
    //
    // unshared_ptr will release p by calling d(p)
    //

    template<class Y, class D> unshared_ptr( Y * p, D d ): px( p ), pn( p, d )
    {
        boost::detail::sp_deleter_construct_unshared( this, p );
    }

#if !defined( BOOST_NO_CXX11_NULLPTR )

    template<class D> unshared_ptr( boost::detail::sp_nullptr_t p, D d ): px( p ), pn( p, d )
    {
    }

#endif

    // As above, but with allocator. A's copy constructor shall not throw.

    template<class Y, class D, class A> unshared_ptr( Y * p, D d, A a ): px( p ), pn( p, d, a )
    {
        boost::detail::sp_deleter_construct_unshared( this, p );
    }

#if !defined( BOOST_NO_CXX11_NULLPTR )

    template<class D, class A> unshared_ptr( boost::detail::sp_nullptr_t p, D d, A a ): px( p ), pn( p, d, a )
    {
    }

#endif

    template <class Y>
    explicit unshared_ptr( shared_ptr<Y> const & r ) : pn( boost::detail::unshared_count( r.pn ) ) // may throw
    {
        boost::detail::sp_assert_convertible< Y, T >();

        // it is now safe to copy r.px, as pn(r.pn) did not throw
        px = r.px;
    }

    template <class Y>
    unshared_ptr( shared_ptr<Y> const & r, boost::detail::sp_nothrow_tag )
    BOOST_SP_NOEXCEPT : px( 0 ), pn( r.pn, boost::detail::sp_nothrow_tag() )
    {
        boost::detail::sp_assert_convertible< Y, T >();

        if( !pn.empty() )
        {
          px = r.px;
        }
    }

    template <class Y>
    explicit unshared_ptr( weak_ptr<Y> const & r ) : pn( boost::detail::unshared_count( r.pn ) ) // may throw
    {
        boost::detail::sp_assert_convertible< Y, T >();

        // it is now safe to copy r.px, as pn(r.pn) did not throw
        px = r.px;
    }

    template <class Y>
    unshared_ptr( weak_ptr<Y> const & r, boost::detail::sp_nothrow_tag )
    BOOST_SP_NOEXCEPT : px( 0 ), pn( r.pn, boost::detail::sp_nothrow_tag() )
    {
        boost::detail::sp_assert_convertible< Y, T >();

        if( !pn.empty() )
        {
          px = r.px;
        }
    }

#ifndef BOOST_NO_AUTO_PTR

    template<class Y>
    explicit unshared_ptr( std::auto_ptr<Y> & r ): px(r.get()), pn()
    {
        boost::detail::sp_assert_convertible< Y, T >();

        Y * tmp = r.get();
        pn = boost::detail::unshared_count( r );

        boost::detail::sp_deleter_construct_unshared( this, tmp );
    }

    template<class Y>
    unshared_ptr( std::auto_ptr<Y> && r ): px(r.get()), pn()
    {
        boost::detail::sp_assert_convertible< Y, T >();

        Y * tmp = r.get();
        pn = boost::detail::unshared_count( r );

        boost::detail::sp_deleter_construct_unshared( this, tmp );
    }

#endif // BOOST_NO_AUTO_PTR

#if !defined( BOOST_NO_CXX11_SMART_PTR )

    template< class Y, class D >
    unshared_ptr( std::unique_ptr< Y, D > && r ): px( r.get() ), pn()
    {
        boost::detail::sp_assert_convertible< Y, T >();

        typename std::unique_ptr< Y, D >::pointer tmp = r.get();

        if( tmp != 0 )
        {
            pn = boost::detail::unshared_count( r );
            boost::detail::sp_deleter_construct_unshared( this, tmp );
        }
    }

#endif

    template< class Y, class D >
    unshared_ptr( boost::movelib::unique_ptr< Y, D > r ): px( r.get() ), pn()
    {
        boost::detail::sp_assert_convertible< Y, T >();

        typename boost::movelib::unique_ptr< Y, D >::pointer tmp = r.get();

        if( tmp != 0 )
        {
            pn = boost::detail::unshared_count( r );
            boost::detail::sp_deleter_construct_unshared( this, tmp );
        }
    }

    // assignment

#ifndef BOOST_NO_AUTO_PTR

    template<class Y>
    unshared_ptr & operator=( std::auto_ptr<Y> & r )
    {
        this_type( r ).swap( *this );
        return *this;
    }

    template<class Y>
    unshared_ptr & operator=( std::auto_ptr<Y> && r )
    {
        this_type( static_cast< std::auto_ptr<Y> && >( r ) ).swap( *this );
        return *this;
    }

#endif // BOOST_NO_AUTO_PTR

#if !defined( BOOST_NO_CXX11_SMART_PTR )

    template<class Y, class D>
    unshared_ptr & operator=( std::unique_ptr<Y, D> && r )
    {
        this_type( static_cast< std::unique_ptr<Y, D> && >( r ) ).swap(*this);
        return *this;
    }

#endif

    template<class Y, class D>
    unshared_ptr & operator=( boost::movelib::unique_ptr<Y, D> r )
    {
        // this_type( static_cast< unique_ptr<Y, D> && >( r ) ).swap( *this );

        boost::detail::sp_assert_convertible< Y, T >();

        typename boost::movelib::unique_ptr< Y, D >::pointer p = r.get();

        unshared_ptr tmp;

        if( p != 0 )
        {
            tmp.px = p;
            tmp.pn = boost::detail::unshared_count( r );

            boost::detail::sp_deleter_construct_unshared( &tmp, p );
        }

        tmp.swap( *this );

        return *this;
    }

// Move support

    unshared_ptr( unshared_ptr && r ) BOOST_SP_NOEXCEPT : px( r.px ), pn()
    {
        pn.swap( r.pn );
        r.px = 0;
    }

    template<class Y>
#if !defined( BOOST_SP_NO_SP_CONVERTIBLE )

    unshared_ptr( unshared_ptr<Y> && r, typename boost::detail::sp_enable_if_convertible<Y,T>::type = boost::detail::sp_empty() )

#else

    unshared_ptr( unshared_ptr<Y> && r )

#endif
    BOOST_SP_NOEXCEPT : px( r.px ), pn()
    {
        boost::detail::sp_assert_convertible< Y, T >();

        pn.swap( r.pn );
        r.px = 0;
    }

    unshared_ptr & operator=( unshared_ptr && r ) BOOST_SP_NOEXCEPT
    {
        this_type( static_cast< unshared_ptr && >( r ) ).swap( *this );
        return *this;
    }

    template<class Y>
    unshared_ptr & operator=( unshared_ptr<Y> && r ) BOOST_SP_NOEXCEPT
    {
        this_type( static_cast< unshared_ptr<Y> && >( r ) ).swap( *this );
        return *this;
    }

    // aliasing move
    template<class Y>
    unshared_ptr( unshared_ptr<Y> && r, element_type * p ) BOOST_SP_NOEXCEPT : px( p ), pn()
    {
        pn.swap( r.pn );
        r.px = 0;
    }

#if !defined( BOOST_NO_CXX11_NULLPTR )

    unshared_ptr & operator=( boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT
    {
        this_type().swap(*this);
        return *this;
    }

#endif

    void reset() BOOST_SP_NOEXCEPT
    {
        this_type().swap(*this);
    }

    template<class Y> void reset( Y * p ) // Y must be complete
    {
        BOOST_ASSERT( p == 0 || p != px ); // catch self-reset errors
        this_type( p ).swap( *this );
    }

    template<class Y, class D> void reset( Y * p, D d )
    {
        this_type( p, d ).swap( *this );
    }

    template<class Y, class D, class A> void reset( Y * p, D d, A a )
    {
        this_type( p, d, a ).swap( *this );
    }

    template<class Y> void reset( unshared_ptr<Y> const & r, element_type * p ) BOOST_SP_NOEXCEPT
    {
        this_type( r, p ).swap( *this );
    }

    template<class Y> void reset( unshared_ptr<Y> && r, element_type * p ) BOOST_SP_NOEXCEPT
    {
        this_type( static_cast< unshared_ptr<Y> && >( r ), p ).swap( *this );
    }

    typename boost::detail::sp_dereference< T >::type operator* () const BOOST_SP_NOEXCEPT_WITH_ASSERT
    {
        BOOST_ASSERT( px != 0 );
        return *px;
    }

    typename boost::detail::sp_member_access< T >::type operator-> () const BOOST_SP_NOEXCEPT_WITH_ASSERT
    {
        BOOST_ASSERT( px != 0 );
        return px;
    }

    typename boost::detail::sp_array_access< T >::type operator[] ( std::ptrdiff_t i ) const BOOST_SP_NOEXCEPT_WITH_ASSERT
    {
        BOOST_ASSERT( px != 0 );
        BOOST_ASSERT( i >= 0 && ( i < boost::detail::sp_extent< T >::value || boost::detail::sp_extent< T >::value == 0 ) );

        return static_cast< typename boost::detail::sp_array_access< T >::type >( px[ i ] );
    }

    element_type * get() const BOOST_SP_NOEXCEPT
    {
        return px;
    }

// implicit conversion to "bool"
#include <boost/smart_ptr/detail/operator_bool.hpp>

    bool unique() const BOOST_SP_NOEXCEPT
    {
        return pn.unique();
    }

    long use_count() const BOOST_SP_NOEXCEPT
    {
        return pn.use_count();
    }

    void swap( unshared_ptr & other ) BOOST_SP_NOEXCEPT
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    template<class Y> bool owner_before( shared_ptr<Y> const & rhs ) const BOOST_SP_NOEXCEPT
    {
        return pn < rhs.pn;
    }

    template<class Y> bool owner_before( unshared_ptr<Y> const & rhs ) const BOOST_SP_NOEXCEPT
    {
        return pn < rhs.pn;
    }

    template<class Y> bool owner_before( weak_ptr<Y> const & rhs ) const BOOST_SP_NOEXCEPT
    {
        return pn < rhs.pn;
    }

    template<class Y> bool owner_equals( shared_ptr<Y> const & rhs ) const BOOST_SP_NOEXCEPT
    {
        return pn == rhs.pn;
    }

    template<class Y> bool owner_equals( unshared_ptr<Y> const & rhs ) const BOOST_SP_NOEXCEPT
    {
        return pn == rhs.pn;
    }

    template<class Y> bool owner_equals( weak_ptr<Y> const & rhs ) const BOOST_SP_NOEXCEPT
    {
        return pn == rhs.pn;
    }

    std::size_t owner_hash_value() const BOOST_SP_NOEXCEPT
    {
        return pn.hash_value();
    }

    void * _internal_get_deleter( boost::detail::sp_typeinfo_ const & ti ) const BOOST_SP_NOEXCEPT
    {
        return pn.get_deleter( ti );
    }

    void * _internal_get_local_deleter( boost::detail::sp_typeinfo_ const & ti ) const BOOST_SP_NOEXCEPT
    {
        return pn.get_local_deleter( ti );
    }

    void * _internal_get_untyped_deleter() const BOOST_SP_NOEXCEPT
    {
        return pn.get_untyped_deleter();
    }

    bool _internal_equiv( unshared_ptr const & r ) const BOOST_SP_NOEXCEPT
    {
        return px == r.px && pn == r.pn;
    }

    boost::detail::unshared_count _internal_count() const BOOST_SP_NOEXCEPT
    {
        return pn;
    }

// Tasteless as this may seem, making all members public allows member templates
// to work in the absence of member template friends. (Matthew Langston)

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

private:

    template<class Y> friend class shared_ptr;
    template<class Y> friend class unshared_ptr;
    template<class Y> friend class weak_ptr;


#endif

    element_type * px;                 // contained pointer
    boost::detail::unshared_count pn;    // reference counter

}; // unshared_ptr

template <class T>
shared_ptr<typename unshared_ptr<T>::element_type> share( unshared_ptr<T> && up ) BOOST_SP_NOEXCEPT
{
    return shared_ptr<typename unshared_ptr<T>::element_type>( up, boost::detail::sp_nothrow_tag() );
}

template <class T>
unshared_ptr<typename shared_ptr<T>::element_type> unshare( shared_ptr<T> const & sp ) BOOST_SP_NOEXCEPT
{
    return unshared_ptr<typename shared_ptr<T>::element_type>( sp, boost::detail::sp_nothrow_tag() );
}

template <class T>
unshared_ptr<typename weak_ptr<T>::element_type> unshare( weak_ptr<T> const & wp ) BOOST_SP_NOEXCEPT
{
    return unshared_ptr<typename weak_ptr<T>::element_type>( wp, boost::detail::sp_nothrow_tag() );
}

template<class T, class U> inline bool operator==(unshared_ptr<T> const & a, unshared_ptr<U> const & b) BOOST_SP_NOEXCEPT
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator==(unshared_ptr<T> const & a, shared_ptr<U> const & b) BOOST_SP_NOEXCEPT
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator==(shared_ptr<T> const & a, unshared_ptr<U> const & b) BOOST_SP_NOEXCEPT
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(unshared_ptr<T> const & a, unshared_ptr<U> const & b) BOOST_SP_NOEXCEPT
{
    return a.get() != b.get();
}

template<class T, class U> inline bool operator!=(unshared_ptr<T> const & a, shared_ptr<U> const & b) BOOST_SP_NOEXCEPT
{
    return a.get() != b.get();
}

template<class T, class U> inline bool operator!=(shared_ptr<T> const & a, unshared_ptr<U> const & b) BOOST_SP_NOEXCEPT
{
    return a.get() != b.get();
}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(unshared_ptr<T> const & a, unshared_ptr<T> const & b) BOOST_SP_NOEXCEPT
{
    return a.get() != b.get();
}

#endif

#if !defined( BOOST_NO_CXX11_NULLPTR )

template<class T> inline bool operator==( unshared_ptr<T> const & p, boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator==( boost::detail::sp_nullptr_t, unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator!=( unshared_ptr<T> const & p, boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT
{
    return p.get() != 0;
}

template<class T> inline bool operator!=( boost::detail::sp_nullptr_t, unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return p.get() != 0;
}

#endif

template<class T, class U> inline bool operator<(unshared_ptr<T> const & a, unshared_ptr<U> const & b) BOOST_SP_NOEXCEPT
{
    return a.owner_before( b );
}

template<class T> inline void swap(unshared_ptr<T> & a, unshared_ptr<T> & b) BOOST_SP_NOEXCEPT
{
    a.swap(b);
}

template<class T, class U> unshared_ptr<T> static_pointer_cast( unshared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) static_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename unshared_ptr<T>::element_type E;

    E * p = static_cast< E* >( r.get() );
    return unshared_ptr<T>( std::move(r), p );
}

template<class T, class U> unshared_ptr<T> const_pointer_cast( unshared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) const_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename unshared_ptr<T>::element_type E;

    E * p = const_cast< E* >( r.get() );
    return unshared_ptr<T>( std::move(r), p );
}

template<class T, class U> unshared_ptr<T> dynamic_pointer_cast( unshared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) dynamic_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename unshared_ptr<T>::element_type E;

    E * p = dynamic_cast< E* >( r.get() );
    return p? unshared_ptr<T>( std::move(r), p ): unshared_ptr<T>();
}

template<class T, class U> unshared_ptr<T> reinterpret_pointer_cast( unshared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) reinterpret_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename unshared_ptr<T>::element_type E;

    E * p = reinterpret_cast< E* >( r.get() );
    return unshared_ptr<T>( std::move(r), p );
}

// get_pointer() enables boost::mem_fn to recognize unshared_ptr

template<class T> inline typename unshared_ptr<T>::element_type * get_pointer(unshared_ptr<T> const & p) BOOST_SP_NOEXCEPT
{
    return p.get();
}

// operator<<

#if !defined(BOOST_NO_IOSTREAM)

#if defined(BOOST_NO_TEMPLATED_IOSTREAMS) || ( defined(__GNUC__) &&  (__GNUC__ < 3) )

template<class Y> std::ostream & operator<< (std::ostream & os, unshared_ptr<Y> const & p)
{
    os << p.get();
    return os;
}

#else

// in STLport's no-iostreams mode no iostream symbols can be used
#ifndef _STLP_NO_IOSTREAMS

# if defined(BOOST_MSVC) && BOOST_WORKAROUND(BOOST_MSVC, < 1300 && __SGI_STL_PORT)
// MSVC6 has problems finding std::basic_ostream through the using declaration in namespace _STL
using std::basic_ostream;
template<class E, class T, class Y> basic_ostream<E, T> & operator<< (basic_ostream<E, T> & os, unshared_ptr<Y> const & p)
# else
template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, unshared_ptr<Y> const & p)
# endif
{
    os << p.get();
    return os;
}

#endif // _STLP_NO_IOSTREAMS

#endif // __GNUC__ < 3

#endif // !defined(BOOST_NO_IOSTREAM)

// get_deleter

namespace detail
{

template<class D, class T> D * basic_get_deleter( unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return static_cast<D *>( p._internal_get_deleter(BOOST_SP_TYPEID_(D)) );
}

template<class D, class T> D * basic_get_local_deleter( D *, unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT;
template<class D, class T> D const * basic_get_local_deleter( D const *, unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT;

} // namespace detail

template<class D, class T> D * get_deleter( unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    D * d = boost::detail::basic_get_deleter<D>( p );

    if( d == 0 )
    {
        d = boost::detail::basic_get_local_deleter( d, p );
    }

    return d;
}

// atomic access

#if !defined(BOOST_SP_NO_ATOMIC_ACCESS)

template<class T> inline bool atomic_is_lock_free( unshared_ptr<T> const * /*p*/ ) BOOST_SP_NOEXCEPT
{
    return false;
}

template<class T> unshared_ptr<T> atomic_load( unshared_ptr<T> const * p ) BOOST_SP_NOEXCEPT
{
    boost::detail::spinlock_pool<2>::scoped_lock lock( p );
    return *p;
}

template<class T, class M> inline unshared_ptr<T> atomic_load_explicit( unshared_ptr<T> const * p, /*memory_order mo*/ M ) BOOST_SP_NOEXCEPT
{
    return atomic_load( p );
}

template<class T> void atomic_store( unshared_ptr<T> * p, unshared_ptr<T> r ) BOOST_SP_NOEXCEPT
{
    boost::detail::spinlock_pool<2>::scoped_lock lock( p );
    p->swap( r );
}

template<class T, class M> inline void atomic_store_explicit( unshared_ptr<T> * p, unshared_ptr<T> r, /*memory_order mo*/ M ) BOOST_SP_NOEXCEPT
{
    atomic_store( p, r ); // std::move( r )
}

template<class T> unshared_ptr<T> atomic_exchange( unshared_ptr<T> * p, unshared_ptr<T> r ) BOOST_SP_NOEXCEPT
{
    boost::detail::spinlock & sp = boost::detail::spinlock_pool<2>::spinlock_for( p );

    sp.lock();
    p->swap( r );
    sp.unlock();

    return r; // return std::move( r )
}

template<class T, class M> unshared_ptr<T> inline atomic_exchange_explicit( unshared_ptr<T> * p, unshared_ptr<T> r, /*memory_order mo*/ M ) BOOST_SP_NOEXCEPT
{
    return atomic_exchange( p, r ); // std::move( r )
}

template<class T> bool atomic_compare_exchange( unshared_ptr<T> * p, unshared_ptr<T> * v, unshared_ptr<T> w ) BOOST_SP_NOEXCEPT
{
    boost::detail::spinlock & sp = boost::detail::spinlock_pool<2>::spinlock_for( p );

    sp.lock();

    if( p->_internal_equiv( *v ) )
    {
        p->swap( w );

        sp.unlock();

        return true;
    }
    else
    {
        unshared_ptr<T> tmp( *p );

        sp.unlock();

        tmp.swap( *v );
        return false;
    }
}

template<class T, class M> inline bool atomic_compare_exchange_explicit( unshared_ptr<T> * p, unshared_ptr<T> * v, unshared_ptr<T> w, /*memory_order success*/ M, /*memory_order failure*/ M ) BOOST_SP_NOEXCEPT
{
    return atomic_compare_exchange( p, v, w ); // std::move( w )
}

#endif // !defined(BOOST_SP_NO_ATOMIC_ACCESS)

// hash_value

template< class T > struct hash;

template< class T > std::size_t hash_value( boost::unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return boost::hash< typename boost::unshared_ptr<T>::element_type* >()( p.get() );
}

} // namespace boost

// std::hash

#if !defined(BOOST_NO_CXX11_HDR_FUNCTIONAL)

namespace std
{

template<class T> struct hash< ::boost::unshared_ptr<T> >
{
    std::size_t operator()( ::boost::unshared_ptr<T> const & p ) const BOOST_SP_NOEXCEPT
    {
        return std::hash< typename ::boost::unshared_ptr<T>::element_type* >()( p.get() );
    }
};

} // namespace std

#endif // #if !defined(BOOST_NO_CXX11_HDR_FUNCTIONAL)

#include <boost/smart_ptr/detail/local_sp_deleter.hpp>

namespace boost
{

namespace detail
{

template<class D, class T> D * basic_get_local_deleter( D *, unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return static_cast<D *>( p._internal_get_local_deleter( BOOST_SP_TYPEID_(local_sp_deleter<D>) ) );
}

template<class D, class T> D const * basic_get_local_deleter( D const *, unshared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return static_cast<D *>( p._internal_get_local_deleter( BOOST_SP_TYPEID_(local_sp_deleter<D>) ) );
}

} // namespace detail

#if defined(__cpp_deduction_guides)

template<class T> unshared_ptr( weak_ptr<T> ) -> unshared_ptr<T>;
template<class T, class D> unshared_ptr( std::unique_ptr<T, D> ) -> unshared_ptr<T>;

#endif

} // namespace boost

#if defined( BOOST_SP_DISABLE_DEPRECATED )
#pragma GCC diagnostic pop
#endif

#endif  // #ifndef BOOST_SMART_PTR_UNSHARED_PTR_HPP_INCLUDED
