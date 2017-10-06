#include <iostream>
#include <vector>
#include <random>
#include <fstream>

using namespace std;

unsigned int FastExpByMod(unsigned int a, unsigned int x, unsigned int p)
{
    unsigned long long temp = 1, s = a;
    while (x) {
        if (x & 1) temp = (temp * s) % p;
        s = (s * s) % p;
        x /= 2;
    }
    return (unsigned int)temp;
}

vector<int> EuclideanAlgorithm(unsigned int a, unsigned int b)
{
    if (a < b) {
        unsigned int t = a;
        a = b;
        b = t;
    }
    vector<int> U = {a, 1, 0}, V = {b, 0, 1}, T(3);
    int q;
    while (V[0] != 0) {
        q = U[0] / V[0];
        T[0] = U[0] % V[0];
        T[1] = U[1] - q * V[1];
        T[2] = U[2] - q * V[2];
        U = V;
        V = T;
    }
    return U;
}

bool SimplicityCheck(unsigned int n)
{
    if (n < 4) return true;
    if ((n & 1) == 0) return false;
    unsigned int d = n - 1, s = 0, k = log2((double)n), a;
    char test;
    random_device rd;
//    cout << "Im checking " << n << endl;
    for (; (d & 1) == 0; d >>= 1) s++;

    for (unsigned int i = 0; i < k; i++) {
        test = 0;
        a = ((double)rd() / ((double)rd.max() + 1.0)) * (n - 4) + 2;
        if (EuclideanAlgorithm(n, a)[0] != 1) return false;
        if (FastExpByMod(a, d, n) == 1) test++;
        for (unsigned int r = 0; r < s; r++) {
            if (FastExpByMod(a, d * pow(2.0, (double)r), n) == (n - 1)) {
                test++;
                break;
            }
        }
        if (test == 0) return false;
    }

    return true;
}

unsigned int PrimeNubmerGen(unsigned int a, unsigned int b)
{
    random_device rd;
    bool f = false;
    unsigned int rand;
    for (int i = 0; (i < 100) && !f; i++) {
        rand = ((double)rd() / ((double)rd.max() + 1.0)) * (b - a) + a;
        f = SimplicityCheck(rand);
    }
    return f ? rand : -1;
}

pair<unsigned int, unsigned int> pgGenerator(void)
{
    pair<unsigned int, unsigned int> pg;
    unsigned int q;
    random_device rd;

    do {
        pg.first = PrimeNubmerGen(1e8, 4e9);
        q = (pg.first - 1) / 2;
//        cout << "THIS IS Q " << q << endl;
    } while (!SimplicityCheck(q));

    do {
        pg.second = ((double)rd() / ((double)rd.max() + 1.0)) * (pg.first - 4) + 2;
    } while (FastExpByMod(pg.second, q, pg.first) == 1);
    return pg;
}

class ShamirUser {
private:
    unsigned int p, c, d;
public:
    ShamirUser(unsigned int a) {
        p = a;
        random_device rd;
        vector<int> out(3);
        do {
            c = ((double)rd() / ((double)rd.max() + 1.0)) * (p - 2) + 1;
            out = EuclideanAlgorithm(c, p - 1);
        } while (out[0] != 1);
        d = (out[2] > 0) ? out[2] : (out[2] + p - 1);
    }

    unsigned int sendToUser(unsigned int m) {
        return FastExpByMod(m, c, p);
    }

    unsigned int receiveFromUser(unsigned int m) {
        return FastExpByMod(m, c, p);
    }

    unsigned int replyToUser(unsigned int m) {
        return FastExpByMod(m, d, p);
    }

    unsigned int decryptMessage(unsigned int m) {
        return FastExpByMod(m, d, p);
    }

    void showMeYourSecrets() {
        cout << "c = " << c << ", d = " << d << ", p = " << p << ", (c*d)%(p-1) = " << ((long long int)c * (long long int)d) % (long long int)(p - 1) << endl;
    }
};

