#include "../experiment5/function.h"


// 项目
class Item {
public:
    string lhs;
    vector<string> rhs;
    int dotPosition;

    bool operator <(const Item &other) const {
        if (lhs != other.lhs) return lhs < other.lhs;
        if (rhs != other.rhs) return rhs < other.rhs;
        return dotPosition < other.dotPosition;
    }

    bool operator ==(const Item &other) const {
        return lhs == other.lhs && rhs == other.rhs && dotPosition == other.dotPosition;
    }

    void printItem() const {
        cout << "    ";
        cout << lhs << " -> ";
        for (size_t i = 0; i < rhs.size(); ++i) {
            if (i == dotPosition) cout << ".";
            cout << rhs[i];
        }
        if (dotPosition == rhs.size()) cout << ".";
        cout << endl;
    }
};

bool isTerminalState(const string &str, const CFG &cfg) {
    return ranges::find(cfg.VT, str) != cfg.VT.end();
}

// 项目集
class ItemSet {
public:
    int itemSetIndex{}; //
    vector<Item> items;

    void addItem(const Item &item) {
        items.push_back(item);
    }

    void printItemSet() const {
        cout << "I" << itemSetIndex << ":\n";
        for (const auto &item: items) {
            item.printItem();
        }
    }

    bool operator ==(const ItemSet &other) const {
        return items == other.items;
    }

    bool operator <(const ItemSet &other) const {
        return items < other.items;
    }
};

// 项目集规范族
class ItemSetFamily {
public:
    map<int, ItemSet> itemSets;
    vector<tuple<int, string, int> > transitions;

    void addItemSet(int index, const ItemSet &itemSet) {
        itemSets[index] = itemSet;
    }

    void addTransition(int from, const string &input, int to) {
        transitions.emplace_back(from, input, to);
    }

    void printItemSetFamily() const {
        cout << "[LR(0) item set cluster]" << endl;
        for (const auto &val: itemSets | views::values) {
            val.printItemSet();
        }
        cout << endl;
    }

    void printTransitions() const {
        cout << "[LR(0) state tran function]" << endl;
        for (const auto &[from, input, to]: transitions) {
            cout << "   " << from << ", " << input << "    ->" << to << endl;
        }
        cout << endl;
    }
};

// 扩展文法
void extendCFG(CFG *cfg) {
    const string start = cfg->S;
    if (ranges::find(cfg->S, '\'') != cfg->S.end()) {
        return;
    }
    cfg->S += "'";
    cfg->P.insert(cfg->P.begin(), make_pair(cfg->S, vector<string>{start}));
    cfg->VN.push_back(cfg->S);
}

// 计算项目集闭包
void closure(const CFG &cfg, ItemSet *itemSet) {
    stack<Item> stack;
    set<Item> visitedItems;

    for (const auto &item: itemSet->items) {
        stack.push(item);
        visitedItems.insert(item);
    }

    while (!stack.empty()) {
        Item currentItem = stack.top();
        stack.pop();

        if (currentItem.dotPosition < currentItem.rhs.size()) {
            if (string nextSymbol = currentItem.rhs[currentItem.dotPosition];
                ranges::find(cfg.VN, nextSymbol) != cfg.VN.end()) {
                for (const auto &[lhs, rhsList]: cfg.P) {
                    if (lhs == nextSymbol) {
                        vector<string> newRhs = rhsList;
                        if (Item newItem(nextSymbol, newRhs, 0); !visitedItems.contains(newItem)) {
                            itemSet->addItem(newItem);
                            stack.push(newItem);
                            visitedItems.insert(newItem);
                        }
                    }
                }
            }
        }
    }
}

ItemSet goTo(const CFG &cfg, const ItemSet &itemSet, const string &symbol) {
    ItemSet newItemSet;

    for (const auto &[lhs, rhs, dotPosition]: itemSet.items) {
        if (dotPosition < rhs.size() && rhs[dotPosition] == symbol) {
            newItemSet.addItem(Item(lhs, rhs, dotPosition + 1));
        }
    }

    closure(cfg, &newItemSet);

    return newItemSet;
}

// 构造项目集规范族
void constructItemSetFamily(const CFG &cfg, ItemSetFamily *itemSetFamily) {
    ItemSet initialItemSet(0);
    vector<string> rhs1 = {};
    for (const auto &[lhs, rhsList]: cfg.P) {
        if (lhs == cfg.S) {
            rhs1 = rhsList;
            break;
        }
    }

    initialItemSet.addItem(Item(cfg.S, rhs1, 0));

    closure(cfg, &initialItemSet);

    itemSetFamily->addItemSet(0, initialItemSet);

    queue<ItemSet> q;
    map<ItemSet, int> visited;

    visited[initialItemSet] = 0;
    q.push(initialItemSet);

    int index = 1;

    while (!q.empty()) {
        ItemSet currentItemSet = q.front();
        q.pop();

        // 终结符
        for (const auto &symbol: cfg.VT) {
            if (ItemSet nextItemSet = goTo(cfg, currentItemSet, symbol);
                !nextItemSet.items.empty() && !visited.contains(nextItemSet)) {
                nextItemSet.itemSetIndex = index;

                itemSetFamily->addItemSet(index, nextItemSet);
                itemSetFamily->addTransition(currentItemSet.itemSetIndex, symbol, index);

                visited[nextItemSet] = index;
                q.push(nextItemSet);
                ++index;
            }
        }

        // 非终结符
        for (const auto &symbol: cfg.VN) {
            if (ItemSet nextItemSet = goTo(cfg, currentItemSet, symbol);
                !nextItemSet.items.empty() && !visited.contains(nextItemSet)) {
                nextItemSet.itemSetIndex = index;

                itemSetFamily->addItemSet(index, nextItemSet);
                itemSetFamily->addTransition(currentItemSet.itemSetIndex, symbol, index);

                visited[nextItemSet] = index;
                q.push(nextItemSet);
                ++index;
            }
        }
    }
}

