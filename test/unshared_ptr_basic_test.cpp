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



namespace shared_unshare
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
        BOOST_TEST(pi);
        BOOST_TEST(pi.unique());
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(!pi.unshared());
        BOOST_TEST(X::instances == 1);
        {
            boost::unshared_ptr<X> upi1(pi);
            BOOST_TEST(X::instances == 1);
            BOOST_TEST(!pi.unique());
            BOOST_TEST(pi.use_count() == 2);
            BOOST_TEST(pi.unshared());
            BOOST_TEST(upi1);
            BOOST_TEST(upi1.use_count()==2);
            BOOST_TEST(!upi1.unique());
            try
            {
                boost::unshared_ptr<X> upi2(pi);
                BOOST_ERROR("upi2 constructor failed to throw");
            }
            catch( boost::unshared_ptr_already_acquired const & )
            {
            }
        }
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(pi.use_count() == 1);
        BOOST_TEST(!pi.unshared());
        {
            boost::unshared_ptr<X> upi3(pi);
            BOOST_TEST(X::instances == 1);
            BOOST_TEST(!pi.unique());
            BOOST_TEST(pi.use_count() == 2);
            BOOST_TEST(pi.unshared());
            BOOST_TEST(upi3);
            BOOST_TEST(upi3.use_count()==2);
            BOOST_TEST(!upi3.unique());
            boost::shared_ptr<X> pi2(pi);
            BOOST_TEST(!pi2.unique());
            BOOST_TEST(pi2.unshared());
            BOOST_TEST(upi3.use_count()==3);
            BOOST_TEST(pi.use_count() == 3);
            try
            {
                boost::unshared_ptr<X> upi4(pi2);
                BOOST_ERROR("upi4 constructor failed to throw");
            }
            catch( boost::unshared_ptr_already_acquired const & )
            {
            }
        }
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(pi.unique());
        BOOST_TEST(pi.use_count() == 1);
        {
            X * const rpi = pi.get();
            boost::unshared_ptr<X> upi5(pi);
            pi.reset();
            BOOST_TEST(upi5.unique());
            BOOST_TEST(upi5.use_count() == 1);
            BOOST_TEST(upi5.get() == rpi);
            boost::shared_ptr<X> pi1(upi5);
            BOOST_TEST(!upi5.unique());
            BOOST_TEST(upi5.use_count() == 2);
            BOOST_TEST(upi5 == pi1);
            BOOST_TEST(!(upi5 != pi1));
            BOOST_TEST(!pi1.unique());
        }
        {
            boost::unshared_ptr<X> upi6;
            try
            {
                boost::shared_ptr<X> pi2(upi6);
                BOOST_ERROR("pi2 constructor failed to throw");
            }
            catch( boost::bad_unshared_ptr const & )
            {
            }
        }
        BOOST_TEST(X::instances == 0);
    }

    void unshare_test()
    {
        boost::shared_ptr<X> pi = boost::make_shared<X>();
        BOOST_TEST(X::instances == 1);
        BOOST_TEST(pi);
        BOOST_TEST(pi.unique());
        BOOST_TEST(!pi.unshared());
        {
            boost::unshared_ptr<X> upi1 = unshare(pi);
            BOOST_TEST(X::instances == 1);
            BOOST_TEST(!pi.unique());
            BOOST_TEST(pi.unshared());
            BOOST_TEST(upi1);
            BOOST_TEST(upi1.use_count()==2);
            BOOST_TEST(!upi1.unique());
            boost::unshared_ptr<X> upi2 = unshare(pi);
            BOOST_TEST(!upi2);
        }
        BOOST_TEST(pi.unique());
        BOOST_TEST(!pi.unshared());
        BOOST_TEST(X::instances == 1);
        {
            boost::unshared_ptr<X> upi3 = unshare(pi);
            BOOST_TEST(!pi.unique());
            BOOST_TEST(pi.unshared());
            BOOST_TEST(upi3);
            BOOST_TEST(upi3.use_count()==2);
            BOOST_TEST(!upi3.unique());
            boost::shared_ptr<X> pi2(pi);
            BOOST_TEST(!pi2.unique());
            BOOST_TEST(pi2.unshared());
            BOOST_TEST(upi3.use_count()==3);
            upi3.reset();
            BOOST_TEST(!pi.unshared());
            BOOST_TEST(!pi2.unshared());
        }
        BOOST_TEST(pi.unique());
        pi.reset();
        BOOST_TEST(X::instances == 0);
    }

    void test()
    {
        constructor_test();
        unshare_test();
    }
}

