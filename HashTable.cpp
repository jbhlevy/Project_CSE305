#include <iostream>
#include <vector>
#include <list>
#include <mutex>
#include <memory>
#include <thread>



template<typename T>
class Node {
    public:
        //Node(){};
        Node(T* val){
            value = val;
            parent = nullptr;
            depth = 0;
            key = val->hash();
            std::cout << "hashing when creating node " << key << std::endl; 
        }

        Node(T* val, Node<T>* par) : value(val), parent(par), key(val->hash()){
            depth = parent->depth +1;
        }

        Node(){};
        
        T* value;
        int depth;
        std::mutex mutex;
        int key;
        Node<T>* parent; //implement similarily to node in tdgit7
        
};  


template<typename T>
class BaseHashSet {
    //std::condition_variable not_full;

public:
    //std::vector<std::list<std::unique_ptr<Node<T>>>> table;
    std::vector<std::list<Node<T>*>> table;

    int max_depth;
    int count;
/*
    BaseHashSet(int max_depth) : setSize(0), table(max_depth) {
        for (int i = 0; i < max_depth; i++) {
            table[i] = std::list<std::unique_ptr<Node>>();
        }
    }
    */
   BaseHashSet(){};

    int hash(int i){
       return i;
    }

    int print(int i){
       std::cout << i;
    }
    


    //QUESTION 
    //Why do we need to initialize one Node in every bucket ?
    BaseHashSet(int max_depth) : count(0), table(max_depth) {
        for (int i = 0; i < max_depth; i++) { 
            //table[i] = std::list<std::unique_ptr<Node>>{std::unique_ptr<Node>(new Node(T{}))};
            table[i].emplace_back(new Node<T>(new T())); 
            
        }
        this->max_depth = max_depth;
        std::cout << "Finished initializing table" << std::endl;
    }

    size_t get_max_depth() const {return this->max_depth;}
    size_t get_count() const {return this->count;}

    bool contains(T* x) {
        int x_key = x->hash(); 
        std::cout << "Checking containing Website: " << x_key << std::endl; 
        int myBucket = x_key % table.size(); //hash defined in website class
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (const auto& node : table[myBucket]) {
            if (node->key == x_key) {
                return true;
            }
        }
        return false;
    }

    bool contains(std::string url){
        int url_key = std::hash<std::string>{}(url); 
        std::cout << "Checking containing url: " << url_key << std::endl; 
        int myBucket = url_key % table.size(); //hash defined in website class
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (const auto& node : table[myBucket]) {
            if (node->key == url_key) {
                return true;
            }
        }
        return false;

    }

//OBS BUG, how do we access added elements, location,add function to access?
    bool add(Node<T> *x) {
        int myBucket = x->value->hash() % table.size(); //hashtable, 
        //int myBucket = x->depth % table.size(); //our hash table will be orgainised by depth. first level forst link.
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        //while (count == max_depth) not_full.wait(lock);// lock when not full anymore

        for (const auto& node : table[myBucket]) {
            if (node->value == x->value) {
                return false; // Element already exists
            }
        }
        //table[myBucket].push_back(std::unique_ptr<Node<T>>(x));
        //Node<T>&n = new Node<T>(x);
        table[myBucket].emplace_back(x);
        std::cout << "Curent size of bucket " << myBucket << ": "<<table[myBucket].size() << std::endl;
        //table[myBucket].push_back(*x);
        //count++; do we want a bounded table?
        return true;
    }

    Node<T>* get(T* x){ //x is hash 
        int x_key = x->hash(); 
        std::cout << "Getting node Website: " << x_key << std::endl; 
        int myBucket = x_key % table.size(); //hash defined in website class
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (Node<T>* node : table[myBucket]) {
            if (node->key == x_key) { 
                return node;
            }
        }
        return nullptr;
    }

    Node<T>* get(std::string url){ //x is hash 
        int url_key = std::hash<std::string>{}(url); 
        std::cout << "Getting node url: " << url_key << std::endl; 
        int myBucket = url_key % table.size(); //hash defined in website class
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (Node<T>* node : table[myBucket]) {
            if (node->key == url_key) { 
                return node;
            }
        }
        return nullptr;
    }

    void printHashtable(){
        for (size_t i = 0; i < table.size(); ++i) {
            std::cout << "Bucket " << i << ": ";
            std::lock_guard<std::mutex> lock(table[i].front()->mutex);
            for (const auto& node : table[i]) {
                node->value->print();              
                if (node->parent != nullptr){
                    std::cout << ", Parent: ";
                    node->parent->value->print();}
                std::cout <<" " <<std::endl;// node->value.print() << " ";
            }
            std::cout << std::endl;
        }
    }


    // Other methods...

};

template<typename T>
class StripedHashSet : public BaseHashSet<T> {
private:
    std::vector<std::mutex> locks;

public:
    StripedHashSet(){};
    StripedHashSet(int max_depth) : BaseHashSet<T>(max_depth), locks(max_depth) {}

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