//
// Created by ZhaoHao on 24-11-24.
//

#include "function.h"

// ��ȡ�ļ�
void readCFG(CFG *cfg, const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(0);
    }

    string line;
    int n;
    int index = 0;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (index == 0) {
            n = stoi(line);
            cfg->VNNum = n;
            index++;
        } else if (index == 1) {
            stringstream ss(line);
            string item;
            while (ss >> item) {
                cfg->VN.push_back(item);
            }
            index++;
        } else if (index == 2) {
            n = stoi(line);
            cfg->VTNum = n;
            index++;
        } else if (index == 3) {
            stringstream ss(line);
            string item;
            while (ss >> item) {
                cfg->VT.push_back(item);
            }
            index++;
        } else if (index == 4) {
            n = stoi(line);
            cfg->PNum = n;
            index++;
        } else if (index >= 5 && index <= (4 + cfg->PNum)) {
            auto pos = line.find("->");

            string left = line.substr(0, pos);
            erase(left, ' ');

            string right = line.substr(pos + 2);
            erase(right, ' ');

            vector<string> rightParts;

            for (auto i = 0; i < right.size(); ++i) {
                string current;

                if (!(static_cast<int>(right[i]) >= 0 && static_cast<int>(right[i]) <= 127)) {
                    current = "��";
                    rightParts.push_back(current);
                    break;
                }

                if (isalpha(right[i])) {
                    current += right[i];
                    if (i + 1 < right.size() && right[i + 1] == '\'') {
                        current += right[i + 1];
                        ++i;
                    }
                } else {
                    current += right[i];
                }
                if (!current.empty()) {
                    rightParts.push_back(current);
                }
            }
            cfg->P.emplace_back(left, rightParts);
            index++;
        } else if (index > (4 + cfg->PNum)) {
            cfg->S = line;
            break;
        }
    }
    file.close();
}

bool isTerminal(const string &symbol, const vector<string> &vt) {
    return ranges::find(vt, symbol) != vt.end();
}

bool isNonTerminal(const string &symbol, const vector<string> &vn) {
    return ranges::find(vn, symbol) != vn.end();
}

// ����First��
void calculateFirst(const CFG &cfg) {
    queue<string> Q;
    map<string, set<string> > tempFirst;

    // ��ʼ��First��
    for (const auto &nonTerminal: cfg.VN) {
        First[nonTerminal] = {};
        tempFirst[nonTerminal] = {};
        Q.push(nonTerminal);
    }

    while (!Q.empty()) {
        string current = Q.front();
        Q.pop();

        // ��ǵ�ǰ���ս���� First ���Ƿ����仯
        bool isModified = false;

        for (const auto &[fst, snd]: cfg.P) {
            if (fst == current) {
                const vector<string> &right = snd;
                bool allNullable = true;

                for (const auto &symbol: right) {
                    // ������ս����ֱ�Ӽ��뵱ǰ���ս���� First ��
                    if (isTerminal(symbol, cfg.VT)) {
                        if (First[current].insert(symbol).second) {
                            isModified = true;
                        }
                        allNullable = false;
                        break;
                    }
                    // ����Ƿ��ս����������First���еķǿ���
                    if (isNonTerminal(symbol, cfg.VN)) {
                        for (const auto &item: First[symbol]) {
                            if (item != "��" && First[current].insert(item).second) {
                                isModified = true;
                            }
                        }

                        // ���First(symbol)�������գ���ֹͣ����������
                        if (!First[symbol].contains("��")) {
                            allNullable = false;
                            break;
                        }

                        // ���ֻ�����գ����ռ��뵱ǰ���ս���� First ��
                        if (First[symbol].size() == 1 && First[symbol].contains("��")) {
                            First[current].insert("��").second;
                            isModified = true;
                            break;
                        }
                    }
                }

                // ����Ҳ�����ȫ�Ƶ����գ����ռ��뵱ǰ���ս���� First ��
                if (allNullable && First[current].insert("��").second) {
                    isModified = true;
                }
            }
        }

        // ��� First �������仯���������÷��ս�������з��ս�����¼������
        if (isModified) {
            for (const auto &[fst, snd]: cfg.P) {
                if (ranges::find(snd, current) != snd.end()) {
                    Q.push(fst);
                }
            }
        }
    }
}

