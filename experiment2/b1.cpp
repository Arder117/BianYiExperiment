//
// Created by ZhaoHao on 24-10-30.
//

// 1. 设计一个函数（方法），实现把两个NFA的合并；
// 2. 设计一个函数（方法），实现把NFA确定化成一个DFA；
// 3. 设计一个函数（方法），实现把DFA最小化；
// 4. 输入多个NFA：NFA描述存储在文本文件中，文件名作为命令行参数输入；
// 5. 输出合并、最小化以后的DFA到标准输出设备。

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

// DFA类
class DFA {
public:
    // 状态集
    vector<vector<int> > stateSet;
    // 状态集大小
    int stateSetSize;

    vector<int> newStates;
    int newStatesSize;

    // 字符集
    vector<int> charSet;
    // 字符集大小
    int charSetSize;

    // 状态转移表,每行第一个元素为当前状态，第二个元素为字符，后面的多个元素为转移后的状态
    vector<vector<int> > transTable;
    // 状态数
    int transTableSize;

    // 初始状态
    int startState;

    // 终止状态
    vector<int> finalStates;

    // 初始化DFA
    void initDFA(NFA nfa) {
        nfa.charSet = {};
        nfa.charSetSize = 0;
        nfa.transTable = {};
        nfa.transTableSize = 0;
        nfa.startStates = {};
        nfa.finalStates = {};
        nfa.stateSetSize = 0;
        nfa.stateSets = {};
    }

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
        if (startState == state) {
            return true;
        }
        return false;
    }
};

// 函数声明
void printNfaData(NFA nfa);

void printDfaData(DFA dfa);

void printDfa(DFA dfa);

// 读取NFA描述文件
NFA readNfaFromFile(string filename) {
    ifstream ReadFile;
    ReadFile.open(filename, ios::in);
    if (ReadFile.fail()) {
        cout << "打开文件失败" << endl;
        exit(1);
    }

    NFA nfa;
    int rowindex = 0;
    string line;

    while (!ReadFile.eof()) {
        getline(ReadFile, line);
        if (!line.empty()) {
            rowindex++;

            // 状态集
            if (rowindex == 1) {
                stringstream ss(line);
                int state = 0;
                int stateSize = 0;

                while (ss >> state) {
                    nfa.stateSets.push_back(state);
                    stateSize++;
                }
                nfa.stateSetSize = stateSize;
            }

            // 字符集
            else if (rowindex == 2) {
                stringstream ss(line);
                int c;
                while (ss >> c) {
                    nfa.charSet.push_back(c);
                }
                nfa.charSetSize = nfa.charSet.size();
            }

            // 状态数
            else if (rowindex == 3) {
                nfa.transTableSize = stoi(line);
            }

            // 状态转移表
            else if (rowindex < 3 + nfa.transTableSize + 1 && rowindex > 3) {
                vector<int> transRow;
                stringstream ss(line);
                int state;
                while (ss >> state) {
                    transRow.push_back(state);
                }
                nfa.transTable.push_back(transRow);
            }

            // 初始状态
            else if (rowindex == 3 + nfa.transTableSize + 1) {
                stringstream ss(line);
                int state = 0;
                while (ss >> state) {
                    nfa.startStates.push_back(state);
                }
            }

            // 终止状态
            if (rowindex == 3 + nfa.transTableSize + 2) {
                stringstream ss(line);
                int state = 0;
                while (ss >> state) {
                    nfa.finalStates.push_back(state);
                }
            }
        }
    }

    // 将状态转移表的元素转换为字符
    for (int i = 0; i < nfa.transTableSize; i++) {
        for (int j = 0; j < nfa.transTable[i].size(); j++) {
            nfa.transTable[i][j] = nfa.transTable[i][j];
        }
    }

    return nfa;
}

// 合并两个数组，去重
void mergeArray(vector<int> array1, vector<int> array2, vector<int> finalArray) {
    set<int> arraySet;
    for (auto &i: array1) {
        arraySet.insert(i);
    }
    for (auto &i: array2) {
        arraySet.insert(i);
    }
    finalArray.assign(arraySet.begin(), arraySet.end());
}

// 合并两个NFA
NFA mergeNFA(NFA nfa1, NFA nfa2) {
    NFA nfa;

    // 初始状态集
    // 生成新起始状态：新起始状态编号为两个NFA状态集合中最大值加1。
    nfa.startStates.push_back(max(*max_element(nfa1.stateSets.begin(), nfa1.stateSets.end()),
                                  *max_element(nfa2.stateSets.begin(), nfa2.stateSets.end())) + 1);

    // 状态集
    // 合并状态：包括两个NFA的状态集合和新起始状态。
    mergeArray(nfa1.stateSets, nfa2.stateSets, nfa.stateSets);

    // 合并终止状态：直接合并两个NFA的终止状态
    mergeArray(nfa1.finalStates, nfa2.finalStates, nfa.finalStates);

    // 合并转移函数：通过字典扩展合并两个NFA的转移函数
    for (int i = 0; i < nfa1.transTableSize; i++) {
        nfa.transTable.push_back(nfa1.transTable[i]);
    }
    for (int i = 0; i < nfa2.transTableSize; i++) {
        nfa.transTable.push_back(nfa2.transTable[i]);
    }

    // 添加空转移：从新起始状态到原两个NFA起始状态添加空转移。
    for (int state: nfa1.startStates) {
    }

    return nfa;
}

