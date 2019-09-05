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
    this->ruleMatrixRow = 0;
    this->cur_state = '0';
}

TuringMachine::~TuringMachine() {
    this->clearRule();
}

bool TuringMachine::clearTape() {
  while( !this->left.empty() ) {
      this->left.pop();
  }
  while( !this->right.empty() ) {
      this->right.pop();
  }
  return true;
}

bool TuringMachine::clearRule() {
    for (int i = 0; i < this->ruleMatrixRow; ++i)
        delete [] this->ruleMatrix[i];
    delete [] this->ruleMatrix;

    return true;
}

bool TuringMachine::printTape() {
    stack<char> left_cpy(this->left);
    stack<char> left_upside_down;
    while( !left_cpy.empty() ) {
        left_upside_down.push(left_cpy.top());
        left_cpy.pop();
    }
    stack<char> right_cpy(this->right);

    while( !left_upside_down.empty() ) {
        cout << left_upside_down.top();
        left_upside_down.pop();
    }
    cout << "H("<< this->cur_state <<")";
    while( !right_cpy.empty() ) {
        cout << right_cpy.top();
        right_cpy.pop();
    }

    return true;
}

void TuringMachine::printRule() {
    if( this->ruleMatrixRow == 0 ) {
        cout << "WARNING: Empty Rules Table!" << endl;
        return;
    }
    
    for(int i = 0; i < this->ruleMatrixRow; ++i) {
        cout << "\t[" << i << "]:";
        for(int j =0; j < 5; ++j) {
            cout << this->ruleMatrix[i][j];
        }
        cout << endl;
    }

    return;
}

bool TuringMachine::setRules(ifstream& in_file) {
    this->ruleMatrixRow = 0;
    string rules = "";
    string temp = "";
    string validtemp = "";
    bool error = false;

    for( ; getline(in_file,temp) ; ) {        // 100, cuz why not, one line shouldn't be expected to be that long anyway from input file. 
        validtemp.erase();
        for (int i = 0; i < temp.length(); ++i ) {
            if( temp[i] == '/' ) {
                break;
            }
            if( temp[i] == ' ' || temp[i] == '\r' || temp[i] == '\t' ) {
                continue;
            }
            validtemp+=temp[i];
        }

        if( validtemp.length() != 5 ) {
            cout << "\t > Unrecognized string... Ignoring: '" << temp << "'";
            //cout << "\n\t validtemp:'" << validtemp << "'";
            cout << endl;
            error = true;
        } else {
            validtemp += '\n';
            rules += validtemp;
            ++this->ruleMatrixRow;
        }
    }
    /* cout << "Here's the rules:\n'" << rules << "'" << endl;
    cout << "Number of line:" << ruleMatrixRow << endl; */


    ruleMatrix = new char*[this->ruleMatrixRow];
    for(int i = 0; i < this->ruleMatrixRow; ++i) {
        this->ruleMatrix[i] = new char[5];
    }

    string::iterator it = rules.begin();
    for( int i = 0; i < this->ruleMatrixRow; ++i ) {
        for( int j = 0; it != rules.end(); ++it ) {
            if(*it == ' ') {
                continue;
            }
            if(*it == '\n') {
                ++it;
                break;
            }
            this->ruleMatrix[i][j] = *it;
            ++j;
        }
    }

    return !error;
}

/* input can be size 0, some langauges can accept the empty string. */
bool TuringMachine::setTape(string input) {
    this->clearTape();
    if(input.length() == 0) {
        return true;
    }
    string::iterator it = input.end();
    /* execute then check the condition */
    do {
        --it;
        this->right.push(*it);
    } while( it != input.begin() );

    return true;
}

/* MUST CONSOLT ERIC, for input 00001 and 0, prety much anything that starts with 0, gets accepted. and if I think of my 
 * tape as begining with all 'B', and following the input state transistion, it make sense. row 1 to row 3
 *  */
bool TuringMachine::run() {
    this->cur_state = '0';  /* Make sure startin state matches what is giving in the file */

    char cur_symbol = 0;

    while(cur_symbol != 'f') {
        if(this->right.empty()) {
            cur_symbol = 'B';
        } else {
            cur_symbol = this->right.top();
        }

        int i = 0;
        for(; i < this->ruleMatrixRow; ++i) {
            
            // If current state and current symbol is found on an instnace in the transition table, do something then break overhead loop.
            if( this->cur_state == this->ruleMatrix[i][0] && cur_symbol == this->ruleMatrix[i][1] ) { 
                cout << "Tape:\t";
                this->printTape();
                cout << endl;
    
                // set new state
                this->cur_state = this->ruleMatrix[i][2];
                if(this->cur_state == 'f') {
                    return true;
                }
                // set new tape symbol (modifying top of right stack)
                if( right.empty() ) {               // if right is empty, you can access its top elment, but will seg fault.
                    right.push(ruleMatrix[i][3]);
                } else {
                    right.top() = ruleMatrix[i][3];
                }
                    
                // determine l->r or r->l, and shift stuff between the two stack
                if( ruleMatrix[i][4] == 'L' ) {      // Head move to Left
                    char tmp;
                    if(left.empty()) {
                        tmp = 'B';
                    } else {
                        tmp = left.top();
                        left.pop();
                    }
                    right.push(tmp);
                } else {    // Head move to Right, NOTE: no chekcing for == 'R' here! Can cause undefined behavior!
                    char tmp;
                    if(right.empty()) {
                        tmp = 'B';
                    } else {
                        tmp = right.top();
                        right.pop();
                    }
                    left.push(tmp);
                }
                // pop and push the correct side
                break;
            }
        }

        if( i==ruleMatrixRow ) {  /* from current state, can't find what to do from transition table, REJECT string! String not in language described by transition table */
            return false;
        }

    }

    return true;
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
    cout << "___Turing Machine______________________________________________________________________________________________" << endl;
    cout << "This is a deterministic Turing Machine. It won't work if you put in a transition table that use non-detminism!|" << endl;
    cout << "______________________________________________________________________________________________________________|" << endl;
    /* Set the rule */
    cout << "Loading in rule...\n" << endl;
    if( TM.setRules(in_file) ) {
        cout << "Rules loaded in successfully!\n" << endl;
    } else {
        cout << "Some rules may not have loaded in successfully (Input file with comments or junks can cause this)" << endl;
    }
    cout << "Here is the transition table:\n" << endl;
    TM.printRule();
    cout << "(note: you may cause the Turing Machine to run forever if transition table describes a Recursively Enumerable Language)" << endl;
    
    /* Get input string from keybaord */
    cout << "\nEnter any string and we will test if it is in the language." << endl;
    cout << "To quit, type in 'quit' without the single quote." << endl;
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
            cout << "______________" << endl;
            cout << "YEP!" << endl;
        } else {
            cout << "______________" << endl;
            cout <<"NOPE!" << endl;
        }

        cout << endl << endl;
        cout << " > ";
    }
    


    return 0;
}