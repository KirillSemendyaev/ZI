#include <iostream>
#include <vector>
#include <random>
#include <map>

using namespace std;

bool TrueSimplicityCheck(unsigned int n)
{
    if (n < 4) return true;
    if ((n & 1) == 0) return false;

    unsigned int k = sqrt(n);
    for (unsigned int i = 3; i < (k + 1); i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

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

vector<unsigned int> EuclideanAlgorithm(unsigned int a, unsigned int b)
{
    if (a < b) {
        unsigned int t = a;
        a = b;
        b = t;
    }
    vector<unsigned int> U = {a, 1, 0}, V = {b, 0, 1}, T(3);
    unsigned int q;
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
    cout << "Im checking " << n << endl;
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
        cout << "THIS IS Q " << q << endl;
    } while (!SimplicityCheck(q));

    do {
        pg.second = ((double)rd() / ((double)rd.max() + 1.0)) * (pg.first - 4) + 2;
    } while (FastExpByMod(pg.second, q, pg.first) == 1);
    return pg;
}

unsigned int BabyStepGiantStep(unsigned int y, unsigned int a, unsigned int p)
{
    unsigned int k, m;
    map<unsigned int, unsigned int> babyStep;
    map<unsigned int, unsigned int>::iterator it;
    k = m = sqrt(p) + 1;
    cout << "k = " << k << ", m = " << m << endl;
    for (unsigned int j = m; j > 0; j--) {
        babyStep.insert(pair<unsigned int, unsigned int>(((long long)FastExpByMod(a, j - 1, p) * (long long)y) % p, j - 1));
        //cout << "j = " << j - 1 << ", (a ^ (j) * y) % p = " << (FastExpByMod(a, j - 1, p) * y) % p << endl;
    }
    cout << endl;
    for (unsigned int i = 1; i <= k; i++) {
        it = babyStep.find(FastExpByMod(a, m * i, p));
        //cout << "i = " << i << ", a ^ (i * m) % p = " << FastExpByMod(a, m * i, p) << endl;
        if ((it != babyStep.end()) && (y == FastExpByMod(a, i * m - it->second, p))) {
            cout << endl;
            return i * m - it->second;
        }
    }
    return 0;
}

class DHuser {
private:
    unsigned int secret_key, p, g;
    random_device rd;
public:
    DHuser(pair<unsigned int, unsigned int> pg) {
        secret_key = ((double)rd() / ((double)rd.max() + 1.0)) * 1e9 + 1e9;
        p = pg.first;
        g = pg.second;
    }

    void ShowMeYourSecret() {
        cout << "My secret = " << secret_key << endl;
    }

    unsigned int GetPublicKey() {
        return FastExpByMod(g, secret_key, p);
    }

    unsigned int CalcCommonKey(unsigned int key) {
        return FastExpByMod(key, secret_key, p);
    }
};

int main(int argc, char** argv)
{
    unsigned int a, x, p, y, simp;
    cout << "Input a, x and p:";
    cin >> a >> x >> p;
    y = FastExpByMod(a, x, p);

    cout << "Output: " << y << endl;

    vector<unsigned int> res(3);

    cout << "Input two numbers:";
    cin >> a >> x;

    res = EuclideanAlgorithm(a, x);

    cout << "GCD = " << res[0] << ", x = " << (signed int)res[1] << ", y = " << (signed int)res[2] << endl;

    simp = PrimeNubmerGen(1000000000, 4290000000);

    cout << "Prime number = " << simp;

    if (TrueSimplicityCheck(simp)) cout << " (its true)" << endl;
    else cout << " (actually, not)" << endl;

    pair<unsigned int, unsigned int> pg;
    pg = pgGenerator();
    cout << "P = " << pg.first << ", G = " << pg.second << endl;

    DHuser user1(pg), user2(pg);

    if (user1.CalcCommonKey(user2.GetPublicKey()) == user2.CalcCommonKey(user1.GetPublicKey())) cout << "DH works" << endl;
    else cout << "DH doesnt work" << endl;

    cout << "Z1 = " << user1.CalcCommonKey(user2.GetPublicKey()) << "\nZ2 = " << user2.CalcCommonKey(user1.GetPublicKey()) << endl;

    x = BabyStepGiantStep(user1.CalcCommonKey(user2.GetPublicKey()), user2.GetPublicKey(), pg.first);
    cout << "Y = " << user1.CalcCommonKey(user2.GetPublicKey()) << ", A = " << user2.GetPublicKey() << ", p = " << pg.first << endl;

    /* x = BabyStepGiantStep(12375846, 487880078 , 530531867); */
    user1.ShowMeYourSecret();
    cout << "X = " << x << endl;
    return 0;
}

