#include <bits/stdc++.h>
using namespace std;

bool typeInit = false;
map<int,int> typeToVal, valToType;

void valToMatrix(int val, int mat[3][3]) {
    for (int i = 0; i < 3; i++) {
        int row = (val >> (3 * i)) & 7;
        for (int j = 0; j < 3; j++) {
            mat[i][j] = (row >> j) & 1;
        }
    }
}

void printMatrix(int mat[3][3]) {
    cout << "[";
    for (int i = 0; i < 3; i++) {
        cout << "[";
        for (int j = 0; j < 3; j++) {
            cout << mat[i][j];
            if (j != 2) cout << ", ";
        }
        cout << "]";
        if (i != 2) cout << '\n';
    }
    cout << "]\n";
}

void initTypeHelper(int i, int& cnt, int cur = 0) {
    if (i == 3) {
        typeToVal[cnt] = cur;
        valToType[cur] = cnt;
        cnt++;
        return;
    }
    initTypeHelper(i + 1, cnt, cur);
    for (int j = 0; j < 8; j++) {
        if (j & (1 << i)) {
            initTypeHelper(i + 1, cnt, cur | (j << (3 * i)));
        }
    }
}

void initType(bool printTypes = false) {
    if (typeInit) return;
    typeToVal.clear();
    valToType.clear();
    int cnt = 0;
    initTypeHelper(0, cnt);
    typeInit = true;

    if (printTypes) {
        for (auto &kv : typeToVal) {
            cout << "Type " << kv.first << ":\n";
            int mat[3][3];
            valToMatrix(kv.second, mat);
            printMatrix(mat);
        }
    }
}

class Item {
    int type;
    int val;
public:
    Item() : type(0), val(0) {}

    static Item fromVal(int v) {
        Item x;
        x.val = v;
        x.type = valToType.at(v);
        return x;
    }

    static Item fromType(int t) {
        Item x;
        x.type = t;
        x.val = typeToVal.at(t);
        return x;
    }

    int getType() const { return type; }
    int getVal()  const { return val; }
};

set<int> remTypes;

class Alloc {
    int res[125];                  
    int alloc[3][125];            
    int deferred[3][125];         

    static int bitAt(int v, int r, int c) {
        return (v >> (3 * r + c)) & 1;
    }

    static bool rowAllOnes(int v, int r) {
        for (int c = 0; c < 3; c++) if (!bitAt(v, r, c)) return false;
        return true;
    }

    static bool colAllOnes(int v, int c) {
        for (int r = 0; r < 3; r++) if (!bitAt(v, r, c)) return false;
        return true;
    }

    static int firstSafeAgentRule2(int v) {
        for (int i = 0; i < 3; i++) {
            if (bitAt(v, i, i) == 0) return i;
        }
        return -1;
    }

    static int firstSafeAgentRule3(int v) {
        for (int i = 0; i < 3; i++) {
            if (rowAllOnes(v, i)) return i;
        }
        return -1;
    }

    static int utilityOfAgentForOwner(int observer, int type, int owner) {
        int v = typeToVal.at(type);
        return bitAt(v, observer, owner);
    }

    static int utilityForAgentUnderTwoItemAssignment(int observer, int ta, int ownerA, int tb, int ownerB) {
        return utilityOfAgentForOwner(observer, ta, ownerA) + utilityOfAgentForOwner(observer, tb, ownerB);
    }


    static bool pairAssignmentIsEF(int ta, int ownerA, int tb, int ownerB) {
        for (int p = 0; p < 3; p++) {
            for (int q = 0; q < 3; q++) {
                if (p == q) continue;

                int curOwnerA = ownerA, curOwnerB = ownerB;
                int swapOwnerA = curOwnerA, swapOwnerB = curOwnerB;

                if (swapOwnerA == p) swapOwnerA = q;
                else if (swapOwnerA == q) swapOwnerA = p;

                if (swapOwnerB == p) swapOwnerB = q;
                else if (swapOwnerB == q) swapOwnerB = p;

                int curU  = utilityForAgentUnderTwoItemAssignment(p, ta, curOwnerA, tb, curOwnerB);
                int swapU = utilityForAgentUnderTwoItemAssignment(p, ta, swapOwnerA, tb, swapOwnerB);
                if (swapU > curU) return false;
            }
        }
        return true;
    }

