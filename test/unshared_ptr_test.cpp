#include <boost/config.hpp>

#if defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

int main()
{
    return 0;
}

#else

#if defined(BOOST_MSVC)

#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#pragma warning(disable: 4355)  // 'this' : used in base member initializer list

#if (BOOST_MSVC >= 1310)
#pragma warning(disable: 4675)  // resolved overload found with Koenig lookup
#endif

#endif

#if defined(__GNUC__) && __GNUC__ > 4
# pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif

//
//  unshared_ptr_test.cpp
//
//  Copyright (c) 2002, 2003 Peter Dimov
//  Copyright (c) 2020 Emil Dotchevski
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/core/lightweight_test.hpp>

#include <boost/unshared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <map>
#include <vector>

//

namespace n_element_type
{

void f(int &)
{
}

void test()
{
    typedef boost::unshared_ptr<int>::element_type T;
    T t;
    f(t);
}

} // namespace n_element_type

namespace n_constructors
{

class incomplete;

void default_constructor()
{
    {
        boost::unshared_ptr<int> pi;
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 0);
    }

    {
        boost::unshared_ptr<void> pv;
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 0);
    }

    {
        boost::unshared_ptr<incomplete> px;
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 0);
    }
}

struct A
{
    int dummy;
};

struct X
{
    static long instances;

    X()
    {
        ++instances;
    }

    ~X()
    {
        --instances;
    }

private:

    X(X const &);
    X & operator= (X const &);
};

long X::instances = 0;

// virtual inheritance stresses the implementation

struct Y: public A, public virtual X
{
    static long instances;

    Y()
    {
        ++instances;
    }

    ~Y()
    {
        --instances;
    }

private:

    Y(Y const &);
    Y & operator= (Y const &);
};

long Y::instances = 0;

template<class T> void pc0_test(T * p)
{
    BOOST_TEST(p == 0);
    boost::unshared_ptr<T> pt(p);
    BOOST_TEST(pt? false: true);
    BOOST_TEST(!pt);
    BOOST_TEST(pt.get() == 0);
    BOOST_TEST(pt.use_count() == 1);
    BOOST_TEST(pt.unique());
}

void pointer_constructor()
{
    pc0_test(static_cast<int*>(0));

#if !defined(BOOST_MSVC) || (BOOST_MSVC > 1300)

    pc0_test(static_cast<int const*>(0));
    pc0_test(static_cast<int volatile*>(0));
    pc0_test(static_cast<int const volatile*>(0));

#endif

    {
        boost::unshared_ptr<int const> pi(static_cast<int*>(0));
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
    }

    {
        boost::unshared_ptr<int volatile> pi(static_cast<int*>(0));
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
    }

    {
        boost::unshared_ptr<void> pv(static_cast<int*>(0));
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    {
        boost::unshared_ptr<void const> pv(static_cast<int*>(0));
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    pc0_test(static_cast<X*>(0));
    pc0_test(static_cast<X const*>(0));
    pc0_test(static_cast<X volatile*>(0));
    pc0_test(static_cast<X const volatile*>(0));

    {
        boost::unshared_ptr<X const> px(static_cast<X*>(0));
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
    }

    {
        boost::unshared_ptr<X> px(static_cast<Y*>(0));
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
    }

    {
        boost::unshared_ptr<X const> px(static_cast<Y*>(0));
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
    }

    {
        boost::unshared_ptr<void> pv(static_cast<X*>(0));
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    {
        boost::unshared_ptr<void const> pv(static_cast<X*>(0));
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    {
        int * p = new int(7);
        boost::unshared_ptr<int> pi(p);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == p);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(*pi == 7);
    }

    {
        int * p = new int(7);
        boost::unshared_ptr<int const> pi(p);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == p);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(*pi == 7);
    }

    {
        int * p = new int(7);
        boost::unshared_ptr<void> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == p);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    {
        int * p = new int(7);
        boost::unshared_ptr<void const> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == p);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    BOOST_TEST(X::instances == 0);

    {
        X * p = new X;
        boost::unshared_ptr<X> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == p);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);
    }

    BOOST_TEST(X::instances == 0);

    {
        X * p = new X;
        boost::unshared_ptr<X const> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == p);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);
    }

    BOOST_TEST(X::instances == 0);

    {
        X * p = new X;
        boost::unshared_ptr<void> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == p);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 1);
    }

    BOOST_TEST(X::instances == 0);

    {
        X * p = new X;
        boost::unshared_ptr<void const> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == p);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 1);
    }

    BOOST_TEST(X::instances == 0);
    BOOST_TEST(Y::instances == 0);

    {
        Y * p = new Y;
        boost::unshared_ptr<X> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == p);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);
    }

    BOOST_TEST(X::instances == 0);
    BOOST_TEST(Y::instances == 0);

    {
        Y * p = new Y;
        boost::unshared_ptr<X const> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == p);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);
    }

    BOOST_TEST(X::instances == 0);
    BOOST_TEST(Y::instances == 0);
}

int m = 0;

void deleter(int * p)
{
    BOOST_TEST(p == 0);
}

void deleter2(int * p)
{
    BOOST_TEST(p == &m);
    ++*p;
}

struct deleter3
{
    void operator()(incomplete * p)
    {
        BOOST_TEST(p == 0);
    }
};

// Borland C++ 5.5.1 fails on static_cast<incomplete*>(0)

incomplete * p0 = 0;

