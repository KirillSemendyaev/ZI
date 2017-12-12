#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <bitset>
#include <tuple>
#include <fstream>

using namespace std;

typedef struct {
    unsigned int first;
    unsigned int second;
} Edge;

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

class UserA {
private:
    unsigned int v, e, nc, p, q, N;
    vector<unsigned int> colors, vertices, c, d, colorization;
    vector<Edge> edges;
public:
    UserA(string str)
    {
        p = PrimeNubmerGen(257, 46340);
        do {
            q = PrimeNubmerGen(257, 46340);
        } while (p == q);
        N = p * q;
        ifstream fin(str, ios_base::in);
        fin >> v;
        fin >> e;
        fin >> nc;
        unsigned int color, index;

        for (unsigned int i = 0; i < nc; i++) {
            fin >> color;
            colors.push_back(color);
        }
        for (int i = 0; i < v; i++) {
            fin >> index;
            colorization.push_back(index);
            vertices.push_back(1);
        }
        for (int i = 0; i < e; i++) {
            Edge edge = {0, 0};
            edges.push_back(edge);
            fin >> index;
            edges[i].first = index;
            fin >> index;
            edges[i].second = index;
        }
        fin.close();
    }

    pair<vector<Edge>, vector<unsigned int>> encryptGraph()
    {
        random_shuffle(colors.begin(), colors.end());
        for (int i = 0; i < v; i++) {
            vertices[i] = ((unsigned int)(((double)rd() / ((double)rd.max() + 1.0)) * (N / 4 - 2) + 1)) * 4 + colors[colorization[i]];
        }

        vector<int> out;
        c.clear();
        d.clear();
        for (int i = 0; i < v; i++) {
            c.push_back(0);
            do {
                c[i] = ((double)rd() / ((double)rd.max() + 1.0)) * ((p - 1) * (q - 1) - 2) + 1;
                out = EuclideanAlgorithm(c[i], (p - 1) * (q - 1));
            } while (out[0] != 1);
            d.push_back((out[2] > 0) ? out[2] : (out[2] + (p - 1) * (q - 1)));
            vertices[i] = FastExpByMod(vertices[i], d[i], N);
        }
        return (pair<vector<Edge>, vector<unsigned int>>){edges, vertices};
    }

    void showMeEdges()
    {
        for (int i = 0; i < e; i++) {
            cout << "Edge " << i + 1 << ":\n first: ";
            cout << bitset<32>(vertices[edges[i].first]);
            cout << "\n second: ";
            cout << bitset<32>(vertices[edges[i].second]) << endl;
        }
    }

    tuple<unsigned int, unsigned int, unsigned int> sendC(unsigned int choice)
    {
        tuple<unsigned int, unsigned int, unsigned int> Cs;
        if ((choice > e) || (choice < 1)) {
            cout << "WHAT?? Choice = " << choice << endl;
            return {0, 0, N};
        }

        Cs = {c[edges[choice - 1].first], c[edges[choice - 1].second], N};

        return Cs;
    }
};

class UserB {
private:
    unsigned int firstV, secondV, choice;
public:
    UserB() {}

    unsigned int chooseEdge(pair<vector<Edge>, vector<unsigned int>> graph)
    {
        choice = (double)rd() / (double)rd.max() * (graph.first.size() - 1) + 1;
        firstV = graph.second[graph.first[choice - 1].first];
        secondV = graph.second[graph.first[choice - 1].second];
        return choice;
    }

    bool checkColors(tuple<unsigned int, unsigned int, unsigned int> Cs)
    {
        firstV = FastExpByMod(firstV, get<0>(Cs), get<2>(Cs));
        secondV = FastExpByMod(secondV, get<1>(Cs), get<2>(Cs));
        cout << "First: " << bitset<32>(firstV) << endl;
        cout << "Second: " << bitset<32>(firstV) << endl;
        if ((firstV & 3) == (secondV & 3))
            return false;
        else
            return true;
    }

};

int main(int argc, char** argv)
{
    UserA userA("GRAPH");
    UserB userB;
    int i;
    bool flag = true;
    for (i = 0; i < 10 && flag; i++) {
        flag = userB.checkColors(userA.sendC(userB.chooseEdge(userA.encryptGraph())));
    }
    if (flag)
        cout << "\n===TRUTH===\n";
    else
        cout << "\n====LIE====\n" << i << endl;
    return 0;
}

