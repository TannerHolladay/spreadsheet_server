//
// Created by noahr on 4/28/2021.
//

#include "Test.h"
#include "cell.h"
#include <iostream>
#include <cassert>

/*
Template 
void TestName_Condition_Result() {
    // Test Logic

    if() {
	std::cout << --- PASS --- <method name> << std::endl;
    }
    else{
	std::cout << --- FAIL --- <method name> << std::endl;
    }
}

*/
int main()
{
////////////CELL: Get Cell Contents/////////////////
    std::cout << "CELL: Get Cell Contents Tests:" << std::endl << std::endl;

    //Test 1
    cell cell1("1","A1");
    assert(cell1.getContents() == "1");


////////////CELL: UpdateContents/////////////////
    std::cout << "CELL: Update Contents Tests:" << std::endl << std::endl;

    //Test 1
    cell cell2("1","A2");
    cell2.updateContents("2");
    assert(cell2.getContents() == "2");

////////////CELL: CanRevert/////////////////
    std::cout << "CELL: canRevert Tests:" << std::endl << std::endl;

    //Test 1
    cell cell3("1","A3");
    static_assert(cell3.canRevert(), "cell3 cannot revert");

    //Test 2
    cell cell4("","A4");
    assert(cell4.canRevert());

    //Test 3
    cell cell5("","A5");
    cell5.updateContents("1");
    assert(cell5.canRevert());

////////////CELL: Revert/////////////////
    std::cout << "CELL: Revert Tests:" << std::endl << std::endl;

    //Test 1
    cell cell6("1","A6");
    cell6.revert();
    assert(cell6.getContents() == "");

    //Test 2
    cell cell7("1","A7");
    cell7.updateContents("2");
    cell7.revert();
    assert(cell7.getContents() == "1");

////////////CELL: Equality operator/////////////////
    std::cout << "CELL: Equality operator Tests:" << std::endl << std::endl;

    //Test 1
    cell cell8("1+1", "A1");
    cell cell9("1+1", "A1");
    assert(cell8 == cell9);


}
