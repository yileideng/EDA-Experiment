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
    bool use; // 是否已被合并

    Term(string value) : value(value), use(false) {}
    ~Term() {} // 析构函数（配合手动释放内存）
};

// 从BLIF文件读取数据
vector<Term*> readBlifFile(const string& filename);
// 迭代合并项，返回质蕴含项
vector<Term*> getPrimeTerms(vector<Term*> terms);
// 尝试合并两个蕴含项
Term* merge(Term* aTerm, Term* bTerm);
// 查看某个蕴含项是否以存在防止重复添加
bool exist(Term* term, vector<Term*> terms);
// 获取最小覆盖
vector<Term*> getEssentialPrimeTerms(vector<vector<int>> form, vector<Term*> input, vector<Term*> primes);
// 判断某一行是否只有一个1,并返回1所在列
int onlyOne(vector<vector<int>> form, int line);
// 判断某一行是否有1，并返回第1个1所在列
int firstOne(vector<vector<int>> form, int line);
// 某项是否被另一项覆盖
bool cover(Term* coveredTerm, Term* coverTerm);
// 判断input[line]是否被覆盖
bool lineUsed(vector<vector<int>> form, int line);
// 判断primes[column]是否使用
bool columnUsed(vector<vector<int>> form, int column);
// 删除第line行
void deleteLine(vector<vector<int>>& form, int line);
// 删除第column列
void deleteColumn(vector<vector<int>>& form, int column);
// 将二进制形式转换为字母形式
string toLiteralForm(string binary, int numVariables);

// 尝试合并两个蕴含项
Term* merge(Term* aTerm, Term* bTerm) {
    string a = aTerm->value;
    string b = bTerm->value;
    if (a.size() != b.size()) {
        return nullptr;
    }

    // 比较两个蕴含项不同位的个数
    int diffCount = 0;
    string result = "";
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            // 两位不同，且含有*。例如1和*
            if (a[i] == '*' || b[i] == '*') {
                return nullptr;
            }
            // 两位不同，且不含*。例如1和0，则可以合并
            else {
                result += "*";
                // 记录合并的次数
                diffCount++;
            }
        }
        else {
            result += a[i];
        }
    }

    // 只合并一次才能返回。例如1111和0011是不能合并为**11的
    if (diffCount == 1) {
        aTerm->use = true;
        bTerm->use = true;
        return new Term(result);
    }

    return nullptr;
}

// 查看某个蕴含项是否以存在防止重复添加
bool exist(Term* term, vector<Term*> terms) {
    for (Term* t : terms) {
        if (t->value == term->value) {
            return true;
        }
    }
    return false;
}

// 迭代合并项，返回质蕴含项
vector<Term*> getPrimeTerms(vector<Term*> terms) {
    vector<Term*> primeTerms;

    do {
        for (Term* term : terms) {
            cout << term->value << " ";
        }
        cout << endl;
        vector<Term*> newTerms; // 存储本轮合并生成的新项

        // 合并所有可能的项对
        for (size_t i = 0; i < terms.size(); ++i) {
            for (size_t j = i + 1; j < terms.size(); ++j) {
                Term* mergedTerm = merge(terms[i], terms[j]);
                if (mergedTerm && !exist(mergedTerm, newTerms)) {
                    newTerms.push_back(mergedTerm);
                }
            }
        }

        // 收集未被合并的项（质蕴含项）
        for (Term* t : terms) {
            if (!t->use) {
                primeTerms.push_back(t);
            }
        }

        terms = newTerms; // 下一轮合并的输入为新生成的项
    } while (!terms.empty()); // 无新项生成时终止
    cout << endl;

    return primeTerms;
}

// 某项是否被另一项覆盖
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

// 判断input[line]是否被覆盖
bool lineUsed(vector<vector<int>> form, int line) {
    for (int j = 0; j < form[line].size(); j++) {
        if (form[line][j] != -1) {
            return false;
        }
    }
    return true;
}

// 判断primes[column]是否使用
bool columnUsed(vector<vector<int>> form, int column) {
    for (int i = 0; i < form.size(); i++) {
        if (form[i][column] != -1) {
            return false;
        }
    }
    return true;
}

// 删除第line行
void deleteLine(vector<vector<int>>& form, int line) {
    for (int j = 0; j < form[line].size(); j++) {
        form[line][j] = -1;
    }
}

// 删除第column列
void deleteColumn(vector<vector<int>>& form, int column) {
    for (int i = 0; i < form.size(); i++) {
        form[i][column] = -1;
    }
}

