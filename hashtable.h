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
class HashTable {
	using hash_t = unsigned long int;

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

public:
	HashTable(size_t binct = 50) : binCount(binct) {
		const size_t sz = sizeof(BinElement) * binCount;
		memory = (BinElement*) ::operator new(sz);
		for (size_t i = 0; i < binCount; i++) memory[i].constructed = false;
	}
	void add(T* t) {
		hash_t hash = hashfunc(t); 
		size_t bin = hash % binCount;
		BinElement& be = memory[bin];
		if (be) {
			Node<T>* head = &be.node;
			while (head->next) head = head->next;
			head->next = new Node<T>(t);
		}
		else {
			be.construct(t);
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