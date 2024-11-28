#include <bits/stdc++.h>
using namespace std;

// NFA��
class NFA {
public:
    vector<int> stateSets;
    int stateSetSize{};

    vector<int> charSet;
    int charSetSize{};

    vector<vector<int> > transTable;
    int transTableSize{};

    vector<int> startStates;

    vector<int> finalStates;

    bool isStartState(auto state) {
        return startStates[0] == state;
    }

    bool isFinalState(auto state) {
        return find(finalStates.begin(), finalStates.end(), state) != finalStates.end();
    }
};

// ��������
void isValidRegex(const string &re);
string addExplicitConcat(const string &re);
void infixToPostfix(const string &infix, string &postfix);
int getPrecedence(auto op);
void updateNFAMetadata(NFA *nfa);
NFA constructBasicNFA(char c, int &stateCounter);
void removeDuplicates(NFA &nfa);
NFA concatenateNFA(NFA &nfa1, NFA &nfa2);
NFA unionNFA(NFA &nfa1, NFA &nfa2, int &stateCounter);
NFA starNFA(NFA &nfa, int &stateCounter);
void reToNfa(const string &postfix, NFA *nfa);
void printNFA(NFA nfa);

// ���������ʽ�Ƿ�Ϸ�
void isValidRegex(const string &re) {
    // ����Ƿ�����Ƿ��ַ�
    for (const char c: re) {
        if (!(isalnum(c) || c == '(' || c == ')' || c == '*' || c == '|' || c == '.' || c == '+')) {
            cout << "����ʽ�����Ƿ��ַ�" << endl;
            exit(1);
        }
    }
    // ��������Ƿ�ƥ��
    stack<char> s;
    for (char c: re) {
        if (c == '(') {
            s.push(c);
        } else if (c == ')') {
            if (s.empty()) {
                cout << "����ʽ���Ų�ƥ��" << endl;
                exit(1);
            }
            s.pop();
        }
    }
}

// �ڲ�����֮�������ʽ���ӷ�
string addExplicitConcat(const string &re) {
    string newRe;
    for (auto i = 0; i < re.size(); ++i) {
        newRe += re[i];
        if (i + 1 < re.size() && (isalnum(re[i]) || re[i] == '*' || re[i] == ')') && (
                isalnum(re[i + 1]) || re[i + 1] == '(')) {
            newRe += '.';
        }
    }
    return newRe;
}

// ��ȡ�����������ȼ�
int getPrecedence(auto op) {
    if (op == '*') return 3;
    if (op == '.') return 2;
    if (op == '|') return 1;
    return 0;
}

// ����׺���ʽת��Ϊ��׺���ʽ
void infixToPostfix(const string &infix, string &postfix) {
    stack<char> ops;
    for (char c: infix) {
        if (isalnum(c)) {
            postfix += c;
        } else if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            // ��ջ�е��������ӵ���׺���ʽ��,ֱ������ '('
            while (!ops.empty() && ops.top() != '(') {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop();
        } else {
            // ��ջ�����ȼ����ڵ��ڵ�ǰ���������������ӵ���׺���ʽ��,ֱ��ջΪ�ջ��������ȼ����͵������
            while (!ops.empty() && getPrecedence(ops.top()) >= getPrecedence(c)) {
                postfix += ops.top();
                ops.pop();
            }
            // ����ǰ��������ջ
            ops.push(c);
        }
    }
    // ��ջ��ʣ����������ӵ���׺���ʽ��
    while (!ops.empty()) {
        postfix += ops.top();
        ops.pop();
    }
}

// ����NFAԪ����
void updateNFAMetadata(NFA *nfa) {
    nfa->stateSetSize = nfa->stateSets.size();
    nfa->charSetSize = nfa->charSet.size();
    nfa->transTableSize = nfa->transTable.size();
}

// ���쵥�ַ���NFA
NFA constructBasicNFA(char c, int &stateCounter) {
    NFA nfa;
    nfa.stateSets = {stateCounter, stateCounter + 1};
    nfa.startStates = {stateCounter};
    nfa.finalStates = {stateCounter + 1};
    nfa.charSet.push_back(c);
    nfa.transTable.push_back({stateCounter, c, stateCounter + 1});
    stateCounter += 2;
    updateNFAMetadata(&nfa);
    return nfa;
}

// ��NFA�ڵ��������ȥ��
void removeDuplicates(NFA &nfa) {
    // ȥ��״̬��
    ranges::sort(nfa.stateSets);
    nfa.stateSets.erase(ranges::unique(nfa.stateSets).begin(), nfa.stateSets.end());
    // ȥ���ַ���
    ranges::sort(nfa.charSet);
    nfa.charSet.erase(ranges::unique(nfa.charSet).begin(), nfa.charSet.end());
    // ȥ��ת�Ʊ�
    ranges::sort(nfa.transTable);
    nfa.transTable.erase(ranges::unique(nfa.transTable).begin(), nfa.transTable.end());
    // ȥ�ؿ�ʼ״̬
    nfa.startStates.erase(ranges::unique(nfa.startStates).begin(), nfa.startStates.end());
    // ȥ����ֹ״̬
    nfa.finalStates.erase(ranges::unique(nfa.finalStates).begin(), nfa.finalStates.end());
}

