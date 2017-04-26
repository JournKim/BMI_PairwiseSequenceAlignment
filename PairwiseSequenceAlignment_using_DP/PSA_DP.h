#pragma once
#include<string>
#include<fstream>
#include<vector>
#include<deque>
#include<stack>
#include<iostream>
using namespace std;

struct node
{
	int row, col;
};

// class for store pointer information
class cell
{
public:
	vector<node> v;

	void push(node n)
	{
		v.push_back(n);
	}
};

class PSA_DP
{
public:
	// string before alignment
	char* str1;
	char* str2;

	// 1 when local
	int local;

	// [0] gap open str1, [1] gap expand str1, [2] gap open str2, [3] gap expand str2
	float penalty[4];

	// number of symbol for each string
	int symbol1count;
	int symbol2count;

	// alignment score
	float alignmentScore;

	// aligned string
	char align_str1[512];
	char align_str2[512];


	// File to write
	ofstream* outFile;
	
	// vector for tracking.
	vector<node> v;
	
	// symbols for string
	string symbol1, symbol2;

	// match matrix
	vector<vector<float>> matchMatrix;

	// store maximum score of each cell
	vector<vector<float>> dp;

	// store pointers of each cell
	vector<vector<cell>> pointerArray;
	
	// Set Data for Alignment
	void setData(ifstream& inputFile, ofstream& _o)
	{
		string _str1,_str2; // intput String
		inputFile >> _str1 >> _str2 >> local
			>> penalty[0] >> penalty[1] >> penalty[2] >> penalty[3];

		inputFile >> symbol1count;
		inputFile >> symbol1;

		inputFile >> symbol2count;
		inputFile >> symbol2;

		matchMatrix.assign(symbol1count, vector<float>(symbol2count, 0));

		int ir, ic;
		char cr, cc;
		for (int i = 0; i < symbol1count; i++)
		{
			for (int j = 0; j < symbol2count; j++)
			{
				inputFile >> ir >> ic >> cr >> cc >> matchMatrix[i][j];
			}
		}

		str1 = new char[_str1.length() + 1];
		strcpy(str1, _str1.c_str());
		str2 = new char[_str2.length() + 1];
		strcpy(str2, _str2.c_str());

		dp.assign(_str1.length()+1, vector<float>(_str2.length()+1, 0));
		pointerArray.assign(_str1.length() + 1, vector<cell>(_str2.length() + 1));
		outFile = &_o;
	}

	void getData()
	{
		cout << "string1 : " << str1 << endl;
		cout << "string2 : " << str2 << endl;
		cout << (local ? "local" : "global") << endl;

		cout << "open1 : " << penalty[0] << ", exp1 : " << penalty[1] << endl;
		cout << "open2 : " << penalty[2] << ", exp2 : " << penalty[3] << endl;

		printMatrix();
		printDP();
	}

	// calculate gap penalty.
	float calcPenalty(int row, int col, int prow, int pcol)
	{
		// if str2 gap;
		if (row - prow > 1)
		{
			return penalty[2] + (penalty[3] * (row - prow - 1));
		}
		// if str1 gap;
		else if (col - pcol > 1)
		{
			return penalty[0] + (penalty[1] * (col - pcol - 1));
		}
		// no gap.
		else
			return 0;
	}

	// calculate score of a cell
	void scoring(int row, int col)
	{
		char a, b;
		a = str1[row - 1];
		b = str2[col - 1];

		// pointer for this cell.
		vector<node>& now = pointerArray[row][col].v;
		
		// match score for this cell
		float score = matchMatrix[symbol1.find(a)][symbol2.find(b)];
		float max = dp[row - 1][col - 1];

		float tmp = 0;

		// get max value among prev col.
		for (int i = 1; i < row-1; i++)
		{
			tmp = dp[i][col - 1] - calcPenalty(row, col, i, col - 1);
			
			if (max < tmp)
			{
				max = tmp;
			}
		}

		// get max value among prev row.
		for (int i = 1; i < col - 1; i++)
		{
			tmp = dp[row - 1][i] - calcPenalty(row, col, row - 1, i);

			if (max < tmp)
			{
				max = tmp;
			}
		}

		// push pointer nodes that equal to max
		if (max != 0)
		{
			for (int i = 1; i < col-1; i++)
			{
				if (max == dp[row - 1][i] - calcPenalty(row, col, row - 1, i))
				{
					now.push_back({ row - 1, i });
				}
			}

			for (int i = 1; i < row - 1; i++)
			{
				if (max == dp[i][col - 1] - calcPenalty(row, col, i, col - 1))
				{
					now.push_back({ i,col - 1 });
				}
			}
			if (max == dp[row - 1][col - 1])
			{
				now.push_back({ row - 1,col - 1 });
			}
		}
	
		
		dp[row][col] = max + score;
	}

