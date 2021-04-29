//
// Created by noahr on 4/28/2021.
//

#include "Test.h"
#include "cell.h"
#include "spreadsheet.h"
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

void Cell_WhenEmptyRevert_ReturnsNoContents();
void Cell_AfterUpdate_ReturnsNoContents();
void Cell_WithContents_CanRevert();
void Cell_WithoutContents_CanRevert();
void Cell_AfterUpdate_CanRevert();
void Cell_Get_Contents1();
void Cell_Update_Contents1();
void Cell_Equality_Operator1();

int main()
{
    std::cout << "CELL: Revert Tests:" << std::endl << std::endl;
    Cell_WhenEmptyRevert_ReturnsNoContents();
    Cell_AfterUpdate_ReturnsNoContents();
    std::cout << std::endl;

    std::cout << "CELL: CanRevert Tests:" << std::endl << std::endl;
    Cell_WithContents_CanRevert();
    Cell_WithoutContents_CanRevert();
    Cell_AfterUpdate_CanRevert();
    std::cout << std::endl;

    std::cout << "CELL: Get Cell Contents Tests:" << std::endl << std::endl;
    Cell_Get_Contents1();
    std::cout << std::endl;

    std::cout << "CELL: Update Contents Tests:" << std::endl << std::endl;
    Cell_Update_Contents1();
    std::cout << std::endl;

    std::cout << "CELL: Equality operator Tests:" << std::endl << std::endl;
    Cell_Equality_Operator1();
    std::cout << std::endl;

}


/*******************************************************
 *
 * Cell tests
 *
 *******************************************************/


void Cell_WhenEmptyRevert_ReturnsNoContents() {

    cell cell1("1","A6");

    cell1.revert();

	bool isPassing = cell1.getContents() == "";

	if(isPassing) {
		std::cout << "---Pass--- Cell_WhenEmptyRevert_ReturnsNoContents" << std::endl;
	}
	else {
		std::cout << "---Fail--- Cell_WhenEmptyRevert_ReturnsNoContents" << std::endl;
	}
}

void Cell_AfterUpdate_ReturnsNoContents() {

    cell cell6("1","A6");
    cell6.revert();
	std::string result = cell6.getContents();

	if(result == "") {
		std::cout << "---Pass--- Cell_AfterUpdate_ReturnsNoContents" << std::endl;
	}
	else {
		std::cout << "---Fail--- Cell_AfterUpdate_ReturnsNoContents" << std::endl;
	}
}

void Cell_WithContents_CanRevert() {

    cell cell1("1","A3");
	if(cell1.canRevert()) {
		std::cout << "---Pass--- Cell_WithContents_CanRevert" << std::endl;
	}
	else {
		std::cout << "---Fail--- Cell_WithContents_CanRevert" << std::endl;
	}
}

void Cell_WithoutContents_CanRevert() {

    cell cell1("","A3");


	if(cell1.canRevert()) {
		std::cout << "---Pass--- Cell_WithoutContents_CanRevert" << std::endl;
	}
	else {
		std::cout << "---Fail--- Cell_WithoutContents_CanRevert" << std::endl;
	}
}

void Cell_AfterUpdate_CanRevert() {

    cell cell1("", "A3");
    cell1.updateContents("2");

    if (cell1.canRevert()) {
        std::cout << "---Pass--- Cell_AfterUpdate_CanRevert" << std::endl;
    } else {
        std::cout << "---Fail--- Cell_AfterUpdate_CanRevert" << std::endl;
    }
}


////////////CELL: GetContents/////////////////
void Cell_Get_Contents1() {
    cell cell1("1", "A1");
    bool isPassing = cell1.getContents() == "1";

    if(isPassing)
    {
        std::cout << "---Pass--- Cell_Get_Contents1" << std::endl;
    }
    else
    {
        std::cout << "---Fail--- Cell_Get_Contents1" << std::endl;
    }
}

////////////CELL: UpdateContents/////////////////

void Cell_Update_Contents1() {
    cell cell2("1", "A2");
    cell2.updateContents("2");
    bool isPassing = cell2.getContents() == "2";

    if(isPassing)
    {
        std::cout << "---Pass--- Cell_Update_Contents1" << std::endl;
    }
    else
    {
        std::cout << "---Fail--- Cell_Update_Contents1" << std::endl;
    }
}

////////////CELL: Equality operator/////////////////


void Cell_Equality_Operator1() {
    cell cell1("1+1", "A1");
    cell cell2("1+1", "A1");
    bool isPassing = cell1 == cell2;

    if(isPassing)
    {
        std::cout << "---Pass--- Cell_Equality_Operator1" << std::endl;
    }
    else
    {
        std::cout << "---Fail--- Cell_Equality_Operator1" << std::endl;
    }
}

