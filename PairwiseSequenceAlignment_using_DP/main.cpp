/* Pairwise Sequence Alignment Using Dynamic Programming
Bio-medical Information 101 hw1.
12121458 ±èÁÖ¿ø
*/

#include<iostream>
#include<string>
#include<fstream>
#include"PSA_DP.h"
using namespace std;




int main()
{
	ifstream inputFile;
	ofstream outputFile;

	string inputFileName, outputFileName;
	

	// Input File Name : only type filename without ".txt"
	cout << "Enter Input File Name : ";
	cin >> inputFileName;

	inputFileName = "./" + inputFileName;
	outputFileName = inputFileName + "_output.txt";
	inputFileName += ".txt";

	inputFile.open(inputFileName);
	outputFile.open(outputFileName);

	PSA_DP a;

	// input function.
	a.setData(inputFile,outputFile);
	
	if (a.local == 1)
	{
		a.localAlignment();
	}
	else if (a.local == 0)
	{
		a.globalAlignment();
	}

	//a.getData();

	//a.printMatrix();
	cout << "score : " << a.alignmentScore<< endl;
	a.result();
}