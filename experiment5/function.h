//
// Created by ZhaoHao on 24-11-21.
//


#ifndef EXPERIMENT5_H
#define EXPERIMENT5_H


#endif //EXPERIMENT5_H

// #pragma once
#include <bits/stdc++.h>
using namespace std;

class CFG {
public:
    int VNNum;
    vector<string> VN;
    int VTNum;
    vector<string> VT;
    int PNum;
    vector<pair<string, vector<string> > > P;
    string S;

    CFG() {
        VNNum = 0;
        VTNum = 0;
        PNum = 0;
        VN.clear();
        VT.clear();
        P.clear();
        S = '\0';
    }
};

inline map<string, set<string> > First;
inline map<string, set<string> > Follow;
inline vector<tuple<string, vector<string>, vector<string> > > Select;

// 读取文件
void readCFG(CFG *cfg, const string &filename);

// 判断是否为终结符
bool isTerminal(const string &symbol, const vector<string> &vt);

// 判断是否为非终结符
bool isNonTerminal(const string &symbol, const vector<string> &vn);

// 计算First集
void calculateFirst(const CFG &cfg);

// 计算Follow集
void calculateFollow(CFG &cfg);

// 计算Select集
void calculateSelect(const CFG &cfg);

// 合并 Select 集合
void mergeSelect(const vector<pair<string, vector<string>>>& Select,
                 map<string, set<string>> *mergedSelect);

// 打印合并后的 Select 集合
void printNewSelect(const map<string, set<string>>& mergedSelect);

// 打印文法
void printCFG(const CFG &cfg);

// 打印First集
void printFirst();

// 打印Follow集
void printFollow();

// 打印Select集
void printSelect();