// 通过广度遍历求state的空闭包ε_closure
vector<int> closure(NFA nfa, vector<int> states) {
    vector<int> closureStates;
    queue<int> queueStates;

    // 如果0在startStates中，则加入closureStates
    if (ranges::find(states, 0) != states.end()) {
        closureStates.push_back(0);
    }

    // 初始化队列
    for (int state: states) {
        queueStates.push(state);
    }

    // 广度遍历
    while (!queueStates.empty()) {
        int state = queueStates.front();
        queueStates.pop();
        // closureStates.push_back(state);
        for (int i = 0; i < nfa.transTableSize; i++) {
            if (nfa.transTable[i][0] == state && nfa.transTable[i][1] == -1) {
                for (int j = 2; j < nfa.transTable[i].size(); j++) {
                    if (ranges::find(closureStates, nfa.transTable[i][j]) == closureStates.end()) {
                        queueStates.push(nfa.transTable[i][j]);
                        closureStates.push_back(nfa.transTable[i][j]);
                    }
                }
            }
        }
    }

    // 去重
    sort(closureStates.begin(), closureStates.end());
    closureStates.erase(unique(closureStates.begin(), closureStates.end()), closureStates.end());

    return closureStates;
}

// 比较两个字符是否相等
int cmp(int c1, int c2) {
    if (c1 == c2) {
        return 1;
    }
    return 0;
}

// 状态转移
vector<int> transition(NFA nfa, vector<int> states, int c) {
    vector<int> transitionStates;

    queue<int> queueStates;

    for (int state: states) {
        for (int i = 0; i < nfa.transTableSize; i++) {
            if (nfa.transTable[i][0] == state && cmp(nfa.transTable[i][1], c) == 1) {
                for (int j = 2; j < nfa.transTable[i].size(); j++) {
                    if (ranges::find(transitionStates, nfa.transTable[i][j]) == transitionStates.end()) {
                        transitionStates.push_back(nfa.transTable[i][j]);
                        queueStates.push(nfa.transTable[i][j]); //将转移后的状态加入队列
                    }
                }
            }
        }
    }

    // 将状态集中的空闭包加入到转移后的状态集中
    while (!queueStates.empty()) {
        int state = queueStates.front();
        queueStates.pop();
        vector<int> closureStates = closure(nfa, {state});
        for (int closureState: closureStates) {
            if (ranges::find(transitionStates, closureState) == transitionStates.end()) {
                transitionStates.push_back(closureState);
            }
        }
    }

    // 去重
    sort(transitionStates.begin(), transitionStates.end());
    transitionStates.erase(unique(transitionStates.begin(), transitionStates.end()), transitionStates.end());

    return transitionStates;
}

// NFA转换成DFA
void nfaToDfa(NFA nfa, DFA *dfa) {
    // 初始化DFA
    dfa->charSet = nfa.charSet;
    dfa->transTableSize = 0;

    vector<int> closureStates = closure(nfa, nfa.startStates);
    // 使用states来存储状态集，下标为状态集的新编号，值为状态集
    dfa->stateSet.push_back(closureStates);

    // 标记状态，0表示未标记，1表示已标记
    vector<int> markedStates = {0};
    markedStates[0] = 1;

    // 广度优先搜索
    queue<vector<int> > queueStates;

    // 将初始状态的空闭包加入队列
    queueStates.push(closureStates);

    int transTableIndex = 0;
    // 遍历队列
    while (!queueStates.empty()) {
        // 取出队列中的状态
        vector<int> states = queueStates.front();

        // 遍历字符集
        for (int c: nfa.charSet) {
            // 状态转移
            vector<int> transitionStates = transition(nfa, states, c);

            // 状态转移为空，跳过
            if (transitionStates.empty()) {
                continue;
            }

            // 状态转移后的状态集是否已存在
            bool exist = false;
            int existIndex = -1;
            for (int i = 0; i < dfa->stateSet.size(); i++) {
                if (dfa->stateSet[i] == transitionStates) {
                    exist = true;
                    existIndex = i;
                }
            }

            // 状态转移后的状态集不存在，加入状态集
            if (!exist) {
                dfa->stateSet.push_back(transitionStates);
                markedStates.push_back(0);
                existIndex = dfa->stateSet.size() - 1;
                queueStates.push(transitionStates);
                markedStates[existIndex] = 1;
            }

            // 查找状态转移前的状态集的编号
            int beforeIndex = 0;
            for (int i = 0; i < dfa->stateSet.size(); i++) {
                if (dfa->stateSet[i] == states) {
                    beforeIndex = i;
                }
            }

            // 添加状态转移
            dfa->transTable.push_back(vector<int>(3, 0));
            dfa->transTable[transTableIndex][0] = beforeIndex;
            dfa->transTable[transTableIndex][1] = c;
            dfa->transTable[transTableIndex][2] = existIndex;
            transTableIndex++;
        }

        queueStates.pop();
    }

    dfa->charSetSize = nfa.charSet.size();
    dfa->transTableSize = transTableIndex;

    // 设置初始状态
    dfa->startState = 0;
    dfa->newStatesSize = 0;
    // 设置状态集
    for (int i = 0; i < dfa->stateSet.size(); i++) {
        dfa->newStates.push_back(i);
        dfa->newStatesSize++;
    }

    // 设置终止状态
    for (int i = 0; i < dfa->stateSet.size(); i++) {
        for (int state: dfa->stateSet[i]) {
            if (nfa.isFinalState(state)) {
                dfa->finalStates.push_back(i);
                break;
            }
        }
    }
}

