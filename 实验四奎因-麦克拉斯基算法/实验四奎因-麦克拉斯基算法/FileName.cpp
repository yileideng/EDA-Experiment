#include<iostream>
#include<vector>
#include<string>
#include<unordered_set>
#include<fstream>
#include<sstream>
using namespace std;

class Term {
public:
    string value;
    bool use; // �Ƿ��ѱ��ϲ�

    Term(string value) : value(value), use(false) {}
    ~Term() {} // ��������������ֶ��ͷ��ڴ棩
};

// ��BLIF�ļ���ȡ����
vector<Term*> readBlifFile(const string& filename);
// �����ϲ���������̺���
vector<Term*> getPrimeTerms(vector<Term*> terms);
// ���Ժϲ������̺���
Term* merge(Term* aTerm, Term* bTerm);
// �鿴ĳ���̺����Ƿ��Դ��ڷ�ֹ�ظ����
bool exist(Term* term, vector<Term*> terms);
// ��ȡ��С����
vector<Term*> getEssentialPrimeTerms(vector<vector<int>> form, vector<Term*> input, vector<Term*> primes);
// �ж�ĳһ���Ƿ�ֻ��һ��1,������1������
int onlyOne(vector<vector<int>> form, int line);
// �ж�ĳһ���Ƿ���1�������ص�1��1������
int firstOne(vector<vector<int>> form, int line);
// ĳ���Ƿ���һ���
bool cover(Term* coveredTerm, Term* coverTerm);
// �ж�input[line]�Ƿ񱻸���
bool lineUsed(vector<vector<int>> form, int line);
// �ж�primes[column]�Ƿ�ʹ��
bool columnUsed(vector<vector<int>> form, int column);
// ɾ����line��
void deleteLine(vector<vector<int>>& form, int line);
// ɾ����column��
void deleteColumn(vector<vector<int>>& form, int column);
// ����������ʽת��Ϊ��ĸ��ʽ
string toLiteralForm(string binary, int numVariables);

// ���Ժϲ������̺���
Term* merge(Term* aTerm, Term* bTerm) {
    string a = aTerm->value;
    string b = bTerm->value;
    if (a.size() != b.size()) {
        return nullptr;
    }

    // �Ƚ������̺��ͬλ�ĸ���
    int diffCount = 0;
    string result = "";
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            // ��λ��ͬ���Һ���*������1��*
            if (a[i] == '*' || b[i] == '*') {
                return nullptr;
            }
            // ��λ��ͬ���Ҳ���*������1��0������Ժϲ�
            else {
                result += "*";
                // ��¼�ϲ��Ĵ���
                diffCount++;
            }
        }
        else {
            result += a[i];
        }
    }

    // ֻ�ϲ�һ�β��ܷ��ء�����1111��0011�ǲ��ܺϲ�Ϊ**11��
    if (diffCount == 1) {
        aTerm->use = true;
        bTerm->use = true;
        return new Term(result);
    }

    return nullptr;
}

// �鿴ĳ���̺����Ƿ��Դ��ڷ�ֹ�ظ����
bool exist(Term* term, vector<Term*> terms) {
    for (Term* t : terms) {
        if (t->value == term->value) {
            return true;
        }
    }
    return false;
}

// �����ϲ���������̺���
vector<Term*> getPrimeTerms(vector<Term*> terms) {
    vector<Term*> primeTerms;

    do {
        for (Term* term : terms) {
            cout << term->value << " ";
        }
        cout << endl;
        vector<Term*> newTerms; // �洢���ֺϲ����ɵ�����

        // �ϲ����п��ܵ����
        for (size_t i = 0; i < terms.size(); ++i) {
            for (size_t j = i + 1; j < terms.size(); ++j) {
                Term* mergedTerm = merge(terms[i], terms[j]);
                if (mergedTerm && !exist(mergedTerm, newTerms)) {
                    newTerms.push_back(mergedTerm);
                }
            }
        }

        // �ռ�δ���ϲ�������̺��
        for (Term* t : terms) {
            if (!t->use) {
                primeTerms.push_back(t);
            }
        }

        terms = newTerms; // ��һ�ֺϲ�������Ϊ�����ɵ���
    } while (!terms.empty()); // ����������ʱ��ֹ
    cout << endl;

    return primeTerms;
}

