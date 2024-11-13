//
// Created by ZhaoHao on 24-11-5.
//

#include <bits/stdc++.h>
using namespace std;

// NFA��
class NFA {
public:
    // ״̬��
    vector<int> stateSets;
    // ״̬����С
    int stateSetSize;

    // �ַ���
    vector<int> charSet;
    // �ַ�����С
    int charSetSize;

    // ״̬ת�Ʊ�
    vector<vector<int> > transTable;
    // ״̬��
    int transTableSize;

    // ��ʼ״̬����
    vector<int> startStates;

    // ��ֹ״̬
    vector<int> finalStates;

    // �ж��Ƿ�Ϊ��ֹ״̬
    bool isFinalState(int state) {
        for (int finalState: finalStates) {
            if (finalState == state) {
                return true;
            }
        }
        return false;
    }

    // �ж��Ƿ�Ϊ��ʼ״̬
    bool isStartState(int state) {
        if (startStates[0] == state) {
            return true;
        }
        return false;
    }
};

// ��������
void inputRE(string &re);

bool isLegalRE(string re);

void reTONFA(string re, NFA &nfa);

int getPrecedence(char op);

void infixToPostfix(string infix, string &postfix);

void printNFA(NFA nfa);

void printNfaData(NFA nfa);

// ����ʽRE��Ϊ�����в������룺ʵ��Դ��������룬�Ӽ�������������������ʽ��Ҫ����������ʽ�ĺϷ��ԣ����ַǷ��ַ���Error
void inputRE(string &re) {
    cout << "������������ʽ��";
    cin >> re;
    if (!isLegalRE(re)) {
        cout << "Error: ������ʽ���Ϸ�" << endl;
        exit(1);
    }
}

// �ж�������ʽ�Ƿ�Ϸ�
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


// ���һ����������������ʵ�ְ�REȷ������һ��NFA����REת��Ϊ��Ӧ��NFA������ͨ��Thompson�㷨
void reTONFA(string re, NFA &nfa) {
    // ����׺���ʽת��Ϊ��׺���ʽ
    string postfix;
    infixToPostfix(re, postfix);

    stack<NFA> nfaStack;

    for (char c: postfix) {
        if (isalnum(c)) {
            // ������ַ����򴴽�һ������NFA
            NFA basicNFA;
            basicNFA.stateSets = {0, 1}; // ״̬����״̬0Ϊ��ʼ��1Ϊ��ֹ
            basicNFA.startStates = {0}; // ��ʼ״̬
            basicNFA.finalStates = {1}; // ��ֹ״̬
            basicNFA.transTable = vector<vector<int> >(2);
            basicNFA.transTable[0].push_back(c); // ״̬0ͨ���ַ�cת�Ƶ�״̬1
            nfaStack.push(basicNFA);
        } else if (c == '.') {
            // ���Ӳ���
            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            // ��nfa1����ֹ״̬���ӵ�nfa2����ʼ״̬
            for (int finalState: nfa1.finalStates) {
                nfa1.transTable[finalState].push_back(nfa2.startStates[0]);
            }
            nfa1.finalStates = nfa2.finalStates;
            nfa1.stateSets.insert(nfa1.stateSets.end(), nfa2.stateSets.begin(), nfa2.stateSets.end());
            nfa1.transTable.insert(nfa1.transTable.end(), nfa2.transTable.begin(), nfa2.transTable.end());
            nfaStack.push(nfa1);
        } else if (c == '|') {
            // ��������
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
            // �հ�����
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

    // ջ�����ս����Ϊ������NFA
    nfa = nfaStack.top();
}


// ����׺���ʽת��Ϊ��׺���ʽ
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


// ����������
void printNFA(NFA nfa) {
    cout << "NFA" << endl;
    cout << "״̬������" << nfa.stateSetSize << endl;
    cout << "�ַ�������" << nfa.charSetSize << endl;

    cout << "״̬ת�ƣ�" << endl;
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

    cout << "��ʼ״̬��";
    for (int state: nfa.startStates) {
        cout << state << " ";
    }
    cout << endl;

    cout << "��ֹ״̬����" << "{ ";
    for (int state: nfa.finalStates) {
        cout << state << " ";
    }
    cout << " }" << endl;
}

// ��ӡNFA���ݣ����ڵ���
void printNfaData(NFA nfa) {
    cout << "״̬����" << endl;
    for (int state: nfa.stateSets) {
        cout << state << " ";
    }
    cout << endl;

    cout << "�ַ�����" << endl;
    for (auto c: nfa.charSet) {
        cout << c << " ";
    }
    cout << endl;

    cout << "״̬ת�Ƹ�����" << endl;
    cout << nfa.transTableSize << endl;
    cout << "״̬ת�Ʊ�" << endl;
    for (vector<int> transRow: nfa.transTable) {
        for (int state: transRow) {
            cout << state << " ";
        }
        cout << endl;
    }

    cout << "��ʼ״̬��" << endl;
    for (int state: nfa.startStates) {
        cout << state << " " << endl;
    }

    cout << "��ֹ״̬��" << endl;
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
