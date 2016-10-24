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
		clusters = new double[parameters.countOfObjects*parameters.countOfDimensions];
		cSizes = new int[parameters.countOfClusters];
		localAvg = new double[parameters.countOfObjects];
		interclusterAvg = new double[parameters.countOfObjects];
	}
	~SilhouetteCoefficient()
	{

	}

	double calculateSilhouette()
	{
		orderClusters();
		calculateCentroids();

		int o = -1;
		double S = 0;
		int* nearestClusters = calculateNearestClusters();
		
		for (int c = 0; c < parameters.countOfClusters; c++)
		{
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
	double* localAvg;
	double* interclusterAvg;
	double* objects;
	int* clusteringResults;

	
	double evklidDistance(double* array1, int object1, double* array2, int object2)
	{
		double sum = 0;

		for (int d = 0; d < parameters.countOfDimensions; d++)
		{
			sum += pow(array1[object1 + d] - array2[object2 + d], 2);
		}

		return sqrt(sum);
	}
	void orderClusters()
	{
		int elementIndex = 0;

		for (int c = 0; c < parameters.countOfClusters; c++)
		{
			int objectsInCluster = 0;

			for (int o = 0; o < parameters.countOfObjects; o++)
			{
				if (clusteringResults[o] == c)
				{
					for (int d = 0; d < parameters.countOfDimensions; d++)
					{
						clusters[elementIndex] = objects[o*parameters.countOfDimensions + d];
						elementIndex++;
					}
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
			int pcSizes = 0;
			for (int pc = 0; pc < c; pc++)
			{
				pcSizes += cSizes[pc] * parameters.countOfDimensions;
			}

			for (int o = 0; o < cSizes[c]; o++)
			{
				for (int d = 0; d < parameters.countOfDimensions; d++)
				{
					if (cSizes[c] > 0)
					{
						centroids[c*parameters.countOfDimensions + d] += clusters[pcSizes + o*parameters.countOfDimensions + d] / cSizes[c];
					}
					else
					{
						centroids[c*parameters.countOfDimensions + d] = -1;
					}
				}
			}
		}
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