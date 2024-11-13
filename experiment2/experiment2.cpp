//
// Created by ZhaoHao on 24-10-30.
//

// 1. ���һ����������������ʵ�ְ�����NFA�ĺϲ���
// 2. ���һ����������������ʵ�ְ�NFAȷ������һ��DFA��
// 3. ���һ����������������ʵ�ְ�DFA��С����
// 4. ������NFA��NFA�����洢���ı��ļ��У��ļ�����Ϊ�����в������룻
// 5. ����ϲ�����С���Ժ��DFA����׼����豸��

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

// DFA��
class DFA {
public:
    // ��״̬��
    vector<vector<int> > stateSet;
    // ״̬����С
    int stateSetSize;

    // ת�������״̬��
    vector<int> newStates;
    int newStatesSize;

    // �ַ���
    vector<int> charSet;
    // �ַ�����С
    int charSetSize;

    // ״̬ת�Ʊ�,ÿ�е�һ��Ԫ��Ϊ��ǰ״̬���ڶ���Ԫ��Ϊ�ַ�������Ķ��Ԫ��Ϊת�ƺ��״̬
    vector<vector<int> > transTable;
    // ״̬��
    int transTableSize;

    // ��ʼ״̬
    int startState;

    // ��ֹ״̬
    vector<int> finalStates;

    // ��ʼ��DFA
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
        if (startState == state) {
            return true;
        }
        return false;
    }
};

// ��������
void printNfaData(NFA nfa);

void printDfaData(DFA dfa);

void printDfa(DFA dfa);

// ��ȡNFA�����ļ�
NFA readNfaFromFile(string filename) {
    ifstream ReadFile;
    ReadFile.open(filename, ios::in);
    if (ReadFile.fail()) {
        cout << "���ļ�ʧ��" << endl;
        exit(1);
    }

    NFA nfa;
    int rowindex = 0;
    string line;

    while (!ReadFile.eof()) {
        getline(ReadFile, line);
        if (!line.empty()) {
            rowindex++;

            // ״̬��
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

            // �ַ���
            else if (rowindex == 2) {
                stringstream ss(line);
                int c;
                while (ss >> c) {
                    nfa.charSet.push_back(c);
                }
                nfa.charSetSize = nfa.charSet.size();
            }

            // ״̬��
            else if (rowindex == 3) {
                nfa.transTableSize = stoi(line);
            }

            // ״̬ת�Ʊ�
            else if (rowindex < 3 + nfa.transTableSize + 1 && rowindex > 3) {
                vector<int> transRow;
                stringstream ss(line);
                int state;
                while (ss >> state) {
                    transRow.push_back(state);
                }
                nfa.transTable.push_back(transRow);
            }

            // ��ʼ״̬
            else if (rowindex == 3 + nfa.transTableSize + 1) {
                stringstream ss(line);
                int state = 0;
                while (ss >> state) {
                    nfa.startStates.push_back(state);
                }
            }

            // ��ֹ״̬
            if (rowindex == 3 + nfa.transTableSize + 2) {
                stringstream ss(line);
                int state = 0;
                while (ss >> state) {
                    nfa.finalStates.push_back(state);
                }
            }
        }
    }

    // ��״̬ת�Ʊ��Ԫ��ת��Ϊ�ַ�
    for (int i = 0; i < nfa.transTableSize; i++) {
        for (int j = 0; j < nfa.transTable[i].size(); j++) {
            nfa.transTable[i][j] = nfa.transTable[i][j];
        }
    }

    return nfa;
}

// �ϲ��������飬ȥ��
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

