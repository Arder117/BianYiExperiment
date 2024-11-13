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
    char text[1000]; // �ļ�����
    int length; // �ļ�����
    int pointer; // �ļ�ָ��
};

FileText fileText;

// ��������
// �����ļ���,�����ļ���
int InputFileName(char filename[]);
// ��ȡ�ļ�����
int ReadText(char filename[], struct FileText *fileText);
// Ԥ����ȥ�����봮�еĻس��������з���������ȱ༭�����֣�����հ׷��ϲ�Ϊһ����
int Pretreatment(struct FileText *fileText);
// �ж��Ƿ�Ϊ�հ׷�
int isSpace(char a);
// �жϵ����ֱ��룬�����ֱ���
int JudgeBreedCode(char word[]);
// �ж��Ƿ�Ϊ�����
int isOperator(char a);
// �ж��Ƿ�Ϊ˫Ŀ�����
int isDoubleOperator(char a, char b);
// �ʷ�����
int TextAnalyze(const FileText *fileText);


// �����ļ���
int InputFileName(char filename[]) {
    printf("Please input the filename:");
    scanf("%s", filename);
    return true;
}

// ��ȡ�ļ�����
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

// Ԥ����ȥ�����봮�еĻس��������з���������ȱ༭�����֣�����հ׷��ϲ�Ϊһ����
int Pretreatment(struct FileText *fileText) {
    // ȥ��ע��
    for (int i = 0; i < strlen(fileText->text); i++) {
        if (fileText->text[i] == '/' && fileText->text[i + 1] == '/') {
            while (fileText->text[i] != '\n') {
                fileText->text[i] = ' ';
                i++;
            }
        }
    }

    // ȥ���س��������з��������
    for (int i = 0; i < strlen(fileText->text); i++) {
        if (fileText->text[i] == '\n' || fileText->text[i] == '\r' || fileText->text[i] == '\t') {
            fileText->text[i] = ' ';
        }
    }

    // ȥ������ո񣬱�����һ���ո�
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

// �ж��Ƿ�Ϊ�ո�
int isSpace(char a) {
    if (a == ' ' || a == NULL) {
        return true;
    }
    return false;
}

// �жϵ����ֱ���
int JudgeBreedCode(char word[]) {
    // �ж��Ƿ�Ϊ�ؼ���
    for (int i = 0; i < 67; ++i) {
        if (strcmp(word, keyWord[i]) == 0) {
            return i + 1;
        }
    }

    // �ж��Ƿ�Ϊ����
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] < '0' || word[i] > '9') {
            return 68;
        }
    }

    // �ж��Ƿ�Ϊ��ʶ��
    for (int i = 0; i < strlen(word); i++) {
        if ((word[i] >= 'a' && word[i] <= 'z') || (
                word[i] >= 'A' && word[i] <= 'Z' || word[i] == '_' || (word[i] >= '0' && word[i] <= '9'))) {
            return 69;
        }
    }

    return 0;
}

// �ж��Ƿ�Ϊ�����
int isOperator(char a) {
    if (a <= 'z' && a >= 'a' || a <= 'Z' && a >= 'A' || a <= '9' && a >= '0' || a == '_') {
        return false;
    }
    return true;
}

// �ж��Ƿ�Ϊ���������
int isDoubleOperator(char a, char b) {
    // ��ϵ�����
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

    // ���������
    if (a == '+' && b == '+') {
        return true;
    }
    if (a == '-' && b == '-') {
        return true;
    }

    // �߼������
    if (a == '&' && b == '&') {
        return true;
    }
    if (a == '|' && b == '|') {
        return true;
    }

    // �������ϸ�ֵ�����
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

    // λ���㸴�ϸ�ֵ�����
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

// �ʷ�����
int TextAnalyze(const FileText *fileText) {
    // �ָ��ַ���������Word����,������������������ַ����ֿ�,
    char Word[1000][100];
    int index = 0;
    int j = 0;
    for (int i = 0; i < fileText->length; i++) {
        if (!isSpace(fileText->text[i])) {
            // �ж��Ƿ�Ϊ�������������
            if (isOperator(fileText->text[i])) {
                // �ж��Ƿ�Ϊ���������
                if (isDoubleOperator(fileText->text[i], fileText->text[i + 1])) {
                    Word[index][j] = fileText->text[i];
                    Word[index][j + 1] = fileText->text[i + 1];
                    Word[index++][j + 2] = '\0';
                    i++;
                    j = 0;
                }
                // Ϊ���������,�����Word����
                else {
                    Word[index][0] = fileText->text[i];
                    Word[index++][1] = '\0';
                }
            }

            // Ϊ�ַ���,����
            else {
                Word[index][j] = fileText->text[i];
                j++;
                // �ж���һ���ַ��Ƿ�Ϊ����
                if (isOperator(fileText->text[i + 1])) {
                    Word[index++][j] = '\0';
                    j = 0;
                }
            }
        }

        // �����ո�
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
