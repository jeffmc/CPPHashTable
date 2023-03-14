// This file contains all data structures used for this project.

#include <cstddef>
#include <cstring>

#define LOG_COLLISIONS 0

template <typename T>
struct Node {
	T* data = nullptr; // shouldn't be nullptr.
	Node* next; // nullptr means end of list.
	Node(Node&& o) : data(o.data) { o.data = nullptr; }
	Node(T* d) : data(d), next(nullptr) { }
	~Node() {
		if (data != nullptr) delete data;
	}
};

template <typename T>
class HashTable { // Each entry must be unique
	using hash_t = unsigned long int;

	size_t entryCt = 0; // Size of the hash table ( Number of (unique) entries! )
	size_t binCount = 0;
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

	BinElement* allocmem(size_t newBinCt) {
		BinElement* mem = (BinElement*) ::operator new(sizeof(BinElement) * newBinCt);
		for (size_t i = 0; i < newBinCt; i++) mem[i].constructed = false;
		return mem;
	}

	// DOESN'T INCREMENT ENTRY COUNTER Return true if the length of chain if added, -1 if if a data (equal hash) is already present.
	int intl_add(T* t) {
		hash_t thash = hashfunc(t); 
		BinElement& be = memory[thash % binCount];
		if (be) {
			int length = 0;
			Node<T>* prev = nullptr;
			Node<T>* head = &be.node;
			while (head) {
				++length;
				if (hashfunc(head->data) == thash) {
					#if LOG_COLLISIONS
					printf("COLLISION FOUND HERE[%lu]:\n", thash);
					printf("	 EXIST:");
					logelement(head->data);
					printf("	FAILED:");
					logelement(t);
					#endif
					return -1;
				}
				prev = head;
				head = head->next;
			}
			prev->next = new Node<T>(t);
			return length;
		}
		else {
			be.construct(t);
			return 1;
		}
	}

	void grow(float factor = 2.3) {
		const size_t oldBinCt = binCount;
		BinElement* oldmem = memory;
		
		binCount *= factor; // growth factor
		if (binCount < 2) binCount = 2;
		memory = allocmem(binCount);
		
		for (size_t i=0;i<oldBinCt;++i) {
			BinElement &old = oldmem[i];
			if (old) {
				intl_add(old.node.data);
				old.node.data = nullptr;
				Node<T>* head = old.node.next;
				old.destruct();
				while (head) {
					intl_add(head->data);
					head->data = nullptr;
					head = head->next;
				}
			}
		}
		delete oldmem;
		return;
	}

public:
	static void logelement(const T *t) {
		printf("[%p] Hashtable Element\n");
	}
	static hash_t hashfunc(T *t) {
		unsigned char *str = (unsigned char*)t;
		hash_t hash = 5381;
		for (size_t i=0;i < sizeof(T); i++) {
			hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
		}
		return hash;
	}
	HashTable(size_t binct = 100) : binCount(binct), memory(allocmem(binCount)) { }

	// Return true if data (equal hash) is present in hash table.
	bool has(T* t) {
		const hash_t thash = hashfunc(t);
		const BinElement& bin = memory[thash % binCount];
		if (!bin) return false;
		const Node<T>* head = &bin.node;
		while (head) {
			if (thash == hashfunc(head->data)) return true;
			head = head.next;
		}
		return false;
	}

	// Return true if the data was added, false if if a data (equal hash) is already present, grow if load factor too high.
	bool add(T* t) {
		int added = intl_add(t);
		if (added >= 0) ++entryCt;
		if (added > 3) grow();
		while (entryCt > ((float)binCount * 0.5)) {  // TODO: Check for linked-entries of length>3
			grow();
		}
		return added >= 0;
	}

	// Return true if an element with equal hash was removed!
	bool remove(T* t) {
		const hash_t thash = hashfunc(t);
		const size_t binid = thash % binCount;
		BinElement& be = memory[binid];
		Node<T>& head = be.node;
		if (be) {
			if (hashfunc(head.data) == thash) {
				Node<T>* next = head.next;
				if (next != nullptr) {
					head.data = next->data;
					head.next = next->next;

					next->data = nullptr;
					next->next = nullptr;
					delete next;
				}
				else {
					be.destruct();
				}
				return true;
			}
			else {
				Node<T>** prev = &head.next;
				Node<T>* curr = head.next;
				while (curr) {
					if (hashfunc(curr->data) == thash) {
						*prev = curr->next;
						delete curr;
						return true;
					}
					else {
						prev = &curr->next;
						curr = curr->next;
					}
				}
				return false;
			}
		} else {
			return false;
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
		delete memory;
		entryCt = 0;
		binCount = 100; // Default 100 bins.
		memory = allocmem(binCount);
	}
	// Return number of elements stored in hash table.
	size_t size() const 
	{
		return entryCt;
	}
	size_t calcsize() const 
	{
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
	// Return bytes occupied
	size_t memsize() const {
		return binCount * sizeof(BinElement);
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