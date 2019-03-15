#include <iostream>
#include <stack>
#include <fstream>
#include <string>

using namespace std;

class TuringMachine {
  private:
    stack<char> left;
    stack<char> right;      // Head points to the top element of right stack.
    char** ruleMatrix;            // Need to dynamically allocated this
    int ruleMatrixRow;

    char cur_state; 

  public:  
    TuringMachine();
    ~TuringMachine();

    bool clearTape();
    bool clearRule();
    bool printTape();
    void printRule();

    bool setRules(ifstream&);     // dynamically allocate column base on number of states in the delta table
    bool setTape(string);       // copy input string into right stack, string.last get pushed first, and string.begin get push last. first char of string is at the top of the stack
    bool run();                 // Test what ever is on the Tape base on the Rule, if Tape is valid stirng, or never halt, or halt, will never know.
};

TuringMachine::TuringMachine() {
    //soemthing 
    ruleMatrixRow = 0;
    cur_state = '0';
}

TuringMachine::~TuringMachine() {
    clearRule();
}

bool TuringMachine::clearTape() {
  while( !left.empty() ) {
      left.pop();
  }
  while( !right.empty() ) {
      right.pop();
  }
  return true;
}

bool TuringMachine::clearRule() {
    for (int i = 0; i < ruleMatrixRow; ++i)
        delete [] ruleMatrix;
    delete [] ruleMatrix;

    return true;
}

bool TuringMachine::printTape() {
    stack<char> left_cpy(left);
    stack<char> left_upside_down;
    while( !left_cpy.empty() ) {
        left_upside_down.push(left_cpy.top());
        left_cpy.pop();
    }
    stack<char> right_cpy(right);

    while( !left_upside_down.empty() ) {
        cout << left_upside_down.top();
        left_upside_down.pop();
    }
    cout << "H";
    while( !right_cpy.empty() ) {
        cout << right_cpy.top();
        right_cpy.pop();
    }

}

void TuringMachine::printRule() {
    for(int i = 0; i < ruleMatrixRow; ++i) {
        cout << "\t[" << i << "]:";
        for(int j =0; j < 5; ++j) {
            cout << ruleMatrix[i][j];
        }
        cout << endl;
    }

    return;
}

bool TuringMachine::setRules(ifstream& in_file) {
    ruleMatrixRow = 0;
    string rules = "";
    string temp = "";

    for( ; getline(in_file,temp) ; ++ruleMatrixRow ) {        // 100, cuz why not, one line shouldn't be expected to be that long anyway from input file. 
        temp += '\n';
        rules += temp;
    }
    /* cout << "Here's the rules:\n'" << rules << "'" << endl;
    cout << "Number of line:" << ruleMatrixRow << endl; */


    ruleMatrix = new char*[ruleMatrixRow];
    for(int i = 0; i < ruleMatrixRow; ++i) {
        ruleMatrix[i] = new char[5];
    }

    string::iterator it = rules.begin();
    for( int i = 0; i < ruleMatrixRow; ++i ) {
        for( int j = 0; it != rules.end(); ++it ) {
            if(*it == ' ') {
                continue;
            }
            if(*it == '\n') {
                ++it;
                break;
            }
            ruleMatrix[i][j] = *it;
            ++j;
        }
    }

    return true;
}

/* input can be size 0, some langauges can accept the empty string. */
bool TuringMachine::setTape(string input) {
    clearTape();
    if(input.length() == 0) {
        return true;
    }
    string::iterator it = input.end();
    /* execute then check the condition */
    do {
        --it;
        right.push(*it);
    } while( it != input.begin() );

    return true;
}

bool TuringMachine::run() {
    cur_state = '0';  /* Make sure startin state matches what is giving in the file */

    char cur_symbol = 0;

    do {
        cout << "Tape:";
        printTape();
        cout << endl;

        if(right.empty()) {
            cur_symbol = 'B';
        } else {
            cur_symbol = right.top();
        }

        int i = 0;
        for(; i < ruleMatrixRow; ++i) {
            if( cur_state == ruleMatrix[i][0] && cur_symbol == ruleMatrix[i][1] ) { 
                // set new state
                cur_state = ruleMatrix[i][2];
                if(cur_state = 'f') {
                    return true;
                }
                    
                // determine l->r or r->l
                if( ruleMatrix[i][4] == 'L' ) {      // Head move to Left
                    right.top() = ruleMatrix[i][3];
                    char tmp = left.top();
                    left.pop();
                    right.push(tmp);
                } else {    // Head move to Right, NOTE: no chekcing for == 'R' here! Can cause undefined behavior!
                    left.top() = ruleMatrix[i][3];
                    char tmp = right.top();
                    right.pop();
                    left.push(tmp);
                }
                // pop and push the correct side
                break;
            }
        }

        if( i==ruleMatrixRow ) {  /* from current state, can't find what to do from transition table, REJECT string! String not in language described by transition table */
            return false;
        }

    } while( true );

    return false;
}

int main(int argc, char** argv) {
    if( argc != 2 ) {
        cout << "Usage: ./TuringMachine transition_table_file_name" << endl;
        return 0;
    }

    /* Read in the rule file */
    ifstream in_file(argv[1], ifstream::in);
    if( !in_file.is_open() ) {
        cout << "Error opening file" << endl;
        return 0;
    }

    TuringMachine TM;
    cout << "This is a deterministic Turing Machine. It won't work if you put in a transition table that use non-detminism!" << endl;
    /* Set the rule */
    if( TM.setRules(in_file) ) {
        cout << " * Rules loaded in successfully" << endl;
    } 
    cout << " * Here is the transition table" << endl;
    TM.printRule();

    /* Get input string from keybaord */
    cout << "you may now enter any string and we will test if it is in the language." << endl;
    cout << "(note: you may cause the Turing Machine to run forever if transition table describes a Recursively Enumerable Language)" << endl;
    cout << "you maybe start typing in the string now, to quit, type in 'quit' without the single quote." << endl;
    cout << "-----------------------------------------------------------------------------------------------------" << endl;
    cout << " > ";
    string input = "";
    while( getline(cin,input) ) {
        if(input=="quit") {
            break;
        }
        TM.setTape(input);

        /* Test input string base on rules */
        if( TM.run() ) {
            // string is indeed in language.
            cout << "YEP!" << endl;
        } else {
            cout <<"NOPE!" << endl;
        }

        cout << " > ";
    }
    


    return 0;
}