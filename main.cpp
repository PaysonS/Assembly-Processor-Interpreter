#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

const int LABEL_COUNT = 100; ///< default number of labels
const int OPCODE_COUNT = 18; ///< default number of opcodes
const int MAX_CHARS = 5;     ///< maximum number of characters
const int STACK_SIZE = 1000; ///< maximum capacity of array

const std::string OPCODE_LIST = "const get put ld st add sub mul\
 div cmp jpos jz j jl jle jg jge halt"; 

struct Opcode
{

    string name;
    int mem;
    Opcode()
    {
        name = "";
        mem = 0;
    }
};

struct Symbol
{
    string name;
    int mem;
    Symbol()
    {
        name = "";
        mem = 0;
    }
};

//splits the list of opcodes into individual ones
void splitOpcodes(string inst, Opcode *opcodes)
{
    int i = 0;
    istringstream ss(inst);
    string substring;
    while (ss >> substring)
    {
        opcodes[i].name = substring;
        opcodes[i].mem = i;
        i++;
    }
}
//checks if the passed word is an opcode
bool isOpCode(string &word, Opcode *opcodes)
{

    for (int i = 0; i < 18; i++)
    {
        if (word == opcodes[i].name)
        {
            return true;
        }
    }
    return false;
}

//returns the location of the opcode 
int getOpcode(string &word, Opcode *opcodes)
{
    for (int i = 0; i < OPCODE_COUNT; i++)
    {
        if (word == opcodes[i].name)
        {
            return i;
        }
    }
    return -1;
}

//checks if the first chracter is an ASCII number
bool isNumber(string &str)
{
    char temp = str[0];

    if ((temp >= 48 && temp <= 57) && (str != "" || str != " "))
    {
        return true;
    }

    return false;
}
//returns the location in memory of the label
int getLocation(string &name, Symbol *labels, int nextLabel)
{

    for (int i = 0; i < LABEL_COUNT; i++)
    {
        if (name == labels[i].name)
        {
            return labels[i].mem;
        }
        else if (isNumber(name))
        {
            return stoi(name);
        }
        else if (name == "" || name == " ")
        {
            return 0;
        }
    }

    return -1;
}

int main(int argc, char *argv[])
{
    ifstream iFile;
    ofstream oFile;
    string line;
    Opcode opcodes[OPCODE_COUNT];
    Symbol labels[LABEL_COUNT];
    string columnCodes[STACK_SIZE];
    int secondmem[STACK_SIZE] = {0};
    int memory[STACK_SIZE] = {0};
    int firstmem[STACK_SIZE] = {0};
    int cnt = 0;

    splitOpcodes(OPCODE_LIST, opcodes);

    std::cout << "Running program..." << std::endl;

    iFile.open(argv[1]);

    if (!(iFile.is_open()))
    {
        cout << "** Program terminated **" << endl;
        return EXIT_SUCCESS;
    }

    stringstream ss(line);
    string characters;

    int label = 0;
    int i = 0;

    while (getline(iFile, line))
    {
        // deletes anything that comes after a semicolon
        int commentPos = line.find(';');
        if (commentPos != std::string::npos)
        {
            line.erase(commentPos);
        }
        if (line.size() == 0)
        {
            continue;
        }
        stringstream ss(line);
        string columnOne, columnTwo, columnThree;
        // skips past any line that starts with a semicolon
        if (line[0] == ';')
        {
            continue;
        }
        else
        {
            // if a line starts with space or tab it puts the
            // second column into the array
            if (line[0] == ' ' || line[0] == '\t')
            {

                while (ss >> columnTwo)
                {
                    columnCodes[i] += columnTwo + " ";
                }
                i++;
            }
            else
            {
                // puts first column into an array
                ss >> columnOne;
                labels[label].name = columnOne;
                labels[label].mem = i;
                while (ss >> columnTwo)
                {
                    columnCodes[i] += columnTwo + " ";
                }
                label++;
                i++;
            }
        }
    }

    iFile.close();

    //outputs the code into the new output file
    oFile.open("out.txt");
    for (int j = 0; j < i; j++)
    {
        oFile << columnCodes[j] << endl;
    }
    oFile.close();

    // pass two

    iFile.open("out.txt");
    string keeper;
    string opHolder;
    string labelHolder;
    int memCount = 0;

//reads from the new file
    while (getline(iFile, keeper))
    {
        stringstream stream(keeper);

        while (stream >> opHolder)
        {

            if (isOpCode(opHolder, opcodes))
            {

                int opLocation = getOpcode(opHolder, opcodes);
                stream >> labelHolder;

                if (!isOpCode(labelHolder, opcodes))
                {

                    int labelLocation = getLocation(labelHolder, labels, label);

                    memory[memCount] = STACK_SIZE * opLocation + labelLocation;
                    memCount++;
                }
            }
            labelHolder = "";
        }
    }

    iFile.close();

    int reg = 0; 
    int ip = 0;
//gets the memory and checks it with each command
    while (ip >= 0)
    {

        int address = memory[ip] % 1000;
        int code = memory[ip] / 1000;
        int input = 0;

        if (code == 0)
        {
            break;
        }
        else if (code == 1)
        {
            cin >> reg;

            if (!cin)
            {
                cout << "break" << endl;
                cout << "ERROR: invalid input!" << endl
                     << "** Program terminated with an error code **";
                exit(0);
            }
            cout << "read: " << reg << endl;

            ip++;
        }
        else if (code == 2)
        {
            cout << "result: " << reg << endl;

            ip++;
        }
        else if (code == 3)
        {
            reg = memory[address];
            ip++;
        }
        else if (code == 4)
        {
            memory[address] = reg;
            ip++;
        }
        else if (code == 5)
        {
            reg = reg + memory[address];
            ip++;
        }
        else if (code == 6)
        {
            reg -= memory[address];
            ip++;
        }
        else if (code == 7)
        {
            reg = reg * memory[address];
            ip++;
        }
        else if (code == 8)
        {
            reg = reg / memory[address];
            ip++;
        }
        else if (code == 9)
        {
            reg = reg - memory[address];
            ip++;
        }
        else if (code == 10)
        {
            if (reg > 0)
            {
                ip = address;
            }
            else
                ip++;
        }
        else if (code == 11)
        {
            if (reg == 0)
            {
                ip = address;
            }
            else
                ip++;
        }
        else if (code == 12)
        {
            ip = address;
        }
        else if (code == 13)
        {
            if (reg < 0)
            {
                ip = address;
            }
            else
                ip++;
        }
        else if (code == 14)
        {
            if (reg <= 0)
            {
                ip = address;
            }
            else
                ip++;
        }
        else if (code == 15)
        {
            if (reg > 0)
            {
                ip = address;
            }
            else
                ip++;
        }
        else if (code == 16)
        {
            if (reg >= 0)
            {
                ip = address;
            }
            else
                ip++;
        }
        else
        {
            cout << "** Program terminated **";
            ip = -1;
        }
    }

    cnt++;

    return EXIT_SUCCESS;
}

