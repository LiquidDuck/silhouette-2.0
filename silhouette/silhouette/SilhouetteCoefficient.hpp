class SilhouetteCoefficient
{

public:
	SilhouetteCoefficient(Parameters _parameters, int* clusteringResults, double* objects)
	{
		parameters = _parameters; 
		centroids = new double[parameters.countOfClusters*parameters.countOfDimensions];
		clusters = new double[parameters.countOfObjects*parameters.countOfDimensions];
		cSizes = new int[parameters.countOfClusters];

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
		double S = 0; 
		double* localAvg = new double[parameters.countOfObjects]; 
		double* interclusterAvg = new double[parameters.countOfObjects]; 
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