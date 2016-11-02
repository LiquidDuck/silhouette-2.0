#include <iostream>
#include <fstream>

class SilhouetteCoefficient
{

public:
	SilhouetteCoefficient(Parameters _parameters, int* _clusteringResults, double* _objects)
	{
		parameters = _parameters;
		clusteringResults = _clusteringResults;
		objects = _objects;

		centroids = new double[parameters.countOfClusters*parameters.countOfDimensions];
		cSizes = new int[parameters.countOfClusters];
		nearestClusters = new int[parameters.countOfObjects];
		distances = new double[parameters.countOfObjects*parameters.countOfObjects];
	}
	~SilhouetteCoefficient()
	{

	}

	double calculateSilhouette()
	{
		calculateDistanceMatrix();
		calculateClustersSizes();
		calculateCentroids();
		calculateNearestClusters();

		double S = 0;

		for (int o1 = 0; o1 < parameters.countOfObjects; o1++)
		{
			int currentCluster = clusteringResults[o1];
			double localAvg = 0, interclusterAvg = 0;

			for (int o2 = 0; o2 < parameters.countOfObjects; o2++)
			{
				if (clusteringResults[o2] == currentCluster)
				{
					localAvg += distances[o1*parameters.countOfObjects + o2];
				}
				else
					if (clusteringResults[o2] == nearestClusters[o1])
					{
						interclusterAvg += distances[o1*parameters.countOfObjects + o2];
					}
			}
			
			localAvg /= cSizes[currentCluster] > 1 ? (cSizes[currentCluster] - 1) : 1;
			interclusterAvg /= cSizes[nearestClusters[o1]];
			double max = localAvg > interclusterAvg ? localAvg : interclusterAvg;
			S += (interclusterAvg - localAvg) / max;
		}

		return S / parameters.countOfObjects;
	}

private:
	Parameters parameters;
	int* cSizes;
	int* clusteringResults;
	int* nearestClusters;
	double* centroids;
	double* objects;
	double* distances;


	double evklidDistance(double* array1, int object1, double* array2, int object2)
	{
		double sum = 0;

		for (int d = 0; d < parameters.countOfDimensions; d++)
		{
			sum += pow(array1[object1 + d] - array2[object2 + d], 2);
		}

		return sqrt(sum);
	}
	void calculateClustersSizes()
	{
		for (int c = 0; c < parameters.countOfClusters; c++)
		{
			int objectsInCluster = 0;
			for (int o = 0; o < parameters.countOfObjects; o++)
			{
				if (clusteringResults[o] == c)
				{
					objectsInCluster++;
				}
			}
			cSizes[c] = objectsInCluster;
		}
	}
	void calculateCentroids()
	{
		for (int i = 0; i < parameters.countOfClusters*parameters.countOfDimensions; i++)
		{
			centroids[i] = 0;
		}

		for (int c = 0; c < parameters.countOfClusters; c++)
		{
			for (int o = 0; o < parameters.countOfObjects; o++)
			{
				if (c == clusteringResults[o])
				{
					for (int d = 0; d < parameters.countOfDimensions; d++)
					{
						centroids[c*parameters.countOfDimensions + d] += objects[o*parameters.countOfDimensions + d] / cSizes[c];
					}
				}
			}
		}
	}
	void calculateNearestClusters()
	{
		for (int o1 = 0; o1 < parameters.countOfObjects; o1++)
		{
			int currentCluster = clusteringResults[o1];
			int nearestCluster;
			int minAvg = -1;

			for (int c = 0; c < parameters.countOfClusters; c++)
			{
				if (currentCluster != c)
				{
					int interclusterAvg = 0;

					for (int o2 = 0; o2 < parameters.countOfObjects; o2++)
					{
						if (clusteringResults[o2] == c)
						{
							interclusterAvg += distances[o1*parameters.countOfObjects + o2];
						}
					}
					interclusterAvg /= cSizes[c];

					if (interclusterAvg < minAvg || minAvg < 0)
					{
						minAvg = interclusterAvg;
						nearestCluster = c;
					}
				}
			}
			nearestClusters[o1] = nearestCluster;
		}
	}
	void calculateDistanceMatrix()
	{
		for (int o1 = 0; o1 < parameters.countOfObjects; o1++)
		{
			for (int o2 = o1; o2 < parameters.countOfObjects; o2++)
			{
				distances[o1 * parameters.countOfObjects + o2] = evklidDistance(objects, o1*parameters.countOfDimensions, objects, o2*parameters.countOfDimensions);
				distances[o2 * parameters.countOfObjects + o1] = distances[o1 * parameters.countOfObjects + o2];
			}
		}
	}

	void writeArray(double* arr, int size, string name)
	{
		ofstream outfile(name + ".csv");

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < parameters.countOfDimensions; j++)
			{
				outfile << arr[parameters.countOfDimensions*i + j];
				if (j != parameters.countOfDimensions - 1)
				{
					outfile << ";";
				}
			}
			outfile << endl;
		}

		outfile.close();
	}
	void writeArray(int* arr, int size, string name)
	{
		ofstream outfile(name + ".csv");

		for (int i = 0; i < size; i++)
		{
			outfile << arr[i] << endl;
		}

		outfile.close();
	}
};