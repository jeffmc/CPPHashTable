#include <iostream>
#include <cstring>

#include "hashtable.h"

struct Student {
    static const size_t NAMESIZE = 16;

    int id;
    // Must be null filled not just terminated, or else non-visual diffs will be present.
    char firstName[NAMESIZE], lastName[NAMESIZE];
    float gpa;

    Student(int id, const char* fn, const char* ln, float gpa) : id(id), gpa(gpa) {
        memset(firstName,0,NAMESIZE);
        strcpy(firstName, fn);
        
        memset(lastName,0,NAMESIZE);
        strcpy(lastName, ln);
    }
    ~Student() {
        printf("Student destructed!\n");
    }
};

template class HashTable<Student>;

void inlinePrintStu(const Student& stu) {
    printf("%i/%s/%s/%.2f ",stu.id,stu.firstName,stu.lastName,stu.gpa);
}
void printBins(const HashTable<Student> &ht) {
    printf("Printing bins:\n");
    for (size_t i=0;i<ht.bins();i++) {
        const Node<Student>* head = ht.bin(i);
        if (!head) continue;
        printf("    %u: ", i);
        while (head) {
            inlinePrintStu(*head->data);
            head = head->next;
        }
        printf("\n");
    }
}

int main() {
    HashTable<Student> ht = {};

    ht.add(new Student(395, "Jeff", "McM", 4.13f));
    ht.add(new Student(395, "Jeff", "McM", 4.13f));

    ht.add(new Student(123, "Steve", "McM", 3.26f));
    ht.add(new Student(6969, "Cole", "Mass", 5.63f));
    ht.add(new Student(333, "Reese", "G", 4.13f));
    ht.add(new Student(444, "Cole", "Thomas", 4.25f));
    ht.add(new Student(399, "Hancock", "Alan", 3.63f));
    ht.add(new Student(423, "James", "Roth", 6.43f));
    ht.add(new Student(6645, "Caden", "Emms", 99.42f));
    ht.add(new Student(12313, "Eric", "Eric", 31.42f));
    ht.add(new Student(1222, "Sponge", "Gary", 23.42f));
    ht.add(new Student(9912, "Partick", "Rats", 64.42f));

    printBins(ht);

}