void calculateFollow(CFG &cfg) {
    queue<string> Q;
    map<string, set<string> > tempFollow;

    for (const auto &nonTerminal: cfg.VN) {
        Follow[nonTerminal] = {};
        tempFollow[nonTerminal] = {};
    }

    Follow[cfg.S].insert("#");

    bool isModified = true;

    while (isModified) {
        isModified = false;
        for (const auto &[fst, snd]: cfg.P) {
            const string &left = fst;
            const vector<string> &right = snd;

            for (auto i = 0; i < right.size(); ++i) {
                if (const string &symbol = right[i]; isNonTerminal(symbol, cfg.VN)) {
                    if (i + 1 < right.size()) {
                        // �����һ���������ս��������First��Follow
                        if (const string &nextSymbol = right[i + 1]; isTerminal(nextSymbol, cfg.VT)) {
                            if (Follow[symbol].insert(nextSymbol).second) {
                                isModified = true;
                            }
                        }
                        // �����һ�������Ƿ��ս��
                        else if (isNonTerminal(nextSymbol, cfg.VN)) {
                            // ����First��Follow,���˦�
                            for (const auto &item: First[nextSymbol]) {
                                if (item != "��" && Follow[symbol].insert(item).second) {
                                    isModified = true;
                                }
                            }

                            // ������Ƶ�����, �������Follow(left)����Follow(symbol)
                            if (First[nextSymbol].contains("��")) {
                                for (const auto &item: Follow[left]) {
                                    if (Follow[symbol].insert(item).second) {
                                        isModified = true;
                                    }
                                }
                            }
                        }
                    } else {
                        // ��� B �ǲ���ʽ�����һ�����ţ����� Follow(A) �� Follow(B)
                        for (const auto &item: Follow[left]) {
                            if (Follow[symbol].insert(item).second) {
                                isModified = true;
                            }
                        }
                    }
                }
            }

        }
    }
}

// ����Select��
void calculateSelect(const CFG &cfg) {
    for (const auto &[fst, snd]: cfg.P) {
        const string &left = fst;
        const vector<string> &right = snd;

        set<string> selectSet;

        // �Ҳ�Ϊ�գ�Follow(left)����Select
        if (right[0] == "��") {
            for (const auto &x: Follow[left]) {
                selectSet.insert(x);
            }
        } else {
            // �ս��
            if (isTerminal(right[0], cfg.VT)) {
                selectSet.insert(right[0]);
            }
            // ���ս��,��First����Select
            else if (isNonTerminal(right[0], cfg.VN)) {
                for (const auto &s: First[right[0]]) {
                    selectSet.insert(s);
                }
            }
        }
        Select.emplace_back(left, right ,vector<string>(selectSet.begin(), selectSet.end()));
    }
}

// �ϲ� Select ����
void mergeSelect(const vector<pair<string, vector<string>>>& Select,
                 map<string, set<string>> *mergedSelect) {
    for (const auto &[A, B] : Select) {
        if (mergedSelect->contains(A)) {
            mergedSelect->at(A).insert(B.begin(), B.end());
        } else {
            mergedSelect->emplace(A, set<string>(B.begin(), B.end()));
        }
    }
}

// ��ӡ�ϲ���� Select ����
void printNewSelect(const map<string, set<string>>& mergedSelect) {
    cout << "�ϲ���� Select ���ϣ�" << endl;
    for (const auto &[A, B] : mergedSelect) {
        cout << A << " -> ";
        for (const auto &b : B) {
            cout << b << " ";
        }
        cout << endl;
    }
}

void printCFG(const CFG &cfg) {
    cout << "VN: ";
    for (const auto &vn: cfg.VN) {
        cout << vn << " ";
    }
    cout << endl;

    cout << "VT: ";
    for (const auto &vt: cfg.VT) {
        cout << vt << " ";
    }
    cout << endl;

    cout << "Production:" << endl;
    for (const auto &[fst, snd]: cfg.P) {
        const auto &left = fst;
        const auto &right = snd;
        cout << left << " -> ";
        for (const string &symbol: right) {
            cout << symbol << " ";
        }
        cout << endl;
    }

    cout << "Start Symbol: " << cfg.S << endl << endl;
}

void printFirst() {
    cout << "[First Set]" << endl;
    for (const auto &[fst, snd]: First) {
        cout << "  " << std::left << std::setw(10) << fst << ": ";
        for (const string &symbol: snd) {
            cout << symbol << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void printFollow() {
    cout << "[Follow Set]" << endl;
    for (const auto &[fst, snd]: Follow) {
        cout << "  " << std::left << std::setw(10) << fst << ": ";
        for (const string &symbol: snd) {
            cout << symbol << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void printSelect() {
    cout << "[Select Set]" << endl;
    int i = 0;
    for (const auto &[fst,middle, snd]: Select) {
        const string &A = fst;
        const vector<string> &selectSet = snd;
        cout << "  " << i << " :" << A << " -> ";
        for (auto &item: middle) {
            cout << item << " ";
        }
        cout << ":  ";
        for (const string &symbol: selectSet) {
            cout << symbol << " ";
        }
        cout << endl;
        ++i;
    }
}