void deleter_constructor()
{
    {
        boost::unshared_ptr<int> pi(static_cast<int*>(0), deleter);
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
    }

    {
        boost::unshared_ptr<void> pv(static_cast<int*>(0), &deleter);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    {
        boost::unshared_ptr<void const> pv(static_cast<int*>(0), deleter);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    {
        boost::unshared_ptr<incomplete> px(p0, deleter3());
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
    }

    {
        boost::unshared_ptr<void> pv(p0, deleter3());
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    {
        boost::unshared_ptr<void const> pv(p0, deleter3());
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    BOOST_TEST(m == 0);

    {
        boost::unshared_ptr<int> pi(&m, deleter2);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == &m);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
    }

    BOOST_TEST(m == 1);

    {
        boost::unshared_ptr<int const> pi(&m, &deleter2);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == &m);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
    }

    BOOST_TEST(m == 2);

    {
        boost::unshared_ptr<void> pv(&m, deleter2);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == &m);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    BOOST_TEST(m == 3);

    {
        boost::unshared_ptr<void const> pv(&m, &deleter2);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == &m);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
    }

    BOOST_TEST(m == 4);
}

void weak_ptr_constructor()
{
    {
        boost::weak_ptr<Y> wp;
        BOOST_TEST(wp.use_count() == 0);

        try
        {
            boost::unshared_ptr<Y> p2(wp);
            BOOST_ERROR("unshared_ptr<Y> p2(wp) failed to throw");
        }
        catch(boost::bad_weak_ptr const&)
        {
        }
    }

    {
        boost::shared_ptr<Y> p;
        boost::weak_ptr<Y> wp(p);

        if(wp.use_count() != 0) // 0 allowed but not required
        {
            boost::unshared_ptr<Y> p2(wp);
            BOOST_TEST(p2.use_count() == wp.use_count());
            BOOST_TEST(p2.get() == 0);
        }
    }

    {
        boost::shared_ptr<Y> p(new Y);
        boost::weak_ptr<Y> wp(p);

        {
            boost::unshared_ptr<Y> p2(wp);
            BOOST_TEST(p2? true: false);
            BOOST_TEST(!!p2);
            BOOST_TEST(p2.get() == p.get());
            BOOST_TEST(p2.use_count() == 2);
            BOOST_TEST(!p2.unique());

            try
            {
                boost::unshared_ptr<X> p3(wp);
                BOOST_ERROR("boost::unshared_ptr<X> p3(wp) failed to throw");
            }
            catch(boost::unshared_ptr_already_acquired const &)
            {
            }

            try
            {
                boost::unshared_ptr<X> p3(wp);
                BOOST_ERROR("boost::unshared_ptr<X> p3(wp) failed to throw");
            }
            catch(boost::unshared_ptr_already_acquired const &)
            {
            }
        }

        p.reset();
        BOOST_TEST(wp.use_count() == 0);

        try
        {
            boost::unshared_ptr<Y> p2(wp);
            BOOST_ERROR("unshared_ptr<Y> p2(wp) failed to throw");
        }
        catch(boost::bad_weak_ptr const&)
        {
        }

        try
        {
            boost::unshared_ptr<X> p3(wp);
            BOOST_ERROR("unshared_ptr<X> p3(wp) failed to throw");
        }
        catch(boost::bad_weak_ptr const&)
        {
        }
    }
}

#if defined(BOOST_DINKUMWARE_STDLIB) && (BOOST_DINKUMWARE_STDLIB < 306)
#  define BOOST_OLD_AUTO_PTR
#endif

void auto_ptr_constructor()
{
#if !defined( BOOST_NO_AUTO_PTR )

    {
        std::auto_ptr<int> p;
        boost::unshared_ptr<int> pi(p);
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<int> p;
        boost::unshared_ptr<int const> pi(p);
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<int> p;
        boost::unshared_ptr<void> pv(p);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<int> p;
        boost::unshared_ptr<void const> pv(p);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<X> p;
        boost::unshared_ptr<X> px(p);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<X> p;
        boost::unshared_ptr<X const> px(p);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<Y> p;
        boost::unshared_ptr<X> px(p);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<Y> p;
        boost::unshared_ptr<X const> px(p);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<Y> p;
        boost::unshared_ptr<void> pv(p);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<Y> p;
        boost::unshared_ptr<void const> pv(p);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(p.get() == 0);
    }

    {
        std::auto_ptr<int> p(new int(7));
        int * q = p.get();
        boost::unshared_ptr<int> pi(p);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == q);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(*pi == 7);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    {
        std::auto_ptr<int> p(new int(7));
        int * q = p.get();
        boost::unshared_ptr<int const> pi(p);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == q);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(*pi == 7);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    {
        std::auto_ptr<int> p(new int(7));
        int * q = p.get();
        boost::unshared_ptr<void> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == q);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    {
        std::auto_ptr<int> p(new int(7));
        int * q = p.get();
        boost::unshared_ptr<void const> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == q);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    BOOST_TEST(X::instances == 0);

    {
        std::auto_ptr<X> p(new X);
        X * q = p.get();
        boost::unshared_ptr<X> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == q);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    BOOST_TEST(X::instances == 0);

    {
        std::auto_ptr<X> p(new X);
        X * q = p.get();
        boost::unshared_ptr<X const> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == q);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    BOOST_TEST(X::instances == 0);

    {
        std::auto_ptr<X> p(new X);
        X * q = p.get();
        boost::unshared_ptr<void> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == q);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    BOOST_TEST(X::instances == 0);

    {
        std::auto_ptr<X> p(new X);
        X * q = p.get();
        boost::unshared_ptr<void const> pv(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == q);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    BOOST_TEST(X::instances == 0);
    BOOST_TEST(Y::instances == 0);

    {
        std::auto_ptr<Y> p(new Y);
        Y * q = p.get();
        boost::unshared_ptr<X> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == q);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    BOOST_TEST(X::instances == 0);
    BOOST_TEST(Y::instances == 0);

    {
        std::auto_ptr<Y> p(new Y);
        Y * q = p.get();
        boost::unshared_ptr<X const> px(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == q);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p.get() == 0);
#endif
    }

    BOOST_TEST(X::instances == 0);
    BOOST_TEST(Y::instances == 0);

#endif // #if !defined( BOOST_NO_AUTO_PTR )
}

void test()
{
    default_constructor();
    pointer_constructor();
    deleter_constructor();
    weak_ptr_constructor();
    auto_ptr_constructor();
}

} // namespace n_constructors

