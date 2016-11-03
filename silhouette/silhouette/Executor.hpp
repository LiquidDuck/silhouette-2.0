#include <fstream>
#include <string>
#include <sstream>
#include <omp.h>
#include <iostream>
#include <ctime>

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

		readObjects(objects);

		if (isFuzzy())
		{
			readFuzzyResults(clusteringResults);
		}
		else
		{
			readClearResults(clusteringResults);
		}

		SilhouetteCoefficient silhouette(parameters, clusteringResults, objects); 
		
		omp_set_num_threads(parameters.countOfThreads);
		double time0 = omp_get_wtime();
		writeResult(silhouette.calculateSilhouette()); 
		double time = omp_get_wtime() - time0;

		writeLog(time);
	}

private:
	Parameters parameters;

	/*void writeArray(double* arr, int size)
	{
		ofstream outfile("readObjects.csv");

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
	}
	bool sameFiles(string path1, string path2)
	{
		ifstream file1(path1);
		ifstream file2(path2);
		string s1, s2;
		
		while (!file1.eof())
		{
			if (!getline(file1, s1) || !getline(file2, s2))
			{
				cout << c << endl;
				file1.close();
				file2.close();
				return false;
			}
			if (s1 != s2)
			{
				cout << c << endl;
				file1.close();
				file2.close();
				return false;
			}
		}
		file1.close();
		file2.close();
		return true;
	}
	void writeArray(int* arr, int size)
	{
		ofstream outfile("readResults.csv");

		for (int i = 0; i < size; i++)
		{
			outfile << arr[i] << endl;			
		}
	}*/

	void readObjects(double* objects)
	{
		ifstream infile(parameters.inputPath);

		int rowIndex = 0;
		long long elementN = 0;
		while (!infile.eof())
		{
			string rowString;
			if (!getline(infile, rowString)) break;

			istringstream ss(rowString);
			int coordIndex = 0;
			while (ss && coordIndex < parameters.countOfClusters)
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
		
		int rowIndex = 0;
		while (!infile.eof())
		{
			string rowString;
			if (!getline(infile, rowString)) break;

			istringstream ss(rowString);

			int currentCluster = 0;
			double maxProbability = 0;

			while (ss && currentCluster < parameters.countOfClusters)
			{

				string coordString;
				double p;
				if (!getline(ss, coordString, ';')) break;

				istringstream iss(coordString);
				if (iss >> p)
				{
					if (p > maxProbability)
					{
						maxProbability = p;
						clusteringResults[rowIndex] = currentCluster;
					}
					currentCluster++;
				}
			}
			rowIndex++;
		}
		infile.close();
	}
	void readClearResults(int* clusteringResults)
	{
		ifstream infile(parameters.clusteringResultsPath);

		int rowIndex = 0;
		while (!infile.eof())
		{
			string rowString;
			if (!getline(infile, rowString)) break;

			istringstream ss(rowString);
			int cluster;

			if (ss >> cluster)
			{
				clusteringResults[rowIndex] = cluster;
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
		infile.close();
		return s.find(";") != string::npos;
	}

	void writeLog(double runtime)
	{
		ofstream log("log.csv", ios::app);

		log << GetDate() << endl;
		log << parameters.countOfObjects<<endl;
		log << parameters.countOfClusters << endl;
		log << parameters.countOfDimensions << endl;
		log << parameters.countOfThreads << endl;
		log << runtime << endl;
		log << "===================" << endl << endl;

		log.close();
	}
	inline string GetDate()
	{
		/*
		time_t rawtime;
		struct tm timeinfo;
		char buffer[80];
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		strftime(buffer, 80, "%d-%m-%Y;%H:%M:%S", &timeinfo);
		std::string str(buffer);
		*/
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%d-%m-%Y;%H:%M:%S", timeinfo);
		return  buffer;
	}
};
