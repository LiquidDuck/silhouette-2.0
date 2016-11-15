#include "AlgorithmParameters.hpp"
#include "SilhouetteCoefficient.hpp"
#include "Executor.hpp"
#include <iostream>

//template <typename Type>
//Type* allocateAlign(unsigned long long count, int length = 64) {
//	return static_cast<Type*>(_mm_malloc(count * sizeof(Type), length));
//}

int main(int argc, char *argv[])
{
	AlgorithmParameters algorithmParameters(argc, argv); 
	Executor executor(algorithmParameters.GetParameters());
	executor.StartTesting();
	
	return 0;
}