// ����NFA
NFA concatenateNFA(NFA &nfa1, NFA &nfa2) {
    NFA result;

    result.stateSets = nfa1.stateSets;
    result.stateSets.insert(result.stateSets.end(), nfa2.stateSets.begin(), nfa2.stateSets.end());

    result.charSet = nfa1.charSet;
    result.charSet.insert(result.charSet.end(), nfa2.charSet.begin(), nfa2.charSet.end());

    for (int finalState: nfa1.finalStates) {
        result.transTable.push_back({finalState, -1, nfa2.startStates[0]});
    }
    result.transTable.insert(result.transTable.end(), nfa1.transTable.begin(), nfa1.transTable.end());
    result.transTable.insert(result.transTable.end(), nfa2.transTable.begin(), nfa2.transTable.end());

    result.startStates = nfa1.startStates;
    result.finalStates = nfa2.finalStates;

    removeDuplicates(result);
    updateNFAMetadata(&result);

    return result;
}

// ����NFA
NFA unionNFA(NFA &nfa1, NFA &nfa2, int &stateCounter) {
    NFA result;
    int newStart = stateCounter++;
    int newFinal = stateCounter++;

    result.stateSets = {newStart};
    result.stateSets.insert(result.stateSets.end(), nfa1.stateSets.begin(), nfa1.stateSets.end());
    result.stateSets.insert(result.stateSets.end(), nfa2.stateSets.begin(), nfa2.stateSets.end());
    result.stateSets.push_back(newFinal);

    result.charSet = nfa1.charSet;
    result.charSet.insert(result.charSet.end(), nfa2.charSet.begin(), nfa2.charSet.end());

    result.transTable.push_back({newStart, -1, nfa1.startStates[0]});
    result.transTable.push_back({newStart, -1, nfa2.startStates[0]});
    for (int finalState: nfa1.finalStates) {
        result.transTable.push_back({finalState, -1, newFinal});
    }
    for (int finalState: nfa2.finalStates) {
        result.transTable.push_back({finalState, -1, newFinal});
    }
    result.transTable.insert(result.transTable.end(), nfa1.transTable.begin(), nfa1.transTable.end());
    result.transTable.insert(result.transTable.end(), nfa2.transTable.begin(), nfa2.transTable.end());

    result.startStates = {newStart};
    result.finalStates = {newFinal};

    removeDuplicates(result);
    updateNFAMetadata(&result);

    return result;
}

// �հ�NFA
NFA starNFA(NFA &nfa, int &stateCounter) {
    NFA result;
    int newStart = stateCounter++;
    int newFinal = stateCounter++;

    result.stateSets = {newStart};
    result.stateSets.insert(result.stateSets.end(), nfa.stateSets.begin(), nfa.stateSets.end());
    result.stateSets.push_back(newFinal);

    result.charSet = nfa.charSet;

    result.transTable.push_back({newStart, -1, nfa.startStates[0]});
    result.transTable.push_back({newStart, -1, newFinal});
    for (int finalState: nfa.finalStates) {
        result.transTable.push_back({finalState, -1, newFinal});
        result.transTable.push_back({finalState, -1, nfa.startStates[0]});
    }
    result.transTable.insert(result.transTable.end(), nfa.transTable.begin(), nfa.transTable.end());

    result.startStates = {newStart};

    result.finalStates = {newFinal};

    removeDuplicates(result);
    updateNFAMetadata(&result);

    return result;
}

void reToNfa(const string &postfix, NFA *nfa) {
    stack<NFA> nfaStack;
    int stateCounter = 0;

    for (char c: postfix) {
        if (isalnum(c)) {
            NFA nfa0 = constructBasicNFA(c, stateCounter);
            nfaStack.push(nfa0);
        } else if (c == '.') {
            // ���Ӳ���,��ջ��������NFA��������
            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            NFA result = concatenateNFA(nfa1, nfa2);
            nfaStack.push(result);
        } else if (c == '|') {
            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            NFA result = unionNFA(nfa1, nfa2, stateCounter);
            nfaStack.push(result);
        } else if (c == '*') {
            NFA nfa0 = nfaStack.top();
            nfaStack.pop();
            NFA result = starNFA(nfa0, stateCounter);
            nfaStack.push(result);
        }
    }

    *nfa = nfaStack.top();
}

// ��ӡNFA��Ϣ
void printNFA(NFA nfa) {
    cout << "NFA" << endl;
    cout << "״̬������" << nfa.stateSetSize << endl;
    cout << "�ַ�������" << nfa.charSetSize << endl;

    cout << "״̬ת�ƣ�" << endl;
    for (auto &row: nfa.transTable) {
        cout << '(' << row[0] << ", ";
        if (row[1] == -1) cout << "��";
        else cout << static_cast<char>(row[1]);
        cout << ") -> ";
        for (size_t i = 2; i < row.size(); ++i) {
            cout << row[i] << (i == row.size() - 1 ? "" : ", ");
        }
        cout << endl;
    }

    cout << "��ʼ״̬��" << nfa.startStates[0] << endl;
    cout << "�ս�״̬��" << "{ ";
    for (int state: nfa.finalStates) {
        cout << state << " ";
    }
    cout << " } "<< endl;
}

int main() {
    string re;
    cout << "����������ʽ: ";
    cin >> re;

    // ����
    // re = "a(b|c)*abc";
    // re = "a*b";

    isValidRegex(re);

    re = addExplicitConcat(re);
    // cout << "������ӷ��������ʽ: " << re << endl;

    string postfix;
    infixToPostfix(re, postfix);
    // cout << "��׺���ʽ: " << postfix << endl;

    NFA nfa;
    reToNfa(postfix, &nfa);

    printNFA(nfa);

    return 0;
}