    int currentUtility(int observer) const {
        int ans = 0;
        for (int a = 0; a < 3; a++) {
            for (int t = 0; t < 125; t++) {
                ans += alloc[a][t] * utilityOfAgentForOwner(observer, t, a);
            }
        }
        return ans;
    }

    int utilityAfterSwap(int observer, int i, int j) const {
        int ans = 0;
        for (int a = 0; a < 3; a++) {
            int mapped = a;
            if (a == i) mapped = j;
            else if (a == j) mapped = i;
            for (int t = 0; t < 125; t++) {
                ans += alloc[a][t] * utilityOfAgentForOwner(observer, t, mapped);
            }
        }
        return ans;
    }

    bool currentAllocationIsEF1() const {
        for (int i = 0; i < 3; i++) {
            int cur = currentUtility(i);
            for (int j = 0; j < 3; j++) {
                if (i == j) continue;
                int sw = utilityAfterSwap(i, i, j);
                if (sw > cur + 1) return false; // binary additive setting, up to one item
            }
        }
        return true;
    }

    void addItemToAgent(int type, int agent) {
        alloc[agent][type]++;
    }

    void rule4() {
        for (int i = 0; i < 125; i++) {
            int k = res[i] / 3;
            for (int j = 0; j < 3; j++) alloc[j][i] += k;
            res[i] %= 3;
            if (res[i] == 0) remTypes.erase(i);
        }
    }

    void rule7() {
        if (res[0] > 0) {
            alloc[0][0] += res[0];
            res[0] = 0;
            remTypes.erase(0);
        }
    }

    void rule1() {
        for (int i = 0; i < 125; i++) {
            if (res[i] == 0) continue;
            int v = typeToVal[i];
            for (int col = 0; col < 3; col++) {
                if (colAllOnes(v, col)) {
                    alloc[col][i] += res[i];
                    res[i] = 0;
                    remTypes.erase(i);
                    break;
                }
            }
        }
    }

    void rule2_start() {
        for (int i = 0; i < 125; i++) {
            if (res[i] == 0) continue;
            int v = typeToVal[i];
            int safe = firstSafeAgentRule2(v);
            if (safe != -1) {
                deferred[safe][i] += res[i];
                res[i] = 0;
                remTypes.erase(i);
            }
        }
    }

    void rule3_start() {
        for (int i = 0; i < 125; i++) {
            if (res[i] == 0) continue;
            int v = typeToVal[i];
            int safe = firstSafeAgentRule3(v);
            if (safe != -1) {
                deferred[safe][i] += res[i];
                res[i] = 0;
                remTypes.erase(i);
            }
        }
    }


    void rule2_3_end() {
        for (int safe = 0; safe < 3; safe++) {
            int a = (safe + 1) % 3;
            int b = (safe + 2) % 3;

            vector<int> leftovers;
            for (int t = 0; t < 125; t++) {
                int cnt = deferred[safe][t];
                if (cnt == 0) continue;

                int half = cnt / 2;
                alloc[a][t] += half;
                alloc[b][t] += half;
                if (cnt % 2) leftovers.push_back(t);
            }

            for (int t : leftovers) {
                alloc[a][t]++;
                if (!currentAllocationIsEF1()) {
                    alloc[a][t]--;
                    alloc[b][t]++;
                    if (!currentAllocationIsEF1()) {
                        alloc[b][t]--;
                        alloc[a][t]++;
                        int scoreA = 0, scoreB = 0;
                        for (int x = 0; x < 3; x++) {
                            int curA = currentUtility(x);
                            int worstA = 0;
                            for (int y = 0; y < 3; y++) if (x != y) worstA = max(worstA, utilityAfterSwap(x, x, y) - curA);
                            scoreA += worstA;
                        }
                        alloc[a][t]--;

                        alloc[b][t]++;
                        for (int x = 0; x < 3; x++) {
                            int curB = currentUtility(x);
                            int worstB = 0;
                            for (int y = 0; y < 3; y++) if (x != y) worstB = max(worstB, utilityAfterSwap(x, x, y) - curB);
                            scoreB += worstB;
                        }
                        alloc[b][t]--;

                        if (scoreA <= scoreB) alloc[a][t]++;
                        else alloc[b][t]++;
                    }
                }
            }
        }
    }