// �ϲ�����NFA
void mergeNFA(NFA nfa1, NFA nfa2, NFA *nfa) {
    // ��������ʼ״̬������ʼ״̬���Ϊ����NFA״̬���������ֵ��1��
    nfa->startStates.push_back(max(*max_element(nfa1.stateSets.begin(), nfa1.stateSets.end()),
                                   *max_element(nfa2.stateSets.begin(), nfa2.stateSets.end())) + 1);
    nfa->stateSetSize = 1;

    // �ϲ�״̬����������NFA��״̬���Ϻ�����ʼ״̬��
    mergeArray(nfa1.stateSets, nfa2.stateSets, &nfa->stateSets);
    nfa->stateSets.push_back(nfa->startStates[0]);
    nfa->stateSetSize = nfa->stateSets.size();

    // �ϲ���ֹ״̬��ֱ�Ӻϲ�����NFA����ֹ״̬
    mergeArray(nfa1.finalStates, nfa2.finalStates, &nfa->finalStates);

    // �ϲ��ַ���
    mergeArray(nfa1.charSet, nfa2.charSet, &nfa->charSet);

    // �ϲ�ת�ƺ�����ͨ���ֵ���չ�ϲ�����NFA��ת�ƺ���
    for (int i = 0; i < nfa1.transTableSize; i++) {
        nfa->transTable.push_back(nfa1.transTable[i]);
    }
    for (int i = 0; i < nfa2.transTableSize; i++) {
        nfa->transTable.push_back(nfa2.transTable[i]);
    }

    // ��ӿ�ת�ƣ�������ʼ״̬��ԭ����NFA��ʼ״̬��ӿ�ת�ơ�
    for (int state: nfa1.startStates) {
        nfa->transTable.push_back({nfa->startStates[0], -1, state});
    }
    for (int state: nfa2.startStates) {
        nfa->transTable.push_back({nfa->startStates[0], -1, state});
    }
    nfa->transTable.erase(unique(nfa->transTable.begin(), nfa->transTable.end()), nfa->transTable.end());
    nfa->transTableSize = nfa->transTable.size();
}

// ͨ����ȱ�����state�Ŀձհ���_closure
vector<int> closure(NFA nfa, vector<int> states) {
    vector<int> closureStates;
    queue<int> queueStates;

    // ���0��startStates�У������closureStates
    if (ranges::find(states, 0) != states.end()) {
        closureStates.push_back(0);
    }

    // ��ʼ������
    for (int state: states) {
        queueStates.push(state);
    }

    // ��ȱ���
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

    // ȥ��
    sort(closureStates.begin(), closureStates.end());
    closureStates.erase(unique(closureStates.begin(), closureStates.end()), closureStates.end());

    return closureStates;
}

// �Ƚ������ַ��Ƿ����
int cmp(int c1, int c2) {
    if (c1 == c2) {
        return 1;
    }
    return 0;
}

// ״̬ת��
vector<int> transition(NFA nfa, vector<int> states, int c) {
    vector<int> transitionStates;

    queue<int> queueStates;

    for (int state: states) {
        for (int i = 0; i < nfa.transTableSize; i++) {
            if (nfa.transTable[i][0] == state && cmp(nfa.transTable[i][1], c) == 1) {
                for (int j = 2; j < nfa.transTable[i].size(); j++) {
                    if (ranges::find(transitionStates, nfa.transTable[i][j]) == transitionStates.end()) {
                        transitionStates.push_back(nfa.transTable[i][j]);
                        queueStates.push(nfa.transTable[i][j]); //��ת�ƺ��״̬�������
                    }
                }
            }
        }
    }

    // ��״̬���еĿձհ����뵽ת�ƺ��״̬����
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

    // ȥ��
    sort(transitionStates.begin(), transitionStates.end());
    transitionStates.erase(unique(transitionStates.begin(), transitionStates.end()), transitionStates.end());

    return transitionStates;
}

