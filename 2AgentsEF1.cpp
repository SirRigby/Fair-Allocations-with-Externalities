#include<bits/stdc++.h>
using namespace std;

class Item{
    int val[2][2];
public:
    int delta() const {
        return val[0][0] - val[0][1];
    }

    Item(){
        memset(val,0,sizeof(val));
    }

    Item(int a, int b, int c, int d){
        val[0][0] = a;
        val[0][1] = b;
        val[1][0] = c;
        val[1][1] = d;
    }

    pair<int,int> alloc(int i) const {
        if(i){
            return {val[0][1],val[1][1]};
        }
        else{
            return {val[0][0],val[1][0]};
        }
    }

    void printMatrix() const {
        cout << "    " << val[0][0] << ' ' << val[0][1] << '\n';
        cout << "    " << val[1][0] << ' ' << val[1][1] << '\n';
    }
};

class Alloc{
    vector<Item> res;
    vector<vector<Item>> alloc;

    void calc(){
        vector<Item> items=res;
        int val[2][2];
        memset(val,0,sizeof(val));
    
        alloc[0].clear();
        alloc[1].clear();
    
        for(const auto& i:items){
            if((val[0][0]-val[0][1]>=0)^(i.delta()>=0)){
                alloc[0].push_back(i);
                val[0][0]+=i.alloc(0).first;
                val[0][1]+=i.alloc(0).second;
                val[1][0]+=i.alloc(1).first;
                val[1][1]+=i.alloc(1).second;
            }
            else{
                alloc[1].push_back(i);
                val[0][1]+=i.alloc(0).first;
                val[0][0]+=i.alloc(0).second;
                val[1][1]+=i.alloc(1).first;
                val[1][0]+=i.alloc(1).second;
            }
        }
    
        if(val[1][0]>val[1][1]){
            swap(alloc[0],alloc[1]);
        }
    }

public:
    Alloc(){
        alloc.resize(2);
    }

    explicit Alloc(vector<Item> r) : res(std::move(r)) {
        alloc.resize(2);
    }
    void calcAlloc() {
        calc();
    }

    void printSummary() const {
        cout << "Exact allocation:\n";

        for (int agent = 0; agent < 2; agent++) {
            cout << "Agent " << agent + 1 << ":\n";

            if (alloc[agent].empty()) {
                cout << "  (no items)\n\n";
                continue;
            }

            long long total1 = 0, total2 = 0;

            for (int idx = 0; idx < (int)alloc[agent].size(); idx++) {
                const Item& it = alloc[agent][idx];

                cout << "  item " << idx + 1 << ":\n";
                cout << "  matrix:\n";
                it.printMatrix();
                cout << '\n';

                if (agent == 0) {
                    total1 += it.alloc(0).first;
                    total2 += it.alloc(0).second;
                } else {
                    total1 += it.alloc(1).first;
                    total2 += it.alloc(1).second;
                }
            }

            cout << "  total value for agent 1 = " << total1 << '\n';
            cout << "  total value for agent 2 = " << total2 << "\n\n";
        }
    }
};

int main(){
    int n = 10; 
    vector<Item> items;
    items.reserve(n);

    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    uniform_int_distribution<int> dist(-10, 10);

    for(int i = 0; i < n; i++){
        int a = dist(rng);
        int b = dist(rng);
        int c = dist(rng);
        int d = dist(rng);

        items.emplace_back(a, b, c, d);
    }

    Alloc A(items);
    A.calcAlloc();
    A.printSummary();

    return 0;
}