// ĳ���Ƿ���һ���
bool cover(Term* coveredTerm, Term* coverTerm) {
    string covered = coveredTerm->value;
    string cover = coverTerm->value;
    if (covered.size() != cover.size()) {
        return false;
    }
    for (int i = 0; i < cover.size(); i++) {
        if (covered[i] != cover[i] && cover[i] != '*') {
            return false;
        }
    }
    return true;
}

// �ж�input[line]�Ƿ񱻸���
bool lineUsed(vector<vector<int>> form, int line) {
    for (int j = 0; j < form[line].size(); j++) {
        if (form[line][j] != -1) {
            return false;
        }
    }
    return true;
}

// �ж�primes[column]�Ƿ�ʹ��
bool columnUsed(vector<vector<int>> form, int column) {
    for (int i = 0; i < form.size(); i++) {
        if (form[i][column] != -1) {
            return false;
        }
    }
    return true;
}

// ɾ����line��
void deleteLine(vector<vector<int>>& form, int line) {
    for (int j = 0; j < form[line].size(); j++) {
        form[line][j] = -1;
    }
}

// ɾ����column��
void deleteColumn(vector<vector<int>>& form, int column) {
    for (int i = 0; i < form.size(); i++) {
        form[i][column] = -1;
    }
}

// �ж�ĳһ���Ƿ�ֻ��һ��1,������1������
int onlyOne(vector<vector<int>> form, int line) {
    int count = 0;
    int oneIndex = 0;
    for (int j = 0; j < form[line].size(); j++) {
        if (form[line][j] == 1) {
            count++;
            oneIndex = j;
        }
    }

    if (count == 1) {
        return oneIndex;
    }

    return -1;
}

// �ж�ĳһ���Ƿ���1�������ص�1��1������
int firstOne(vector<vector<int>> form, int line) {
    for (int j = 0; j < form[line].size(); j++) {
        if (form[line][j] == 1) {
            return j;
        }
    }
    return -1;
}

// ��ȡ��С����
vector<Term*> getEssentialPrimeTerms(vector<vector<int>> form, vector<Term*> input, vector<Term*> primes) {
    vector<Term*> essentialPrimes;
    int coverCount = 0;

    while (coverCount != input.size()) {
        int oneColumn = -1;
        int line = -1;

        // ���ȴ���һ��ֻ��1��1
        for (int i = 0; i < form.size(); i++) {
            oneColumn = onlyOne(form, i);
            if (oneColumn != -1) {
                line = i;
                break;
            }
        }
        // ��δ���һ���ж��1
        if (oneColumn == -1) {
            for (int i = 0; i < form.size(); i++) {
                oneColumn = firstOne(form, i);
                if (oneColumn != -1) {
                    line = i;
                    break;
                }
            }
        }

        if (oneColumn != -1) {
            // ɾ��1������
            deleteLine(form, line);
            coverCount++;
            // ����1�����У������е�ĳ��Ϊ1����ɾ��ĳ��1������
            for (int j = 0; j < form.size(); j++) {
                if (form[j][oneColumn] == 1) {
                    deleteLine(form, j);
                    coverCount++;
                }
            }
            // ɾ��1������
            deleteColumn(form, oneColumn);
            essentialPrimes.push_back(primes[oneColumn]);
        }
    }

    return essentialPrimes;
}

// ����������ʽת��Ϊ��ĸ��ʽ
string toLiteralForm(string binary, int numVariables) {
    string result = "";
    char var = 'A';

    for (int i = 0; i < binary.size(); i++) {
        if (binary[i] == '0') {
            result += var;
            result += '\'';
        }
        else if (binary[i] == '1') {
            result += var;
        }
        var++;
    }

    if (result.empty()) {
        result = "1"; // ���û�б����������߼�1
    }

    return result;
}

