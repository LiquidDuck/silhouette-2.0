#include <fstream>
#include <string>
#include <sstream>

#include <iostream>

using namespace std;

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

		if (isFuzzy())
		{
			cout << "fuzzy" << endl;
			readFuzzyResults(clusteringResults);
			writeArray(clusteringResults, parameters.countOfObjects);
			readObjects(objects);
			cout << "obje" << endl;
			writeArray(objects, parameters.countOfObjects);
		}
		else
		{
			cout << "not fuzzy" << endl;
			exit(1);
		}
		SilhouetteCoefficient silhouette(parameters, clusteringResults, objects); 
		writeResult(silhouette.calculateSilhouette()); 
	}

private:
	Parameters parameters;

	void writeArray(double* arr, int size)
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < parameters.countOfDimensions; j++)
			{
				cout << arr[parameters.countOfDimensions*i + j] << " ";
			}
			cout << endl;
		}
	}

	void writeArray(int* arr, int size)
	{
		for (int i = 0; i < size; i++)
		{
			cout << arr[i] << endl;			
		}
	}

	void readObjects(double* objects)
	{
		ifstream infile(parameters.clusteringResultsPath);
		infile.seekg(0, infile.beg);

		int rowIndex = 0;
		long long elementN = 0;
		while (infile && rowIndex<parameters.countOfObjects)
		{
			string rowString;
			if (!getline(infile, rowString)) break;

			istringstream ss(rowString);
			int coordIndex = 0;
			while (ss && coordIndex<parameters.countOfClusters)
			{

				string coordString;
				double p;
				if (!getline(ss, coordString, ';')) break;

				istringstream iss(coordString);
				if (iss >> p)
				{
					objects[elementN] = p;
					coordIndex++;
					elementN++;
				}
			}
			rowIndex++;
		}
		infile.close();
	}
	void readFuzzyResults(int* clusteringResults)
	{
		ifstream infile(parameters.clusteringResultsPath);
		infile.seekg(0, infile.beg);
		
		int rowIndex = 0;
		while (infile && rowIndex<parameters.countOfObjects)
		{
			string rowString;
			if (!getline(infile, rowString)) break;

			istringstream ss(rowString);

			int currentCluster = 0;
			clusteringResults[rowIndex] = 0;
			double maxProbability = 0;

			while (ss && currentCluster<parameters.countOfClusters)
			{

				string coordString;
				double p;
				if (!getline(ss, coordString, ';')) break;

				istringstream iss(coordString);
				if (iss >> p)
				{
					if (p > maxProbability) { maxProbability = p; clusteringResults[rowIndex] = currentCluster; }
					currentCluster++;
				}
			}
			rowIndex++;
		}
		infile.close();
	}
	void writeResult(double S)
	{
		ofstream outfile(parameters.outputPath);
		outfile << S;
		outfile.close();
	}

	bool isFuzzy()
	{
		ifstream infile(parameters.clusteringResultsPath);
		string s;
		getline(infile, s);
		cout << parameters.clusteringResultsPath <<"__"<<s << endl;
		infile.close();
		return s.find(";");
	}
};
