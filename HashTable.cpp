#include <iostream>
#include <vector>
#include <list>
#include <mutex>
#include <memory>

#include <thread>

template<typename T>
class BaseHashSet {
protected:
    struct Node {
        Node(T val) : value(val) {}
        std::mutex mutex;
        T value;
        //int key;
        //T* parent; //implement similatrily to node in td7
        //int capacity; //add as setqueeu td7
    };

    std::vector<std::list<std::unique_ptr<Node>>> table;
    //std::vector<std::unique_ptr<Node>> table;
    int setSize;

public:
/*
    BaseHashSet(int capacity) : setSize(0), table(capacity) {
        for (int i = 0; i < capacity; i++) {
            table[i] = std::list<std::unique_ptr<Node>>();
        }
    }
    */

    BaseHashSet(int capacity) : setSize(0), table(capacity) {
        for (int i = 0; i < capacity; i++) { 
            //table[i] = std::list<std::unique_ptr<Node>>{std::unique_ptr<Node>(new Node(T{}))};
            table[i].emplace_back(new Node(T{}));
            
        }
    }

    bool contains(T x) {
        int myBucket = std::hash<T>{}(x) % table.size();
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (const auto& node : table[myBucket]) {
            if (node->value == x) {
                return true;
            }
        }
        return false;
    }

    bool add(T x) {
        int myBucket = std::hash<T>{}(x) % table.size();
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (const auto& node : table[myBucket]) {
            if (node->value == x) {
                return false; // Element already exists
            }
        }
        table[myBucket].push_back(std::unique_ptr<Node>(new Node(x)));
        setSize++;
        return true;
    }

    // Other methods...

};

template<typename T>
class StripedHashSet : public BaseHashSet<T> {
private:
    std::vector<std::mutex> locks;

public:
    StripedHashSet(){};
    StripedHashSet(int capacity) : BaseHashSet<T>(capacity), locks(capacity) {}

    void acquire(T x) {
        locks[std::hash<T>{}(x) %  locks.size()].lock();
    }

    void release(T x) {
        locks[std::hash<T>{}(x) %  locks.size()].unlock();
    }

};

/*

void testHashSet(StripedHashSet<int>& set, int start, int end) {
    for (int i = start; i < end; i++) {
        set.add(i);
    }
}

int main() {
    StripedHashSet<int> set(10);

    // Create multiple threads to test concurrent access
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back(testHashSet, std::ref(set), i*20, (i+1)*20);
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Print the contents of the set
    for (int i = 0; i < 100; i++) {
        if (set.contains(i)) {
            std::cout << i << " is in the set." << std::endl;
        }
    }

    return 0;
}
*/