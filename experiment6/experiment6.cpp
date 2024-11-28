//
// Created by ZhaoHao on 24-11-18.
//

#include "../experiment5/function.h"

class LLTable {
public:
    vector<string> VT;
    vector<string> VN;
    vector<vector<string> > table;
};


void readString(vector<string> *inputString, const string &stringFile) {
    ifstream file(stringFile);
    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << stringFile << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string word;
        while (ss >> word) {
            inputString->emplace_back(word);
        }
    }
    file.close();
}

void buildLLTable(const CFG &cfg, LLTable &llTable) {
    llTable.VT = cfg.VT;
    llTable.VT.emplace_back("#");
    llTable.VN = cfg.VN;
    llTable.table.resize(llTable.VN.size(), vector<string>(llTable.VT.size(), ""));

    for (const auto &select: Select) {
        auto &[left,middle, right] = select;

        for (const auto &vn: llTable.VN) {
            for (auto vt: llTable.VT) {
                if (vn == left) {
                    for (const auto &item: right) {
                        if (vt == item) {
                            // �ҵ�vn, vt��Ӧ���
                            const int vnIndex = find(llTable.VN.begin(), llTable.VN.end(), vn) - llTable.VN.begin();
                            const int vtIndex = find(llTable.VT.begin(), llTable.VT.end(), vt) - llTable.VT.begin();
                            const int itemIndex = find(Select.begin(), Select.end(), select) - Select.begin();
                            llTable.table[vnIndex][vtIndex] = to_string(itemIndex);
                        }
                        // ���Ϊ��ת��
                        else if (item == "��") {
                            vt = "#";
                            const int vnIndex = find(llTable.VN.begin(), llTable.VN.end(), vn) - llTable.VN.begin();
                            const int vtIndex = find(llTable.VT.begin(), llTable.VT.end(), vt) - llTable.VT.begin();
                            const int itemIndex = find(Select.begin(), Select.end(), select) - Select.begin();
                            llTable.table[vnIndex][vtIndex] = to_string(itemIndex);
                        }
                    }
                }
            }
        }
    }
}

void printLLTable(const LLTable &llTable) {
    constexpr int colWidth = 5;
    cout << "LL(1)Ԥ�������:" << endl;

    cout << setw(colWidth) << left << " ";
    for (const auto &vt: llTable.VT) {
        cout << setw(colWidth) << left << vt;
    }
    cout << endl;

    cout << string(colWidth * (llTable.VT.size() + 1), '-') << endl;

    for (int i = 0; i < llTable.table.size(); i++) {
        cout << setw(colWidth) << left << llTable.VN[i];
        for (const auto &j: llTable.table[i]) {
            if (!j.empty()) {
                cout << setw(colWidth) << left << ('p' + j);
            } else {
                cout << setw(colWidth) << left << " ";
            }
        }
        cout << endl;
    }

    cout << string(colWidth * (llTable.VT.size() + 1), '-') << endl << endl;
}

void printAnalysisProcess(const CFG &cfg, const LLTable &llTable, const vector<string> &inputString) {
    vector<string> stack;
    vector<string> input = inputString;
    input.emplace_back("#");

    stack.emplace_back("#");
    stack.push_back(cfg.S);

    cout << "(a,a)#�������̣�" << endl;
    cout << "��ʼ����#��ջ��" << cfg.S << "��ջ��" << endl;

    auto inputPointer = 0;
    int step = 1;

    while (!stack.empty()) {
        string top = stack.back();

        if (stack.back() == "#") {
            cout << setw(2) << setfill('0') << step++ << ":��ջX=" << top
                    << "�� ����c=" << input[inputPointer]
                    << "��ƥ�䣬�ɹ���" << endl;
            return;
        }

        if (const string &currentInput = input[inputPointer]; top == currentInput) {
            cout << setw(2) << setfill(' ') << step++ << ":";
            cout << "��ջX=" << top
                    << "�� ����c=" << currentInput
                    << "��ƥ�䣬����ָ����ƣ�" << endl;
            stack.pop_back();
            inputPointer++;
        } else if (isTerminal(top, cfg.VT)) {
            cout << setw(2) << setfill(' ') << step++ << ":";
            cout << "��ջX=" << top
                    << "�� ����c=" << currentInput
                    << "�������޷�ƥ�䣻" << endl;
            return;
        } else {
            const auto vnIndex = ranges::find(llTable.VN, top) - llTable.VN.begin();
            const auto vtIndex = ranges::find(llTable.VT, currentInput) - llTable.VT.begin();

            if (vnIndex >= llTable.VN.size() || vtIndex >= llTable.VT.size() || llTable.table[vnIndex][vtIndex].
                empty()) {
                cout << setw(2) << setfill(' ') << step++ << ":";
                cout << "��ջX=" << top << "�� ����c=" << currentInput << "�����󣬲��ʧ�ܣ�" << endl;
                return;
            }

            string productionIndex = llTable.table[vnIndex][vtIndex];
            const auto &[fst, snd] = cfg.P[stoi(productionIndex)];

            cout << setw(2) << setfill(' ') << step++ << ":";
            cout << "��ջX=" << top << "�� ����c=" << currentInput << "�����M[X,c]=" << top << "->";
            for (auto &it: snd) {
                cout << it;
            }
            cout << "������ʽ�Ҳ�������ջ��" << endl;

            stack.pop_back();
            if (snd[0] != "��") {
                for (const auto &it: std::ranges::reverse_view(snd)) {
                    stack.push_back(it);
                }
            }
        }
    }

    cout << "����ʧ�ܣ�ջδ�ջ�����δ������ϡ�" << endl;
}


int main() {
    CFG cfg;
    const string grammarFile = "D:/Projects/CLionProjects/Bianyi/experiment6/test";
    const string stringFile = "D:/Projects/CLionProjects/Bianyi/experiment6/test1";

    vector<string> inputString;

    readCFG(&cfg, grammarFile);
    readString(&inputString, stringFile);

    calculateFirst(cfg);
    calculateFollow(cfg);
    calculateSelect(cfg);

    LLTable llTable;

    buildLLTable(cfg, llTable);
    printLLTable(llTable);

    printAnalysisProcess(cfg, llTable, inputString);

    return 0;
}
