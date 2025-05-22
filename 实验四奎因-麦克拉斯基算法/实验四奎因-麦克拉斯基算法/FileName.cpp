#include<iostream>
#include<vector>
#include<string>
#include<unordered_set>
using namespace std;

class Term {
public:
    string value;
    bool use; // 是否已被合并

    Term(string value) : value(value), use(false) {}
    ~Term() {} // 析构函数（配合手动释放内存）
};

// 尝试合并两个蕴含项
Term* merge(Term* aTerm, Term* bTerm);
// 查看某个蕴含项是否以存在防止重复添加
bool exist(Term* term, vector<Term*> terms);



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
vector<Term*> genPrimeTerms(vector<Term*> terms) {
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

    return primeTerms;
}

int main() {
    // 输入最小项（4变量，二进制形式）
    vector<Term*> input = {
        new Term("0000"), new Term("1100"), new Term("1101"),
        new Term("0011"), new Term("0111"), new Term("1111"), new Term("1011")
    };

    // 生成质蕴含项
    vector<Term*> primes = genPrimeTerms(input);

    // 输出质蕴含项
    cout << "质蕴含项：" << endl;
    for (Term* p : primes) {
        cout << p->value << endl;
    }


    return 0;
}