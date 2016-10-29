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
		nearestClusters = new int[parameters.countOfClusters];
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
					if (clusteringResults[o2] == nearestClusters[currentCluster])
					{
						interclusterAvg += distances[o1*parameters.countOfObjects + o2];
					}
			}
			
			localAvg /= (cSizes[currentCluster] - 1);
			interclusterAvg /= cSizes[nearestClusters[currentCluster]];
			double max = localAvg > interclusterAvg ? localAvg : interclusterAvg;
			S += (interclusterAvg - localAvg) / max;
			/*cout << "for " << o + 1 << " objects S is " << (iAvg - lAvg) / max << endl;*/
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
		for (int c = 0; c < parameters.countOfClusters; c++)
		{
			int minNum = -1;
			double minDis = -1;

			for (int n = 0; n < parameters.countOfClusters; n++)
			{
				if (c != n)
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