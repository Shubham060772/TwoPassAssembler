/*
						NAME  	          : Shubham Agrawal
						ROLL NUMBER       : 2301AI44
						Declaration of Authorship:
						This document is a submission for the course CS1202 
						at the Department of Computer Science and Engineering, IIT Patna.
						I have tried my level best to program the assembler and emulator with extensive debugging. Enjoyed it!

*/
#include <bits/stdc++.h>
#include <fstream>
using namespace std;
#define tt int t;cin>>t;
#define ff first
#define ss second
#define pb push_back
#define cno cout<<"NO"<<endl;
#define cyes cout<<"YES"<<endl;
#define all(v) (v).begin(),(v).end()
template <typename T>
void printv(vector<T> &v){
    for(auto it:v){
        cout<<it<<' '; 
    }
    cout<<endl;
}

unordered_map<string,int>instructionsets(){
    unordered_map<string, int> instructions;
    ifstream file("DataFile/instructionsets.txt");// Open the instructions.txt file for reading
    if (!file.is_open()) {
        cerr << "Error: Could not open file instructionsets.txt" << endl;
        return instructions;
    }
    string line;
    int opcode = 0;
    while (getline(file, line)) {
        istringstream iss(line);
        string instruction;
        
        if (iss >> instruction) {
            instructions[instruction] = opcode++;
        }
    }
    file.close();
    return instructions;
}

unordered_map<int,string>Errors(){
    unordered_map<int, string> errors;
    ifstream file("DataFile/errors.txt");// Open the errors.txt file for reading
    if (!file.is_open()) {
        cerr << "Error: Could not open file errors.txt" << endl;
        return errors;
    }
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int errorCode;
        string errorMessage;

        if (iss >> errorCode && getline(iss, errorMessage)) {
            if (!errorMessage.empty() && errorMessage[0] == ' ')
                errorMessage.erase(0, 1);

            errors[errorCode] = errorMessage;
        }
    }
    file.close();
    return errors;
}

unordered_map<int, string> Warnings() {
    unordered_map<int, string> warnings;
    ifstream file("DataFile/warnings.txt");// Open the warnings.txt file for reading
    if (!file.is_open()) {
        cerr << "Error: Could not open file warnings.txt" << endl;
        return warnings;
    }
    string line;
    // Read the file line by line
    while (getline(file, line)) {
        istringstream iss(line);
        int warningCode;
        string warningMessage;
        if (iss >> warningCode && getline(iss, warningMessage)) { // Try to extract an integer (warning code) and the rest of the line as the warning message
            if (!warningMessage.empty() && warningMessage[0] == ' ') // If the message starts with a space, remove it to avoid leading spaces
                warningMessage.erase(0, 1);
                warnings[warningCode] = warningMessage; // Add the parsed warning code and message to the unordered_map
        }
    }
    file.close();
    return warnings;
}
unordered_map<string,int>opCodeTable=instructionsets();
unordered_map<int,string>errors=Errors();
unordered_map<int,string>warnings=Warnings();
unordered_map<string,int>symTable;
unordered_map<string, int> opcodeTableOperand = { {"ldc", 0},{"adc", 1},{"ldl", 2},{"stl", 3},{"ldnl", 4},{"stnl", 5},{"adj", 10},{"call", 13},{"brz", 15},{"brlz", 16},{"br", 17} };                                                            
// Mnemonics without Operand
unordered_map<string, int> opcodeNoOperand = {{"add", 6},{"sub", 7},{"shl", 8},{"shr", 9},{"a2sp", 11},{"sp2a", 12},{"return", 14},{"HALT", 18} };
int PC=0; //PROGRAM COUNTER
bool foundHalt = false; // To track if we've encountered a HALT for unreachable code detection
bool haltFlag = false;
unordered_set<string> usedLabels;
vector<pair<int, string>> warningsList;
vector<pair<int, string>> errorsList;

string DecToHex(int intValue){ //function to convert decimal to hexadecimal
    stringstream ss;
    ss << setfill('0') << setw(8) << hex << intValue;
    return ss.str();
}
int HextoDec(const std::string& hex) {
    return std::stoi(hex, nullptr, 16);
}
string removeComments(const string& line) { //function to remove comments
    auto commentPos = line.find(';');
    return (commentPos == string::npos) ? line : line.substr(0, commentPos);
}
string trim(const string& str) { //function to trim lines
    auto first = find_if(str.begin(), str.end(), [](char c) { return !isspace(c); });
    auto last = find_if(str.rbegin(), str.rend(), [](char c) { return !isspace(c); }).base();
    return (first < last) ? string(first, last) : "";
}