class RSAUser{
private:
    unsigned int p, q, c, d, n;
public:
    RSAUser(unsigned int a, unsigned int b) {
        random_device rd;
        vector<int> out(3);
        p = PrimeNubmerGen(a, b);
        do {
            q = PrimeNubmerGen(a, b);
        } while (p == q);
        n = p * q;
        do {
            c = ((double)rd() / ((double)rd.max() + 1.0)) * ((p - 1) * (q - 1) - 2) + 1;
            out = EuclideanAlgorithm(c, (p - 1) * (q - 1));
        } while (out[0] != 1);
        d = (out[2] > 0) ? out[2] : (out[2] + (p - 1) * (q - 1));
    }

    unsigned int encryptMessage(unsigned int m, unsigned int db, unsigned int nb) {
        return FastExpByMod(m, db, nb);
    }

    unsigned int decryptMessage(unsigned int m) {
        return FastExpByMod(m, c, n);
    }

    int encryptFile(const char* str, unsigned int db, unsigned int nb) {
        ifstream fin(str, ios_base::binary | ios_base::in);
        ofstream fout("out.encrypted", ios_base::binary | ios_base::out |ios_base::trunc);
        if (!fin.is_open() || !fout.is_open()) {
            cout << "Can't open file" << endl;
            return -1;
        }
        unsigned int e;
        char symb;

        while (!fin.eof()) {
            fin.read(&symb, sizeof(symb));
            if (!fin.eof()) {
                e = FastExpByMod(symb, db, nb);
                fout.write((char *)&e, sizeof(e));
            }
        }
        fin.close();
        fout.close();
        return 0;
    }

    int decryptFile(const char* str) {
        ifstream fin(str, ios_base::binary | ios_base::in);
        ofstream fout("out.decrypted", ios_base::binary | ios_base::out | ios_base::trunc);
        if (!fin.is_open() || !fout.is_open()) {
            cout << "Can't open file" << endl;
            return -1;
        }
        char e;
        unsigned int symb;

        while (!fin.eof()) {
            fin.read((char *)&symb, sizeof(symb));
            if (!fin.eof()) {
                e = FastExpByMod(symb, c, n);
                fout.write(&e, sizeof(e));
            }
        }
        fin.close();
        fout.close();
        return 0;
    }

    void showMeYourSecrets() {
        cout << "P = " << p << ", Q = " << q << ", N = " << n << endl;
        cout << "C = " << c << ", D = " << d << endl;
    }

    unsigned int getN() {
        return n;
    }

    unsigned int getD() {
        return d;
    }

};

int main(int argc, char** argv)
{
    random_device rd;
    unsigned int p = PrimeNubmerGen(1e8, 2e9), m, x1, x2, x3, x4;
    ShamirUser shamiruser1(p), shamiruser2(p);
    shamiruser1.showMeYourSecrets();
    shamiruser2.showMeYourSecrets();

    m = ((double)rd() / ((double)rd.max() + 1.0)) * (p - 2) + 1;
    cout << "Message = " << m << endl;

    x1 = shamiruser1.sendToUser(m);
    cout << "x1 = " << x1 << endl;

    x2 = shamiruser2.receiveFromUser(x1);
    cout << "x2 = " << x2 << endl;

    x3 = shamiruser1.replyToUser(x2);
    cout << "x3 = " << x3 << endl;

    x4 = shamiruser2.decryptMessage(x3);
    cout << "x4 = " << x4 << endl;

    RSAUser rsauser1(256, 46340), rsauser2(256, 46340);

    rsauser1.showMeYourSecrets();
    rsauser2.showMeYourSecrets();

    m = ((double)rd() / ((double)rd.max() + 1.0)) * (rsauser2.getN() - 2) + 1;
    cout << "Message = " << m << endl;

    x1 = rsauser1.encryptMessage(m, rsauser2.getD(), rsauser2.getN());
    cout << "e = " << x1 << endl;

    x2 = rsauser2.decryptMessage(x1);
    cout << "m' = " << x2 << endl;

    rsauser1.encryptFile("Makefile", rsauser2.getD(), rsauser2.getN());
    rsauser2.decryptFile("out.encrypted");

    return 0;
}