namespace n_assignment
{

class incomplete;

struct A
{
    int dummy;
};

struct X
{
    static long instances;

    X()
    {
        ++instances;
    }

    ~X()
    {
        --instances;
    }

private:

    X(X const &);
    X & operator= (X const &);
};

long X::instances = 0;

struct Y: public A, public virtual X
{
    static long instances;

    Y()
    {
        ++instances;
    }

    ~Y()
    {
        --instances;
    }

private:

    Y(Y const &);
    Y & operator= (Y const &);
};

long Y::instances = 0;

void auto_ptr_assignment()
{
#if !defined( BOOST_NO_AUTO_PTR )

    {
        boost::unshared_ptr<int> p1;

        std::auto_ptr<int> p2;

        p1 = p2;
        BOOST_TEST(p1? false: true);
        BOOST_TEST(!p1);
        BOOST_TEST(p1.get() == 0);
        BOOST_TEST(p1.use_count() == 1);

        int * p = new int;
        std::auto_ptr<int> p3(p);

        p1 = p3;
        BOOST_TEST(p1.get() == p);
        BOOST_TEST(p1.use_count() == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p3.get() == 0);
#endif

        p1 = p2;
        BOOST_TEST(p1? false: true);
        BOOST_TEST(!p1);
        BOOST_TEST(p1.get() == 0);
        BOOST_TEST(p1.use_count() == 1);
    }

    {
        boost::unshared_ptr<void> p1;

        std::auto_ptr<int> p2;

        p1 = p2;
        BOOST_TEST(p1? false: true);
        BOOST_TEST(!p1);
        BOOST_TEST(p1.get() == 0);
        BOOST_TEST(p1.use_count() == 1);

        int * p = new int;
        std::auto_ptr<int> p3(p);

        p1 = p3;
        BOOST_TEST(p1.get() == p);
        BOOST_TEST(p1.use_count() == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p3.get() == 0);
#endif

        p1 = p2;
        BOOST_TEST(p1? false: true);
        BOOST_TEST(!p1);
        BOOST_TEST(p1.get() == 0);
        BOOST_TEST(p1.use_count() == 1);
    }


    {
        boost::unshared_ptr<X> p1;

        std::auto_ptr<Y> p2;

        p1 = p2;
        BOOST_TEST(p1? false: true);
        BOOST_TEST(!p1);
        BOOST_TEST(p1.get() == 0);
        BOOST_TEST(p1.use_count() == 1);
        BOOST_TEST(X::instances == 0);
        BOOST_TEST(Y::instances == 0);

        Y * p = new Y;
        std::auto_ptr<Y> p3(p);

        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);

        p1 = p3;
        BOOST_TEST(p1.get() == p);
        BOOST_TEST(p1.use_count() == 1);
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);

#if !defined(BOOST_OLD_AUTO_PTR)
        BOOST_TEST(p3.get() == 0);
#endif

        p1 = p2;
        BOOST_TEST(p1? false: true);
        BOOST_TEST(!p1);
        BOOST_TEST(p1.get() == 0);
        BOOST_TEST(p1.use_count() == 1);
        BOOST_TEST(X::instances == 0);
        BOOST_TEST(Y::instances == 0);
    }

#endif // #if !defined( BOOST_NO_AUTO_PTR )
}

void test()
{
    auto_ptr_assignment();
}

} // namespace n_assignment

namespace n_reset
{

class incomplete;

incomplete * p0 = 0;

void deleter(incomplete *)
{
}

struct X
{
    static long instances;

    X()
    {
        ++instances;
    }

    ~X()
    {
        --instances;
    }

private:

    X(X const &);
    X & operator= (X const &);
};

long X::instances = 0;

void plain_reset()
{
    {
        boost::unshared_ptr<int> pi;
        pi.reset();
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 0);
    }

