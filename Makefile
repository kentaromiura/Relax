test:
	clang++ -std=c++14 Specs/behaviour.cpp -o Build/behaviour && ./Build/behaviour
test11:
	clang++ -std=c++11 Specs/behaviour.cpp -o Build/behaviour && ./Build/behaviour
