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

void ReTONFA(string re, NFA &nfa);

void printNFA(NFA nfa);

void printNfaData(NFA nfa);

// 正则式RE作为命令行参数输入：实现源程序的输入，从键盘输入流输入正则表达式，要考虑正则表达式的合法性，出现非法字符则Error
void inputRE(string &re) {
    cout << "请输入正则表达式：" << endl;
    cin >> re;
    if (!isLegalRE(re)) {
        cout << "Error: 正则表达式不合法" << endl;
        exit(1);
    }
}

// 判断正则表达式是否合法
bool isLegalRE(string re) {
    for (auto c: re) {
    }
    return true;
}

// 设计一个函数（方法），实现把RE确定化成一个NFA：将RE转换为相应的NFA，可以通过Thompson算法
void ReTONFA(string re, NFA &nfa) {
}


// 输出转换好的NFA到标准输出设备：将转换好的NFA按照文档格式输出
void printNFA(NFA nfa) {
}

// 打印NFA数据(调试用)
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
    // NFA nfa;
    // ReTONFA(re, nfa);
    // printNFA(nfa);
    return 0;
}
