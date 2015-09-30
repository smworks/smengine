#ifndef HASHMAP_H_
#define HASHMAP_H_

#include "Multiplatform/Ghost.h"

//template<class T>
//struct HashConfiguration{
//	enum {              
//		bucket_size = 4,  // 0 < bucket_size
//		min_buckets = 8}; // min_buckets = 2 ^^ N, 0 < N
//
//	size_t operator()(const T& key_value) const {
//		for (UINT32 i = 0; i < k.length; i++) {
//			h = 5 * h + k.buffer[i];
//		}
//		return size_t(h);
//	}
//
//	bool operator()(const T& left, const T& right) const {
//		return memcmp(left, right, sizeof(T));
//		//return (int)left < (int)right;
//	}
//};

//struct ByteBufferHash {
//	ByteBufferHash(SIZE sizeOfBuffer) : bufferSize_(sizeOfBuffer) {}
//	size_t operator()(const INT8* buffer) const {
//		size_t h = 0;
//		for (UINT32 i = 0; i < bufferSize_; i++) {
//			h = 5 * h + buffer[i];
//		}
//		return size_t(h);
//	}
//	SIZE bufferSize_;
//};

// Hash node class template
//template <typename K, typename V>
//class HashNode {
//public:
//    HashNode(const K &key, const V &value) : key(key), value(value), next(NULL) {}
// 
//    K getKey() const {
//        return key;
//    }
// 
//    V getValue() const {
//        return value;
//    }
// 
//    void setValue(V value) {
//        HashNode::value = value;
//    }
// 
//    HashNode *getNext() const {
//        return next;
//    }
// 
//    void setNext(HashNode *next) {
//        HashNode::next = next;
//    }
//private:
//    // key-value pair
//    K key;
//    V value;
//    // next bucket with the same key
//    HashNode *next;
//};
//
//static const int TABLE_SIZE = 1024 * 1024;
//
//// Default hash function class
//template <typename K>
//struct KeyHash {
//    UINT64 operator()(const K& key) const {
//        return reinterpret_cast<UINT64>(key) % TABLE_SIZE;
//    }
//};
//
//struct IntHash {
//    UINT64 operator()(const int& k) const {
//        return k % TABLE_SIZE;
//    }
//};
//
//struct StringHash {
//	UINT64 operator()(const string& key) const {
//		const char* str = key.c_str();
//		UINT64 h = 0;
//		for ( ; *str; ++str) {
//			h = 5 * h + *str;
//		}
//		return UINT64(h) % TABLE_SIZE;
//	}
//};
//
//struct ByteBuffer {
//	ByteBuffer(INT8* b, SIZE l) : buffer(b), length(l) {}
//	INT8* buffer;
//	SIZE length;
//	bool ByteBuffer::operator==(const ByteBuffer &other) const {
//		return memcmp(&buffer, &other.buffer, length) == 0;
//	}
//	bool ByteBuffer::operator!=(const ByteBuffer &other) const {
//		return memcmp(&buffer, &other.buffer, length) > 0;
//	}
//};
//
//struct ByteBufferHash {
//    UINT64 operator()(const ByteBuffer& k) const {
//		UINT64 h = 0;
//		for (UINT32 i = 0; i < k.length; i++) {
//			h = 5 * h + k.buffer[i];
//		}
//		return UINT64(h) % TABLE_SIZE;
//    }
//};

//// Hash map class template
//template <typename K, typename V, typename F = KeyHash<K> >
//class HashMap {
//public:
//    HashMap() : collisions_(0), longestCollision_(0) {
//		LOGD("Size of hashmap: %f MB.", (sizeof(HashNode<K, V>) + sizeof(K) + sizeof(V)) * TABLE_SIZE / 1024.0f / 1024.0f);
//        // construct zero initialized hash table of size
//        table = NEW HashNode<K, V> *[TABLE_SIZE]();
//    }
//
//    ~HashMap() {
//        // destroy all buckets one by one
//        for (int i = 0; i < TABLE_SIZE; ++i) {
//            HashNode<K, V> *entry = table[i];
//            while (entry != NULL) {
//                HashNode<K, V> *prev = entry;
//                entry = entry->getNext();
//                delete prev;
//            }
//            table[i] = NULL;
//        }
//        // destroy the hash table
//        delete [] table;
//    }
//
//    bool get(const K &key, V &value) {
//        UINT64 hashValue = hashFunc(key);
//        HashNode<K, V> *entry = table[hashValue];
//
//        while (entry != NULL) {
//			LOGD("ONE");
//            if (entry->getKey() == key) {
//                value = entry->getValue();
//				LOGD("TWO");
//                return true;
//            }
//            entry = entry->getNext();
//        }
//        return false;
//    }
//
//    void put(const K &key, const V &value) {
//        UINT64 hashValue = hashFunc(key);
//        HashNode<K, V> *prev = NULL;
//        HashNode<K, V> *entry = table[hashValue];
//		int c = 0;
//        while (entry != NULL && entry->getKey() != key) {
//			collisions_++;
//			c++;
//            prev = entry;
//            entry = entry->getNext();
//        }
//		if (c > longestCollision_) {
//			longestCollision_ = c;
//		}
//
//        if (entry == NULL) {
//            entry = NEW HashNode<K, V>(key, value);
//            if (prev == NULL) {
//                // insert as first bucket
//                table[hashValue] = entry;
//            } else {
//                prev->setNext(entry);
//            }
//        } else {
//            // just update the value
//            entry->setValue(value);
//        }
//    }
//
//    void remove(const K &key) {
//        UINT64 hashValue = hashFunc(key);
//        HashNode<K, V> *prev = NULL;
//        HashNode<K, V> *entry = table[hashValue];
//
//        while (entry != NULL && entry->getKey() != key) {
//            prev = entry;
//            entry = entry->getNext();
//        }
//
//        if (entry == NULL) {
//            // key not found
//            return;
//        }
//        else {
//            if (prev == NULL) {
//                // remove first bucket of the list
//                table[hashValue] = entry->getNext();
//            } else {
//                prev->setNext(entry->getNext());
//            }
//            delete entry;
//        }
//    }
//
//	int size() {
//		int s = 0;
//		for (int i = 0; i < TABLE_SIZE; ++i) {
//			HashNode<K, V> *entry = table[i];
//			while (entry != NULL) {
//				s++;
//				entry = entry->getNext();
//			}
//		}
//		return s;
//	}
//
//private:
//    // hash table
//    HashNode<K, V> **table;
//    F hashFunc;
//public:
//	UINT32 collisions_;
//	UINT32 longestCollision_;
//};

