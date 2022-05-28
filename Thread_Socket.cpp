#include <iostream>

int start_socket();
int foo1();

void main()
{
    start_socket();
}

int foo1()
{
    if (0)
        return 1;
    else
        std::cout << "###";
}
