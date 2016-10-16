#include <fstream>
#include <string>
#include <sstream>

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
		infile.close();
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
		infile.close();
	}
	void writeResult(double S)
	{
		ofstream outfile(parameters.outputPath);
		outfile << S;
		outfile.close();
	}
};
