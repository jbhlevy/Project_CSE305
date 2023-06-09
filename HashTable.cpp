#include <iostream>
#include <vector>
#include <list>
#include <mutex>
#include <memory>
#include <thread>
#include <fstream>


template<typename T>
class Node {
    public:
        T* value;
        int depth;
        std::mutex mutex;
        int key;
        Node<T>* parent; //implement similarily to node in tdgit7

        Node(T* val){
            value = val;
            parent = nullptr;
            depth = 0;
            key = val->hash();
        }

        Node(T* val, Node<T>* par) : value(val), key(val->hash()), parent(par){
            depth = parent->depth +1;
        }
        Node(){};
};  

template<typename T>
class BaseHashSet {

public:
    std::vector<std::list<Node<T>*>> table;
    int max_depth;
    int count;

   BaseHashSet(){};

    //QUESTION 
    //Why do we need to initialize one Node in every bucket ?
    BaseHashSet(int max_depth) : table(max_depth), count(0) {
        for (int i = 0; i < max_depth; i++) { 
            table[i].emplace_back(new Node<T>(new T())); 
        }
        this->max_depth = max_depth;
    }

    size_t get_max_depth() const {return this->max_depth;}
    size_t get_count() const {return this->count;}

    bool add(Node<T> *x) {
        int myBucket = x->value->hash() % table.size(); //hashtable, 
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);

        for (const auto& node : table[myBucket]) {
            if (node->value == x->value) {
                return false; // Element already exists
            }
        }
        table[myBucket].emplace_back(x);
        //std::cout << table[myBucket].size() << std::endl;
        return true;
    }

    bool contains(T* x) {
        int x_key = x->hash(); 
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
        int myBucket = url_key % table.size(); //hash defined in website class
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (const auto& node : table[myBucket]) {
            if (node->key == url_key) {
                return true;
            }
        }
        return false;

    }

    Node<T>* get(T* x){ 
        int x_key = x->hash(); 
        int myBucket = x_key % table.size(); //hash defined in website class
        std::lock_guard<std::mutex> lock(table[myBucket].front()->mutex);
        for (Node<T>* node : table[myBucket]) {
            if (node->key == x_key) { 
                return node;
            }
        }
        return nullptr;
    }

    Node<T>* get(std::string url){
        int url_key = std::hash<std::string>{}(url); 
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
                std::cout <<" " <<std::endl;
            }
            std::cout << std::endl;
        }
    }

    void writeHashtableToFile(const std::string& filename) {
        std::ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        for (size_t i = 0; i < table.size(); ++i) {
            outputFile << "Bucket " << i << ": ";
            std::lock_guard<std::mutex> lock(table[i].front()->mutex);
            for (const auto& node : table[i]) {
                outputFile << node->value->toString();
                if (node->parent != nullptr) {
                    outputFile << ", Parent: " << node->parent->value->toString();
                }
                outputFile << std::endl;
                }
            outputFile << std::endl;
            }
       outputFile.close();
    }
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
