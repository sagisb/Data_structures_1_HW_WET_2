#pragma once
#include <new>

struct IntHasher {
    unsigned int operator()(int key) const {
        unsigned int k = static_cast<unsigned int>(key);
        k = ((k >> 16) ^ k) * 0x45d9f3b; k = ((k >> 16) ^ k) * 0x45d9f3b; k = (k >> 16) ^ k;
        return k;
    }
};

template <class K, class V, class H = IntHasher>
class HashTable {
private:
    struct Node { K key; V value; Node* next; };
    Node** table;
    int capacity;
    int size;
    int hash(const K& key) const { H h; return h(key) % capacity; }
    bool rehash() {
        int old_capacity = capacity;
        Node** old_table = table;
        capacity = (capacity > 0) ? capacity * 2 : 11;
        table = new (std::nothrow) Node*[capacity];
        if (table == nullptr) { capacity = old_capacity; table = old_table; return false; }
        for (int i = 0; i < capacity; ++i) table[i] = nullptr;
        int old_size = size;
        size = 0;
        for (int i = 0; i < old_capacity; ++i) {
            Node* current = old_table[i];
            while (current != nullptr) {
                insert(current->key, current->value);
                Node* to_delete = current;
                current = current->next;
                delete to_delete;
            }
        }
        delete[] old_table;
        size = old_size;
        return true;
    }
public:
    HashTable() : capacity(6007), size(0) {
        table = new (std::nothrow) Node*[capacity];
        if (table == nullptr) capacity = 0;
        else for (int i = 0; i < capacity; ++i) table[i] = nullptr;
    }
    ~HashTable() {
        for (int i = 0; i < capacity; ++i) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* to_delete = current;
                current = current->next;
                delete to_delete;
            }
        }
        delete[] table;
    }
    bool insert(const K& key, const V& value) {
        if (capacity == 0) return false;
        if (find(key) != nullptr) return false;
        if ((double)(size + 1) / capacity > 0.75) { if (!rehash()) return false; }
        int index = hash(key);
        Node* new_node = new (std::nothrow) Node();
        if (new_node == nullptr) return false;
        new_node->key = key; new_node->value = value; new_node->next = table[index];
        table[index] = new_node;
        size++;
        return true;
    }
    V* find(const K& key) const {
        if (capacity == 0 || size == 0) return nullptr;
        int index = hash(key);
        Node* current = table[index];
        while (current != nullptr) {
            if (current->key == key) return &(current->value);
            current = current->next;
        }
        return nullptr;
    }
    bool remove(const K& key) {
        if (capacity == 0 || size == 0) return false;
        int index = hash(key);
        Node* current = table[index];
        Node* prev = nullptr;
        while (current != nullptr) {
            if (current->key == key) {
                if (prev == nullptr) table[index] = current->next;
                else prev->next = current->next;
                delete current;
                size--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }
};