// 判断文法是否是LR(0)文法
void isLR0(const ItemSetFamily &itemSetFamily, const CFG &cfg) {
    bool isLR0Grammar = true;

    for (const auto &val: itemSetFamily.itemSets | views::values) {
        const auto &[itemSetIndex, items] = val;
        bool hasReduction = false;
        bool hasShift = false;

        for (const auto &item: items) {
            if (item.dotPosition == item.rhs.size()) {
                if (hasReduction) {
                    isLR0Grammar = false;
                }
                hasReduction = true;
            } else {
                if (isTerminalState(item.rhs[item.dotPosition], cfg)) {
                    hasShift = true;
                }
            }
            if (hasReduction && hasShift) {
                isLR0Grammar = false;
            }
            if (!isLR0Grammar) {
                break;
            }
        }
        if (!isLR0Grammar) {
            break;
        }
    }

    if (isLR0Grammar) {
        cout << "文法是 LR(0) 文法！" << endl;
    } else {
        cout << "文法不是 LR(0) 文法！" << endl;
    }
}

// 文法的Action表和Goto表,存储到创建的指定LR分析表文件，文件名与文法描述文件同名，扩展名为lrtbl
void createLRTable(const ItemSetFamily &itemSetFamily, const CFG &cfg, const string &grammarFile) {
    string lrtblFile = grammarFile + ".lrtbl";
    ofstream outFile(lrtblFile);

    if (!outFile.is_open()) {
        cerr << "Error: Unable to open file " << lrtblFile << endl;
        return;
    }

    vector<tuple<int, string, string> > actionTable; // 存储ACTION表，
    vector<tuple<int, string, int> > gotoTable; // 存储GOTO表，

    for (const auto &[index, itemSet]: itemSetFamily.itemSets) {
        // 处理ACTION表
        for (const auto &[lhs, rhs, dotPosition]: itemSet.items) {
            if (dotPosition < rhs.size()) {
                if (string symbol = rhs[dotPosition]; ranges::find(cfg.VT, symbol) != cfg.VT.end()) {
                    auto [itemSetIndex, items] = goTo(cfg, itemSet, symbol);
                    for (const auto &[nextIndex, nextItemSet]: itemSetFamily.itemSets) {
                        if (nextItemSet.items == items) {
                            actionTable.emplace_back(index, symbol, "s" + to_string(nextIndex));
                            break;
                        }
                    }
                }
            } else if (lhs != cfg.S) {
                for (int i = 0; i < cfg.P.size(); ++i) {
                    if (cfg.P[i].first == lhs && cfg.P[i].second == rhs) {
                        for (const string &term: cfg.VT) {
                            actionTable.emplace_back(index, term, "r" + to_string(i + 1));
                        }
                        actionTable.emplace_back(index, "#", "r" + to_string(i + 1));
                        break;
                    }
                }
            } else if (lhs == cfg.S && dotPosition == rhs.size()) {
                actionTable.emplace_back(index, "#", "acc");
            }
        }

        // 处理GOTO表，非终结符的转移
        for (const string &nonTerminal: cfg.VN) {
            auto [itemSetIndex, items] = goTo(cfg, itemSet, nonTerminal);
            for (const auto &[nextIndex, nextItemSet]: itemSetFamily.itemSets) {
                if (nextItemSet.items == items) {
                    gotoTable.emplace_back(index, nonTerminal, nextIndex);
                    break;
                }
            }
        }
    }

    // 写入ACTION表到文件
    outFile << actionTable.size() << endl;
    for (const auto &[first, change, second]: actionTable) {
        outFile << setw(4) << first << "   " << setw(2) << change << "  " << setw(4) << second << endl;
    }

    // 写入GOTO表到文件
    outFile << gotoTable.size() << endl;
    for (const auto &[first, change, second]: gotoTable) {
        outFile << setw(4) << first << "   " << setw(2) << change << "  " << setw(4) << second << endl;
    }

    outFile.close();
}


int main() {
    const string grammarFile = "../experiment7/test";
    CFG cfg;

    readCFG(&cfg, grammarFile);
    printCFG(cfg);

    extendCFG(&cfg);
    cout << "扩展后的文法：" << endl;
    printCFG(cfg);

    ItemSetFamily itemSetFamily;
    constructItemSetFamily(cfg, &itemSetFamily);

    itemSetFamily.printItemSetFamily();
    itemSetFamily.printTransitions();

    isLR0(itemSetFamily, cfg);

    createLRTable(itemSetFamily, cfg, grammarFile);

    return 0;
}
