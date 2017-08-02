#include <iostream>
#include <memory>

class A
{
public:
	A() { std::cout << "Construct\n"; }
	virtual ~A() { std::cout << "Destruct\n"; }
};

void print(A* a)
{
}

void change_owner(std::unique_ptr<A> && a_owned)
{
}

int main(int argc, char **argv)
{
	auto a = std::make_unique<A>();
    print(a.get());
    change_owner(std::move(a));
	return 0;
}

