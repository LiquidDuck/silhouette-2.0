#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

//template <typename Type>
//Type* allocateAlign(unsigned long long count, int length = 64) {
//	return static_cast<Type*>(_mm_malloc(count * sizeof(Type), length));
//}

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
		for (int i = 1; i < argc; i += 2)
		{
			if ((string)argv[i] == "-i")
			{
				parameters.inputPath = argv[i + 1];
			}
			else
				if ((string)argv[i] == "-r")
				{
					parameters.clusteringResultsPath = argv[i + 1];
				}
				else
					if ((string)argv[i] == "-o")
					{
						parameters.outputPath = argv[i + 1];
					}
					else
					{
						if ((string)argv[i] == "-c")
							parameters.countOfClusters = atoi(argv[i + 1]);
						else
						{
							if ((string)argv[i] == "-n")
							{
								parameters.countOfObjects = atoi(argv[i + 1]);
							}
							else
							{
								if ((string)argv[i] == "-d")
								{
									parameters.countOfDimensions = atoi(argv[i + 1]);
								}
							}
						}
					}
		}
	}
};

class SilhouetteCoefficient
{

public:
	SilhouetteCoefficient(Parameters _parameters, int* clusteringResults, double* objects)
	{ 
		parameters = _parameters; 
		centroids = new double[parameters.countOfClusters*parameters.countOfDimensions];
		clusters = new double[parameters.countOfObjects*parameters.countOfDimensions];
		int* cSizes = new int[parameters.countOfClusters];
		double* cluster =  new double[parameters.countOfObjects];
		double* centroid = new double[parameters.countOfDimensions];
		
		for (int c = 0; c < parameters.countOfClusters; c++)
		{
			int objectsInCluster = 0;
			
			for (int d = 0; d < parameters.countOfDimensions; d++)
			{
				centroid[d] = 0;
			}
			
			for (int o = 0; o < parameters.countOfObjects; o++)
			{
				if (clusteringResults[o] == c)
				{
					for (int d = 0; d < parameters.countOfDimensions; d++)
					{
						cluster[objectsInCluster*parameters.countOfDimensions + d] = objects[o*parameters.countOfDimensions + d];
						centroid[d] += objects[o*parameters.countOfDimensions + d];
					}
					objectsInCluster++;
				}
			}
			
			if (objectsInCluster > 0)
			{
				for (int d = 0; d < parameters.countOfDimensions; d++)
				{
					centroids[c*parameters.countOfDimensions + d] = centroid[d] / objectsInCluster;
				}
			}

			int pcSizes = 0;
			for (int pc = 0; pc < c; pc++)
			{
				pcSizes += cSizes[pc] * parameters.countOfDimensions;
			}

			for (int o = 0; o < objectsInCluster; o++)
			{
				for (int d = 0; d < parameters.countOfDimensions; d++)
				{
					clusters[pcSizes + o*parameters.countOfDimensions + d] = cluster[o*parameters.countOfDimensions + d];
				}
			}

			cSizes[c] = objectsInCluster;
		}
	}
	~SilhouetteCoefficient()
	{

	}

	double calculateSilhouette()
	{
		int o = -1;
		double S = 0; cout << "new local memory..." << endl;
		double* localAvg = new double[parameters.countOfObjects]; cout << "new intercluster memory..." << endl;
		double* interclusterAvg = new double[parameters.countOfObjects];
		int* nearestClusters = calculateNearestClusters();
		
		for (int c = 0; c < parameters.countOfClusters; c++)
		{
			cout << "starting with cluster " << c << "..." << endl;
			int pcSizes = 0;
			for (int pc = 0; pc < c; pc++)
			{
				pcSizes += cSizes[pc] * parameters.countOfDimensions;
			}

			int ncpSizes = 0;
			for (int pc = 0; pc < nearestClusters[c]; pc++)
			{
				ncpSizes += cSizes[pc] * parameters.countOfDimensions;
			}

			for (int o1 = 0; o1 < cSizes[c]; o1++)
			{
				double lAvg = 0, iAvg = 0;

				for (int o2 = 0; o2 < cSizes[c]; o2++)
				{
					lAvg += evklidDistance(clusters, pcSizes + o1*parameters.countOfDimensions, clusters, pcSizes + o2*parameters.countOfDimensions);
				}

				for (int o2 = 0; o2 < cSizes[nearestClusters[c]]; o2++)
				{
					iAvg += evklidDistance(clusters, pcSizes + o1*parameters.countOfDimensions, clusters, ncpSizes + o2*parameters.countOfDimensions);
				}

				o++;
				localAvg[o] = lAvg / (cSizes[c] - 1);
				interclusterAvg[o] = iAvg / cSizes[nearestClusters[c]];
			}
		}

		for (int o = 0; o < parameters.countOfObjects; o++)
		{
			double max = localAvg[o] > interclusterAvg[o] ? localAvg[o] : interclusterAvg[o];
			S += (interclusterAvg[o] - localAvg[o]) / max;
		}

		return S / parameters.countOfObjects;
	}

private:
	Parameters parameters;
	double* centroids;
	double* clusters;
	int* cSizes;
	
