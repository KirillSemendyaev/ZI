#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <time.h>

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

class PokerPlayer {
private:
    unsigned int c, d, P;
    vector<unsigned int> hand = {0, 0};
public:
    PokerPlayer(unsigned int Pin) {
        P = Pin;
        vector<int> out(3);
        do {
            c = ((double)rd() / ((double)rd.max() + 1.0)) * (P - 3) + 1;
            out = EuclideanAlgorithm(c, P - 1);
        } while (out[0] != 1);
        d = (out[2] > 0) ? out[2] : (out[2] + P - 1);
    }

    unsigned int encryptDeck(vector<unsigned int> *deck) {
        for (unsigned int &i: *deck) i = FastExpByMod(i, c, P);
        shuffle(deck->begin(), deck->end(), default_random_engine(time(0)));
    }

    void setHand(unsigned int a, unsigned int b) {
        hand[0] = a;
        hand[1] = b;
    }

    vector<unsigned int>* getHand() {
        return &hand;
    }

    void decryptHand(vector<unsigned int>* h) {
        for (vector<unsigned int>::iterator it = h->begin(); it != h->end(); it++)
            *it = FastExpByMod(*it, d, P);
    }

    void showHand() {
        cout << "First: ";
        switch (hand[0] % 100) {
            case 1:
                cout << "Ace";
                break;
            case 2:
                cout << "2";
                break;
            case 3:
                cout << "3";
                break;
            case 4:
                cout << "4";
                break;
            case 5:
                cout << "5";
                break;
            case 6:
                cout << "6";
                break;
            case 7:
                cout << "7";
                break;
            case 8:
                cout << "8";
                break;
            case 9:
                cout << "9";
                break;
            case 10:
                cout << "10";
                break;
            case 11:
                cout << "Jack";
                break;
            case 12:
                cout << "Queen";
                break;
            case 13:
                cout << "King";
                break;
            default:
                cout << "###";
        }

        switch (hand[0] / 100) {
            case 1:
                cout << " of Spades";
                break;
            case 2:
                cout << " of Hearts";
                break;
            case 3:
                cout << " of Clubs";
                break;
            case 4:
                cout << " of Diamonds";
                break;
            default:
                cout << " of UNKNOWN";
        }

        cout << endl << "Second: ";

        switch (hand[1] % 100) {
            case 1:
                cout << "Ace";
                break;
            case 2:
                cout << "2";
                break;
            case 3:
                cout << "3";
                break;
            case 4:
                cout << "4";
                break;
            case 5:
                cout << "5";
                break;
            case 6:
                cout << "6";
                break;
            case 7:
                cout << "7";
                break;
            case 8:
                cout << "8";
                break;
            case 9:
                cout << "9";
                break;
            case 10:
                cout << "10";
                break;
            case 11:
                cout << "Jack";
                break;
            case 12:
                cout << "Queen";
                break;
            case 13:
                cout << "King";
                break;
            default:
                cout << "###";
        }

        switch (hand[1] / 100) {
            case 1:
                cout << " of Spades";
                break;
            case 2:
                cout << " of Hearts";
                break;
            case 3:
                cout << " of Clubs";
                break;
            case 4:
                cout << " of Diamonds";
                break;
            default:
                cout << " of UNKNOWN";
        }
        cout << endl;
    }
};

int main(int argc, char** argv)
{
    vector<unsigned int> deck =  {101, 201, 301, 401,
                                  102, 202, 302, 402,
                                  103, 203, 303, 403,
                                  104, 204, 304, 404,
                                  105, 205, 305, 405,
                                  106, 206, 306, 406,
                                  107, 207, 307, 407,
                                  108, 208, 308, 408,
                                  109, 209, 309, 409,
                                  110, 210, 310, 410,
                                  111, 211, 311, 411,
                                  112, 212, 312, 412,
                                  113, 213, 313, 413};
    vector<unsigned int> table(5);
    unsigned int P = PrimeNubmerGen(1E9, 4E9);
    if (argc == 1)
        return -1;
    if (atoi(argv[1]) > 23)
        return -1;

    vector<PokerPlayer> players;
    for (int i = 0; i < atoi(argv[1]); i++) {
        PokerPlayer player(P);
        players.push_back(player);
    }

    for (PokerPlayer &x : players) {
        x.encryptDeck(&deck);
    }

    cout << "Size = " << atoi(argv[1]) << endl;
    cout << "P = " << P << endl;
    cout << "S = " << players.size() << endl;
    for (unsigned int &x: deck) {
        cout << x << ' ';
    }
    cout << endl;

    pair<unsigned int, unsigned int> temp;
    for (int i = 0; i < 5; i++) {
        table[i] = deck.back();
        deck.pop_back();
    }
    for (PokerPlayer &i : players) {
        temp.first = deck.back();
        deck.pop_back();
        temp.second = deck.back();
        deck.pop_back();
        i.setHand(temp.first, temp.second);
        for (int j = 0; j < atoi(argv[1]); j++) {
            if (&(players[j]) == &i)
                continue;
            players[j].decryptHand(i.getHand());
        }
        i.decryptHand(i.getHand());
        i.decryptHand(&table);
    }

    cout << endl << endl << "Table: " << endl;
    for (int i = 0; i < 5; i++) {
        cout << table[i] << ": ";
        switch (table[i] % 100) {
            case 1:
                cout << "Ace";
                break;
            case 2:
                cout << "2";
                break;
            case 3:
                cout << "3";
                break;
            case 4:
                cout << "4";
                break;
            case 5:
                cout << "5";
                break;
            case 6:
                cout << "6";
                break;
            case 7:
                cout << "7";
                break;
            case 8:
                cout << "8";
                break;
            case 9:
                cout << "9";
                break;
            case 10:
                cout << "10";
                break;
            case 11:
                cout << "Jack";
                break;
            case 12:
                cout << "Queen";
                break;
            case 13:
                cout << "King";
                break;
            default:
                cout << "###";
        }

        switch (table[i] / 100) {
            case 1:
                cout << " of Spades";
                break;
            case 2:
                cout << " of Hearts";
                break;
            case 3:
                cout << " of Clubs";
                break;
            case 4:
                cout << " of Diamonds";
                break;
            default:
                cout << " of UNKNOWN";
        }
        cout << endl;
    }

    for (int i = 0; i < atoi(argv[1]); i++) {
        cout << i + 1 << " player:" << endl;
        players[i].showHand();
    }


    return 0;
}

