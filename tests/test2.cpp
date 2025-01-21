#include <iostream>

class Base
{
public:
    Base(int a)
    {
        std::cout << "Base constructor\n";
    }
    virtual ~Base()
    {
        std::cout << "Base destructor\n";
    }
};

class Derived : public Base {};

int main()
{




    return 0;
}