    {
        boost::unshared_ptr<int> pi(static_cast<int*>(0));
        pi.reset();
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 0);
    }

    {
        boost::unshared_ptr<int> pi(new int);
        pi.reset();
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 0);
    }

    {
        boost::unshared_ptr<incomplete> px;
        px.reset();
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 0);
    }

    {
        boost::unshared_ptr<incomplete> px(p0, deleter);
        px.reset();
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 0);
    }

    {
        boost::unshared_ptr<X> px;
        px.reset();
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 0);
    }

    {
        BOOST_TEST(X::instances == 0);
        boost::unshared_ptr<X> px(new X);
        BOOST_TEST(X::instances == 1);
        px.reset();
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(X::instances == 0);
    }

    {
        boost::unshared_ptr<void> pv;
        pv.reset();
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 0);
    }

    {
        BOOST_TEST(X::instances == 0);
        boost::unshared_ptr<void> pv(new X);
        BOOST_TEST(X::instances == 1);
        pv.reset();
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 0);
        BOOST_TEST(X::instances == 0);
    }
}

struct A
{
    int dummy;
};

struct Y: public A, public virtual X
{
    static long instances;

    Y()
    {
        ++instances;
    }

    ~Y()
    {
        --instances;
    }

private:

    Y(Y const &);
    Y & operator= (Y const &);
};

long Y::instances = 0;

void pointer_reset()
{
    {
        boost::unshared_ptr<int> pi;

        pi.reset(static_cast<int*>(0));
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());

        int * p = new int;
        pi.reset(p);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == p);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());

        pi.reset(static_cast<int*>(0));
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());
    }

    {
        boost::unshared_ptr<X> px;

        px.reset(static_cast<X*>(0));
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 0);

        X * p = new X;
        px.reset(p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == p);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);

        px.reset(static_cast<X*>(0));
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 0);
        BOOST_TEST(Y::instances == 0);

        Y * q = new Y;
        px.reset(q);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == q);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);

        px.reset(static_cast<Y*>(0));
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());
        BOOST_TEST(X::instances == 0);
        BOOST_TEST(Y::instances == 0);
    }

    {
        boost::unshared_ptr<void> pv;

        pv.reset(static_cast<X*>(0));
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 0);

        X * p = new X;
        pv.reset(p);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == p);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 1);

        pv.reset(static_cast<X*>(0));
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 0);
        BOOST_TEST(Y::instances == 0);

        Y * q = new Y;
        pv.reset(q);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == q);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(Y::instances == 1);

        pv.reset(static_cast<Y*>(0));
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());
        BOOST_TEST(X::instances == 0);
        BOOST_TEST(Y::instances == 0);
    }
}

void * deleted = 0;

void deleter2(void * p)
{
    deleted = p;
}

void deleter_reset()
{
    {
        boost::unshared_ptr<int> pi;

        pi.reset(static_cast<int*>(0), deleter2);
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());

        deleted = &pi;

        int m = 0;
        pi.reset(&m, deleter2);
        BOOST_TEST(deleted == 0);
        BOOST_TEST(pi? true: false);
        BOOST_TEST(!!pi);
        BOOST_TEST(pi.get() == &m);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());

        pi.reset(static_cast<int*>(0), deleter2);
        BOOST_TEST(deleted == &m);
        BOOST_TEST(pi? false: true);
        BOOST_TEST(!pi);
        BOOST_TEST(pi.get() == 0);
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(pi.unique());

        pi.reset();
        BOOST_TEST(deleted == 0);
    }

    {
        boost::unshared_ptr<X> px;

        px.reset(static_cast<X*>(0), deleter2);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        deleted = &px;

        X x;
        px.reset(&x, deleter2);
        BOOST_TEST(deleted == 0);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == &x);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        px.reset(static_cast<X*>(0), deleter2);
        BOOST_TEST(deleted == &x);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        Y y;
        px.reset(&y, deleter2);
        BOOST_TEST(deleted == 0);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(px.get() == &y);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        px.reset(static_cast<Y*>(0), deleter2);
        BOOST_TEST(deleted == &y);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        px.reset();
        BOOST_TEST(deleted == 0);
    }

    {
        boost::unshared_ptr<void> pv;

        pv.reset(static_cast<X*>(0), deleter2);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());

        deleted = &pv;

        X x;
        pv.reset(&x, deleter2);
        BOOST_TEST(deleted == 0);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == &x);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());

        pv.reset(static_cast<X*>(0), deleter2);
        BOOST_TEST(deleted == &x);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());

        Y y;
        pv.reset(&y, deleter2);
        BOOST_TEST(deleted == 0);
        BOOST_TEST(pv? true: false);
        BOOST_TEST(!!pv);
        BOOST_TEST(pv.get() == &y);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());

        pv.reset(static_cast<Y*>(0), deleter2);
        BOOST_TEST(deleted == &y);
        BOOST_TEST(pv? false: true);
        BOOST_TEST(!pv);
        BOOST_TEST(pv.get() == 0);
        BOOST_TEST(pv.use_count() == 1);
        BOOST_TEST(pv.unique());

        pv.reset();
        BOOST_TEST(deleted == 0);
    }

    {
        boost::unshared_ptr<incomplete> px;

        px.reset(p0, deleter2);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        deleted = &px;
        px.reset(p0, deleter2);
        BOOST_TEST(deleted == 0);
    }
}

void test()
{
    plain_reset();
    pointer_reset();
    deleter_reset();
}

} // namespace n_reset

namespace n_access
{

struct X
{
};

void test()
{
    {
        boost::unshared_ptr<X> px;
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using boost::get_pointer;
#endif

        BOOST_TEST(get_pointer(px) == px.get());
    }

    {
        boost::unshared_ptr<X> px(static_cast<X*>(0));
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using boost::get_pointer;
#endif

        BOOST_TEST(get_pointer(px) == px.get());
    }

    {
        boost::unshared_ptr<X> px(static_cast<X*>(0), boost::checked_deleter<X>());
        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px? false: true);
        BOOST_TEST(!px);

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using boost::get_pointer;
#endif

        BOOST_TEST(get_pointer(px) == px.get());
    }

