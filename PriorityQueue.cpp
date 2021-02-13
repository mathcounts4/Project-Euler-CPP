#define DEBUG 0

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

using namespace std;

#define DEBUG 0

// must have Value < Value defined or give a comparator class
// and map<Key,int> defined
// and = operator for Key and Value
// if DEBUG is defined as nonzero,
// to_str(Key) and to_str(Value)
// must also be defined
// which one can define here or as Key.toString() and Value.toString()
template <typename Key, typename Value, bool neverAgain = true, typename Comp = less<Value>> class p_queue {
private:
    template <typename T>
    string to_str(const T& x) {return x.to_string();}
    string to_str(int x) {return to_string(x);}
    string to_str(const vector<int>& x) {
        string res;
        res += "{";
        int s = x.size();
        if (s)
            res += to_str(x[0]);
        for (int i = 1; i < s; ++i)
            res += "," + to_str(x[i]);
        res += "}";
        return res;
    }
    string to_str(const pair<int,int>& p) {
        string res;
        res += "{";
        res += to_str(p.first);
        res += ",";
        res += to_str(p.second);
        res += "}";
        return res;
    }
private:
    int s; // size
    vector<Value> q; // priority queue vector
    map<Key,int> key_to_i; // maps key -> i
    vector<Key> i_to_key; // maps i -> key
    Comp comp;
    
    // maps i and key to each other
    void set_i_key(int i, const Key& key) {
        key_to_i[key] = i;
        i_to_key[i] = key;
    }
    
    // swaps values and necessary info at indiced i_1, i_2
    void swap_vals(int i_1, int i_2) {
        swap(q[i_1],q[i_2]);
        Key key_1 = i_to_key[i_1];
        Key key_2 = i_to_key[i_2];
        set_i_key(i_1,key_2);
        set_i_key(i_2,key_1);
    }
    
    /*
     *     0
     *   1   2
     *  3 4 5 6
     * 
     * parent(i) = (i-1)/2
     * child1(i) = 2*i+1
     * child2(i) = 2*i+2
     * 
     */
    // index of parent of i
    int parent(int i) {
        return --i/2;
    }
    
    // index of left child of i
    int child1(int i) {
        return 2*i+1;
    }
    
    // index of right child of i
    int child2(int i) {
        return 2*i+2;
    }
    
    // bubbles up starting from index i
    void bubble_up(int i) {
        for (int p; i; i = p) {
            p = parent(i);
            if (comp(q[i],q[p])) {
                swap_vals(i,p);
            } else {
                return;
            }
        }
    }
    
    // bubbles down starting from index i
    void bubble_down(int i) {
        for (int c; (c = child1(i)) < s; i = c) {
            if (comp(q[c],q[i])) {
                if (c+1 < s && comp(q[c+1],q[c]))
                    ++c;
                swap_vals(c,i);
            } else if (++c < s && comp(q[c],q[i])) {
                swap_vals(c,i);
            } else {
                return;
            }
        }
    }
    
public:
    // n is the max size and must be given
    p_queue(int n) : s(0), q(n), i_to_key(n) {}
    
    int size() {
        return s;
    }
    
    // returns the min of the array
    pair<Key,Value> getMin() {
        return {i_to_key[0],q[0]};
    }
    
    pair<Key,Value> removeMin() {
        auto ans = getMin();
        --s;
        if (s) {
            swap_vals(s,0);
            bubble_down(0);
        }
        if (neverAgain) {
            key_to_i[ans.first] = -1;
        } else {
            key_to_i.erase(ans.first);
        }
        return ans;
    }
    
    void updateKey(const Key& key, const Value& val, bool onlyDecrease = false) {
        if (key_to_i.count(key) == 0) {
            q[s] = val;
#if DEBUG
            cout << "inserted key " << to_str(key) << " with val " << to_str(val) << endl;
#endif
            set_i_key(s,key);
            bubble_up(s);
            ++s;
        } else {
            int i = key_to_i[key];
            if (i >= 0 && i < s) {
                Value& old = q[i];
                if (comp(val,old)) {
                    old = val;
                    bubble_up(i);
#if DEBUG
                    cout << "decreased key = " << to_str(key) << " to val " << to_str(val) << " " << i << endl;
#endif
                } else if (!onlyDecrease && comp(old,val)) {
                    old = val;
                    bubble_down(i);
#if DEBUG
                    cout << "increased key = " << to_str(key) << " to val " << to_str(val) << endl;
#endif
                }
            }
        }
    }
    
    void print() {
        cout << endl << "i:  ";
        for (int i = 0; i < s; ++i)
            printf(" %10d",i);
        cout << endl << "key:";
        for (int i = 0; i < s; ++i)
            printf(" %10s",to_str(i_to_key[i]).c_str());
        cout << endl << "val:";
        for (int i = 0; i < s; ++i)
            printf(" %10s",to_str(q[i]).c_str());
        cout << endl << endl;
    }
};
