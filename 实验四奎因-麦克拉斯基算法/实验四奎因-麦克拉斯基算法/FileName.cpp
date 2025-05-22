#include<iostream>
#include<vector>
#include<string>
#include<unordered_set>
using namespace std;

class Term {
public:
    string value;
    bool use; // �Ƿ��ѱ��ϲ�

    Term(string value) : value(value), use(false) {}
    ~Term() {} // ��������������ֶ��ͷ��ڴ棩
};

// ���Ժϲ������̺���
Term* merge(Term* aTerm, Term* bTerm);
// �鿴ĳ���̺����Ƿ��Դ��ڷ�ֹ�ظ����
bool exist(Term* term, vector<Term*> terms);



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
vector<Term*> genPrimeTerms(vector<Term*> terms) {
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

    return primeTerms;
}

int main() {
    // ������С�4��������������ʽ��
    vector<Term*> input = {
        new Term("0000"), new Term("1100"), new Term("1101"),
        new Term("0011"), new Term("0111"), new Term("1111"), new Term("1011")
    };

    // �������̺���
    vector<Term*> primes = genPrimeTerms(input);

    // ������̺���
    cout << "���̺��" << endl;
    for (Term* p : primes) {
        cout << p->value << endl;
    }


    return 0;
}