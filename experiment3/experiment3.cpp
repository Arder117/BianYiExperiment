//
// Created by ZhaoHao on 24-11-5.
//

#include <bits/stdc++.h>
using namespace std;

// NFA类
class NFA {
public:
    // 状态集
    vector<int> stateSets;
    // 状态集大小
    int stateSetSize;

    // 字符集
    vector<int> charSet;
    // 字符集大小
    int charSetSize;

    // 状态转移表
    vector<vector<int> > transTable;
    // 状态数
    int transTableSize;

    // 初始状态集合
    vector<int> startStates;

    // 终止状态
    vector<int> finalStates;

    // 判断是否为终止状态
    bool isFinalState(int state) {
        for (int finalState: finalStates) {
            if (finalState == state) {
                return true;
            }
        }
        return false;
    }

    // 判断是否为初始状态
    bool isStartState(int state) {
        if (startStates[0] == state) {
            return true;
        }
        return false;
    }
};

// 函数声明
void inputRE(string &re);

bool isLegalRE(string re);

void reTONFA(string re, NFA &nfa);

int getPrecedence(char op);

void infixToPostfix(string infix, string &postfix);

void printNFA(NFA nfa);

void printNfaData(NFA nfa);

// 正则式RE作为命令行参数输入：实现源程序的输入，从键盘输入流输入正则表达式，要考虑正则表达式的合法性，出现非法字符则Error
void inputRE(string &re) {
    cout << "请输入正则表达式：";
    cin >> re;
    if (!isLegalRE(re)) {
        cout << "Error: 正则表达式不合法" << endl;
        exit(1);
    }
}

// 判断正则表达式是否合法
vector<char> legalChar = {'(', ')', '*', '|', '+', '?', '[', ']', '{', '}', '.'};

bool isLegalRE(string re) {
    for (auto c: re) {
        if (!isdigit(c) && !isalpha(c)) {
            bool isLegal = false;
            for (auto legal: legalChar) {
                if (c == legal) {
                    isLegal = true;
                    break;
                }
            }
            if (!isLegal) return false;
        }
    }
    return true;
}


// 设计一个函数（方法），实现把RE确定化成一个NFA：将RE转换为相应的NFA，可以通过Thompson算法
void reTONFA(string re, NFA &nfa) {
    // 将中缀表达式转换为后缀表达式
    string postfix;
    infixToPostfix(re, postfix);

    stack<NFA> nfaStack;

    for (char c: postfix) {
        if (isalnum(c)) {
            // 如果是字符，则创建一个基本NFA
            NFA basicNFA;
            basicNFA.stateSets = {0, 1}; // 状态集，状态0为起始，1为终止
            basicNFA.startStates = {0}; // 初始状态
            basicNFA.finalStates = {1}; // 终止状态
            basicNFA.transTable = vector<vector<int> >(2);
            basicNFA.transTable[0].push_back(c); // 状态0通过字符c转移到状态1
            nfaStack.push(basicNFA);
        } else if (c == '.') {
            // 连接操作
            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            // 将nfa1的终止状态连接到nfa2的起始状态
            for (int finalState: nfa1.finalStates) {
                nfa1.transTable[finalState].push_back(nfa2.startStates[0]);
            }
            nfa1.finalStates = nfa2.finalStates;
            nfa1.stateSets.insert(nfa1.stateSets.end(), nfa2.stateSets.begin(), nfa2.stateSets.end());
            nfa1.transTable.insert(nfa1.transTable.end(), nfa2.transTable.begin(), nfa2.transTable.end());
            nfaStack.push(nfa1);
        } else if (c == '|') {
            // 并联操作
            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            NFA newNFA;
            int start = 0, end = nfa1.stateSets.size() + nfa2.stateSets.size() + 1;
            newNFA.startStates = {start};
            newNFA.finalStates = {end};
            newNFA.stateSets = {start};
            newNFA.stateSets.insert(newNFA.stateSets.end(), nfa1.stateSets.begin(), nfa1.stateSets.end());
            newNFA.stateSets.insert(newNFA.stateSets.end(), nfa2.stateSets.begin(), nfa2.stateSets.end());
            newNFA.stateSets.push_back(end);
            newNFA.transTable = vector<vector<int> >(end + 1);
            newNFA.transTable[start].push_back(nfa1.startStates[0]);
            newNFA.transTable[start].push_back(nfa2.startStates[0]);
            for (int finalState: nfa1.finalStates) {
                newNFA.transTable[finalState].push_back(end);
            }
            for (int finalState: nfa2.finalStates) {
                newNFA.transTable[finalState].push_back(end);
            }
            nfaStack.push(newNFA);
        } else if (c == '*') {
            // 闭包操作
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            NFA newNFA;
            int start = 0, end = nfa1.stateSets.size() + 1;
            newNFA.startStates = {start};
            newNFA.finalStates = {end};
            newNFA.stateSets = {start};
            newNFA.stateSets.insert(newNFA.stateSets.end(), nfa1.stateSets.begin(), nfa1.stateSets.end());
            newNFA.stateSets.push_back(end);
            newNFA.transTable = vector<vector<int> >(end + 1);
            newNFA.transTable[start].push_back(nfa1.startStates[0]);
            newNFA.transTable[start].push_back(end);
            for (int finalState: nfa1.finalStates) {
                newNFA.transTable[finalState].push_back(nfa1.startStates[0]);
                newNFA.transTable[finalState].push_back(end);
            }
            nfaStack.push(newNFA);
        }
    }

    // 栈中最终结果即为完整的NFA
    nfa = nfaStack.top();
}