    bool isRule5A(int t) {
        int v = typeToVal[t];
        int mask  = (1<<0) | (1<<1) | (1<<2) | (1<<4) | (1<<8);
        int value = (1<<0) | (1<<4) | (1<<8);
        return (v & mask) == value;
    }

    bool isRule5B(int t) {
        int v = typeToVal[t];
        int mask  = (1<<0) | (1<<3) | (1<<4) | (1<<5) | (1<<8);
        int value = (1<<0) | (1<<4) | (1<<8);
        return (v & mask) == value;
    }

    bool isRule5C(int t) {
        int v = typeToVal[t];
        int mask  = (1<<0) | (1<<4) | (1<<6) | (1<<7) | (1<<8);
        int value = (1<<0) | (1<<4) | (1<<8);
        return (v & mask) == value;
    }

    bool canTakeTriple(int ta, int tb, int tc) {
        int need[125] = {};
        need[ta]++;
        need[tb]++;
        need[tc]++;
        for (int i = 0; i < 125; i++) {
            if (need[i] > res[i]) return false;
        }
        return true;
    }

    void takeRule5Triple(int ta, int tb, int tc) {
        res[ta]--; res[tb]--; res[tc]--;
        alloc[0][ta]++;
        alloc[1][tb]++;
        alloc[2][tc]++;
    }

    void rule5() {
        while (true) {
            bool found = false;
            for (int ta = 0; ta < 125 && !found; ta++) {
                if (res[ta] == 0 || !isRule5A(ta)) continue;
                for (int tb = 0; tb < 125 && !found; tb++) {
                    if (res[tb] == 0 || !isRule5B(tb)) continue;
                    for (int tc = 0; tc < 125; tc++) {
                        if (res[tc] == 0 || !isRule5C(tc)) continue;
                        if (canTakeTriple(ta, tb, tc)) {
                            takeRule5Triple(ta, tb, tc);
                            found = true;
                            break;
                        }
                    }
                }
            }
            if (!found) break;
        }
    }

    void rule6() {
        while (true) {
            bool found = false;

            for (int ta = 0; ta < 125 && !found; ta++) {
                if (res[ta] == 0) continue;
                for (int tb = ta; tb < 125 && !found; tb++) {
                    if (res[tb] == 0) continue;
                    if (ta == tb && res[ta] < 2) continue;

                    for (int i = 0; i < 3 && !found; i++) {
                        for (int j = 0; j < 3 && !found; j++) {
                            if (i == j) continue;
                            if (pairAssignmentIsEF(ta, i, tb, j)) {
                                res[ta]--;
                                res[tb]--;
                                alloc[i][ta]++;
                                alloc[j][tb]++;
                                found = true;
                            }
                        }
                    }
                }
            }

            if (!found) break;
        }
    }

    int totalRemainingItems() const {
        int s = 0;
        for (int t = 0; t < 125; t++) s += res[t];
        return s;
    }

    int utilityWithExtra(
        int observer,
        const int extra[3][125],
        bool doSwap = false,
        int si = -1,
        int sj = -1,
        int remOwner = -1,
        int remType = -1
    ) const {
        int ans = 0;
        for (int owner = 0; owner < 3; owner++) {
            int mapped = owner;
            if (doSwap) {
                if (owner == si) mapped = sj;
                else if (owner == sj) mapped = si;
            }

            for (int t = 0; t < 125; t++) {
                int cnt = alloc[owner][t] + extra[owner][t];
                if (owner == remOwner && t == remType) cnt--;
                if (cnt < 0) return INT_MIN / 4;
                ans += cnt * utilityOfAgentForOwner(observer, t, mapped);
            }
        }
        return ans;
    }

