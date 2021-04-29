/* For more information check out the primer:  		https://google.github.io/googletest/primer.html
 * For example tests check out the github examples: 	https://github.com/google/googletest/tree/master/googletest/samples
 * 
 * Each of the tests can have multiple assertions but they typically cover a single feature.
 * The test output will look best if the class and the feature are passed into TEST().
 *
 * To run the tests go to the program root directory and run:
 * 
 * cmake -S . -B build
 * cmake --build build
 * cd build && ctest
 *
 */

#include <gtest/gtest.h>
#include "cell.h"
#include "spreadsheet.h"
#include "client.h"

TEST(Cell, EqualityOperator) {
    cell cell1("1+1", "A1");
    cell cell2("1+1", "A1");
    cell cell3("1+1", "A2");

    ASSERT_TRUE(cell1 == cell2);
    ASSERT_FALSE(cell1 == cell3);
}

TEST(Spreadsheet, updateCell) {
    ASSERT_EQ("spreadsheet", "spreadsheet");
}

TEST(Client, sendMessage) {
    ASSERT_GE(2, 1);
}



