#include "mytest.h"
#include <iostream>
#include <string>

int main(int argc, char **argv){

MyTest test;
MyTest::Debug_Level level;
MyTest::ALGORITHM uhh = MyTest::ALGORITHM::add;

std::cout << " arg = " << std::string(argv[2]) << " " << std::endl;

if(std::string(argv[2]) == "full") level = MyTest::Debug_Level::FULL;
if(std::string(argv[2]) == "partial")level = MyTest::Debug_Level::PARTIAL;
if(std::string(argv[2]) == "brief") level = MyTest::Debug_Level::BRIEF;
if(std::string(argv[2]) == "none") level = MyTest::Debug_Level::NONE;

test.set_debug(level);

test.set_x(294);
test.set_y(3);
test.compute(uhh);

return 0;
}
