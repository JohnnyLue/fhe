#include <iostream>
#include <random>
#include <ctime>
#include <cmath>

using namespace std;

typedef unsigned long long ull;

// implememtation of simple FHE
class FHE {
    public:
        FHE(ull maxInt = 1000, int maxMulDepth = 1) {
            random_device rd;
            gen = new mt19937(rd());
            this->maxInt = maxInt;
            this->maxMulDepth = maxMulDepth;
            cout << "MaxInt: " << maxInt << ", MaxMulDepth: " << maxMulDepth << endl;
        }

        bool keygen() {
            // max value of r*sk = pow(ull max value, 1 / (maxMulDepth+1)) - maxInt
            ull maxSk = ((ull)1 << (64/(maxMulDepth + 1)));
            if (maxSk < maxInt) {
                cout << "Invalid maxInt and maxMulDepth." << endl;
                return false;
            }
            maxSk -= maxInt;
            maxSk >>= 5; // room for r
            if (maxSk < maxInt) {
                cout << "Invalid maxInt and maxMulDepth." << endl;
                return false;
            }
            cout << "Max sk: " << maxSk << endl;

            ull minSk = max(maxInt, maxSk >> 1); // sk's least value
            if (minSk >= maxSk) {
                cout << "Invalid maxInt and maxMulDepth." << endl;
                return false;
            }
            cout << "Min sk: " << minSk << endl;
            
            sk =  (*gen)() % (maxSk - minSk) + minSk; // random number from minSk to maxSk - 1
            if (~sk & 1) sk++; // make sure sk is odd and in the range
            cout << "Secret key: " << sk << endl;
            return true;
        }

        ull encrypt(ull p) {
            if (sk == 0) {
                cout << "Key is not generated." << endl;
                return -1;
            }
            p = p % maxInt;
            int r = 1 + (*gen)() % 30; // random number from 1 to 31: 5 bits
            return sk * r + p;
        }

        ull decrypt(ull c) {
            if (sk == 0) {
                cout << "Key is not generated." << endl;
                return -1;
            }
            return (c % sk) % maxInt;
        }

        ~FHE() {
            delete gen;
        }

    private:
        ull sk = 0;
        ull maxInt;
        int maxMulDepth;
        mt19937* gen;
};

ull cal(ull a, ull b, ull c) {
    return a*a*a + b*c + c;
}

int main(int argc, char** argv) {
    ull maxInt = 1000;
    int maxMulDepth = 3;
    if (argc == 3) {
        maxInt = atoi(argv[1]);
        maxMulDepth = atoi(argv[2]);
    }
    FHE fhe = FHE(maxInt, maxMulDepth);
    bool success = fhe.keygen();
    if (!success) {
        cout << "Key generation failed." << endl;
        return 1;
    }
    cout << endl;
    ull p1 = 11;
    ull p2 = 20;
    ull p3 = 18;
    ull c1 = fhe.encrypt(p1);
    ull c2 = fhe.encrypt(p2);
    ull c3 = fhe.encrypt(p3);
    cout << "Values:" << endl;
    cout << "p1: " << p1 << ", p2: " << p2 << ", p3: " << p3 << endl;
    cout << "After ecrypt:" << endl;
    cout << "c1: " << c1 << ", c2: " << c2 << ", c3: " << c3 << endl << endl;
    cout << "p1^3 + p2*p3 + p3: " << cal(p1, p2, p3) << endl;
    cout << "c1^3 + c2*c3 + c3: " << cal(c1, c2, c3);
    cout << ", after decrypt: " << fhe.decrypt(cal(c1, c2, c3)) << endl;

    return 0;
}
