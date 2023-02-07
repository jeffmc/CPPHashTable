#include <cstddef>
#include <cstring>

template <typename T>
struct Node {
	T* data; // shouldn't be nullptr.
	Node* next; // nullptr means end of list.
	Node(Node&& o) : data(o.data) { o.data = nullptr; }
	Node(T* d) : data(d), next(nullptr) { }
	~Node() {
		delete data;
	}
};

template <typename T>
class HashTable { // Each entry must be unique
	using hash_t = unsigned long int;

	size_t entryCt; // Size of the hash table ( Number of (unique) entries! )
	size_t binCount;
	struct BinElement {
		bool constructed;
		Node<T> node;

		operator bool() const { return constructed; }
		void construct(T* t) {
			new (&node) Node<T>(t);
			constructed = true;
		}
		void destruct() {
			node.~Node();
			constructed = false;
		}
	};
	BinElement* memory;

	hash_t hashfunc(T *t) {
		unsigned char *str = (unsigned char*)t;
		hash_t hash = 5381;
		for (size_t i=0;i < sizeof(T); i++) {
			hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
		}
		return hash;
	}

	BinElement* allocmem(size_t newBinCt) {
		BinElement* mem = (BinElement*) ::operator new(sizeof(BinElement) * newBinCt);
		for (size_t i = 0; i < newBinCt; i++) mem[i].constructed = false;
		return mem;
	}

	void grow() {
		const size_t oldBinCt = binCount;
		BinElement* oldmem = memory;
		
		binCount *= 1.5; // growth factor
		if (binCount < 2) binCount = 2;
		memory = allocmem(binCount);
		
	}

public:
	HashTable(size_t binct = 50) : binCount(binct), memory(allocmem(binCount)) {
		// memory = allocmem(binCount);
	}

	// Return true if data (equal hash) is present in hash table.
	bool has(T* t) {
		const hash_t thash = hashfunc(t);
		const size_t tbin = thash % binCount;
		const BinElement& bin = memory[tbin];
		if (!bin) return false;
		const Node<T>* head = &bin.node;
		while (head) {
			if (thash == hashfunc(head->data)) return true;
			head = head.next;
		}
		return false;
	}

	// Return true if the data was added, false if if a data (equal hash) is already present.
	bool add(T* t) {
		hash_t thash = hashfunc(t); 
		size_t tbin = thash % binCount;
		BinElement& be = memory[tbin];
		if (be) {
			Node<T>* head = &be.node;
			while (head->next) {
				if (hashfunc(head->data) == thash) return false;
				head = head->next;
			}
			if (hashfunc(head->data) == thash) return false;
			head->next = new Node<T>(t);
			++entryCt;
			return true;
		}
		else {
			be.construct(t);
			++entryCt;
			return true;
		}
	}
	void clear() {
		for (size_t i=0;i<binCount;i++) {
			BinElement& be = memory[i];
			if (be) {
				Node<T>* head = be.node.next;
				be.destruct();
				if (head) {
					Node<T>* next = head->next;
					while (next) {
						delete head;
						head = next;
						next = head->next;
					}
					delete head;
				}
			}
		}
		entryCt = 0;
	}
	size_t size() const {
		size_t sz = 0;
		for (size_t i=0;i<binCount;i++) {
			const BinElement& be = memory[i];
			if (be) {
				++sz;
				Node<T>* n = be.node.next;
				while (n) {
					++sz;
					n = n->next;
				}
			}
		}
		return sz;
	}

	size_t entries() const { return entryCt; }
	// Bins
	size_t bins() const { return binCount; }
	Node<T>* bin(size_t i) {
		BinElement& be = memory[i];
		if (!be) return nullptr;
		return &be.node;
	}
	const Node<T>* bin(size_t i) const { 
		BinElement& be = memory[i];
		if (!be) return nullptr;
		return &be.node;
	}
	

};