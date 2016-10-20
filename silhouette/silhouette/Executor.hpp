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

		cout << "before fuzzy" << endl;
		/*if (isFuzzy(file))
		{
			cout << "fuzzy" << endl;*/
			readFuzzyResults(clusteringResults/*, file)*/);
		/*}
		else
		{
			cout << "not fuzzy" << endl;
			exit(1);
		}*/
		readObjects(objects); 
		/*writeArray(objects);*/
		/*writeArray(objects, parameters.countOfObjects);*/
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
	void readFuzzyResults(int* clusteringResults/*, ifstream &infile*/)
	{
		ifstream infile(parameters.clusteringResultsPath);
		infile.seekg(0, infile.beg);

		string s;
		double d;
		int o = -1;
		char semicolon;
		cout << "pre-while" << endl;
		cout << infile.good() << endl;
		while (infile)
		{
			cout << "yes" << endl;
			getline(infile, s);
			istringstream iss(s);

			double maxRes = -1;
			int num = -1;
			int maxNum = 0;

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
			cout << maxNum << endl;
			clusteringResults[o] = maxNum;
		}
		/*else
		{
			cout << "chet" << endl;
			o++;
			istringstream ss(s);
			ss >> d;
			clusteringResults[o] = d - 1;

			while (infile)
			{
				getline(infile, s);
				istringstream iss(s);
				iss >> d;

				o++;
				clusteringResults[o] = d - 1;
			}
		}*/
		infile.close();
	}
	void writeResult(double S)
	{
		ofstream outfile(parameters.outputPath);
		outfile << S;
		outfile.close();
	}
	bool isFuzzy(ifstream &file)
	{
		/*ifstream infile(parameters.clusteringResultsPath);*/

		string s;
		getline(file, s);
		file.close();
		/*infile.clear();*/

		return s.find(";");
	}
};
