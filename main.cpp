#include <iostream>
#include <cstring>

#include "hashtable.h"
#include "names.h"

struct Student {
    static const size_t NAMESIZE = 16;

    int id;
    // Must be null filled not just terminated, or else non-visual diffs will be present.
    char firstName[NAMESIZE], lastName[NAMESIZE];
    float gpa;

    Student(bool random) {
        if (random) {
            id = 1000 + rand() % 9000;
            gpa = 2.0f + 3.0f * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
            memset(firstName,0,NAMESIZE);
            memset(lastName,0,NAMESIZE);

            strcpy(firstName,randomFirstName());
            strcpy(lastName,randomLastName());
        }
        else {
            id = 0;
            *firstName = '\0';
            *lastName = '\0';
            gpa = 0.0f;
        }
    }

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
    printf("%i %*s %*s %.2f /-/ ",
        stu.id,
        Student::NAMESIZE, stu.firstName,
        Student::NAMESIZE, stu.lastName,
        stu.gpa);
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
    srand(time(NULL));

    HashTable<Student> ht = {};

    for (int i=0;i<25;i++) {
        ht.add(new Student(true));
    }

    printBins(ht);

}