// 将中缀表达式转换为后缀表达式
int getPrecedence(char op) {
    if (op == '*') return 3;
    if (op == '.' || op == '|') return 2;
    return 1;
}

void infixToPostfix(string infix, string &postfix) {
    stack<char> ops;
    for (char c: infix) {
        if (isalnum(c)) {
            postfix += c;
        } else if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop();
        } else {
            while (!ops.empty() && getPrecedence(ops.top()) >= getPrecedence(c)) {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);
        }
    }
    while (!ops.empty()) {
        postfix += ops.top();
        ops.pop();
    }
}


// 最终输出结果
void printNFA(NFA nfa) {
    cout << "NFA" << endl;
    cout << "状态个数：" << nfa.stateSetSize << endl;
    cout << "字符个数：" << nfa.charSetSize << endl;

    cout << "状态转移：" << endl;
    for (int i = 0; i < nfa.transTableSize; i++) {
        for (int j = 0; j < nfa.transTable[i].size(); j++) {
            if (j == 0) {
                cout << '(' << nfa.transTable[i][j] << ',';
            } else if (j == 1) {
                cout << nfa.transTable[i][j] << ")->";
            } else {
                cout << nfa.transTable[i][j] << endl;
            }
        }
    }

    cout << "开始状态：";
    for (int state: nfa.startStates) {
        cout << state << " ";
    }
    cout << endl;

    cout << "终止状态集：" << "{ ";
    for (int state: nfa.finalStates) {
        cout << state << " ";
    }
    cout << " }" << endl;
}

// 打印NFA数据，用于调试
void printNfaData(NFA nfa) {
    cout << "状态集：" << endl;
    for (int state: nfa.stateSets) {
        cout << state << " ";
    }
    cout << endl;

    cout << "字符集：" << endl;
    for (auto c: nfa.charSet) {
        cout << c << " ";
    }
    cout << endl;

    cout << "状态转移个数：" << endl;
    cout << nfa.transTableSize << endl;
    cout << "状态转移表：" << endl;
    for (vector<int> transRow: nfa.transTable) {
        for (int state: transRow) {
            cout << state << " ";
        }
        cout << endl;
    }

    cout << "初始状态：" << endl;
    for (int state: nfa.startStates) {
        cout << state << " " << endl;
    }

    cout << "终止状态：" << endl;
    for (int state: nfa.finalStates) {
        cout << state << " ";
    }
    cout << endl;
}

int main() {
    string re;
    inputRE(re);
    NFA nfa;
    reTONFA(re, nfa);
    printNFA(nfa);
    return 0;
}
