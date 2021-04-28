#include <string>
#include "cell.h"

cell::cell() {
}

cell::cell(std::string contents, std::string cellName) {
    this->contents = contents;
    this->cellName = cellName;
}

void cell::updateContents(std::string contents) {
    //push the old contents so we may revert
    revertStack.push(this->contents);

    this->contents = contents;
}

bool cell::operator==(const cell& c) const {
    return ((this->contents == c.contents) && (this->cellName == c.cellName));
}

std::string cell::getContents() const {
    return this->contents;
}

std::string cell::revert() {
    std::string oldContents = revertStack.top();
    revertStack.pop();

    this->contents = oldContents;
    return contents;
}

bool cell::canRevert() {
    return !revertStack.empty();
}

