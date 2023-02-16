// Jeff McMillan 2-16-2023 C++ Hash Table Project
// This project contains a list of students within a hash table. The user is able to add,
// remove, and print out the list of students.

#include <iostream>
#include <cstring>

#include "hashtable.h"
#include "names.h"

struct Student {
    static const size_t NAMESIZE = 26;

    int id;
    // Must be null filled not just terminated, or else non-visual diffs will be present.
    char firstName[NAMESIZE], lastName[NAMESIZE];
    float gpa;

    Student(bool random = false) 
    {
        if (random) {
            id = 100000 + rand() % 900000;
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

    Student(int id, const char* fn, const char* ln, float gpa) : id(id), gpa(gpa) 
    {
        strcpy(firstName, fn);
        strcpy(lastName, ln);
    }
    ~Student() { }
};

// ONLY HASH THE STUDENT ID, AS THAT IS THE ONLY UNIQUE IDENTIFIER IN THIS SET OF STUDENTS! 
template <>
HashTable<Student>::hash_t HashTable<Student>::hashfunc(Student *t) 
{
    unsigned char * const str = (unsigned char*)t;
    hash_t hash = 5381;
    for (size_t i=offsetof(Student,id);i < offsetof(Student,id) + sizeof(int); i++) {
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    }
    // const size_t fn = offsetof(Student,firstName), ln = offsetof(Student,lastName);
    // unsigned char *name = str + fn;
    // while (*name) hash = ((hash << 5) + hash) + *name++;
    // name = str + ln;
    // while (*name) hash = ((hash << 5) + hash) + *name++;

    // for (size_t i=offsetof(Student,gpa);i < offsetof(Student,gpa) + sizeof(float); i++) {
    //     hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    // }

    return hash;
}

void inlinePrintStu(const Student& stu) 
{
    printf("%6i %*s %*s %4.2f --- ",
        stu.id,
        Student::NAMESIZE, stu.firstName,
        Student::NAMESIZE, stu.lastName,
        stu.gpa);
}

template <>
void HashTable<Student>::logelement(const Student *t) 
{
    inlinePrintStu(*t);
    printf("\n");
}

void printStuHeader() 
{
    printf("%6s %*s %*s %4s --- ",
        "ID",
        Student::NAMESIZE, "FIRST",
        Student::NAMESIZE, "LAST",
        "GPA");
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

void printBins(const HashTable<Student> &ht) 
{
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

void printElements(const HashTable<Student> &ht) 
{
    const size_t len = numDigits(ht.bins());
    printf("Hashtable: %i bins, %i entries\n", ht.bins(), ht.entries());
    size_t stuCt = 0;
    size_t binlen = numDigits(ht.bins());
    if (binlen < 3) binlen = 3;
    printf("  %*s  ", binlen, "BIN");
    printStuHeader();
    printf("\n");
    for (size_t i=0;i<ht.bins();i++) {
        const Node<Student>* head = ht.bin(i);
        while (head) {
            ++stuCt;
            printf("  %*u  ", binlen, i);
            inlinePrintStu(*head->data);
            printf("\n");
            head = head->next;
        }
    }
    printf("---END OF ELEMENTS---\n");
}

void printStats(const HashTable<Student> &ht) 
{
    printf("%i bins, %i entries(real: %i) (%i bytes)\n", 
        ht.bins(), ht.entries(), ht.size(), ht.memsize());
}

// Return number of collisions (temporarys that didn't join table)
size_t addRandoms(HashTable<Student> &ht, const size_t ct) 
{
    size_t collisions = 0;
    for (size_t i=0;i<ct;++i) {
        if (!ht.add(new Student(true))) 
        {
            ++collisions;
        }
    }
    return collisions;
}

///// COMMANDS ////////

// Read in a line from the console.
void consolein(char* cstr, std::streamsize n) {
	std::cin.getline(cstr, n);
	if (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore();
	}
	#ifdef CONSOLEINDBG
	printf("\"%s\"\n",cstr);
	#endif
}


void randomStudents(HashTable<Student> &ht) {
    printf("How many to random students should be added: ");
	char conversions[32];
	consolein(conversions,32);
	const size_t randCt = strtol(conversions,nullptr,10);

    printf("Adding %u students...\n", randCt);
    const size_t collisions  = addRandoms(ht,randCt);
    printf("%u duplicate collisions!\n", collisions);
}


// Create a student using fields provided by user through the console.
Student* constructStudent() {
	Student* newstu = new Student();
	
	char conversions[32];

	printf("Student ID: ");
	consolein(conversions,32);
	newstu->id = strtol(conversions,nullptr,10);

	printf("First name: ");
	consolein(newstu->firstName, Student::NAMESIZE);
	
	printf("Last name: ");
	consolein(newstu->lastName, Student::NAMESIZE);

	printf("GPA: ");
	consolein(conversions,32);
	newstu->gpa = strtof(conversions,nullptr);

	return newstu;
}

// Print a single student!
void printStudent(const Student &stu) {
	printf("%7i %*s %*s %.2f\n", stu.id, 
	Student::NAMESIZE, stu.firstName, 
	Student::NAMESIZE,  stu.lastName, 
	stu.gpa);
}

int main() 
{
    srand(time(NULL)); // Init random seed using current system time
    HashTable<Student> ht{}; // Init empty hash table.
    bool running = true;
	char cmd[16];
	const char* helpstr = "Command list: ADD PRINT TBLPRINT STATS RAND DELETE CLEAR QUIT HELP";
	printf("%s\n", helpstr);
	// Command loop!
	while (running) {
		printf(":");
		consolein(cmd, 16);
		
		if (strcmp(cmd,"ADD") == 0) {
			Student* newstu = constructStudent();
			
			printf("Created Student:\n");
			printStudent(*newstu);
			bool added = ht.add(newstu);
            if (!added) printf("Not added! Student with ID already exists in table!");
		}
		else if (strcmp(cmd,"PRINT") == 0) {
			printElements(ht);
		}
		else if (strcmp(cmd,"TBLPRINT") == 0) {
			printBins(ht);
		}
		else if (strcmp(cmd,"STATS") == 0) {
			printStats(ht);
		}
        else if (strcmp(cmd,"RAND") == 0) {
            randomStudents(ht);
        }
		else if (strcmp(cmd,"DELETE") == 0) {
			char conversions[16];
			printf("ID TO DELETE: ");
			consolein(conversions,16);
			int id = strtol(conversions,nullptr,10);
			Student shell = Student(id,"","",0.0f);
            bool removed = ht.remove(&shell);
            if (removed) {
                printf("Student removed!\n");
            }
            else {
                printf("No student found!\n");
            }
		}
        else if (strcmp(cmd,"CLEAR") == 0) {
            ht.clear();
            printf("Cleared table of students!\n");
        }
		else if (strcmp(cmd,"QUIT") == 0) {
			running = false;
		}
		else if (strcmp(cmd,"HELP") == 0) {
			printf("%s\n",helpstr);
		}
        else {
            printf("\"%s\" not a known command!", cmd);
        }

		printf("\n");
	}
	
	printf("Goodbye World!\n");
	return 0;

}
