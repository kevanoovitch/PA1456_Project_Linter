

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "inputHandler.h"
#include <typeinfo>

TEST(inputHandler, inputArgsTest)
{

    std::string arg1 = "https://github.com/kevanoovitch/-DV1697-OS-course";
    std::string arg2 = "https://www.dn.se/";
    std::string arg3 = "github.com/kevanoovitch/-DV1697-OS-course";

    /*test that a correct url works*/

    inputHandler handler1(arg1);

    ASSERT_EQ(handler1.argumentChecker(arg1), true)
        << "Expected argChecker to return true on okay URL, but it returned false";

    ASSERT_EQ(typeid(*(handler1.inputStrategy)), typeid(typeURL)) // returns true if x,x are equal type
        << "Expected inputStrategy to be of typeURL, but it wasn't";

    /*--- Test: non-git url gets denied ---*/

    inputHandler handler2(arg2);

    ASSERT_EQ(handler2.argumentChecker(arg2), false)
        << "Expected argChecker to return false on invalid URL, but it returned true";

    ASSERT_EQ(handler2.inputStrategy, nullptr)
        << "Expected inputStrategy to be null ptr when invalid URL was inputed, but it wasn't";

    /* Test minimal link works*/

    inputHandler handler3(arg3);

    ASSERT_EQ(handler3.argumentChecker(arg3), true)
        << "Expected argChecker to return true on okay URL, but it returned false";

    ASSERT_EQ(typeid(*(handler3.inputStrategy)), typeid(typeURL))
        << "Expected inputStrategy to be null ptr when invalid URL was inputed, but it wasn't";
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
    /*--- Test 2: test if all files are listed ---*/
}

/*--- Test 2: test if all files are listed ---*/