// NFAת����DFA
void nfaToDfa(NFA nfa, DFA *dfa) {
    // ��ʼ��DFA
    dfa->charSet = nfa.charSet;
    dfa->transTableSize = 0;

    vector<int> closureStates = closure(nfa, nfa.startStates);
    // ʹ��states���洢״̬�����±�Ϊ״̬�����±�ţ�ֵΪ״̬��
    dfa->stateSet.push_back(closureStates);

    // ���״̬��0��ʾδ��ǣ�1��ʾ�ѱ��
    vector<int> markedStates = {0};
    markedStates[0] = 1;

    // �����������
    queue<vector<int> > queueStates;

    // ����ʼ״̬�Ŀձհ��������
    queueStates.push(closureStates);

    int transTableIndex = 0;
    // ��������
    while (!queueStates.empty()) {
        // ȡ�������е�״̬
        vector<int> states = queueStates.front();

        // �����ַ���
        for (int c: nfa.charSet) {
            // ״̬ת��
            vector<int> transitionStates = transition(nfa, states, c);

            // ״̬ת��Ϊ�գ�����
            if (transitionStates.empty()) {
                continue;
            }

            // ״̬ת�ƺ��״̬���Ƿ��Ѵ���
            bool exist = false;
            int existIndex = -1;
            for (int i = 0; i < dfa->stateSet.size(); i++) {
                if (dfa->stateSet[i] == transitionStates) {
                    exist = true;
                    existIndex = i;
                }
            }

            // ״̬ת�ƺ��״̬�������ڣ�����״̬��
            if (!exist) {
                dfa->stateSet.push_back(transitionStates);
                markedStates.push_back(0);
                existIndex = dfa->stateSet.size() - 1;
                queueStates.push(transitionStates);
                markedStates[existIndex] = 1;
            }

            // ����״̬ת��ǰ��״̬���ı��
            int beforeIndex = 0;
            for (int i = 0; i < dfa->stateSet.size(); i++) {
                if (dfa->stateSet[i] == states) {
                    beforeIndex = i;
                }
            }

            // ���״̬ת��
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

    // ���ó�ʼ״̬
    dfa->startState = 0;
    dfa->newStatesSize = 0;
    // ����״̬��
    for (int i = 0; i < dfa->stateSet.size(); i++) {
        dfa->newStates.push_back(i);
        dfa->newStatesSize++;
    }

    // ������ֹ״̬
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


// DFA��С����ʹ��Hopcroft�㷨��С��DFA
void minimizeDfa(DFA dfa, DFA *finalDfa) {
    // ��ʼ����С�����DFA
    DFA minimizedDfa;
    minimizedDfa.initDFA();

    // ��ʼ������
    vector<vector<int> > partition = initializePartition(dfa);

    // ��ʼ��������ķ�������
    queue<vector<int> > worklist;
    for (auto &part: partition) {
        worklist.push(part);
    }

    // ��ʼ�ݹ�ָ�
    while (!worklist.empty()) {
        vector<int> currentPartition = worklist.front();
        worklist.pop();

        // �Ե�ǰ�������в��
        splitPartition(dfa, currentPartition, worklist, partition);
    }


    // ��С��DFA�Ĺ��죨������ת��Ϊ��״̬����״̬ת�Ʊ�
    buildMinimizedDfa(dfa, partition, minimizedDfa);

    // ����С�����DFA��ֵ��finalDfa
    *finalDfa = minimizedDfa;
}

// ��ʼ����������DFA״̬��Ϊ��ֹ״̬���ͷ���ֹ״̬��
vector<vector<int> > initializePartition(DFA dfa) {
    vector<vector<int> > partition;
    vector<int> nonFinalStates;
    vector<int> finalStates;

    // ����״̬�Ƿ�����ֹ״̬����Ϊ��ֹ״̬�ͷ���ֹ״̬
    for (int i = 0; i < dfa.newStates.size(); i++) {
        if (dfa.isFinalState(dfa.newStates[i])) {
            finalStates.push_back(i);
        } else {
            nonFinalStates.push_back(i);
        }
    }

    // ��ʼ������
    if (!finalStates.empty()) partition.push_back(finalStates);
    if (!nonFinalStates.empty()) partition.push_back(nonFinalStates);
    return partition;
}

// ��ֵ�ǰ����������Ƿ���Ҫ�������
void splitPartition(DFA dfa, vector<int> &currentPartition, queue<vector<int> > &worklist,
                    vector<vector<int> > &partition) {
    vector<vector<int> > newPartitions;
    map<vector<int>, vector<int> > targetStateMap;

    // ������ǰ�����е�����״̬
    for (int state: currentPartition) {
        vector<int> targetStateGroups(dfa.charSet.size(), -1);

        // ��ÿ�������ַ�������״̬��ת��Ŀ��
        for (int i = 0; i < dfa.charSet.size(); i++) {
            int targetState = getTargetState(dfa, state, dfa.charSet[i]);
            targetStateGroups[i] = targetState;
        }

        // ��״̬�������ǵ�Ŀ��״̬��������
        targetStateMap[targetStateGroups].push_back(state);
    }

    // ����Ŀ��״̬����в��
    for (auto &entry: targetStateMap) {
        newPartitions.push_back(entry.second);
    }

    // ������������˲�֣����·�������
    if (newPartitions.size() > 1) {
        // ɾ��ԭʼ����
        auto it = find(partition.begin(), partition.end(), currentPartition);
        if (it != partition.end()) {
            partition.erase(it);
        }

        // ���·������빤�����кͷ�������
        for (auto &part: newPartitions) {
            if (part.size() > 1) {
                worklist.push(part);
            }
            partition.push_back(part);
        }
    }
}


// ��ȡ״̬��ĳ���ַ��µ�Ŀ��״̬
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

// ������С��DFA�����ڷ��������µ�״̬����ת�Ʊ�
void buildMinimizedDfa(DFA dfa, vector<vector<int> > &partition, DFA &minimizedDfa) {
    // �����С�����DFA���ݽṹ
    minimizedDfa.initDFA();

    // �����µ�״̬��
    for (int i = 0; i < partition.size(); i++) {
        minimizedDfa.stateSet.push_back(partition[i]);
    }

    // ��״̬����ÿ�������ĵ�һ��״̬��������
    sort(minimizedDfa.stateSet.begin(), minimizedDfa.stateSet.end(),
         [](const vector<int> &a, const vector<int> &b) { return a[0] < b[0]; });

    // ����������״̬�������µ�״̬�����µ�״̬��Ϊ����������
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        minimizedDfa.newStates.push_back(i); // ��״̬����ÿ������������
    }

    // �ַ�����ֱ�Ӵ�ԭʼDFA�����ַ�����
    minimizedDfa.charSet = dfa.charSet;

    // ��ʼ״̬������״̬���İ���ԭʼDFA�ĳ�ʼ״̬�ķ���������
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        for (auto state: minimizedDfa.stateSet[i]) {
            if (dfa.isStartState(state)) {
                minimizedDfa.startState = i; // ����Ϊ�÷���������
                break;
            }
        }
    }

    // ��ֹ״̬��, ����״̬���İ���ԭʼDFA����ֹ״̬�ķ���������
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        for (auto state: minimizedDfa.stateSet[i]) {
            if (dfa.isFinalState(state)) {
                minimizedDfa.finalStates.push_back(i); // ��ֹ״̬�Ǹ÷���������
                break;
            }
        }
    }

    // Ϊÿ��״̬������״̬ת�Ʊ�
    for (int i = 0; i < minimizedDfa.stateSet.size(); i++) {
        for (int c: dfa.charSet) {
            // ��ȡ��ǰ�����е�һ��״̬��ת��Ŀ��
            int currentState = minimizedDfa.stateSet[i][0]; // ��ȡ��ǰ�����еĵ�һ��״̬
            int targetState = getTargetState(dfa, currentState, c); // ��ȡ��ǰ״̬��ת��Ŀ��

            // ��ȡĿ��״̬���ڵķ�������
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

            // ����ת�Ʊ�i��ʾ��ǰ������c��ʾ�����ַ���targetStateGroup��ʾĿ�����
            minimizedDfa.transTable.push_back({i, c, targetStateGroup});
        }
    }
    minimizedDfa.transTableSize = minimizedDfa.transTable.size();
}

