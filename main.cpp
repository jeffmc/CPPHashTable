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
    printf("Hashtable: %i bins, %i entries\n", ht.bins(), ht.entries());
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
    printf("---END OF BINS---\n");
}

void printStats(const HashTable<Student> &ht) {
    printf("%i bins, %i entries (%i bytes)\n", ht.bins(), ht.entries(), ht.bytesize());
}

void add(HashTable<Student> &ht, size_t ct) {
    for (size_t i=0;i<ct;i++) {
        ht.add(new Student(true));
    }
}

int main(int argc, char* argv[]) {
    const int genCt = argc > 1 ? atoi(argv[1]) : 100;

    srand(time(NULL));
    HashTable<Student> ht{};

    for (size_t i=0;i<10;i++) {
        add(ht,10);
        printStats(ht);
    }


    printf("Finished!\n");
    printBins(ht);
    
    printf("Clearing...\n");
    ht.clear();

    printBins(ht);

}
