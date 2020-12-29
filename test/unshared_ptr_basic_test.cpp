#include <boost/config.hpp>

#if defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

int main()
{
    return 0;
}

#else

#include <boost/unshared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/core/lightweight_test.hpp>



namespace shared_ptr_interaction
{
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

    void constructor_test()
    {
        boost::shared_ptr<X> pi = boost::make_shared<X>();
        BOOST_TEST( pi );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        BOOST_TEST( X::instances == 1 );
        {
            X * const rpi = pi.get();
            boost::unshared_ptr<X> upi1( std::move(pi) );
            BOOST_TEST( X::instances == 1 );
            BOOST_TEST( !pi );
            BOOST_TEST( !pi.unique() );
            BOOST_TEST( pi.use_count() == 0 );
            BOOST_TEST( upi1 );
            BOOST_TEST( upi1.get() == rpi );
            boost::unshared_ptr<X> upi2( std::move(pi) );
            BOOST_TEST( !upi2 );
            pi = std::move( upi1 );
        }
        BOOST_TEST( X::instances == 1 );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        BOOST_TEST( X::instances == 1 );
        {
            boost::shared_ptr<X> pi2(pi);
            try
            {
                boost::unshared_ptr<X> upi3( std::move(pi) );
                BOOST_ERROR("upi3 constructor failed to throw");
            }
            catch( boost::shared_ptr_not_unique const & )
            {
            }
        }
        BOOST_TEST( X::instances == 1 );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        BOOST_TEST( X::instances == 1 );
        {
            boost::unshared_ptr<X> upi4( std::move(pi) );
        }
        BOOST_TEST( X::instances == 0 );
    }

    void unshare_test()
    {
        boost::shared_ptr<X> pi = boost::make_shared<X>();
        BOOST_TEST( pi );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        BOOST_TEST( X::instances == 1 );
        {
            X * const rpi = pi.get();
            boost::unshared_ptr<X> upi1 = pi.unshare();
            BOOST_TEST( X::instances == 1 );
            BOOST_TEST( !pi );
            BOOST_TEST( !pi.unique() );
            BOOST_TEST( pi.use_count() == 0 );
            BOOST_TEST( upi1 );
            BOOST_TEST( upi1.get() == rpi );
            boost::unshared_ptr<X> upi2( std::move(pi) );
            BOOST_TEST(!upi2);
            pi = std::move( upi1 );
        }
        BOOST_TEST( X::instances == 1 );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        BOOST_TEST( X::instances == 1 );
        {
            boost::shared_ptr<X> pi2( pi );
            boost::unshared_ptr<X> upi3 = pi.unshare();
            BOOST_TEST(!upi3);
        }
        BOOST_TEST( X::instances == 1 );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        BOOST_TEST( X::instances == 1 );
        pi.reset();
        BOOST_TEST( X::instances == 0 );
    }

    void test()
    {
        constructor_test();
        unshare_test();
    }
}

namespace weak_ptr_interaction
{
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

    void lock_expired_test()
    {
        boost::shared_ptr<X> pi = boost::make_shared<X>();
        BOOST_TEST( pi );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        boost::weak_ptr<X> wpi( pi );
        BOOST_TEST(!wpi.expired());
        {
            boost::unshared_ptr<X> upi1 = pi.unshare();
            BOOST_TEST( upi1 );
            BOOST_TEST( !pi.unique() );
            BOOST_TEST( pi.use_count() == 0 );
            BOOST_TEST( wpi.use_count() == 0 );
            BOOST_TEST( wpi.expired() );
            BOOST_TEST( !wpi.lock() );
            pi = std::move(upi1);
        }
        BOOST_TEST( pi );
        BOOST_TEST( pi.unique() );
        BOOST_TEST( pi.use_count() == 1 );
        BOOST_TEST( !wpi.owner_before(pi) && !pi.owner_before(wpi) );
        {
            boost::shared_ptr<X> pi1 = wpi.lock();
            BOOST_TEST( !pi.unshare() );
            BOOST_TEST( pi );
            BOOST_TEST( pi1 );
            BOOST_TEST( !pi.unique() );
            BOOST_TEST( !pi1.unique() );
            BOOST_TEST( pi.use_count() == 2 );
            BOOST_TEST( pi1.use_count() == 2 );
        }
    }

    void test()
    {
        lock_expired_test();
    }
}

namespace standalone
{
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

    void test()
    {
        X * pi = new X;
        boost::unshared_ptr<X> upi1(pi);
        BOOST_TEST(upi1);
        BOOST_TEST(upi1.get() == pi);
        boost::unshared_ptr<X> upi2 = std::move(upi1);
        BOOST_TEST(!upi1);
        BOOST_TEST(upi1.get() == 0);
        BOOST_TEST(upi2);
        BOOST_TEST(upi2.get() == pi);
    }
}

int main()
{
    shared_ptr_interaction::test();
    weak_ptr_interaction::test();
    standalone::test();

    return boost::report_errors();
}

#endif
