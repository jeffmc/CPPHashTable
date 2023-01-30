#include <cstddef>
#include <cstring>

template <typename T>
struct Node {
	T* data; // shouldn't be nullptr.
	Node* next; // nullptr means end of list.
	Node(T* d) : data(d), next(nullptr) { }
	~Node() { delete data; }
};

template <typename T>
class HashTable {

	using hash_t = unsigned long int;

	size_t binCount;
	Node<T>** binHeads; // array of binCount size
	
	hash_t hashfunc(T *t) {
		unsigned char *str = (unsigned char*)t;
		hash_t hash = 5381;
		for (size_t i=0;i < sizeof(T); i++) {
			hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
		}
		return hash;
	}

public:
	HashTable() : binCount(100) {
		binHeads = new Node<T>*[binCount];
	}
	void add(T* t) {
		hash_t hash = hashfunc(t); 
		size_t bin = hash % binCount;
		Node<T>*& head = binHeads[bin];
		if (!head) {
			head = new Node<T>(t);
		}
		else {
			Node<T>* n = head;
			while (n->next) n = n->next;
			n->next = new Node<T>(t);
		}
	}

	// Bins
	size_t bins() const { return binCount; }
	const Node<T>* bin(size_t i) const { return binHeads[i]; }
	Node<T>* bin(size_t i) { return binHeads[i]; }

};