// 判断某一行是否只有一个1,并返回1所在列
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

// 判断某一行是否有1，并返回第1个1所在列
int firstOne(vector<vector<int>> form, int line) {
    for (int j = 0; j < form[line].size(); j++) {
        if (form[line][j] == 1) {
            return j;
        }
    }
    return -1;
}

// 获取最小覆盖
vector<Term*> getEssentialPrimeTerms(vector<vector<int>> form, vector<Term*> input, vector<Term*> primes) {
    vector<Term*> essentialPrimes;
    int coverCount = 0;

    while (coverCount != input.size()) {
        int oneColumn = -1;
        int line = -1;

        // 优先处理一行只有1个1
        for (int i = 0; i < form.size(); i++) {
            oneColumn = onlyOne(form, i);
            if (oneColumn != -1) {
                line = i;
                break;
            }
        }
        // 其次处理一行有多个1
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
            // 删除1所在行
            deleteLine(form, line);
            coverCount++;
            // 遍历1所在列，若该列的某项为1，则删除某项1所在行
            for (int j = 0; j < form.size(); j++) {
                if (form[j][oneColumn] == 1) {
                    deleteLine(form, j);
                    coverCount++;
                }
            }
            // 删除1所在列
            deleteColumn(form, oneColumn);
            essentialPrimes.push_back(primes[oneColumn]);
        }
    }

    return essentialPrimes;
}

// 将二进制形式转换为字母形式
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
        result = "1"; // 如果没有变量，代表逻辑1
    }

    return result;
}

// 从BLIF文件读取数据
vector<Term*> readBlifFile(const string& filename) {
    vector<Term*> input;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        return input;
    }

    string line;
    vector<string> inputs;
    bool inNamesSection = false;
    vector<string> cubes;

    while (getline(file, line)) {
        // 移除行首和行尾的空白字符
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // 跳过空行和注释
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        string token;
        iss >> token;

        if (token == ".model") {
            // 模型名称，忽略
        }
        else if (token == ".inputs") {
            // 读取输入变量
            inputs.clear();
            while (iss >> token) {
                inputs.push_back(token);
            }
        }
        else if (token == ".outputs") {
            // 读取输出变量，暂时忽略
        }
        else if (token == ".names") {
            // 开始读取逻辑函数
            inNamesSection = true;
            cubes.clear();

            // 读取该函数的输入变量
            vector<string> functionInputs;
            while (iss >> token) {
                functionInputs.push_back(token);
            }
        }
        else if (token == ".end") {
            // 文件结束
            break;
        }
        else if (inNamesSection) {
            // 读取立方体（minterms）
            string cube = token;
            string output;
            iss >> output;

            // 只处理输出为1的情况
            if (output == "1") {
                cubes.push_back(cube);
            }
        }
    }

    // 处理读取到的立方体
    for (const string& cube : cubes) {
        // 转换为二进制表示
        string binary;
        for (char c : cube) {
            if (c == '0' || c == '1') {
                binary += c;
            }
            else {
                // 处理无关项（如'-'），暂时忽略
                binary += '0'; // 简单处理，实际中可能需要更复杂的逻辑
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

    // 从BLIF文件读取数据
    input = readBlifFile("input.blif");

    if (input.empty()) {
        cerr << "未成功读取任何最小项，使用默认示例..." << endl;

        // 使用默认示例
        input = {
            new Term("0000"), new Term("1100"), new Term("1101"),
            new Term("0011"), new Term("0111"), new Term("1111"), new Term("1011")
        };
    }

    // 生成质蕴含项
    vector<Term*> primes = getPrimeTerms(input);

    // 输出质蕴含项
    cout << "质蕴含项：" << endl;
    for (Term* p : primes) {
        cout << p->value << endl;
    }

    // 初始化质蕴含项表
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

    // 打印质蕴含项表
    cout << "\n质蕴含项表：" << endl;
    for (int i = 0; i < input.size(); i++) {
        for (int j = 0; j < primes.size(); j++) {
            cout << form[i][j] << " ";
        }
        cout << endl;
    }

    // 获取最小覆盖
    vector<Term*> essentialPrimes = getEssentialPrimeTerms(form, input, primes);

    // 输出最小覆盖（二进制形式）
    cout << "\n最小覆盖（二进制形式）：" << endl;
    for (Term* t : essentialPrimes) {
        cout << t->value << endl;
    }

    // 输出最小覆盖（字母形式）
    cout << "\n最小覆盖（字母形式）：" << endl;
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
