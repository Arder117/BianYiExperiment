//
// Created by ZhaoHao on 24-11-24.
//

#include "function.h"

// 读取文件
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
                    current = "ε";
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

// 计算First集
void calculateFirst(const CFG &cfg) {
    queue<string> Q;
    map<string, set<string> > tempFirst;

    // 初始化First集
    for (const auto &nonTerminal: cfg.VN) {
        First[nonTerminal] = {};
        tempFirst[nonTerminal] = {};
        Q.push(nonTerminal);
    }

    while (!Q.empty()) {
        string current = Q.front();
        Q.pop();

        // 标记当前非终结符的 First 集是否发生变化
        bool isModified = false;

        for (const auto &[fst, snd]: cfg.P) {
            if (fst == current) {
                const vector<string> &right = snd;
                bool allNullable = true;

                for (const auto &symbol: right) {
                    // 如果是终结符，直接加入当前非终结符的 First 集
                    if (isTerminal(symbol, cfg.VT)) {
                        if (First[current].insert(symbol).second) {
                            isModified = true;
                        }
                        allNullable = false;
                        break;
                    }
                    // 如果是非终结符，加入其First集中的非空项
                    if (isNonTerminal(symbol, cfg.VN)) {
                        for (const auto &item: First[symbol]) {
                            if (item != "ε" && First[current].insert(item).second) {
                                isModified = true;
                            }
                        }

                        // 如果First(symbol)不包含空，则停止检查后续符号
                        if (!First[symbol].contains("ε")) {
                            allNullable = false;
                            break;
                        }

                        // 如果只包含空，将空加入当前非终结符的 First 集
                        if (First[symbol].size() == 1 && First[symbol].contains("ε")) {
                            First[current].insert("ε").second;
                            isModified = true;
                            break;
                        }
                    }
                }

                // 如果右部能完全推导出空，将空加入当前非终结符的 First 集
                if (allNullable && First[current].insert("ε").second) {
                    isModified = true;
                }
            }
        }

        // 如果 First 集发生变化，将依赖该非终结符的所有非终结符重新加入队列
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
                        // 如果下一个符号是终结符，加入First到Follow
                        if (const string &nextSymbol = right[i + 1]; isTerminal(nextSymbol, cfg.VT)) {
                            if (Follow[symbol].insert(nextSymbol).second) {
                                isModified = true;
                            }
                        }
                        // 如果下一个符号是非终结符
                        else if (isNonTerminal(nextSymbol, cfg.VN)) {
                            // 加入First到Follow,除了ε
                            for (const auto &item: First[nextSymbol]) {
                                if (item != "ε" && Follow[symbol].insert(item).second) {
                                    isModified = true;
                                }
                            }

                            // 如果能推导出ε, 则继续将Follow(left)加入Follow(symbol)
                            if (First[nextSymbol].contains("ε")) {
                                for (const auto &item: Follow[left]) {
                                    if (Follow[symbol].insert(item).second) {
                                        isModified = true;
                                    }
                                }
                            }
                        }
                    } else {
                        // 如果 B 是产生式的最后一个符号，加入 Follow(A) 到 Follow(B)
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

// 计算Select集
void calculateSelect(const CFG &cfg) {
    for (const auto &[fst, snd]: cfg.P) {
        const string &left = fst;
        const vector<string> &right = snd;

        set<string> selectSet;

        // 右部为空，Follow(left)加入Select
        if (right[0] == "ε") {
            for (const auto &x: Follow[left]) {
                selectSet.insert(x);
            }
        } else {
            // 终结符
            if (isTerminal(right[0], cfg.VT)) {
                selectSet.insert(right[0]);
            }
            // 非终结符,将First加入Select
            else if (isNonTerminal(right[0], cfg.VN)) {
                for (const auto &s: First[right[0]]) {
                    selectSet.insert(s);
                }
            }
        }
        Select.emplace_back(left, right ,vector<string>(selectSet.begin(), selectSet.end()));
    }
}

// 合并 Select 集合
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

// 打印合并后的 Select 集合
void printNewSelect(const map<string, set<string>>& mergedSelect) {
    cout << "合并后的 Select 集合：" << endl;
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