    bool ef1ExactWithExtra(const int extra[3][125]) const {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (i == j) continue;

                int cur = utilityWithExtra(i, extra, false);
                int sw  = utilityWithExtra(i, extra, true, i, j);

                if (cur >= sw) continue; 

                bool fixed = false;

                for (int owner = 0; owner < 3 && !fixed; owner++) {
                    for (int t = 0; t < 125 && !fixed; t++) {
                        int cnt = alloc[owner][t] + extra[owner][t];
                        if (cnt == 0) continue;

                        int cur2 = utilityWithExtra(i, extra, false, -1, -1, owner, t);
                        int sw2  = utilityWithExtra(i, extra, true, i, j, owner, t);

                        if (cur2 >= sw2) fixed = true;
                    }
                }

                if (!fixed) return false;
            }
        }
        return true;
    }

    bool dfsResidual(
        int idx,
        const vector<int>& items,
        int extra[3][125]
    ) {
        if (idx == (int)items.size()) {
            return ef1ExactWithExtra(extra);
        }

        int t = items[idx];

        for (int agent = 0; agent < 3; agent++) {
            extra[agent][t]++;

            if (dfsResidual(idx + 1, items, extra)) return true;

            extra[agent][t]--;
        }

        return false;
    }

    bool exhaustiveSearchRemaining() {
        vector<int> items;
        for (int t = 0; t < 125; t++) {
            for (int k = 0; k < res[t]; k++) items.push_back(t);
        }

        if (items.empty()) return true;

        if ((int)items.size() > 12) {
            cerr << "Residual instance too large for exhaustive search: "
                 << items.size() << " items\n";
            return false;
        }

        int extra[3][125];
        memset(extra, 0, sizeof(extra));

        bool ok = dfsResidual(0, items, extra);
        if (!ok) return false;

        for (int a = 0; a < 3; a++) {
            for (int t = 0; t < 125; t++) {
                alloc[a][t] += extra[a][t];
            }
        }

        for (int t = 0; t < 125; t++) {
            res[t] = 0;
            remTypes.erase(t);
        }

        return true;
    }

    void calcAlloc() {
        rule4();
        rule7();
        rule1();
        rule2_start();
        rule3_start();
        rule5();
        rule6();
        rule2_3_end();
        bool ok = exhaustiveSearchRemaining();
        if (!ok) {
            cerr << "No EF1 completion found for residual instance.\n";
        }
    }

public:
    Alloc() {
        memset(res, 0, sizeof(res));
        memset(alloc, 0, sizeof(alloc));
        memset(deferred, 0, sizeof(deferred));
    }

    explicit Alloc(int r[125]) {
        for (int i = 0; i < 125; i++) res[i] = r[i];
        memset(alloc, 0, sizeof(alloc));
        memset(deferred, 0, sizeof(deferred));
    }

    void calc() {
        calcAlloc();
    }

    void printSummary() const {
        cout << "Exact allocation:\n";
        for (int agent = 0; agent < 3; agent++) {
            cout << "Agent " << agent + 1 << ":\n";

            bool any = false;
            long long total = 0;

            for (int t = 0; t < 125; t++) {
                if (alloc[agent][t] == 0) continue;
                any = true;
                total += alloc[agent][t];

                cout << "  type " << t << " x " << alloc[agent][t] << "\n";

                int mat[3][3];
                valToMatrix(typeToVal.at(t), mat);
                cout << "  matrix:\n";
                for (int i = 0; i < 3; i++) {
                    cout << "    ";
                    for (int j = 0; j < 3; j++) {
                        cout << mat[i][j] << (j == 2 ? '\n' : ' ');
                    }
                }
                cout << '\n';
            }

            if (!any) {
                cout << "  (no items)\n\n";
            } else {
                cout << "  total = " << total << "\n\n";
            }
        }

        cout << "Residual unassigned items:\n";
        bool anyRes = false;
        for (int t = 0; t < 125; t++) {
            if (res[t] == 0) continue;
            anyRes = true;
            cout << "  type " << t << " x " << res[t] << "\n";
        }
        if (!anyRes) cout << "  (none)\n";
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    initType(false);
    remTypes.clear();
    for (int i = 0; i < 125; i++) remTypes.insert(i);

    int res[125];
    for (int i = 0; i < 125; i++) res[i] = 2;

    Alloc a(res);
    a.calc();
    a.printSummary();
    cout << remTypes.size() << '\n';
    return 0;
}