	double evklidDistance(double* array1, int object1, double* array2, int object2)
	{
		double sum = 0;

		for (int d = 0; d < parameters.countOfDimensions; d++)
		{
			sum += pow(array1[object1 + d] - array2[object2 + d], 2);
		}

		return sqrt(sum);
	}
	int* calculateNearestClusters()
	{
		int* nearestClusters = new int[parameters.countOfClusters];

		for (int c = 0; c < parameters.countOfClusters; c++)
		{
			int minNum = -1;
			double minDis = -1;

			for (int n = 0; n < parameters.countOfClusters; n++)
			{
				if (c != n && cSizes[n] > 0)
				{
					double dis = evklidDistance(centroids, c*parameters.countOfDimensions, centroids, n*parameters.countOfDimensions);

					if (dis < minDis || minNum < 0)
					{
						minDis = dis;
						minNum = n;
					}
				}
			}

			nearestClusters[c] = minNum;
		}

		return nearestClusters;
	}
};

class Executor
{
public:
	Executor(Parameters _parameters)
	{
		parameters = _parameters;
	}
	~Executor()
	{

	}

	void StartTesting()
	{
		double* objects = new double[parameters.countOfObjects*parameters.countOfDimensions];
		int* clusteringResults = new int[parameters.countOfObjects];
		
		readObjects(objects);
		readClustingResults(clusteringResults);
		
		SilhouetteCoefficient silhouette(parameters, clusteringResults, objects); 
		writeResult(silhouette.calculateSilhouette());
	}

private:
	Parameters parameters;

	void readObjects(double* objects)
	{
		ifstream infile(parameters.inputPath);
		int i = -1;

		while (infile)
		{
			string s;
			if (!getline(infile, s)) break;

			istringstream ss(s);
			double* record = new double[parameters.countOfDimensions];
			int d = -1;

			while (ss)
			{
				string s;
				double p;

				if (!getline(ss, s, ';'))
				{
					break;
				}

				istringstream iss(s);
				iss >> p;

				d++; 
				record[d] = p;
			}
			
			for (int j = 0; j < parameters.countOfDimensions; j++)
			{
				i++;
				objects[i] = record[j];
			}
		}
	}
	void readClustingResults(int* clusteringResults)
	{
		ifstream infile(parameters.clusteringResultsPath);

		string s;
		double d;
		int o = -1;

		getline(infile, s);
		istringstream ss(s);
		ss >> d;

		if (ss.rdbuf()->in_avail() != 0)
		{
			char semicolon;
			double maxRes = d;
			int num = 0, maxNum = 0;

			while (ss)
			{
				num++;
				ss >> semicolon;
				ss >> d;
				if (d > maxRes)
				{
					maxRes = d;
					maxNum = num;
				}
			}

			o++;
			clusteringResults[o] = maxNum;

			while (infile)
			{
				getline(infile, s);
				istringstream iss(s);

				maxRes = -1;
				num = -1;
				maxNum = 0;

				while (iss)
				{
					iss >> d;
					iss >> semicolon;
					num++;
					if (d > maxRes)
					{
						maxRes = d;
						maxNum = num;
					}
				}

				o++;
				clusteringResults[o] = maxNum;
			}
		}
		else
		{
			o++;
			clusteringResults[o] = d - 1;

			while (infile)
			{
				getline(infile, s);
				istringstream iss(s);
				iss >> d;

				o++;
				clusteringResults[o] = d - 1;
			}
		}
	}
	void writeResult(double S)
	{
		ofstream outfile(parameters.outputPath);
		outfile << S;
	}
};

void main(int argc, char *argv[])
{
	try 
	{
		AlgorithmParameters algorithmParameters(argc, argv);
		Executor executor(algorithmParameters.GetParameters());
		executor.StartTesting();
	}
	catch (...)
	{
		cout << "An error occured." << endl;
	}

	system("pause");
}