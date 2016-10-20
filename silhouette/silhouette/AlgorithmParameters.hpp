#include <string>
#include<iostream>

using namespace std;

struct Parameters
{
	int countOfClusters;
	int countOfObjects;
	int countOfDimensions;
	string inputPath;
	string clusteringResultsPath;
	string outputPath;
};

class AlgorithmParameters
{
public:
	AlgorithmParameters(int argc, char *argv[])
	{
		parse(argc, argv);
	}
	Parameters GetParameters()
	{
		return parameters;
	}

private:
	Parameters parameters;

	void parse(int argc, char *argv[])
	{
		parameters.clusteringResultsPath = "results.csv";
		parameters.countOfClusters = 3;
		parameters.countOfDimensions = 3;
		parameters.countOfObjects = 13;
		parameters.inputPath = "input.csv";
		parameters.outputPath = "output.csv";

		for (int i = 1; i < argc; i += 2)
		{
			if ((string)argv[i] == "-i")
			{
				parameters.inputPath = argv[i + 1];
			}
			if ((string)argv[i] == "-r")
			{
				parameters.clusteringResultsPath = argv[i + 1];
			}
			if ((string)argv[i] == "-o")
			{
				parameters.outputPath = argv[i + 1];
			}

			if ((string)argv[i] == "-c")
			{
				parameters.countOfClusters = atoi(argv[i + 1]);
			}
			if ((string)argv[i] == "-n")
			{
				parameters.countOfObjects = atoi(argv[i + 1]);
			}
			if ((string)argv[i] == "-d")
			{
				parameters.countOfDimensions = atoi(argv[i + 1]);
			}
		}
	}
};