// ���DFA
void printDfa(DFA dfa) {
    // cout << "DFA" << endl;
    cout << "״̬���� " << "{ ";
    for (auto state: dfa.newStates) {
        cout << state << " ";
    }
    cout << "}" << endl;

    cout << "���ű� " << "{ ";
    for (int c: dfa.charSet) {
        cout << c << " ";
    }
    cout << "}" << endl;

    cout << "״̬ת�ƣ� " << endl;
    for (int i = 0; i < dfa.transTableSize; i++) {
        cout << "(" << dfa.transTable[i][0] << ", " << dfa.transTable[i][1] << ")" << " -> " << dfa.transTable[i][2]
                <<
                endl;
    }

    cout << "��ʼ״̬�� " << dfa.startState << endl;

    cout << "����״̬���� " << "{ ";
    for (int state: dfa.finalStates) {
        cout << state << " ";
    }
    cout << "}" << endl;
}

// ��ӡNFA����
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

// ��ӡDFA����
void printDfaData(DFA dfa) {
    cout << "״̬����" << endl;
    int index = 0;
    for (auto state: dfa.stateSet) {
        cout << "DFA: " << index++ << " " << "NFA: ";
        for (auto state2: state) {
            cout << state2 << " ";
        }
        cout << endl;
    }

    cout << "��״̬����" << endl;
    for (int state: dfa.newStates) {
        cout << state << " ";
    }
    cout << endl;

    cout << "�ַ�����" << endl;
    for (auto c: dfa.charSet) {
        cout << c << " ";
    }
    cout << endl;

    cout << "״̬ת�Ƹ�����" << endl;
    cout << dfa.transTableSize << endl;
    cout << "״̬ת�Ʊ�" << endl;
    for (vector<int> transRow: dfa.transTable) {
        for (auto state: transRow) {
            cout << state << " ";
        }
        cout << endl;
    }

    cout << "��ʼ״̬��" << endl << dfa.startState << endl;

    cout << "��ֹ״̬��" << endl;
    for (int state: dfa.finalStates) {
        cout << state << " ";
    }
    cout << endl;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        // ֻ����ת������С��
        NFA nfa = readNfaFromFile(argv[1]);
        cout << "NFA���ݣ�" << endl;
        printNfaData(nfa);
        cout << endl;

        DFA dfa;
        nfaToDfa(nfa, &dfa);
        cout << "DFA���ݣ�" << endl;
        printDfaData(dfa);
        cout << endl;

        DFA minimizedDfa;
        minimizeDfa(dfa, &minimizedDfa);
        cout << "��С�����DFA���ݣ�" << endl;
        printDfa(minimizedDfa);
        return 1;
    }

    NFA nfa1 = readNfaFromFile(argv[1]);
    NFA nfa2 = readNfaFromFile(argv[2]);
    NFA nfa;
    mergeNFA(nfa1, nfa2, &nfa);
    cout << "NFA�ϲ�������ݣ�" << endl;
    printNfaData(nfa);
    cout << endl;

    DFA dfa;
    nfaToDfa(nfa1, &dfa);
    cout << "DFA���ݣ�" << endl;
    printDfaData(dfa);
    cout << endl;

    DFA minimizedDfa;
    minimizeDfa(dfa, &minimizedDfa);
    cout << "��С�����DFA���ݣ�" << endl;
    printDfa(minimizedDfa);

    // string filename = "D:/Projects/CLionProjects/Bianyi/experiment2/nfa1";
    // string filename1 = "D:/Projects/CLionProjects/Bianyi/experiment2/nfa2";
    // string filename2 = "D:/Projects/CLionProjects/Bianyi/experiment2/nfa3";

    // ����NFA�ϲ�
    // NFA nfa1 = readNfaFromFile(filename);
    // NFA nfa2 = readNfaFromFile(filename1);
    // NFA nfa3;
    // mergeNFA(nfa1, nfa2, &nfa3);
    // cout << "NFA�ϲ�������ݣ�" << endl;
    // printNfaData(nfa3);

    // NFA nfa = readNfaFromFile(filename);
    // cout << "NFA���ݣ�" << endl;
    // printNfaData(nfa);
    // cout << endl;
    //
    // // ����NFAתΪDFA
    // DFA dfa = DFA();
    // nfaToDfa(nfa, &dfa);
    // cout << "DFA���ݣ�" << endl;
    // printDfaData(dfa);
    // cout << endl;
    //
    // // ����DFA��С��
    // DFA minimizedDfa = DFA();
    // minimizedDfa.initDFA();
    // minimizeDfa(dfa, &minimizedDfa);
    // cout << "��С�����DFA���ݣ�" << endl;
    // printDfaData(minimizedDfa);
    // cout << endl;
    //
    // cout << "����������DFA���ݣ�" << endl;
    // printDfa(minimizedDfa);

    return 0;
}
