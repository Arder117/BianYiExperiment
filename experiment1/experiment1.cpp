#include <stdio.h>
#include <string.h>

char keyWord[67][20] = {
    "main", "if", "then", "else", "while", "do", "repeat", "until", "for",
    "from", "to", "step", "switch", "of", "case", "default", "return", "integer",
    "real", "char", "bool", "and", "or", "not", "mod", "read", "write",
    "=", "+", "-", "*", "/", "<", ">", "==", "!=",
    "<=", ">=", "++", "--", "&&", "||", "!", "+=", "-=",
    "*=", "/=", "%=", "&=", "|=", "^=", ">>", "<<", ".",
    "~", "&", "^", "|", ",", ";", ":", "{", "}",
    "[", "]", "(", ")"
};

struct FileText {
    char text[1000]; // 文件内容
    int length; // 文件长度
    int pointer; // 文件指针
};

FileText fileText;

// 声明函数
// 输入文件名,返回文件名
int InputFileName(char filename[]);
// 读取文件内容
int ReadText(char filename[], struct FileText *fileText);
// 预处理，去掉输入串中的回车符、换行符和跳格符等编辑性文字，多个空白符合并为一个。
int Pretreatment(struct FileText *fileText);
// 判断是否为空白符
int isSpace(char a);
// 判断单词种别码，返回种别码
int JudgeBreedCode(char word[]);
// 判断是否为运算符
int isOperator(char a);
// 判断是否为双目运算符
int isDoubleOperator(char a, char b);
// 词法分析
int TextAnalyze(const FileText *fileText);


// 输入文件名
int InputFileName(char filename[]) {
    printf("Please input the filename:");
    scanf("%s", filename);
    return true;
}

// 读取文件内容
int ReadText(char filename[], struct FileText *fileText) {
    FILE *fp = fopen(filename, "r");
    if (fp == nullptr) {
        printf("File not found\n");
        return false;
    }
    fread(fileText->text, sizeof(char), 1000, fp);
    fileText->length = strlen(fileText->text);
    fclose(fp);
    return true;
}

// 预处理，去掉输入串中的回车符、换行符和跳格符等编辑性文字，多个空白符合并为一个。
int Pretreatment(struct FileText *fileText) {
    // 去除注释
    for (int i = 0; i < strlen(fileText->text); i++) {
        if (fileText->text[i] == '/' && fileText->text[i + 1] == '/') {
            while (fileText->text[i] != '\n') {
                fileText->text[i] = ' ';
                i++;
            }
        }
    }

    // 去除回车符、换行符和跳格符
    for (int i = 0; i < strlen(fileText->text); i++) {
        if (fileText->text[i] == '\n' || fileText->text[i] == '\r' || fileText->text[i] == '\t') {
            fileText->text[i] = ' ';
        }
    }

    // 去除多个空格，保留第一个空格
    for (int i = 0; i < strlen(fileText->text); i++) {
        if (fileText->text[i] == ' ') {
            while (fileText->text[i + 1] == ' ') {
                for (int j = i; j < strlen(fileText->text); j++) {
                    fileText->text[j] = fileText->text[j + 1];
                }
            }
        }
    }
    fileText->length = strlen(fileText->text);

    return true;
}

// 判断是否为空格
int isSpace(char a) {
    if (a == ' ' || a == NULL) {
        return true;
    }
    return false;
}

// 判断单词种别码
int JudgeBreedCode(char word[]) {
    // 判断是否为关键字
    for (int i = 0; i < 67; ++i) {
        if (strcmp(word, keyWord[i]) == 0) {
            return i + 1;
        }
    }

    // 判断是否为数字
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] < '0' || word[i] > '9') {
            return 68;
        }
    }

    // 判断是否为标识符
    for (int i = 0; i < strlen(word); i++) {
        if ((word[i] >= 'a' && word[i] <= 'z') || (
                word[i] >= 'A' && word[i] <= 'Z' || word[i] == '_' || (word[i] >= '0' && word[i] <= '9'))) {
            return 69;
        }
    }

    return 0;
}

// 判断是否为运算符
int isOperator(char a) {
    if (a <= 'z' && a >= 'a' || a <= 'Z' && a >= 'A' || a <= '9' && a >= '0' || a == '_') {
        return false;
    }
    return true;
}

// 判断是否为两个运算符
int isDoubleOperator(char a, char b) {
    // 关系运算符
    if (a == '=' && b == '=') {
        return true;
    }
    if (a == '!' && b == '=') {
        return true;
    }
    if (a == '<' && b == '=') {
        return true;
    }
    if (a == '>' && b == '=') {
        return true;
    }

    // 算术运算符
    if (a == '+' && b == '+') {
        return true;
    }
    if (a == '-' && b == '-') {
        return true;
    }

    // 逻辑运算符
    if (a == '&' && b == '&') {
        return true;
    }
    if (a == '|' && b == '|') {
        return true;
    }

    // 算术复合赋值运算符
    if (a == '+' && b == '=') {
        return true;
    }
    if (a == '-' && b == '=') {
        return true;
    }
    if (a == '*' && b == '=') {
        return true;
    }
    if (a == '/' && b == '=') {
        return true;
    }
    if (a == '%' && b == '=') {
        return true;
    }

    // 位运算复合赋值运算符
    if (a == '&' && b == '=') {
        return true;
    }
    if (a == '|' && b == '=') {
        return true;
    }
    if (a == '^' && b == '=') {
        return true;
    }
    if (a == '>' && b == '>') {
        return true;
    }
    if (a == '<' && b == '<') {
        return true;
    }

    return false;
}

// 词法分析
int TextAnalyze(const FileText *fileText) {
    // 分割字符串，存入Word数组,将运算符，标点符号与字符串分开,
    char Word[1000][100];
    int index = 0;
    int j = 0;
    for (int i = 0; i < fileText->length; i++) {
        if (!isSpace(fileText->text[i])) {
            // 判断是否为运算符，标点符号
            if (isOperator(fileText->text[i])) {
                // 判断是否为两个运算符
                if (isDoubleOperator(fileText->text[i], fileText->text[i + 1])) {
                    Word[index][j] = fileText->text[i];
                    Word[index][j + 1] = fileText->text[i + 1];
                    Word[index++][j + 2] = '\0';
                    i++;
                    j = 0;
                }
                // 为单个运算符,则存入Word数组
                else {
                    Word[index][0] = fileText->text[i];
                    Word[index++][1] = '\0';
                }
            }

            // 为字符串,数字
            else {
                Word[index][j] = fileText->text[i];
                j++;
                // 判断下一个字符是否为符号
                if (isOperator(fileText->text[i + 1])) {
                    Word[index++][j] = '\0';
                    j = 0;
                }
            }
        }

        // 遇到空格
        else {
            if (j != 0) {
                Word[index++][j] = '\0';
                j = 0;
            }
        }
    }

    for (int i = 0; i < index; i++) {
        printf("%s  %d \n", Word[i], JudgeBreedCode(Word[i]));
    }

    return true;
}

int main() {
    char filename[50] = "";
    InputFileName(filename);
    ReadText(filename, &fileText);
    Pretreatment(&fileText);
    TextAnalyze(&fileText);

    return 0;
}