    {
        X * p = new X;
        boost::unshared_ptr<X> px(p);
        BOOST_TEST(px.get() == p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(&*px == px.get());
        BOOST_TEST(px.operator ->() == px.get());

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using boost::get_pointer;
#endif

        BOOST_TEST(get_pointer(px) == px.get());
    }

    {
        X * p = new X;
        boost::unshared_ptr<X> px(p, boost::checked_deleter<X>());
        BOOST_TEST(px.get() == p);
        BOOST_TEST(px? true: false);
        BOOST_TEST(!!px);
        BOOST_TEST(&*px == px.get());
        BOOST_TEST(px.operator ->() == px.get());

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using boost::get_pointer;
#endif

        BOOST_TEST(get_pointer(px) == px.get());
    }
}

} // namespace n_access

namespace n_use_count
{

struct X
{
};

void test()
{
    {
        boost::unshared_ptr<X> px(static_cast<X*>(0));
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        boost::unshared_ptr<X> px2(std::move(px));
        BOOST_TEST(px2.use_count() == 1);
        BOOST_TEST(px2.unique());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(!px.unique());
    }

    {
        boost::unshared_ptr<X> px(new X);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        boost::unshared_ptr<X> px2(std::move(px));
        BOOST_TEST(px2.use_count() == 1);
        BOOST_TEST(px2.unique());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(!px.unique());
    }

    {
        boost::unshared_ptr<X> px(new X, boost::checked_deleter<X>());
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px.unique());

        boost::unshared_ptr<X> px2(std::move(px));
        BOOST_TEST(px2.use_count() == 1);
        BOOST_TEST(px2.unique());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(!px.unique());
    }
}

} // namespace n_use_count

namespace n_swap
{

struct X
{
};

void test()
{
    {
        boost::unshared_ptr<X> px;
        boost::unshared_ptr<X> px2;

        px.swap(px2);

        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px2.get() == 0);

        using std::swap;
        swap(px, px2);

        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px2.get() == 0);
    }

    {
        X * p = new X;
        boost::unshared_ptr<X> px;
        boost::unshared_ptr<X> px2(p);

        px.swap(px2);

        BOOST_TEST(px.get() == p);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px2.get() == 0);

        using std::swap;
        swap(px, px2);

        BOOST_TEST(px.get() == 0);
        BOOST_TEST(px2.get() == p);
        BOOST_TEST(px2.use_count() == 1);
    }

    {
        X * p1 = new X;
        X * p2 = new X;
        boost::unshared_ptr<X> px(p1);
        boost::unshared_ptr<X> px2(p2);

        px.swap(px2);

        BOOST_TEST(px.get() == p2);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px2.get() == p1);
        BOOST_TEST(px2.use_count() == 1);

        using std::swap;
        swap(px, px2);

        BOOST_TEST(px.get() == p1);
        BOOST_TEST(px.use_count() == 1);
        BOOST_TEST(px2.get() == p2);
        BOOST_TEST(px2.use_count() == 1);
    }
}

} // namespace n_swap

namespace n_comparison
{

struct X
{
    int dummy;
};

struct Y
{
    int dummy2;
};

struct Z: public X, public virtual Y
{
};

void test()
{
    {
        boost::unshared_ptr<X> px;
        BOOST_TEST(px == px);
        BOOST_TEST(!(px != px));
        BOOST_TEST(!(px < px));

        boost::unshared_ptr<X> px2;

        BOOST_TEST(px.get() == px2.get());
        BOOST_TEST(px == px2);
        BOOST_TEST(!(px != px2));
        BOOST_TEST(!(px < px2 && px2 < px));
    }

    {
        boost::unshared_ptr<X> px;
        boost::unshared_ptr<X> px2(new X);

        BOOST_TEST(px2 == px2);
        BOOST_TEST(!(px2 != px2));
        BOOST_TEST(!(px2 < px2));

        BOOST_TEST(px.get() != px2.get());
        BOOST_TEST(px != px2);
        BOOST_TEST(!(px == px2));
        BOOST_TEST(px < px2 || px2 < px);
        BOOST_TEST(!(px < px2 && px2 < px));
    }

    {
        boost::unshared_ptr<X> px(new X);
        boost::unshared_ptr<X> px2(new X);

        BOOST_TEST(px.get() != px2.get());
        BOOST_TEST(px != px2);
        BOOST_TEST(!(px == px2));
        BOOST_TEST(px < px2 || px2 < px);
        BOOST_TEST(!(px < px2 && px2 < px));
    }

    {
        boost::unshared_ptr<X> px(new X);
        boost::unshared_ptr<Y> py(new Y);
        boost::unshared_ptr<Z> pz(new Z);

        BOOST_TEST(px.get() != pz.get());
        BOOST_TEST(px != pz);
        BOOST_TEST(!(px == pz));

        BOOST_TEST(py.get() != pz.get());
        BOOST_TEST(py != pz);
        BOOST_TEST(!(py == pz));

        BOOST_TEST(px < py || py < px);
        BOOST_TEST(px < pz || pz < px);
        BOOST_TEST(py < pz || pz < py);

        BOOST_TEST(!(px < py && py < px));
        BOOST_TEST(!(px < pz && pz < px));
        BOOST_TEST(!(py < pz && pz < py));

        boost::unshared_ptr<void> pvx(std::move(px));

        BOOST_TEST(pvx == pvx);
        BOOST_TEST(!(pvx != pvx));
        BOOST_TEST(!(pvx < pvx));

        boost::unshared_ptr<void> pvy(std::move(py));
        boost::unshared_ptr<void> pvz(std::move(pz));

        BOOST_TEST(pvx < pvy || pvy < pvx);
        BOOST_TEST(pvx < pvz || pvz < pvx);
        BOOST_TEST(pvy < pvz || pvz < pvy);

        BOOST_TEST(!(pvx < pvy && pvy < pvx));
        BOOST_TEST(!(pvx < pvz && pvz < pvx));
        BOOST_TEST(!(pvy < pvz && pvz < pvy));
    }
}

} // namespace n_comparison

