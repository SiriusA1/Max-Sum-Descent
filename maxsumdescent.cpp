/*******************************************************************************
 * Name        : maxsumdescent.cpp
 * Author      : Conor McGullam
 * Version     : 1.0
 * Date        : 4/26/2020
 * Description : Dynamic programming solution to max sum descent problem.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

const char DELIMITER = ' ';

int **values, // This is your 2D array of values, read from the file.
    **sums;   // This is your 2D array of partial sums, used in DP.

int num_rows; // num_rows tells you how many rows the 2D array has.
              // The first row has 1 column, the second row has 2 columns, and
              // so on...
int max_index = -1; //index of max partial sum in last row of sums
int num_length = 0; //used for setw

/**
 * Displays the 2D array of values read from the file in the format of a table.
 */
void display_table() {
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < i; ++j) {
			cout << setw(num_length) << values[i][j] << " ";
		}
		cout << setw(num_length) << values[i][i] << endl;
	}
}

void display_sums_table() {
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < i; ++j) {
			cout << sums[i][j] << " ";
		}
		cout << sums[i][i] << endl;
	}
}

/**
 * Returns the maximum sum possible in the triangle of values.
 * By starting at the top of the triangle and moving to adjacent numbers on the
 * row below, this function uses dynamic programming to build up another table
 * of partial sums.
 */
int compute_max_sum() {
	sums = new int*[num_rows];
	for(int i = 0; i < num_rows; ++i) {
		sums[i] = new int[i+1];
		if(i == 0) {
			sums[0][0] = values[0][0];
		}else {
			for(int j = 0; j < i+1; ++j) {
				if(j == 0) {
					sums[i][j] = sums[i-1][j] + values[i][j];
				}else if(j == i) {
					sums[i][j] = sums[i-1][j-1] + values[i][j];
				}else {
					sums[i][j] = max(sums[i-1][j-1] + values[i][j], sums[i-1][j] + values[i][j]);
				}
			}
		}
	}
	if(num_rows == 0) {
		return 0;
	}else {
		int max = 0;
		max_index = 0;
		for(int j = 0; j < num_rows; ++j) {
			if(sums[num_rows-1][j] > max) {
				max = sums[num_rows-1][j];
				max_index = j;
			}
		}
		return max;
	}
}

/**
 * Returns a vector of ints with the values from the top to the bottom of the
 * triangle that comprise the maximum sum.
 */
vector<int> backtrack_solution() {
    vector<int> solution = {};
	if(max_index < 0) {
		goto end;
	}else {
		int curr = max_index;
		for(int i = num_rows-1; i >= 0; --i) {
			solution.push_back(values[i][curr]);
			if(curr == 0 || (i>0 && sums[i][curr] == sums[i-1][curr] + values[i][curr])) {
				//nothing
			}else {
				--curr;
			}
		}
	}
	end:
		//have to read backwards
		return solution;
}

/**
 * Reads the contents of the file into the global 2D array 'values'. If
 * successful, the function also allocates memory for the 2D array 'sums'.
 */
bool load_values_from_file(const string &filename) {
    ifstream input_file(filename.c_str());
    if (!input_file) {
        cerr << "Error: Cannot open file '" << filename << "'." << endl;
        return false;
    }
    input_file.exceptions(ifstream::badbit);
    string line;
    vector<string> data;
    try {
        while (getline(input_file, line)) {
            data.push_back(line);
        }
        input_file.close();
    } catch (const ifstream::failure &f) {
        cerr << "Error: An I/O error occurred reading '" << filename << "'.";
        return false;
    }
    
	num_rows = (int)data.size();
	values = new int*[num_rows];
	for(int i = 0; i < num_rows; ++i) {
		values[i] = new int[i+1];
		num_length = max(num_length, ((int)data[i].size()-(i))/(i+1));
		stringstream ss(data[i]);
		for(int j=0; j<=i; j++) {
			ss >> values[i][j];
		}
		ss.clear();
	}

    return true;
}

/**
 * Frees up the memory allocated for the 2D array of values and the 2D array of
 * partial sums.
 */
void cleanup() {
    for(int i = 0; i < num_rows; ++i) {
		delete [] values[i];
		delete [] sums[i];
	}
	delete [] values;
	delete [] sums;
}

int main(int argc, char * const argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    string filename(argv[1]);
    if (!load_values_from_file(filename)) {
        return 1;
    }
		
    display_table();
	cout << "Max sum: " << compute_max_sum() << endl;
	vector<int> path = backtrack_solution();
	cout << "Values: [";
	for(int i = path.size()-1; i >= 0; --i) {
		if(i > 0) {
			cout << path[i] << ", ";
		}else {
			cout << path[i];
		}
	}
	cout << "]" << endl;

    return 0;
}
