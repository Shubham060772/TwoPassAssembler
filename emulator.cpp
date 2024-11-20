#include <bits/stdc++.h>
#include <fstream>
using namespace std;

unordered_map<int, string> warningVector = { {} };

int pc = 0, sp = 0, A = 0, B = 0, offset = 0;
long long int mem[5000];


vector<string> hexCode;

//Remove surrounded spaces
string trim(const string& str) {
	// Define whitespace characters to include spaces, tabs, and newline characters
	const string whitespace = " \t\n\r";

	size_t first = str.find_first_not_of(whitespace);
	size_t last = str.find_last_not_of(whitespace);

	if (first == string::npos || last == string::npos) return "";
	return str.substr(first, (last - first + 1));
}

//Change from hexadecimal to decimal
long long int HextoDec(const string& value) {
	string trimmedValue = trim(value);

	if (trimmedValue.length() < 8) {

		if (trimmedValue.front() == 'f') {
			trimmedValue.insert(0, "ff");
		}
	}

	// Convert the hex string to an unsigned 32-bit integer using stoull for safety
	unsigned int unsignedValue = stoul(trimmedValue, nullptr, 16);

	// Check if the value should be interpreted as a negative number (32-bit signed range)
	if (unsignedValue >= 0x80000000) {
		// Apply 2's complement to get the negative value
		return static_cast<int>(unsignedValue - 0x100000000);
	}
	else {
		// Otherwise, it's a positive value within 32-bit range
		return static_cast<int>(unsignedValue);
	}
}

//Store the machine code in a file
void storeCode(string filepath) {
	ifstream infile(filepath);
	string line;
	int index = 0;
	bool data = false;

	while (getline(infile, line)) {
		hexCode.push_back(line);
		if (line.substr(6, 2) == "ff") {  
			data = true;
		}
		if (data) {
			mem[index]=HextoDec(line.substr(0, 6));
			index++;
		}
		data = false;
	}
}

//Start performing instructions
void parseCode() {
	while (true) {
		string hexOpCode = hexCode[pc].substr(6, 2);
		string hexValue = hexCode[pc].substr(0, 6);

		long long int opCode = HextoDec(hexOpCode);
		long long int value = HextoDec(hexValue);

		cout << pc << "\t" << sp << "\t" << A << "\t" << B << "\n";
		for (auto i : mem) {
			if(i!=0)
			cout << i << "\t";
		}
		cout << "\n";
		switch (opCode)
		{
		case 0:
			B = A;
			A = value;
			pc++;
			break;

		case 1:
			A = A + value;
			pc++;
			break;

		case 2:
			B = A;
			A = mem[sp + value];
			pc++;
			break;

		case 3:
			mem[sp + value] = A;
			A = B;
			pc++;
			break;

		case 4:
			A = mem[sp + value];
			pc++;
			break;
			
		case 5:
			mem[A + value] = B;
			pc++;
			break;

		case 6:
			A = B + A;
			pc++;
			break;

		case 7:
			A = B - A;
			pc++;
			break;

		case 8:
			A = B << A;
			pc++;
			break;

		case 9:
			A = B >> A;
			pc++;
			break;

		case 10:
			sp = sp + value;
			pc++;
			break;

		case 11:
			sp = A;
			A = B;
			pc++;
			break;

		case 12:
			B = A;
			A = sp;
			pc++;
			break;

		case 13:
			B = A;
			A = pc;
			pc = pc + value;
			break;

		case 14:
			pc = A;
			A = B;
			pc++;
			break;

		case 15:
			if (A == 0) pc = pc + value;
			else pc++;
			break;

		case 16:
			if (A < 0) pc = pc + value;
			else pc++;
			break;

		case 17:
			pc = pc + value;
			break;

		case 18:
			return;

		default:
			pc++;
			break;
		}
		if (pc >= hexCode.size()) {
			return;
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
        cerr << "Error: Please provide a filename as an argument!" << endl;
        return 1;  // Return an error code
    }
	string objfile = argv[1];
	for (int i = 0; i < 1000; i++) {
		mem[i] = 0;
	}
	storeCode(objfile);
	parseCode();
	cout << "PC= "<<pc << "\t"<< " SP= " << sp  << " regA= "<<A << "\t"<< " regB= "<< "\t" << B << "\n";
	cout<<"Memory Dump: \t";
		for (auto i : mem) {
			cout <<i << "\t";
		}
		cout << "\n";

	return 0;
}