// Hash map class template
//class ByteBufferHashMap {
//public:
//    ByteBufferHashMap(SIZE length, SIZE size) : size_(size), collisions_(0), longestCollision_(0) {
//		LOGD("Size of hashmap: %f MB.", (sizeof(HashNode<INT8*, int>) + sizeof(INT8*) + sizeof(int) + size) * TABLE_SIZE / 1024.0f / 1024.0f);
//        // construct zero initialized hash table of size
//        table = NEW HashNode<INT8*, int> *[TABLE_SIZE]();
//    }
//
//    ~ByteBufferHashMap() {
//        // destroy all buckets one by one
//        for (int i = 0; i < TABLE_SIZE; ++i) {
//            HashNode<ByteBuffer, int> *entry = table[i];
//            while (entry != NULL) {
//                HashNode<ByteBuffer, int> *prev = entry;
//                entry = entry->getNext();
//                delete prev;
//            }
//            table[i] = NULL;
//        }
//        // destroy the hash table
//        delete [] table;
//    }
//
//	UINT64 hash(INT8* buffer) {
//		UINT64 h = 0;
//		for (UINT32 i = 0; i < size_; i++) {
//			h = 5 * h + buffer[i];
//		}
//		return UINT64(h) % TABLE_SIZE;
//	}
//
//    bool get(INT8* key, int &value) {
//        UINT64 hashValue = hash(key);
//        HashNode<INT8*, int> *entry = table[hashValue];
//		INT8* memkey = entry->getKey();
//
//        while (entry != NULL) {
//            if (memcmp(&memkey, &key, size_) == 0) {
//                value = entry->getValue();
//				LOGD("TWO");
//                return true;
//            }
//            entry = entry->getNext();
//        }
//        return false;
//    }
//
//    void put(INT8* key, const int &value) {
//        UINT64 hashValue = hash(key);
//        HashNode<INT8*, int> *prev = NULL;
//        HashNode<INT8*, int> *entry = table[hashValue];
//		INT8* memkey = entry->getKey();
//		int c = 0;
//        while (entry != NULL && memcmp(&memkey, &key, size_) > 0) {
//			collisions_++;
//			c++;
//            prev = entry;
//            entry = entry->getNext();
//        }
//		if (c > longestCollision_) {
//			longestCollision_ = c;
//		}
//
//        if (entry == NULL) {
//            entry = NEW HashNode<INT8*, V>(key, value);
//            if (prev == NULL) {
//                // insert as first bucket
//                table[hashValue] = entry;
//            } else {
//                prev->setNext(entry);
//            }
//        } else {
//            // just update the value
//            entry->setValue(value);
//        }
//    }
//
//    void remove(const K &key) {
//        UINT64 hashValue = hashFunc(key);
//        HashNode<K, V> *prev = NULL;
//        HashNode<K, V> *entry = table[hashValue];
//
//        while (entry != NULL && entry->getKey() != key) {
//            prev = entry;
//            entry = entry->getNext();
//        }
//
//        if (entry == NULL) {
//            // key not found
//            return;
//        }
//        else {
//            if (prev == NULL) {
//                // remove first bucket of the list
//                table[hashValue] = entry->getNext();
//            } else {
//                prev->setNext(entry->getNext());
//            }
//            delete entry;
//        }
//    }
//
//	int size() {
//		int s = 0;
//		for (int i = 0; i < TABLE_SIZE; ++i) {
//			HashNode<K, V> *entry = table[i];
//			while (entry != NULL) {
//				s++;
//				entry = entry->getNext();
//			}
//		}
//		return s;
//	}
//
//private:
//    // hash table
//    HashNode<INT8*, int>** table;
//	SIZE size_;
//public:
//	UINT32 collisions_;
//	UINT32 longestCollision_;
//};

#endif