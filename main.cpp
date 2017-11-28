#include <iostream>
#include <vector>
#include <random>
#include "cppcrypto/sha256.h"

using namespace std;

random_device rd;

typedef struct {
    unsigned int denomination;
    unsigned int n;
    uint32_t* r;
    uint32_t* rr;
    uint8_t* hash;
    uint32_t* cryptedhash;
    uint32_t* cryptedsign;
    uint32_t* sign;
} BankNote;

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
    vector<long long int> U = {a, 1, 0}, V = {b, 0, 1}, T(3);
    long long int q;
    while (V[0] != 0) {
        q = U[0] / V[0];
        T[0] = U[0] % V[0];
        T[1] = U[1] - q * V[1];
        T[2] = U[2] - q * V[2];
        U = V;
        V = T;
    }
    vector<int> out(3);
    out[0] = U[0];
    out[1] = U[1];
    out[2] = U[2];
    return out;
}

bool SimplicityCheck(unsigned int n)
{
    if (n < 4) return true;
    if ((n & 1) == 0) return false;
    unsigned int d = n - 1, s = 0, k = log2((double)n), a;
    char test;
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
    bool f = false;
    unsigned int rand;
    /*for (int i = 0; (i < 100) && !f; i++) {
        rand = ((double)rd() / ((double)rd.max() + 1.0)) * (b - a) + a;
        f = SimplicityCheck(rand);
    }*/

    do {
        rand = ((double)rd() / ((double)rd.max() + 1.0)) * (b - a) + a;
        f = SimplicityCheck(rand);
    } while(!f);
    return f ? rand : 0;
}

class Bank
{
private:
    unsigned int p, q, N;
    unsigned int c[9], d[9];
public:
    Bank()
    {
        p = PrimeNubmerGen(257, 46340);
        do {
            q = PrimeNubmerGen(257, 46340);
        } while (q == p);
        N = p * q;
        vector<int> out;

        for (int i = 0; i < 9; i++) {
            c[i] = ((double) rd() / ((double) rd.max() + 1.0)) * ((p - 1) * (q - 1) - 2) + 1;
            out = EuclideanAlgorithm(c[i], (p - 1) * (q - 1));
            while (out[0] != 1) {
                c[i] = ((double) rd() / ((double) rd.max() + 1.0)) * ((p - 1) * (q - 1) - 2) + 1;
                out = EuclideanAlgorithm(c[i], (p - 1) * (q - 1));
            }
            d[i] = (out[2] > 0) ? out[2] : (out[2] + (p - 1) * (q - 1));
        }
    }

    void signBankNotes(vector<BankNote> *banknotes)
    {
        for (BankNote &x: *banknotes) {
            x.cryptedsign = new uint32_t[32];
            switch (x.denomination) {
                case 5000:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[0], N);
                    break;
                case 1000:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[1], N);
                    break;
                case 500:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[2], N);
                    break;
                case 100:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[3], N);
                    break;
                case 50:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[4], N);
                    break;
                case 10:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[5], N);
                    break;
                case 5:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[6], N);
                    break;
                case 2:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[7], N);
                    break;
                case 1:
                    for (int i = 0; i < 32; i++)
                        x.cryptedsign[i] = FastExpByMod(x.cryptedhash[i], c[8], N);
                    break;
            }

        }
    }

    bool verifySignature(vector<BankNote>* banknotes)
    {
        bool flag1 = true, flag2;
        uint8_t temphash[32];
        cppcrypto::sha256 hc;
        for (BankNote &x: *banknotes) {
            hc.init();
            hc.update(((uint8_t*)&(x.n)), 4);
            hc.final(temphash);
            flag2 = true;
            for (int i = 0; (i < 32) && flag2; i++) {
                switch (x.denomination) {
                    case 5000:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[0], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 1000:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[1], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 500:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[2], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 100:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[3], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 50:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[4], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 10:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[5], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 5:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[6], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 2:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[7], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                    case 1:
                        if (temphash[i] != FastExpByMod(x.sign[i], d[8], N)) {
                            flag2 = false;
                            flag1 = false;
                        }
                        break;
                }
            }
            cout << "Denom: " << x.denomination << "\nn: " << x.n << "\nIt's ";
            if (flag2)
                cout << "OK" << endl << endl;
            else
                cout << "NOT OK" << endl << endl;
        }
        return flag1;
    }

    unsigned int getN()
    {
        return N;
    }

    unsigned int* getD()
    {
        return d;
    }
};

