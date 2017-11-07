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

vector<unsigned int> qPaGenerator(void)
{
    vector<unsigned int> qPa(3);
    /*do {
        qPa[1] = PrimeNubmerGen(1 << 31, UINT32_MAX);
        qPa[0] = PrimeNubmerGen(1 << 16, (1 << 17) - 1);
        for (int i = 0; (i < 50) && (((qPa[1] - 1) % qPa[0]) != 0); i++)
            qPa[0] = PrimeNubmerGen(1 << 16, (1 << 17) - 1);

    } while (((qPa[1] - 1) % qPa[0]) != 0);*/

    unsigned long long int b;
    do {
        qPa[0] = PrimeNubmerGen(1 << 16, (1 << 17) - 1);
        b = ((double)rd() / ((double)rd.max() + 1.0)) * ((double)2147483647 / qPa[0]) + (double)2147483648 / qPa[0];
        qPa[1] = qPa[0] * b + 1;
        //cout << "P = " << qPa[1] << endl;
    } while (!SimplicityCheck(qPa[1]) || (qPa[1] < 2147483648));

    /*bool flag = true;
    do {
        qPa[1] = PrimeNubmerGen(2147483648, 4294967295);
        cout << "P = " << qPa[1] << endl;
        for (qPa[0] = (1 << 16) + 1; qPa[0] <= ((1 << 17) - 1); qPa[0] += 2) {
            if (SimplicityCheck(qPa[0])) {
                if (((qPa[1] - 1) % qPa[0]) == 0) {
                    flag = false;
                    break;
                }
            }
        }
    } while (flag);*/
    unsigned int g = ((double)rd() / ((double)rd.max() + 1.0)) * (qPa[1] - 2) + 1;
    qPa[2] = FastExpByMod(g, (qPa[1] - 1) / qPa[0], qPa[1]);
    return qPa;
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

        for (int i = 0; i < 32; i++) {
            while (gdc[0] != 1) {
                k = ((double)rd() / ((double)rd.max() + 1.0)) * (p - 3) + 1;
                gdc = EuclideanAlgorithm(p - 1, k);
            }
            k_inv = (gdc[2] > 0) ? gdc[2] : (gdc[2] + p - 1);

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

class RSAUser {
private:
    unsigned int p, q, N, c, d;
public:
    RSAUser() {
        p = PrimeNubmerGen(257, 46340);
        do {
            q = PrimeNubmerGen(257, 46340);
        } while (q == p);
        N = p * q;
        c = ((double)rd() / ((double)rd.max() + 1.0)) * ((p - 1) * (q - 1) - 2) + 1;
        vector<int> out = EuclideanAlgorithm(c, (p - 1) * (q - 1));
        while (out[0] != 1) {
            c = ((double)rd() / ((double)rd.max() + 1.0)) * ((p - 1) * (q - 1) - 2) + 1;
            out = EuclideanAlgorithm(c, (p - 1) * (q - 1));
        }
        d = (out[2] > 0) ? out[2] : (out[2] + (p - 1) * (q - 1));
    }

    unsigned int getN() {
        return N;
    }

    unsigned int getD() {
        return d;
    }

    unsigned int signFile(const string &str) {
        ifstream fin(str, ios_base::in | ios_base::binary);
        ofstream fout(str + ".RSAsignature", ios_base::out | ios_base::trunc | ios_base::binary);

        if (!fin.is_open() || !fout.is_open()) {
            cout << "Can't open file" << endl;
            return 1;
        }

        uint8_t symb;
        uint8_t hash[32] = {0};
        cppcrypto::sha256 hc;
        hc.init();
        fin.read((char *)&symb, 1);
        while (!fin.eof()) {
            hc.update(&symb, 1);
            fin.read((char *)&symb, 1);
        }
        hc.final(hash);

        unsigned int e;
        for (int i = 0; i < 32; i++) {
            e = FastExpByMod(hash[i], c, N);
            fout.write((char *)&e, 4);
        }
        fin.close();
        fout.close();
        return 0;
    }

    bool verifySignature(const string &str, unsigned int Nb, unsigned int db) {
        ifstream fin(str, ios_base::in | ios_base::binary);
        ifstream fin_sign(str + ".RSAsignature", ios_base::in | ios_base::binary);

        if (!fin.is_open() || !fin_sign.is_open()) {
            cout << "Can't open file" << endl;
            return false;
        }

        uint8_t symb;
        uint8_t hash[32] = {0};
        cppcrypto::sha256 hc;
        hc.init();
        fin.read((char *)&symb, 1);
        while (!fin.eof()) {
            hc.update(&symb, 1);
            fin.read((char *)&symb, 1);
        }
        hc.final(hash);

        unsigned int e, i = 0;
        bool flag = true;
        fin_sign.read((char *) &e, 4);
        while(!fin_sign.eof()/* && flag*/) {
            e = FastExpByMod(e, db, Nb);
            cout << e << " = " << (unsigned int)hash[i] << endl;
            if (e != hash[i])
                flag = false;
            fin_sign.read((char *) &e, 4);
            i++;
        }

        fin.close();
        fin_sign.close();
        return flag;
    }
};

class GOSTUser {
private:
    unsigned int q, a, P, x, y;
public:
    GOSTUser(vector<unsigned int> &qPa) {
        q = qPa[0];
        P = qPa[1];
        a = qPa[2];
        x = ((double)rd() / ((double)rd.max() + 1.0)) * (q - 2) + 1;
        y = FastExpByMod(a, x, P);
    }

    unsigned int signFile(const string &str) {
        ifstream fin(str, ios_base::in | ios_base::binary);
        ofstream fout(str + ".GOSTsignature", ios_base::out | ios_base::trunc | ios_base::binary);

        if (!fin.is_open() || !fout.is_open()) {
            cout << "Can't open file" << endl;
            return 1;
        }

        uint8_t symb;
        uint8_t hash[32] = {0};
        cppcrypto::sha256 hc;
        hc.init();
        fin.read((char *) &symb, 1);
        while (!fin.eof()) {
            hc.update(&symb, 1);
            fin.read((char *) &symb, 1);
        }
        hc.final(hash);

        unsigned int k, s = 0, r = 0;

        for(int i = 0; i < 32; i++) {
            r = 0;
            s = 0;
            while ((r == 0) || (s == 0)) {
                k = ((double) rd() / ((double) rd.max() + 1.0)) * (q - 2) + 1;
                r = FastExpByMod(a, k, P) % q;
                if (r == 0) continue;
                s = ((unsigned long long int)k * (unsigned long long int)hash[i] + (unsigned long long int)x * (unsigned long long int)r) % q;
            }
            //cout << r << ", " << s << endl;
            fout.write((char *)&r, 4);
            fout.write((char *)&s, 4);
        }

        fin.close();
        fout.close();
        return 0;
    }

    bool verifySignature(const string &str, unsigned int yb) {
        ifstream fin(str, ios_base::in | ios_base::binary);
        ifstream fin_sign(str + ".GOSTsignature", ios_base::in | ios_base::binary);

        if (!fin.is_open() || !fin_sign.is_open()) {
            cout << "Can't open file" << endl;
            return false;
        }

        uint8_t symb;
        uint8_t hash[32] = {0};
        cppcrypto::sha256 hc;
        hc.init();
        fin.read((char *) &symb, 1);
        while (!fin.eof()) {
            hc.update(&symb, 1);
            fin.read((char *) &symb, 1);
        }
        hc.final(hash);

        bool flag = true;
        int temp;
        unsigned int u1, u2, r, s, v;
        for (int i = 0; i < 32 ; i++) {
            fin_sign.read((char *) &r, 4);
            fin_sign.read((char *) &s, 4);
            temp = EuclideanAlgorithm(hash[i], q)[2];
            temp = (temp > 0) ? temp : (temp + q);
            u1 = ((unsigned long long int) s * (unsigned long long int) temp) % q;
            u2 = ((unsigned long long int)(q - r) * (unsigned long long int) temp) % q;
            v = (((unsigned long long int)FastExpByMod(a, u1, P) * (unsigned long long int)FastExpByMod(yb, u2, P)) % P) % q;
            cout << v << " = " << r << endl;
            if (v != r)
                flag = false;
        }


        fin.close();
        fin_sign.close();
        return flag;
    }

    unsigned int getY() {
        return y;
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

    RSAUser rsaUser1, rsaUser2;
    rsaUser1.signFile("fb.deb");
    if (rsaUser2.verifySignature("fb.deb", rsaUser1.getN(), rsaUser1.getD()))
        cout << "It's OK" << endl;
    else
        cout << "It's NOT OK" << endl;

    vector<unsigned int> qPa = qPaGenerator();
    cout << qPa[0] << endl << qPa[1] << endl << qPa[2] << endl;

    GOSTUser gostUser1(qPa), gostUser2(qPa);

    gostUser1.signFile("fb.deb");
    if (gostUser2.verifySignature("fb.deb", gostUser1.getY()))
        cout << "It's OK" << endl;
    else
        cout << "It's NOT OK" << endl;

    return 0;
}

