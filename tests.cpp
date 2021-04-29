#include <iostream>
#include <cassert> 

namespace test
{
	int main() {

    	static_assert(true, "it was false");
		return 0;
	}
}
