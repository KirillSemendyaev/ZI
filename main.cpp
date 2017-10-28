#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include "cppcrypto/sha256.h"

using namespace std;

random_device rd;

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

    do {
        pg.first = PrimeNubmerGen(1e8, 4e9);
        if (pg.first == (unsigned int)-1)
            continue;
        q = (pg.first - 1) / 2;
    } while (!SimplicityCheck(q));

    do {
        pg.second = ((double)rd() / ((double)rd.max() + 1.0)) * (pg.first - 4) + 2;
    } while (FastExpByMod(pg.second, q, pg.first) == 1);
    return pg;
}

class ELGUser {
private:
    unsigned int p, g, x, y;
public:
    ELGUser(pair<unsigned int, unsigned int> pg) {
        p = pg.first;
        g = pg.second;
        x = ((double)rd() / ((double)rd.max() + 1.0)) * (p - 2) + 1;
        y = FastExpByMod(g, x, p);
    }

    unsigned int getY() {
        return y;
    }

    unsigned int signFile(const string &str) {
        ifstream fin(str, ios_base::in | ios_base::binary);
        ofstream fout(str + ".ELGsignature", ios_base::out | ios_base::trunc | ios_base::binary);

        if (!fin.is_open() || !fout.is_open()) {
            cout << "Can't open file" << endl;
            return 1;
        }

        uint8_t c;
        uint8_t hash[32] = {0};
        cppcrypto::sha256 hc;
        hc.init();
        fin.read((char *)&c, 1);
        while (!fin.eof()) {
            hc.update(&c, 1);
            fin.read((char *)&c, 1);
        }
        hc.final(hash);

        unsigned int s, r, k = ((double)rd() / ((double)rd.max() + 1.0)) * (p - 3) + 1;
        long long temp;
        unsigned int k_inv;
        vector<int> gdc = EuclideanAlgorithm(p - 1, k);
        while (gdc[0] != 1) {
            k = ((double)rd() / ((double)rd.max() + 1.0)) * (p - 3) + 1;
            gdc = EuclideanAlgorithm(p - 1, k);
        }
        k_inv = (gdc[2] > 0) ? gdc[2] : (gdc[2] + p - 1);

        for (int i = 0; i < 32; i++) {
            r = FastExpByMod(g, k, p);
            temp = (unsigned long long)hash[i] - ((unsigned long long)x * (unsigned long long)r) % (p - 1);
            s = (((temp > 0) ? temp : (temp + p - 1)) * (unsigned long long)k_inv) % (p - 1);
            fout.write((char*)&r, 4);
            fout.write((char*)&s, 4);
        }

        fin.close();
        fout.close();
        return 0;
    }

    bool verifySignature (const string &str, unsigned int ya) {
        ifstream fin(str, ios_base::in | ios_base::binary);
        ifstream fin_sign(str + ".ELGsignature", ios_base::in | ios_base::binary);

        if (!fin.is_open() || !fin_sign.is_open()) {
            cout << "Can't open file" << endl;
            return false;
        }

        uint8_t c;
        uint8_t hash[32] = {0};
        cppcrypto::sha256 hc;
        hc.init();
        fin.read((char *)&c, 1);
        while (!fin.eof()) {
            hc.update(&c, 1);
            fin.read((char *)&c, 1);
        }
        hc.final(hash);

        unsigned int s, r;
        int i = 0;
        bool flag = true;
        unsigned long long temp;
        fin_sign.read((char *)&r, 4);
        fin_sign.read((char *)&s, 4);
        while (!fin_sign.eof()/* && flag*/) {
            temp = ((unsigned long long)FastExpByMod(ya, r, p) * (unsigned long long)FastExpByMod(r, s, p)) % p;
            if ((unsigned int)temp != FastExpByMod(g, hash[i], p))
                flag = false;
            cout << temp << " = " << FastExpByMod(g, hash[i], p) << endl;
            fin_sign.read((char *)&r, 4);
            fin_sign.read((char *)&s, 4);
            i++;
        }
        fin.close();
        fin_sign.close();
        return flag;
    }
};

int main(int argc, char** argv)
{
    pair<unsigned int, unsigned int> pg = pgGenerator();
    ELGUser elgUser1(pg), elgUser2(pg);
    elgUser1.signFile("fb.deb");
    if (elgUser2.verifySignature("fb.deb", elgUser1.getY()))
        cout << "It's OK" << endl;
    else
        cout << "It's NOT OK" << endl;
    return 0;
}

