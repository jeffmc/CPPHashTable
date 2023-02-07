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

    Student(bool random = false) {
        if (random) {
            id = 1000 + rand() % 9000;
            gpa = 2.0f + 3.0f * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
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
        strcpy(firstName, fn);
        strcpy(lastName, ln);
    }
    ~Student() { }
};

// Only hash characters before null-termination inside the Student member.
template <>
HashTable<Student>::hash_t HashTable<Student>::hashfunc(Student *t) {
    unsigned char * const str = (unsigned char*)t;
    hash_t hash = 5381;
    for (size_t i=offsetof(Student,id);i < offsetof(Student,id) + sizeof(int); i++) {
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    }
    const size_t fn = offsetof(Student,firstName), ln = offsetof(Student,lastName);
    unsigned char *name = str + fn;
    while (*name) hash = ((hash << 5) + hash) + *name++;
    name = str + ln;
    while (*name) hash = ((hash << 5) + hash) + *name++;

    for (size_t i=offsetof(Student,gpa);i < offsetof(Student,gpa) + sizeof(float); i++) {
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    }

    return hash;
}

void printStuHeader() {
    printf("%4s %*s %*s %4s --- ",
        "ID",
        Student::NAMESIZE, "FIRST",
        Student::NAMESIZE, "LAST",
        "GPA");
}

void inlinePrintStu(const Student& stu) {
    printf("%4i %*s %*s %4.2f --- ",
        stu.id,
        Student::NAMESIZE, stu.firstName,
        Student::NAMESIZE, stu.lastName,
        stu.gpa);
}

template <class T>
int numDigits(T number)
{
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

void printBins(const HashTable<Student> &ht) {
    const size_t len = numDigits(ht.bins());
    printf("PRINTING HASHTABLE BINS:\n");
    printf("    %*c  ", len, ' ');
    for (int i=0;i<3;i++) printStuHeader();
    printf("\n");
    for (size_t i=0;i<ht.bins();i++) {
        const Node<Student>* head = ht.bin(i);
        if (!head) continue;
        printf("    %*u: ", len, i);
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

    ht.add(new Student());
    ht.add(new Student());
    for (int i=0;i<35;i++) {
        ht.add(new Student(true));
    }

    printBins(ht);
    printf("Size: %lu\n", ht.size());

    printf("Clearing...\n");
    ht.clear();

    printf("Size: %lu\n", ht.size());

}