namespace weak_unshare
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
        BOOST_TEST(pi);
        BOOST_TEST(pi.unique());
        BOOST_TEST(!pi.unshared());
        boost::weak_ptr<X> wpi( pi );
        BOOST_TEST(!wpi.expired());
        BOOST_TEST(!wpi.unshared());
        {
            boost::unshared_ptr<X> upi1(wpi);
            BOOST_TEST(!pi.unique());
            BOOST_TEST(pi.unshared());
            BOOST_TEST(wpi.unshared());
            BOOST_TEST(upi1);
            BOOST_TEST(upi1.use_count()==2);
            BOOST_TEST(!upi1.unique());
            try
            {
                boost::unshared_ptr<X> upi2(wpi);
                BOOST_ERROR("upi2 constructor failed to throw");
            }
            catch( boost::unshared_ptr_already_acquired const & )
            {
            }
            try
            {
                boost::unshared_ptr<X> upi3(pi);
                BOOST_ERROR("upi3 constructor failed to throw");
            }
            catch( boost::unshared_ptr_already_acquired const & )
            {
            }
        }
        BOOST_TEST(pi.unique());
        BOOST_TEST(!pi.unshared());
        pi.reset();
        try
        {
            boost::unshared_ptr<X> upi4(wpi);
            BOOST_ERROR("upi3 constructor failed to throw");
        }
        catch( boost::bad_weak_ptr const & )
        {
        }
    }

    void unshare_test()
    {
        boost::shared_ptr<X> pi = boost::make_shared<X>();
        BOOST_TEST(pi);
        BOOST_TEST(pi.unique());
        BOOST_TEST(!pi.unshared());
        boost::weak_ptr<X> wpi( pi );
        BOOST_TEST(!wpi.expired());
        BOOST_TEST(!wpi.unshared());
        {
            boost::unshared_ptr<X> upi1 = unshare(wpi);
            BOOST_TEST(!pi.unique());
            BOOST_TEST(pi.unshared());
            BOOST_TEST(wpi.unshared());
            BOOST_TEST(upi1);
            BOOST_TEST(upi1.use_count()==2);
            BOOST_TEST(!upi1.unique());
            boost::unshared_ptr<X> upi2 = unshare(wpi);
            BOOST_TEST(!upi2);
            boost::unshared_ptr<X> upi3 = unshare(pi);
            BOOST_TEST(!upi3);
        }
        BOOST_TEST(pi.unique());
        BOOST_TEST(!pi.unshared());
        boost::unshared_ptr<X> upi4 = unshare(pi);
        BOOST_TEST(!upi4.unique());
        pi.reset();
        BOOST_TEST(upi4.unique());
        boost::unshared_ptr<X> upi5 = unshare(wpi);
        BOOST_TEST(!upi5);
        boost::unshared_ptr<X> upi6 = unshare(pi);
        BOOST_TEST(!upi6);
    }

    void test()
    {
        constructor_test();
        unshare_test();
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
        BOOST_TEST(upi1.unique());
        BOOST_TEST(upi1.get() == pi);
        boost::unshared_ptr<X> upi2 = std::move(upi1);
        BOOST_TEST(!upi1);
        BOOST_TEST(!upi1.unique());
        BOOST_TEST(upi1.get() == 0);
        BOOST_TEST(upi2);
        BOOST_TEST(upi2.unique());
        BOOST_TEST(upi2.get() == pi);
    }
}

int main()
{
    shared_unshare::test();
    weak_unshare::test();
    standalone::test();

    return boost::report_errors();
}

#endif