class Anon
{
private:
    vector<BankNote> banknotes;
public:
    Anon() {}
    void generateBankNotes(unsigned int n)
    {
        unsigned int temp = n, code;
        for (BankNote &x: banknotes) {
            delete[](x.r);
            delete[](x.sign);
            delete[](x.cryptedhash);
            delete[](x.cryptedsign);
            delete[](x.hash);
            delete[](x.rr);
        }
        banknotes.clear();
        while (temp >= 5000) {
            temp -= 5000;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {5000, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        while (temp >= 1000) {
            temp -= 1000;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {1000, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        while (temp >= 500) {
            temp -= 500;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {500, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        while (temp >= 100) {
            temp -= 100;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {100, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        while (temp >= 50) {
            temp -= 50;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {50, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        while (temp >= 10) {
            temp -= 10;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {10, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        while (temp >= 5) {
            temp -= 5;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {5, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        while (temp >= 2) {
            temp -= 2;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {2, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
        if (temp >= 1) {
            temp -= 1;
            code = ((double) rd() / ((double) rd.max() + 1.0)) * 4294967295 + 1;
            banknotes.insert(banknotes.end(), {1, code, NULL, NULL, NULL, NULL, NULL, NULL});
        }
    }

    vector<BankNote>* giveMeYourMoney()
    {
        return &banknotes;
    }

    void hashBankNotes(unsigned int Nb, unsigned int *db)
    {
        cppcrypto::sha256 hc;
        long long int temp;

        for (BankNote &x: banknotes) {
            x.hash = new uint8_t[32];
            hc.init();
            hc.update(((uint8_t*)&(x.n)), 4);
            hc.final(x.hash);
            x.cryptedhash = new uint32_t[32];
            x.r = new uint32_t[32];
            x.rr = new uint32_t[32];

            for (int i = 0; i < 32; i++) {
                x.r[i] = ((double) rd() / ((double) rd.max() + 1.0)) * (Nb - 2) + 1;
                temp = EuclideanAlgorithm(Nb, x.r[i])[2];
                x.rr[i] = (temp > 0) ? temp : (temp + Nb);
                switch (x.denomination) {
                    case 5000:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[0], Nb)) % Nb;
                        break;
                    case 1000:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[1], Nb)) % Nb;
                        break;
                    case 500:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[2], Nb)) % Nb;
                        break;
                    case 100:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[3], Nb)) % Nb;
                        break;
                    case 50:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[4], Nb)) % Nb;
                        break;
                    case 10:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[5], Nb)) % Nb;
                        break;
                    case 5:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[6], Nb)) % Nb;
                        break;
                    case 2:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[7], Nb)) % Nb;
                        break;
                    case 1:
                        x.cryptedhash[i] = ((uint64_t)x.hash[i] * (uint64_t)FastExpByMod(x.r[i], db[8], Nb)) % Nb;
                        break;
                }
            }
        }
    }

    void showMoney()
    {
        if (banknotes.empty()) {
            cout << "No money" << endl;
            return;
        }

        for (BankNote &x: banknotes) {
            cout << "Denom: " << x.denomination << endl;
            cout << "N: " << x.n << endl;
            cout << "r:";
            if (x.r == NULL)
                 cout << " " << x.r << endl;
            else {
                for (int i = 0; i < 32; i++) {
                    cout << " " << x.r[i];
                }
                cout << endl;
            }
            cout << "rr:";
            if (x.rr == NULL)
                cout << " " << x.rr << endl;
            else {
                for (int i = 0; i < 32; i++) {
                    cout << " " << x.rr[i];
                }
                cout << endl;
            }
            cout << "hash:";
            if (x.hash == NULL)
                cout << " " << x.hash << endl;
            else {
                for (int i = 0; i < 32; i++) {
                    cout << " " << x.hash[i];
                }
                cout << endl;
            }
            cout << "cryptedhash:";
            if (x.cryptedhash == NULL)
                cout << " " << x.cryptedhash << endl;
            else {
                for (int i = 0; i < 32; i++) {
                    cout << " " << x.cryptedhash[i];
                }
                cout << endl;
            }
            cout << "cryptedsign:";
            if (x.cryptedsign == NULL)
                cout << " " << x.cryptedsign << endl;
            else {
                for (int i = 0; i < 32; i++) {
                    cout << " " << x.cryptedsign[i];
                }
                cout << endl;
            }
            cout << "sign:";
            if (x.sign == NULL)
                cout << " " << x.sign << endl;
            else {
                for (int i = 0; i < 32; i++) {
                    cout << " " << x.sign[i];
                }
                cout << endl;
            }
        }
    }

    void decryptSign(unsigned int Nb)
    {
        for (BankNote &x: banknotes) {
            x.sign = new uint32_t[32];
            for (int i = 0; i < 32; i++) {
                x.sign[i] = ((uint64_t)x.cryptedsign[i] * (uint64_t)x.rr[i]) % Nb;
            }
        }
    }
    ~Anon()
    {
        for (BankNote &x: banknotes) {
            delete[](x.r);
            delete[](x.sign);
            delete[](x.cryptedhash);
            delete[](x.cryptedsign);
            delete[](x.hash);
            delete[](x.rr);
        }
        banknotes.clear();
    }
};

int main(int argc, char** argv)
{
    Bank bank;
    Anon anon;
    anon.generateBankNotes(150);
    anon.hashBankNotes(bank.getN(), bank.getD());
    bank.signBankNotes(anon.giveMeYourMoney());
    anon.decryptSign(bank.getN());
    anon.showMoney();
    bank.verifySignature(anon.giveMeYourMoney());
    return 0;
}