// ��BLIF�ļ���ȡ����
vector<Term*> readBlifFile(const string& filename) {
    vector<Term*> input;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << filename << endl;
        return input;
    }

    string line;
    vector<string> inputs;
    bool inNamesSection = false;
    vector<string> cubes;

    while (getline(file, line)) {
        // �Ƴ����׺���β�Ŀհ��ַ�
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // �������к�ע��
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        string token;
        iss >> token;

        if (token == ".model") {
            // ģ�����ƣ�����
        }
        else if (token == ".inputs") {
            // ��ȡ�������
            inputs.clear();
            while (iss >> token) {
                inputs.push_back(token);
            }
        }
        else if (token == ".outputs") {
            // ��ȡ�����������ʱ����
        }
        else if (token == ".names") {
            // ��ʼ��ȡ�߼�����
            inNamesSection = true;
            cubes.clear();

            // ��ȡ�ú������������
            vector<string> functionInputs;
            while (iss >> token) {
                functionInputs.push_back(token);
            }
        }
        else if (token == ".end") {
            // �ļ�����
            break;
        }
        else if (inNamesSection) {
            // ��ȡ�����壨minterms��
            string cube = token;
            string output;
            iss >> output;

            // ֻ�������Ϊ1�����
            if (output == "1") {
                cubes.push_back(cube);
            }
        }
    }

    // �����ȡ����������
    for (const string& cube : cubes) {
        // ת��Ϊ�����Ʊ�ʾ
        string binary;
        for (char c : cube) {
            if (c == '0' || c == '1') {
                binary += c;
            }
            else {
                // �����޹����'-'������ʱ����
                binary += '0'; // �򵥴���ʵ���п�����Ҫ�����ӵ��߼�
            }
        }

        if (!binary.empty()) {
            input.push_back(new Term(binary));
        }
    }

    file.close();
    return input;
}

int main() {
    vector<Term*> input;

    // ��BLIF�ļ���ȡ����
    input = readBlifFile("input.blif");

    if (input.empty()) {
        cerr << "δ�ɹ���ȡ�κ���С�ʹ��Ĭ��ʾ��..." << endl;

        // ʹ��Ĭ��ʾ��
        input = {
            new Term("0000"), new Term("1100"), new Term("1101"),
            new Term("0011"), new Term("0111"), new Term("1111"), new Term("1011")
        };
    }

    // �������̺���
    vector<Term*> primes = getPrimeTerms(input);

    // ������̺���
    cout << "���̺��" << endl;
    for (Term* p : primes) {
        cout << p->value << endl;
    }

    // ��ʼ�����̺����
    vector<vector<int>> form(input.size(), vector<int>(primes.size()));
    for (int i = 0; i < input.size(); i++) {
        for (int j = 0; j < primes.size(); j++) {
            if (cover(input[i], primes[j])) {
                form[i][j] = 1;
            }
            else {
                form[i][j] = 0;
            }
        }
    }

    // ��ӡ���̺����
    cout << "\n���̺����" << endl;
    for (int i = 0; i < input.size(); i++) {
        for (int j = 0; j < primes.size(); j++) {
            cout << form[i][j] << " ";
        }
        cout << endl;
    }

    // ��ȡ��С����
    vector<Term*> essentialPrimes = getEssentialPrimeTerms(form, input, primes);

    // �����С���ǣ���������ʽ��
    cout << "\n��С���ǣ���������ʽ����" << endl;
    for (Term* t : essentialPrimes) {
        cout << t->value << endl;
    }

    // �����С���ǣ���ĸ��ʽ��
    cout << "\n��С���ǣ���ĸ��ʽ����" << endl;
    cout << "F = ";
    for (int i = 0; i < essentialPrimes.size();i++) {        
        cout << toLiteralForm(essentialPrimes[i]->value, essentialPrimes[i]->value.size()) ;
        if (i != essentialPrimes.size() - 1) {
            cout << " + ";
        }
    }
    cout << endl;
    return 0;
}