	// global alignment.
	void globalAlignment()
	{
		int row = dp.size();
		int col = dp[0].size();


		// set 0th row and column.
		for (int i = 0; i < row; i++)
		{
			dp[i][0] = 0;
		}
		for (int i = 0; i < col; i++)
		{
			dp[0][i] = 0;
		}
		
		//scoring every cell
		for (int i = 1; i < row; i++)
		{
			for (int j = 1; j < col; j++)
			{
				scoring(i, j);
			}
		}

		// get max value among last row and last column.
		// end gap 처리 가능.
		float max = 0;
		int maxrow = row-1;
		int maxcol = col-1;
		for (int i = 1; i < row; i++)
		{
			if (dp[i][col-1] > max)
			{
				max = dp[i][col - 1];
			}
		}

		for (int i = 1; i < col; i++)
		{
			if (dp[row - 1][i] > max)
			{
				max = dp[row - 1][i];
			}
		}

		// vector for max nodes.
		vector<node> result;

		// push every node that have max value.
		if (max == dp[row - 1][col - 1])
			result.push_back({ row - 1,col - 1 });

		for (int i = 1; i < row-1; i++)
		{
			if (max == dp[i][col-1])
			{
				result.push_back({ i,col - 1 });
			}
		}

		for (int i = 1; i < col-1; i++)
		{
			if (dp[row - 1][i] == max)
			{
				result.push_back({ row - 1,i });
			}
		}

		alignmentScore = max;

		*outFile << "Maximum Srore : " << alignmentScore << endl;


		// print every aligned string.
		for (int i = 0; i < result.size(); i++)
		{
			dfs(result[i].row, result[i].col);
		}

	}

	// local alignment.
	void localAlignment()
	{
		float max = 0;

		// set 0th row and column
		for (int i = 0; i < dp.size(); i++)
		{
			dp[i][0] = 0;
		}
		for (int i = 0; i < dp[0].size(); i++)
		{
			dp[0][i] = 0;
		}


		// scoring every cell.
		for (int i = 1; i < dp.size(); i++)
		{
			for (int j = 1; j < dp[0].size(); j++)
			{
				scoring(i, j);

				// if score is negative, set zero.
				if (dp[i][j] < 0)
				{
					dp[i][j] = 0;
					pointerArray[i][j].v.clear();
				}

				// store max value.
				if (dp[i][j] > max)
				{
					max = dp[i][j];
				}
			}
		}
		

		// get every max cell.
		vector<node> result;
		for (int i = 1; i < dp.size(); i++)
		{
			for (int j = 1; j < dp[0].size(); j++)
			{
				if (dp[i][j] == max)
				{
					result.push_back({ i,j });
				}
			}
		}

		alignmentScore = max;

		*outFile << "Maximum Srore : " << alignmentScore << endl;

		// print every aligned string.
		for (int i = 0; i < result.size(); i++)
		{
			dfs(result[i].row, result[i].col);
		}
	}

	// print every aligned string with pointers.
	void dfs(int row, int col)
	{
		// push this cell to vevtor ' v ' .
		v.push_back({ row,col });

		vector<node>& pointerList = pointerArray[row][col].v;
		
		// if first cell of aligned string
		if (pointerList.empty())
		{
			print();
		}
		else
		{
			// trace every linked pointer.
			for (int i = 0; i < pointerList.size(); i++)
			{
				dfs(pointerList[i].row, pointerList[i].col);
			}
		}
		v.pop_back();
	}

	//test function
	void printMatrix()
	{
		cout << "Matrix" << endl;
		for (int i = 0; i < matchMatrix.size(); i++)
		{
			for (int j = 0; j < matchMatrix[0].size(); j++)
			{
				printf("\t%.2f", matchMatrix[i][j]);
			}
			cout << endl;
		}
	}
	// test function.
	void printDP()
	{
		cout << "dpdpdpdpdpdpdpdp" << endl;

		for (int i = 1; i < dp.size(); i++)
		{
			for (int j = 1; j < dp[0].size(); j++)
			{
				printf("%.1f\t ",dp[i][j]);
				//cout << dp[i][j] << "\t";
			}
			cout << endl;
		}
		cout << "*****************************" << endl;
	}

	// print aligned string with vector ' v '.
	void print()
	{
		int n1 = 0, n2 = 0;
		int row=0, col=0;
		int prow = 0, pcol = 0;
		
		for (int i = v.size()-1; i >= 0; i--)
		{
			row = v[i].row;
			col = v[i].col;
			
			// calculate gap size.
			int dist = row - prow - col + pcol;

			// if last character.
			if (i == v.size() - 1)
			{
				dist = 0;
			}

			// str2에 gap이 생긴다.
			if (dist > 0)
			{
				for (int i = 0; i < dist; i++)
				{
					align_str1[n1++] = str1[row - dist + i-1];
					align_str2[n2++] = '_';
				}
			}

			// str1에 gap이 생긴다.
			else if (dist < 0)
			{
				dist = -dist;
				for (int i = 0; i < dist; i++)
				{
					align_str1[n1++] = '_';
					align_str2[n2++] = str2[col - dist + i-1];
				}
			}

			align_str1[n1++] = str1[row-1];
			align_str2[n2++] = str2[col-1];
			prow = row;
			pcol = col;
			align_str1[n1] = '\0';
			align_str2[n2] = '\0';
		}
		align_str1[n1] = '\0';
		align_str2[n2] = '\0';

		//cout << align_str1 << endl;
		//cout << align_str2 << endl << endl;

		// write aligned string.
		*outFile << align_str1 << endl;
		*outFile << align_str2 << endl << endl<<endl;

	}

	void result()
	{
		cout << "Maximum Srore : " << alignmentScore << endl;
	}
};