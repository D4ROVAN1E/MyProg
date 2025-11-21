#define BOOST_TEST_MODULE MyTests
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(addition_test)
{
    int a = 2;
    int b = 3;
    BOOST_CHECK(a + b == 5);
}

BOOST_AUTO_TEST_CASE(fail_test)
{
    int x = 10;
    BOOST_CHECK(x == 20); // этот тест упадет
}
