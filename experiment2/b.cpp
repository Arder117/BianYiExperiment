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
    // 旧状态集
    vector<vector<int> > stateSet;
    // 状态集大小
    int stateSetSize;

    // 转换后的新状态集
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
    void initDFA() {
        stateSetSize = 0;
        transTableSize = 0;
        startState = 0;
        newStatesSize = 0;
        newStates.clear();
        finalStates.clear();
        stateSet.clear();
        transTable.clear();
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
void mergeArray(vector<int> array1, vector<int> array2, vector<int> *finalArray) {
    set<int> arraySet;
    for (auto &i: array1) {
        arraySet.insert(i);
    }
    for (auto &i: array2) {
        arraySet.insert(i);
    }
    finalArray->assign(arraySet.begin(), arraySet.end());
}

// 合并两个NFA
void mergeNFA(NFA nfa1, NFA nfa2, NFA *nfa) {
    // 生成新起始状态：
    nfa->startStates.push_back(max(*max_element(nfa1.stateSets.begin(), nfa1.stateSets.end()),
                                   *max_element(nfa2.stateSets.begin(), nfa2.stateSets.end())) + 1);

    nfa->stateSetSize = 1;

    // 合并状态：包括两个NFA的状态集合和新起始状态。
    mergeArray(nfa1.stateSets, nfa2.stateSets, &nfa->stateSets);
    nfa->stateSets.push_back(nfa->startStates[0]);

    nfa->stateSetSize = nfa->stateSets.size();

    // 合并终止状态：直接合并两个NFA的终止状态
    mergeArray(nfa1.finalStates, nfa2.finalStates, &nfa->finalStates);

    // 合并字符集
    mergeArray(nfa1.charSet, nfa2.charSet, &nfa->charSet);

    // 合并转移函数：通过字典扩展合并两个NFA的转移函数
    for (int i = 0; i < nfa1.transTableSize; i++) {
        nfa->transTable.push_back(nfa1.transTable[i]);
    }
    for (int i = 0; i < nfa2.transTableSize; i++) {
        nfa->transTable.push_back(nfa2.transTable[i]);
    }

    // 添加空转移：从新起始状态到原两个NFA起始状态添加空转移。
    for (int state: nfa1.startStates) {
        nfa->transTable.push_back({nfa->startStates[0], -1, state});
    }
    for (int state: nfa2.startStates) {
        nfa->transTable.push_back({nfa->startStates[0], -1, state});
    }
    nfa->transTable.erase(unique(nfa->transTable.begin(), nfa->transTable.end()), nfa->transTable.end());

    nfa->transTableSize = nfa->transTable.size();

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

int getTargetStateGroup(vector<vector<int> > &partition, int targetState);

vector<vector<int> > initializePartition(DFA dfa);

void splitPartition(DFA dfa, vector<int> &currentPartition, queue<vector<int> > &worklist,
                    vector<vector<int> > &partition);

void buildMinimizedDfa(DFA dfa, vector<vector<int> > &partition, DFA &minimizedDfa);

int getTargetState(DFA dfa, int stateIndex, int c);


// DFA最小化，使用Hopcroft算法最小化DFA
void minimizeDfa(DFA dfa, DFA *finalDfa) {
    // 初始化最小化后的DFA
    DFA minimizedDfa;
    minimizedDfa.initDFA();

    // 初始化分区
    vector<vector<int> > partition = initializePartition(dfa);

    // 初始化待处理的分区集合
    queue<vector<int> > worklist;
    for (auto &part: partition) {
        worklist.push(part);
    }

    // 开始递归分割
    while (!worklist.empty()) {
        vector<int> currentPartition = worklist.front();
        worklist.pop();

        // 对当前分区进行拆分
        splitPartition(dfa, currentPartition, worklist, partition);
    }


    // 最小化DFA的构造（将分区转化为新状态集和状态转移表）
    buildMinimizedDfa(dfa, partition, minimizedDfa);

    // 将最小化后的DFA赋值给finalDfa
    *finalDfa = minimizedDfa;
}

// 初始化分区：将DFA状态分为终止状态集和非终止状态集
vector<vector<int> > initializePartition(DFA dfa) {
    vector<vector<int> > partition;
    vector<int> nonFinalStates;
    vector<int> finalStates;

    // 根据状态是否是终止状态，分为终止状态和非终止状态
    for (int i = 0; i < dfa.newStates.size(); i++) {
        if (dfa.isFinalState(dfa.newStates[i])) {
            finalStates.push_back(i);
        } else {
            nonFinalStates.push_back(i);
        }
    }

    // 初始化分区
    if (!finalStates.empty()) partition.push_back(finalStates);
    if (!nonFinalStates.empty()) partition.push_back(nonFinalStates);
    return partition;
}

// 拆分当前分区，检查是否需要继续拆分
void splitPartition(DFA dfa, vector<int> &currentPartition, queue<vector<int> > &worklist,
                    vector<vector<int> > &partition) {
    vector<vector<int> > newPartitions;
    map<vector<int>, vector<int> > targetStateMap;

    // 遍历当前分区中的所有状态
    for (int state: currentPartition) {
        vector<int> targetStateGroups(dfa.charSet.size(), -1);

        // 对每个输入字符，检查该状态的转移目标
        for (int i = 0; i < dfa.charSet.size(); i++) {
            int targetState = getTargetState(dfa, state, dfa.charSet[i]);
            targetStateGroups[i] = targetState;
        }

        // 将状态按照它们的目标状态组来分类
        targetStateMap[targetStateGroups].push_back(state);
    }

    // 根据目标状态组进行拆分
    for (auto &entry: targetStateMap) {
        newPartitions.push_back(entry.second);
    }

    // 如果分区发生了拆分，更新分区集合
    if (newPartitions.size() > 1) {
        // 删除原始分区
        auto it = find(partition.begin(), partition.end(), currentPartition);
        if (it != partition.end()) {
            partition.erase(it);
        }

        // 将新分区加入工作队列和分区集合
        for (auto &part: newPartitions) {
            if (part.size() > 1) {
                worklist.push(part);
            }
            partition.push_back(part);
        }
    }
}


// 获取状态在某个字符下的目标状态
int getTargetState(DFA dfa, int state, int c) {
    int targetState = -1;
    for (int i = 0; i < dfa.transTable.size(); i++) {
        if (dfa.transTable[i][0] == state && dfa.transTable[i][1] == c) {
            targetState = dfa.transTable[i][2];
            break;
        }
    }
    return targetState;
}

// 构建最小化DFA：基于分区构建新的状态集和转移表
void buildMinimizedDfa(DFA dfa, vector<vector<int> > &partition, DFA &minimizedDfa) {
    // 清空最小化后的DFA数据结构
    minimizedDfa.initDFA();

    // 构建新的状态集
    for (int i = 0; i < partition.size(); i++) {
        minimizedDfa.stateSet.push_back(partition[i]);
    }

    // 对状态集按每个分区的第一个状态进行排序
    sort(minimizedDfa.stateSet.begin(), minimizedDfa.stateSet.end(),
         [](const vector<int> &a, const vector<int> &b) { return a[0] < b[0]; });

    // 根据排序后的状态集构建新的状态集，新的状态集为分区的索引
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        minimizedDfa.newStates.push_back(i); // 新状态集是每个分区的索引
    }

    // 字符集（直接从原始DFA复制字符集）
    minimizedDfa.charSet = dfa.charSet;

    // 初始状态，根据状态集的包含原始DFA的初始状态的分区来设置
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        for (auto state: minimizedDfa.stateSet[i]) {
            if (dfa.isStartState(state)) {
                minimizedDfa.startState = i; // 设置为该分区的索引
                break;
            }
        }
    }

    // 终止状态集, 根据状态集的包含原始DFA的终止状态的分区来设置
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        for (auto state: minimizedDfa.stateSet[i]) {
            if (dfa.isFinalState(state)) {
                minimizedDfa.finalStates.push_back(i); // 终止状态是该分区的索引
                break;
            }
        }
    }

    // 为每个状态集构建状态转移表
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        for (int c: dfa.charSet) {
            // 获取当前分区中第一个状态的转移目标
            int currentState = minimizedDfa.stateSet[i][0]; // 获取当前分区中的第一个状态
            int targetState = getTargetState(dfa, currentState, c); // 获取当前状态的转移目标

            // 获取目标状态所在的分区索引
            int targetStateGroup = -1;
            for (int j = 0; j < minimizedDfa.stateSet.size(); j++) {
                for (int state: minimizedDfa.stateSet[j]) {
                    if (state == targetState) {
                        targetStateGroup = j;
                        break;
                    }
                }
                if (targetStateGroup != -1) {
                    break;
                }
            }

            // 构建转移表：i表示当前分区，c表示输入字符，targetStateGroup表示目标分区
            minimizedDfa.transTable.push_back({i, c, targetStateGroup});
        }
    }
    minimizedDfa.transTableSize = minimizedDfa.transTable.size();
}