namespace n_static_cast
{

struct X
{
};

struct Y: public X
{
};

void test()
{
    {
        boost::unshared_ptr<void> pv;

        boost::unshared_ptr<int> pi = boost::static_pointer_cast<int>(std::move(pv));
        BOOST_TEST(pi.get() == 0);
    }

    {
        boost::unshared_ptr<void> pv;

        boost::unshared_ptr<X> px = boost::static_pointer_cast<X>(std::move(pv));
        BOOST_TEST(px.get() == 0);
    }

    {
        int * const rpi = new int;
        boost::unshared_ptr<int> pi(rpi);
        boost::unshared_ptr<void> pv(std::move(pi));

        boost::unshared_ptr<int> pi2 = boost::static_pointer_cast<int>(std::move(pv));
        BOOST_TEST(rpi == pi2.get());
        BOOST_TEST(pi.use_count() == 0);
        BOOST_TEST(pv.use_count() == 0);
        BOOST_TEST(pi2.use_count() == 1);
    }

    {
        X * const rpx = new X;
        boost::unshared_ptr<X> px(rpx);
        boost::unshared_ptr<void> pv(std::move(px));

        boost::unshared_ptr<X> px2 = boost::static_pointer_cast<X>(std::move(pv));
        BOOST_TEST(rpx == px2.get());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(pv.use_count() == 0);
        BOOST_TEST(px2.use_count() == 1);
    }

    {
        X * const rpy = new Y;
        boost::unshared_ptr<X> px(rpy);

        boost::unshared_ptr<Y> py = boost::static_pointer_cast<Y>(std::move(px));
        BOOST_TEST(rpy == py.get());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(py.use_count() == 1);

        boost::unshared_ptr<X> px2(std::move(py));
        BOOST_TEST(rpy == px2.get());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(py.use_count() == 0);
        BOOST_TEST(px2.use_count() == 1);
    }
}

} // namespace n_static_cast

namespace n_const_cast
{

struct X;

void test()
{
    {
        boost::unshared_ptr<void const volatile> px;

        boost::unshared_ptr<void> px2 = boost::const_pointer_cast<void>(std::move(px));
        BOOST_TEST(px2.get() == 0);
    }

    {
        boost::unshared_ptr<int const volatile> px;

        boost::unshared_ptr<int> px2 = boost::const_pointer_cast<int>(std::move(px));
        BOOST_TEST(px2.get() == 0);
    }

    {
        boost::unshared_ptr<X const volatile> px;

        boost::unshared_ptr<X> px2 = boost::const_pointer_cast<X>(std::move(px));
        BOOST_TEST(px2.get() == 0);
    }

    {
        int * const rpi = new int;
        boost::unshared_ptr<void const volatile> px(rpi);

        boost::unshared_ptr<void> px2 = boost::const_pointer_cast<void>(std::move(px));
        BOOST_TEST(rpi == px2.get());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(px2.use_count() == 1);
    }

    {
        int * const rpi = new int;
        boost::unshared_ptr<int const volatile> px(rpi);

        boost::unshared_ptr<int> px2 = boost::const_pointer_cast<int>(std::move(px));
        BOOST_TEST(rpi == px2.get());
        BOOST_TEST(px.use_count() == 0);
        BOOST_TEST(px2.use_count() == 1);
    }
}

} // namespace n_const_cast

#if !defined( BOOST_NO_RTTI )

namespace n_dynamic_cast
{

struct V
{
    virtual ~V() {}
};

struct W: public V
{
};

void test()
{
    {
        boost::unshared_ptr<V> pv;
        boost::unshared_ptr<W> pw = boost::dynamic_pointer_cast<W>(std::move(pv));
        BOOST_TEST(pw.get() == 0);
    }

    {
        boost::unshared_ptr<V> pv(static_cast<V*>(0));

        boost::unshared_ptr<W> pw = boost::dynamic_pointer_cast<W>(std::move(pv));
        BOOST_TEST(pw.get() == 0);
    }

    {
        boost::unshared_ptr<V> pv(static_cast<W*>(0));

        boost::unshared_ptr<W> pw = boost::dynamic_pointer_cast<W>(std::move(pv));
        BOOST_TEST(pw.get() == 0);
    }

    {
        V * rpv = new V;
        boost::unshared_ptr<V> pv(rpv);

        boost::unshared_ptr<W> pw = boost::dynamic_pointer_cast<W>(std::move(pv));
        BOOST_TEST(pw.get() == 0);
    }

    {
        V * rpv = new W;
        boost::unshared_ptr<V> pv(rpv);

        boost::unshared_ptr<W> pw = boost::dynamic_pointer_cast<W>(std::move(pv));
        BOOST_TEST(pw.get() == rpv);
        BOOST_TEST(pv.use_count() == 0);
        BOOST_TEST(pw.use_count() == 1);
    }
}

} // namespace n_dynamic_cast

