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

void Cell_WhenEmptyRevert_ReturnsNoContets();
void Cell_AfterUpdate_ReturnsNoContets();
void Cell_WithContents_CanRevert();
void Cell_WithoutContents_CanRevert();
void Cell_AfterUpdate_CanRevert();

int main()
{
    Cell_WhenEmptyRevert_ReturnsNoContets();
    Cell_AfterUpdate_ReturnsNoContets();
    Cell_WithContents_CanRevert();
    Cell_WithoutContents_CanRevert();
    Cell_AfterUpdate_CanRevert();
}

/*******************************************************
 *
 * Spreadsheet tests
 *
 *******************************************************/

void Cell_WhenEmptyRevert_ReturnsNoContets() {

    cell cell1("1","A6");
    cell1.revert();
	std::string result = cell1.getContents();

	if(result == "") {
		std::cout << "---Pass--- Cell_WhenEmptyRevert_ReturnsNoContets" << std::endl;
	}
	else {
		std::cout << "---Fail--- Cell_WhenEmptyRevert_ReturnsNoContets" << std::endl;
	}
}

void Cell_AfterUpdate_ReturnsNoContets() {

    cell cell6("1","A6");
    cell6.revert();
	std::string result = cell6.getContents();

	if(result == "") {
		std::cout << "---Pass--- Cell_AfterUpdate_ReturnsNoContets" << std::endl;
	}
	else {
		std::cout << "---Fail--- Cell_AfterUpdate_ReturnsNoContets" << std::endl;
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

    cell cell1("","A3");
    cell1.updateContents("2");

	if(cell1.canRevert()) {
		std::cout << "---Pass--- Cell_AfterUpdate_CanRevert" << std::endl;
	}
	else {
		std::cout << "---Fail--- Cell_AfterUpdate_CanRevert" << std::endl;
	}
}