// 输出DFA
void printDfa(DFA dfa) {
    // cout << "DFA" << endl;
    cout << "状态集： " << "{ ";
    for (auto state: dfa.newStates) {
        cout << state << " ";
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

    cout << "状态转移个数：" << endl;
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
    // if (argc != 3) {
    //     // 只进行转换，最小化
    //     NFA nfa = readNfaFromFile(argv[1]);
    //     cout << "NFA数据：" << endl;
    //     printNfaData(nfa);
    //     cout << endl;
    //
    //     DFA dfa;
    //     nfaToDfa(nfa, &dfa);
    //     cout << "DFA数据：" << endl;
    //     printDfaData(dfa);
    //     cout << endl;
    //
    //     DFA minimizedDfa;
    //     minimizeDfa(dfa, &minimizedDfa);
    //     cout << "最小化后的DFA数据：" << endl;
    //     printDfa(minimizedDfa);
    //     return 1;
    // }
    //
    // NFA nfa1 = readNfaFromFile(argv[1]);
    // NFA nfa2 = readNfaFromFile(argv[2]);
    // NFA nfa;
    // mergeNFA(nfa1, nfa2, &nfa);
    // cout << "NFA合并后的数据：" << endl;
    // printNfaData(nfa);
    // cout << endl;
    //
    // DFA dfa;
    // nfaToDfa(nfa1, &dfa);
    // cout << "DFA数据：" << endl;
    // printDfaData(dfa);
    // cout << endl;
    //
    // DFA minimizedDfa;
    // minimizeDfa(dfa, &minimizedDfa);
    // cout << "最小化后的DFA数据：" << endl;
    // printDfa(minimizedDfa);

    string filename = "D:/Projects/CLionProjects/Bianyi/experiment2/nfa1";
    string filename1 = "D:/Projects/CLionProjects/Bianyi/experiment2/nfa2";
    string filename2 = "D:/Projects/CLionProjects/Bianyi/experiment2/nfa3";

    // 测试NFA合并
    NFA nfa1 = readNfaFromFile(filename);
    NFA nfa2 = readNfaFromFile(filename1);
    NFA nfa3;
    mergeNFA(nfa1, nfa2, &nfa3);
    cout << "NFA合并后的数据：" << endl;
    printNfaData(nfa3);

    NFA nfa = readNfaFromFile(filename);
    cout << "NFA数据：" << endl;
    printNfaData(nfa);
    cout << endl;

    // 测试NFA转为DFA
    DFA dfa = DFA();
    nfaToDfa(nfa, &dfa);
    cout << "DFA数据：" << endl;
    printDfaData(dfa);
    cout << endl;

    // 测试DFA最小化
    DFA minimizedDfa = DFA();
    minimizedDfa.initDFA();
    minimizeDfa(dfa, &minimizedDfa);
    cout << "最小化后的DFA数据：" << endl;
    printDfaData(minimizedDfa);
    cout << endl;

    cout << "最终输出结果DFA数据：" << endl;
    printDfa(minimizedDfa);

    return 0;
}