bool isValidInteger(const string& operand) {
    try {
        size_t pos;
        stoi(operand, &pos);
        if (pos != operand.length())throw invalid_argument("");
            return true;
    }
    catch (...) {return false;}
}
bool isValidSetValue(const string& value, int& result) {
    try {
        // Handle hex values prefixed with 0x
        if (value.length() > 2 && value.substr(0, 2) == "0x") {
            size_t pos = 0;
            result = stoi(value.substr(2), &pos, 16);
            return pos == value.length() - 2;
        }
        // Handle decimal values
        size_t pos = 0;
        result = stoi(value, &pos);
        return pos == value.length();
    } catch (...) {
        return false;
    }
}
bool isValidLabel(const string& label) {    //function to check label 
    if (label.empty()) return false;
    if (isdigit(label[0])) return false;
    return all_of(label.begin(), label.end(), [](char c) {
        return isalnum(c) || c == '_';
    });
}
void trackLabelUsage(const string& operation, const string& argument) {
    if (operation == "br" || operation == "ldc") {
        if (symTable.find(argument) != symTable.end()) {
            usedLabels.insert(argument);
        }
    }
}
void FirstPass(const string& file) {
    ifstream infile(file);
    ofstream outfile("list1.txt");
    if (!infile.is_open()) {
        errorsList.push_back({0, "Could not open file " + file});
        return;
    }
    string line; int lineNum = 0;
    while (getline(infile, line)) {
        lineNum++;
        string originalLine = line;
        line = trim(removeComments(line));
        if (line.empty()) continue;
        size_t labelPos = line.find(':');
        if (labelPos != string::npos) {
            string label = trim(line.substr(0, labelPos));
            if (!isValidLabel(label)) {
                errorsList.push_back({lineNum, "Invalid label name '" + label + "'"});
                continue;
            }
            if (symTable.find(label) != symTable.end()) {
                errorsList.push_back({lineNum, "Duplicate label '" + label + "'"});
                continue;
            }
            line = trim(line.substr(labelPos + 1)); 
            istringstream iss(line);
            string directive, value;
            iss >> directive >> value;
            if (directive == "SET") { // Enhanced SET directive handling
                if (value.empty()) {
                    errorsList.push_back({lineNum, "Missing value for SET directive"});
                    continue;
                }
                int setValue;
                if (!isValidSetValue(value, setValue)) {
                    errorsList.push_back({lineNum, "Invalid value for SET directive: '" + value + "'"});
                    continue;
                }
                // Check for value range
                if (setValue > INT_MAX || setValue < INT_MIN) {
                    errorsList.push_back({lineNum, "SET value out of range"});
                    continue;
                }
                symTable[label] = setValue;
                outfile << DecToHex(PC) << " SET " << setValue << endl;
                continue;
            }
            symTable[label] = PC;
        }
        if (!line.empty()) {
            istringstream iss(line);
            string instruction;
            iss >> instruction;
            if (opCodeTable.find(instruction) == opCodeTable.end() && 
                instruction != "data" && 
                instruction != "SET") {
                errorsList.push_back({lineNum, "Unknown instruction '" + instruction + "'"});
            }
            outfile << DecToHex(PC) << " " << line << endl;
            if (instruction != "SET")PC++;
        }
    }
    infile.close();outfile.close();
}


