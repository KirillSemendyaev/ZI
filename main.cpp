#include <iostream>
#include <vector>
#include <random>

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

int main(int argc, char** argv)
{
    random_device rd;
    unsigned int p = PrimeNubmerGen(1e8, 2e9), m, x1, x2, x3, x4;
    ShamirUser user1(p), user2(p);
    user1.showMeYourSecrets();
    user2.showMeYourSecrets();

    m = ((double)rd() / ((double)rd.max() + 1.0)) * (p - 2) + 1;
    cout << "Message = " << m << endl;

    x1 = user1.sendToUser(m);
    cout << "x1 = " << x1 << endl;

    x2 = user2.receiveFromUser(x1);
    cout << "x2 = " << x2 << endl;

    x3 = user1.replyToUser(x2);
    cout << "x3 = " << x3 << endl;

    x4 = user2.decryptMessage(x3);
    cout << "x4 = " << x4 << endl;

    return 0;
}