#endif

namespace n_map
{

struct X
{
};

void test()
{
    std::vector< boost::unshared_ptr<int> > vi;

    {
        boost::unshared_ptr<int> pi1(new int);
        boost::unshared_ptr<int> pi2(new int);
        boost::unshared_ptr<int> pi3(new int);

        vi.push_back(std::move(pi1));
        vi.push_back(std::move(pi2));
        vi.push_back(std::move(pi3));
    }

    std::vector< boost::unshared_ptr<X> > vx;

    {
        boost::unshared_ptr<X> px1(new X);
        boost::unshared_ptr<X> px2(new X);
        boost::unshared_ptr<X> px3(new X);

        vx.push_back(std::move(px2));
        vx.push_back(std::move(px1));
        vx.push_back(std::move(px3));
    }

    std::map< boost::unshared_ptr<void>, long > m;

    {
        for(std::vector< boost::unshared_ptr<int> >::iterator i = vi.begin(); i != vi.end(); ++i)
        {
            ++m[std::move(*i)];
        }
    }

    {
        for(std::vector< boost::unshared_ptr<X> >::iterator i = vx.begin(); i != vx.end(); ++i)
        {
            ++m[std::move(*i)];
        }
    }

    {
        for(std::map< boost::unshared_ptr<void>, long >::iterator i = m.begin(); i != m.end(); ++i)
        {
            BOOST_TEST(i->first.use_count() == 1);
        }
    }
}

} // namespace n_map

namespace n_transitive
{

struct X
{
    X(): next() {}
    boost::unshared_ptr<X> next;
};

void test()
{
    boost::unshared_ptr<X> p(new X);
    p->next = boost::unshared_ptr<X>(new X);
    BOOST_TEST(!p->next->next);
    p = std::move(p->next);
    BOOST_TEST(!p->next);
}

} // namespace n_transitive

namespace n_report_1
{

class foo
{
public:

    foo(): m_self(this)
    {
    }

    void suicide()
    {
        m_self.reset();
    }

private:

    boost::unshared_ptr<foo> m_self;
};

void test()
{
    foo * foo_ptr = new foo;
    foo_ptr->suicide();
}

} // namespace n_report_1

namespace n_spt_incomplete
{

class file;

boost::unshared_ptr<file> fopen(char const * name, char const * mode);
void fread(file & f, void * data, long size);

int file_instances = 0;

void test()
{
    BOOST_TEST(file_instances == 0);

    {
        boost::unshared_ptr<file> pf = fopen("name", "mode");
        BOOST_TEST(file_instances == 1);
        fread(*pf, 0, 17041);
    }

    BOOST_TEST(file_instances == 0);
}

} // namespace n_spt_incomplete

namespace n_spt_pimpl
{

class file
{
private:

    class impl;
    boost::unshared_ptr<impl> pimpl_;

public:

    file(char const * name, char const * mode);

    // compiler generated members are fine and useful

    void read(void * data, long size);

    long total_size() const;
};

int file_instances = 0;

void test()
{
    BOOST_TEST(file_instances == 0);

    {
        file f("name", "mode");
        BOOST_TEST(file_instances == 1);
        f.read(0, 152);

        file f2(std::move(f));
        BOOST_TEST(file_instances == 1);
        f2.read(0, 894);

        BOOST_TEST(f2.total_size() == 152+894);

        {
            file f3("name2", "mode2");
            BOOST_TEST(file_instances == 2);
        }

        BOOST_TEST(file_instances == 1);
    }

    BOOST_TEST(file_instances == 0);
}

} // namespace n_spt_pimpl

namespace n_spt_abstract
{

class X
{
public:

    virtual void f(int) = 0;
    virtual int g() = 0;

protected:

    ~X() {}
};

boost::unshared_ptr<X> createX();

int X_instances = 0;

void test()
{
    BOOST_TEST(X_instances == 0);

    {
        boost::unshared_ptr<X> px = createX();

        BOOST_TEST(X_instances == 1);

        px->f(18);
        px->f(152);

        BOOST_TEST(px->g() == 170);
    }

    BOOST_TEST(X_instances == 0);
}

} // namespace n_spt_abstract

namespace n_spt_preventing_delete
{

int X_instances = 0;

class X
{
private:

    X()
    {
        ++X_instances;
    }

    ~X()
    {
        --X_instances;
    }

    class deleter;
    friend class deleter;

    class deleter
    {
    public:

        void operator()(X * p) { delete p; }
    };

public:

    static boost::unshared_ptr<X> create()
    {
        boost::unshared_ptr<X> px(new X, X::deleter());
        return px;
    }
};

void test()
{
    BOOST_TEST(X_instances == 0);

    {
        boost::unshared_ptr<X> px = X::create();
        BOOST_TEST(X_instances == 1);
    }

    BOOST_TEST(X_instances == 0);
}

} // namespace n_spt_preventing_delete

namespace n_spt_array
{

int X_instances = 0;

struct X
{
    X()
    {
        ++X_instances;
    }