void secondPass(const string& Infile) { // second pass :  this will generate machine code using the list.txt file generated by the first pass.
    ifstream infile(Infile);            
    ofstream listFile("program.lst");
    ofstream objFile("obj.o");
    string line;
    listFile << "Line    Code        Source" << endl;
    listFile << "----   --------    ------" << endl;
    while (getline(infile, line)) {
        istringstream iss(line);
        string programCounter, instruction;//Get both program counter and instruction from the temporary list file
        iss >> programCounter;
        getline(iss, instruction);  
        instruction = trim(instruction);
        if (instruction.empty()) {      //Continue if line is empty
            listFile << line << endl;
            continue;}
        if (symTable.find(instruction) != symTable.end()) {
            listFile << line << endl;
            continue;}
        string mnemonic, operand;
        size_t spacePos = instruction.find(' ');
        if (spacePos != string::npos){
            mnemonic = instruction.substr(0, spacePos);
            operand = trim(instruction.substr(spacePos + 1));}
        else {
            mnemonic = instruction;
            operand = "";
        }
        //If given Halt in the middle
        if (foundHalt) {
            cerr << warnings[2] << ": Unreachable code found after HALT at PC " << programCounter << "\n";
            warningsList.push_back({HextoDec(programCounter), warnings[2] + " Unreachable code found after HALT at PC "});
            foundHalt = false; //Stop repeated warnings
        }
        if (mnemonic == "HALT") {foundHalt = true; haltFlag = true;}
        //Check for loop: br loop
        if ((mnemonic == "br" || mnemonic == "brz" || mnemonic == "brlz" || mnemonic == "call")) {  
            if (symTable.find(operand) != symTable.end()) { // operand here is label to which counter has to jump 
                if (symTable[operand] == stoi(programCounter, nullptr, 16)) {
                cerr << warnings[4] << ": at " << programCounter << endl;
                warningsList.push_back({HextoDec(programCounter), warnings[4] + ":at " + programCounter});
                }
            }
            else if (isValidInteger(operand)) {
                if (stoi(operand) == stoi(programCounter, nullptr, 16)) {
                    cerr << warnings[4] << ": at " << programCounter << endl;
                    warningsList.push_back({HextoDec(programCounter), warnings[4] + ":at " + programCounter});
                }
            }
        }
        string machinecode;
        //Data mnemonic opCode = -1
        if (mnemonic == "data") {
            if (operand.empty()) {
                cerr << errors[4] << ": Operand not found " << programCounter << "\n";
                errorsList.push_back({HextoDec(programCounter), errors[4] + " : Operand not found"});
                continue;
            }
            try {
                int value = stoi(operand);
                stringstream ss;
                ss << hex << -1;
                string machinecode = DecToHex(value).substr(2, 6);
                machinecode = machinecode + ss.str().substr(6, 2);
                // Write each data value to list and object file
                listFile << programCounter << " " << machinecode << " data " << operand << endl;
                objFile << machinecode << endl;
                // Increment programCounter for each data value 
                programCounter = DecToHex(stoi(programCounter, nullptr, 16) + 1);
            }
            catch (const invalid_argument&) {
                cerr << errors[3] << ": Invalid operand for 'data' at PC " << programCounter << "\n";
                errorsList.push_back({HextoDec(programCounter), errors[3] + ": Invalid operand for 'data' at PC "+programCounter});
                continue;
            }
            catch (const out_of_range&) {
                cerr << errors[3] << ": Operand out of range for 'data' at PC " << programCounter << "\n";
                errorsList.push_back({HextoDec(programCounter), errors[3] + " : Operand out of range for 'data' at PC "+programCounter});
                continue;
            }continue;}
        // Handle other instructions
        else if (opCodeTable.find(mnemonic) != opCodeTable.end()) {
            int opcodeValue = opCodeTable[mnemonic];
            stringstream ss;
            ss << setfill('0') << setw(2) << hex << opcodeValue;
            string opcode = ss.str();
            string value;
            if (operand.empty()) {
                if (opcodeTableOperand.find(mnemonic) != opcodeTableOperand.end()) {
                    cerr << errors[4] << ": Operand not found " << programCounter << "\n";
                    errorsList.push_back({HextoDec(programCounter), errors[3] + "Unexpected Operand"});
                    continue;
                }
                value = "000000";
            }
            else if (!operand.empty() && opcodeNoOperand.find(mnemonic) != opcodeNoOperand.end()) {
                cerr << errors[5] << ": Unexpected Operand " << programCounter << endl;
                errorsList.push_back({HextoDec(programCounter), errors[3] + "Unexpected Operand"});
            }
            else if (operand[0] == '+' || operand[0] == '-' || isdigit(operand[0])) {
                try {
                    size_t pos;
                    int numValue = stoi(operand, &pos);
                    if (pos != operand.length()) {
                        throw invalid_argument("");
                    }

                    if (mnemonic == "ldc" || mnemonic == "adc" || mnemonic == "adj" || mnemonic == "ldl" || mnemonic == "stl" || mnemonic == "ldnl" || mnemonic == "stnl") {
                        value = DecToHex(numValue).substr(2, 6);  //Get only last 6 digits
                    }
                    else {
                        int tempValue = stoi(programCounter, nullptr, 16) - numValue;
                        tempValue = -tempValue;
                        value = DecToHex(tempValue).substr(2, 6);
                    }
                }
                catch (const invalid_argument&) {
                    cerr << errors[3] << ": Invalid numeric operand '" << operand << "' at PC " << programCounter << "\n";
                    continue;
                }
                catch (const out_of_range&) {
                    cerr << errors[3] << ": Numeric operand out of range '" << operand << "' at PC " << programCounter << "\n";
                    errorsList.push_back({HextoDec(programCounter), errors[3] + ": Numeric operand out of range "+operand+"at PC"+programCounter});
                    continue;
                }
            }
            else if (symTable.find(operand) != symTable.end()) {
                int numValue;
                numValue = symTable[operand];
                if (mnemonic == "ldc" || mnemonic == "adc" || mnemonic == "adj" || mnemonic == "ldl" || mnemonic == "stl" || mnemonic == "ldnl" || mnemonic == "stnl") {
                    value = DecToHex(numValue).substr(2, 6); //Get only 6 value
                }
                else {
                    int tempValue = stoi(programCounter, nullptr, 16) - numValue;
                    tempValue = -tempValue;
                    value = DecToHex(tempValue).substr(2, 6); 
                }
                usedLabels.insert(operand);  //Mark this label as used when found as operand
            }
            else {
                cerr << errors[1] << ": Undefined label '" << operand << "' at PC " << programCounter << "\n";
                errorsList.push_back({HextoDec(programCounter), errors[3] + ": Undefined Label"+operand+" at PC"+programCounter});
                continue;
            }
            machinecode = value + opcode;//Add value and opcode to make the machinecode
        }
        else {
            cerr << errors[0] << ": Unknown instruction '" << mnemonic << "' at PC " << programCounter << "\n";
            errorsList.push_back({HextoDec(programCounter), errors[0] + "Unknown Instruction"+mnemonic+"at PC"+programCounter});
            continue;
        }

        if (!machinecode.empty()) {
            listFile << programCounter << " " << machinecode << " " << instruction << endl;
            objFile << machinecode << endl;
        }
        else {
            listFile << line << endl;}}
    listFile << "\nSymbol Table" << endl;
    listFile << "------------" << endl;
    listFile << "Symbol    Value" << endl;
    listFile << "------    -----" << endl;
    vector<pair<string, int>> sortedSymbols(symTable.begin(), symTable.end());
    sort(sortedSymbols.begin(), sortedSymbols.end()); 
    for (const auto& symbol : sortedSymbols) {
        listFile << left << setw(10) << symbol.first << "0x" 
                << setfill('0') << setw(8) << hex << symbol.second << endl;
    }
    infile.close();
    listFile.close();
    objFile.close();
}
void generateLog(const string& logFile) {
    ofstream log(logFile);
    if (!log.is_open()) {
        cerr << "Error: Could not create log file" << endl;
        return;}
    // Write LabelName => Value section
    log << "LabelName => Value" << endl;
    vector<pair<string, int>> sortedSymbols(symTable.begin(), symTable.end());
    sort(sortedSymbols.begin(), sortedSymbols.end());
    for (const auto& symbol : sortedSymbols) {
        log << symbol.first << " => " << symbol.second << endl;
    }
    // Write UsedLabelName section
    log << "\nUsedLabelName" << endl;
    vector<string> sortedUsedLabels(usedLabels.begin(), usedLabels.end());
    sort(sortedUsedLabels.begin(), sortedUsedLabels.end());
    for (const auto& label : sortedUsedLabels) {
        log << label << endl;
    }
    // Write Warnings section
    log << "\nWarnings" << endl;
    sort(warningsList.begin(), warningsList.end());
    for (const auto& wrn : warningsList) {
        log << "Line " << wrn.first << " : " << wrn.second << endl;
    }
    // Write Errors section
    log << "\nErrors" << endl;
    sort(errorsList.begin(), errorsList.end());
    for (const auto& error : errorsList) {
        log << "Line " << error.first << " : " << error.second << endl;
    }
    log.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Error: Please provide a filename as an argument!" << endl;
        return 1;  // Return an error code
    }
    string filename = argv[1];
    // string filename;
    // cin >> filename;
    string inputfile = "TestFiles/" + filename;
    
    FirstPass(inputfile);
    secondPass("list1.txt");
    
    int extnpos = filename.find('.');
    string extn = filename.substr(0, extnpos);
    string logfilename = extn + ".log";
    generateLog(logfilename);
    string ofname = extn + ".o";
    string lstfname = extn + ".lst";
    rename("obj.o", ofname.c_str());
    rename("program.lst", lstfname.c_str());
    
    return 0;
}