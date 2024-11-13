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

void ReTONFA(string re, NFA &nfa);

void printNFA(NFA nfa);

void printNfaData(NFA nfa);

// ����ʽRE��Ϊ�����в������룺ʵ��Դ��������룬�Ӽ�������������������ʽ��Ҫ����������ʽ�ĺϷ��ԣ����ַǷ��ַ���Error
void inputRE(string &re) {
    cout << "������������ʽ��" << endl;
    cin >> re;
    if (!isLegalRE(re)) {
        cout << "Error: ������ʽ���Ϸ�" << endl;
        exit(1);
    }
}

// �ж�������ʽ�Ƿ�Ϸ�
bool isLegalRE(string re) {
    for (auto c: re) {
    }
    return true;
}

// ���һ����������������ʵ�ְ�REȷ������һ��NFA����REת��Ϊ��Ӧ��NFA������ͨ��Thompson�㷨
void ReTONFA(string re, NFA &nfa) {
}


// ���ת���õ�NFA����׼����豸����ת���õ�NFA�����ĵ���ʽ���
void printNFA(NFA nfa) {
}

// ��ӡNFA����(������)
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
    // NFA nfa;
    // ReTONFA(re, nfa);
    // printNFA(nfa);
    return 0;
}