// DFA最小化
DFA minimizeDfa(DFA dfa) {
    // 初始化DFA
    DFA minimizedDfa = DFA();

    // 初始化状态集，将终止状态和非终止状态分开
    vector<vector<int> > stateSets = {dfa.finalStates, {}};


    return dfa;
}

// 输出DFA
void printDfa(DFA dfa) {
    cout << "DFA" << endl;
    cout << "状态集： " << "{ ";
    for (auto state: dfa.stateSet) {
        for (auto state2: state) {
            cout << state2 << " ";
        }
        cout << endl;
    }
    cout << "}" << endl;

    cout << "符号表： " << "{ ";
    for (int c: dfa.charSet) {
        cout << c << " ";
    }
    cout << "}" << endl;

    cout << "状态转移： " << endl;
    for (int i = 0; i < dfa.transTableSize; i++) {
        cout << "(" << dfa.transTable[i][0] << ", " << dfa.transTable[i][1] << ")" << " -> " << dfa.transTable[i][2]
                <<
                endl;
    }

    cout << "开始状态： " << dfa.startState << endl;

    cout << "结束状态集： " << "{ ";
    for (int state: dfa.finalStates) {
        cout << state << " ";
    }
    cout << "}" << endl;
}

// 打印NFA数据
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

    cout << "状态数：" << endl;
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

// 打印DFA数据
void printDfaData(DFA dfa) {
    cout << "状态集：" << endl;
    int index = 0;
    for (auto state: dfa.stateSet) {
        cout << "DFA: " << index++ << " " << "NFA: ";
        for (auto state2: state) {
            cout << state2 << " ";
        }
        cout << endl;
    }

    cout << "新状态集：" << endl;
    for (int state: dfa.newStates) {
        cout << state << " ";
    }
    cout << endl;

    cout << "字符集：" << endl;
    for (auto c: dfa.charSet) {
        cout << c << " ";
    }
    cout << endl;

    cout << "状态数：" << endl;
    cout << dfa.transTableSize << endl;
    cout << "状态转移表：" << endl;
    for (vector<int> transRow: dfa.transTable) {
        for (auto state: transRow) {
            cout << state << " ";
        }
        cout << endl;
    }

    cout << "初始状态：" << endl << dfa.startState << endl;

    cout << "终止状态：" << endl;
    for (int state: dfa.finalStates) {
        cout << state << " ";
    }
    cout << endl;
}


int main(int argc, char *argv[]) {
    // NFA nfa1 = readNfaFromFile(argv[1]);
    // NFA nfa2 = readNfaFromFile(argv[2]);
    // NFA mergedNfa = mergeNFA(nfa1, nfa2);
    // DFA dfa;
    // nfaToDfa(nfa, dfa);
    // DFA minimizedDfa = minimizeDfa(dfa);
    // printDfa(minimizedDfa);


    // string filename = "D:/Projects/CLionProjects/Bianyi/experiment2/test";
    string filename1 = "D:/Projects/CLionProjects/Bianyi/experiment2/test1";
    // string filename2 = "D:/Projects/CLionProjects/Bianyi/experiment2/test2";

    NFA nfa = readNfaFromFile(filename1);
    // printNfaData(nfa);


    // 测试NFA转为DFA
    DFA dfa = DFA();
    nfaToDfa(nfa, &dfa);
    // printDfaData(dfa);
    // cout << endl;

    // 测试DFA最小化
    DFA minimizedDfa = minimizeDfa(dfa);
    printDfaData(minimizedDfa);


    return 0;
}