    ~X()
    {
        --X_instances;
    }
};

void test()
{
    BOOST_TEST(X_instances == 0);

    {
        boost::unshared_ptr<X> px(new X[4], boost::checked_array_deleter<X>());
        BOOST_TEST(X_instances == 4);
    }

    BOOST_TEST(X_instances == 0);
}

} // namespace n_spt_array

namespace n_spt_static
{

class X
{
public:

    X()
    {
    }

private:

    void operator delete(void *)
    {
        // Comeau 4.3.0.1 wants a definition
        BOOST_ERROR("n_spt_static::X::operator delete() called.");
    }
};

struct null_deleter
{
    void operator()(void const *) const
    {
    }
};

static X x;

void test()
{
    boost::unshared_ptr<X> px(&x, null_deleter());
}

} // namespace n_spt_static

namespace n_spt_another_sp
{

template<class T> class another_ptr: private boost::shared_ptr<T>
{
private:

    typedef boost::shared_ptr<T> base_type;

public:

    explicit another_ptr(T * p = 0): base_type(p)
    {
    }

    void reset()
    {
        base_type::reset();
    }

    T * get() const
    {
        return base_type::get();
    }
};

class event_handler
{
public:

    virtual ~event_handler() {}
    virtual void begin() = 0;
    virtual void handle(int event) = 0;
    virtual void end() = 0;
};

int begin_called = 0;
int handle_called = 0;
int end_called = 0;

class event_handler_impl: public event_handler
{
public:

    virtual void begin()
    {
        ++begin_called;
    }

    virtual void handle(int event)
    {
        handle_called = event;
    }

    virtual void end()
    {
        ++end_called;
    }
};

another_ptr<event_handler> get_event_handler()
{
    another_ptr<event_handler> p(new event_handler_impl);
    return p;
}

boost::unshared_ptr<event_handler> current_handler;

void install_event_handler(boost::unshared_ptr<event_handler> && p)
{
    p->begin();
    current_handler = std::move(p);
}

void handle_event(int event)
{
    current_handler->handle(event);
}

void remove_event_handler()
{
    current_handler->end();
    current_handler.reset();
}

template<class P> class smart_pointer_deleter
{
private:

    P p_;

public:

    smart_pointer_deleter(P const & p): p_(p)
    {
    }

    void operator()(void const *)
    {
        p_.reset();
    }
};

void test()
{
    another_ptr<event_handler> p = get_event_handler();

    boost::unshared_ptr<event_handler> q(p.get(), smart_pointer_deleter< another_ptr<event_handler> >(p));

    p.reset();

    BOOST_TEST(begin_called == 0);

    install_event_handler(std::move(q));

    BOOST_TEST(begin_called == 1);

    BOOST_TEST(handle_called == 0);

    handle_event(17041);

    BOOST_TEST(handle_called == 17041);

    BOOST_TEST(end_called == 0);

    remove_event_handler();

    BOOST_TEST(end_called == 1);
}

} // namespace n_spt_another_sp

int main()
{
    n_element_type::test();
    n_constructors::test();
    n_assignment::test();
    n_reset::test();
    n_access::test();
    n_use_count::test();
    n_swap::test();
    n_comparison::test();
    n_static_cast::test();
    n_const_cast::test();
#if !defined( BOOST_NO_RTTI )
    n_dynamic_cast::test();
#endif

    n_map::test();

    n_transitive::test();
    n_report_1::test();

    n_spt_incomplete::test();
    n_spt_pimpl::test();
    n_spt_abstract::test();
    n_spt_preventing_delete::test();
    n_spt_array::test();
    n_spt_static::test();
    n_spt_another_sp::test();

    return boost::report_errors();
}

namespace n_spt_incomplete
{

class file
{
public:

    file(): fread_called(false)
    {
        ++file_instances;
    }

    ~file()
    {
        BOOST_TEST(fread_called);
        --file_instances;
    }

    bool fread_called;
};

boost::unshared_ptr<file> fopen(char const *, char const *)
{
    boost::unshared_ptr<file> pf(new file);
    return pf;
}

void fread(file & f, void *, long)
{
    f.fread_called = true;
}

} // namespace n_spt_incomplete

namespace n_spt_pimpl
{

class file::impl
{
private:

    impl(impl const &);
    impl & operator=(impl const &);

    long total_size_;

public:

    impl(char const *, char const *): total_size_(0)
    {
        ++file_instances;
    }

    ~impl()
    {
        --file_instances;
    }

    void read(void *, long size)
    {
        total_size_ += size;
    }

    long total_size() const
    {
        return total_size_;
    }
};

file::file(char const * name, char const * mode): pimpl_(new impl(name, mode))
{
}

void file::read(void * data, long size)
{
    pimpl_->read(data, size);
}

long file::total_size() const
{
    return pimpl_->total_size();
}

} // namespace n_spt_pimpl

namespace n_spt_abstract
{

class X_impl: public X
{
private:

    X_impl(X_impl const &);
    X_impl & operator=(X_impl const &);

    int n_;

public:

    X_impl(): n_(0)
    {
        ++X_instances;
    }

    ~X_impl()
    {
        --X_instances;
    }

    virtual void f(int n)
    {
        n_ += n;
    }

    virtual int g()
    {
        return n_;
    }
};

boost::unshared_ptr<X> createX()
{
    boost::unshared_ptr<X> px(new X_impl);
    return px;
}

} // namespace n_spt_abstract

#endif
