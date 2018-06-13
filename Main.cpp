//Program Created by Jermaine Lara
// 6/12/18
// Program adds, deletes and prints names of students, their ID's and GPA's, stores data in a hash table

#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>//Used only for reading names from files, not for hash table
#include <cstdlib>
#include <fstream>
using namespace std;

struct Student {
	char firstName[10];
	char lastName[10];
	int studentId;
	float gpa;
};

struct HashTableEntry {
	Student* student;
	int hashCode;
	HashTableEntry* next;
};

struct HashTable {
	HashTableEntry** slots;
	int slotCount;
};

void addStudent(HashTable* hashTable, Student* student);
void printStudents(HashTable* hashTable);
void deleteStudent(HashTable* hashTable, int studentId);
HashTable* createHashTable(int slotCount);
int getSlotIndex(HashTable* hashTable, int hashCode);
int computeHashCode(Student* student);
void reHashTable(HashTable* hashTable);
void readListFromFile(const char* fileName, vector<string> &names);

int main () {
	//Init random seed
	srand(time(NULL));
	HashTable* hashTable = createHashTable(100);
	char command[81];
	int studentIdGenerator = 1;
	vector<string> randomFirstNames;
	vector<string> randomLastNames;
	readListFromFile("firstNames.txt", randomFirstNames);
	readListFromFile("lastNames.txt", randomLastNames);
	//Main Loop 
	do {
		cout << "Enter Command (ADD, PRINT, DELETE, RANDOMSTUDENT or QUIT)" << endl;
		cin >> command;
		if (strcmp(command,"ADD") == 0) {
			Student* student =  new Student;
			cout << "Enter First Name" << endl;
			cin >> student->firstName;
			cout << "Enter Last Name" << endl;
			cin >> student->lastName;
			cout << "Enter student ID" << endl;
			cin >> student->studentId;
			cout << "Enter student GPA" << endl;
			cin >> student->gpa;
			addStudent(hashTable, student);
		}
		else if (strcmp(command, "PRINT") == 0) {
			printStudents(hashTable);
		}
		else if (strcmp(command, "DELETE") == 0) {
			cout << "Enter the student ID number to delete them from the list" << endl;
			int studentId;
			cin >> studentId;
			deleteStudent(hashTable, studentId);
		}
		else if (strcmp(command, "RANDOMSTUDENT") == 0) {
			Student* student = new Student;
			strcpy(student->firstName, randomFirstNames[rand() % randomFirstNames.size()].c_str());
			strcpy(student->lastName, randomLastNames[rand() % randomLastNames.size()].c_str());
			student->studentId = studentIdGenerator++;
			student->gpa = 5.0f * rand() / RAND_MAX;
			addStudent(hashTable, student);
			cout << "Added: " << student->firstName << " " << student->lastName << ", " << student->studentId 
				<< ", " << fixed << setprecision(2) << student->gpa << endl;
		}
		else if (strcmp(command, "QUIT") != 0) {
			cout << "Invalid Command" << endl;
		}
	} while(strcmp(command, "QUIT") != 0); 
}

//Adds a student into the hash table
void addStudent(HashTable* hashTable, Student* student) {
	HashTableEntry* entry = new HashTableEntry;
	entry->student = student;
	entry->hashCode = computeHashCode(student);
	//compute slot index based on the slot count and hashcode
	int slotIndex = getSlotIndex(hashTable, entry->hashCode);
	if(hashTable->slots[slotIndex] == NULL) {
		// We are the first entry in this slot
		entry->next = NULL;
		hashTable->slots[slotIndex] = entry;
	}
	else {
		//Insert our entry as the new head of the slot
		entry->next = hashTable->slots[slotIndex];
		hashTable->slots[slotIndex] = entry;
	}
	//Compute the number of collisions in the current slot
	int collisionCount = 0;
	for(HashTableEntry* entry = hashTable->slots[slotIndex]; entry != NULL; entry = entry->next) {
		collisionCount++;
	}
	//Re hash if we see more than 3 collisions in the current slot
	if(collisionCount > 3) {
		reHashTable(hashTable);
	}
 }

 //Prints all students
void printStudents(HashTable* hashTable) {
	for(int i = 0; i < hashTable->slotCount; i++) {
		for(HashTableEntry* entry = hashTable->slots[i]; entry != NULL; entry = entry->next) {
			Student* student = entry->student;
			cout << student->firstName << " " << student->lastName << ", " << student->studentId 
				<< ", " << fixed << setprecision(2) << student->gpa << endl;
		}
	}
}

//Deletes a student based on the ID number
void deleteStudent(HashTable* hashTable, int studentId) {
	for(int i = 0; i < hashTable->slotCount; i++) {
		HashTableEntry* prevEntry = NULL;
		for(HashTableEntry* entry = hashTable->slots[i]; entry != NULL; entry = entry->next) {
			if(studentId == entry->student->studentId) {
				if(prevEntry == NULL) {
					//We are removing the head
					hashTable->slots[i] = NULL;
				}
				else {
					//Reconnect previous with next entry
					prevEntry->next = entry->next;
				} 
				//Free memory
				delete entry->student;
				delete entry;
				return;
			}
		}
	}
}

//Create hash table of the specified slot count
HashTable* createHashTable(int slotCount) {
	HashTable* hashTable = new HashTable;
	hashTable->slots = new HashTableEntry*[slotCount];
	hashTable->slotCount = slotCount;
	for(int i = 0; i < slotCount; i++) {
		hashTable->slots[i] = NULL;
	}
	return hashTable;
}

//rehashes a table by doubling the capacity
void reHashTable(HashTable* hashTable) {
	//Create a new table with twice the size and add all students from the current table
	HashTable* newHashTable = createHashTable(hashTable->slotCount*2);
	for(int i = 0; i < hashTable->slotCount; i++) {
		for(HashTableEntry* entry = hashTable->slots[i]; entry != NULL; entry = entry->next) {
			addStudent(newHashTable, entry->student);
		}
	}
	//Reassign the slots from the new table into the current table 
	delete hashTable->slots;
	hashTable->slots = newHashTable->slots;
	hashTable->slotCount = newHashTable->slotCount;
	//New table no longer needed, free it
	delete newHashTable;
}

//Compute the hash code based on the first and last name and student ID
int computeHashCode(Student* student) {
	int hashCode = 0;
	for(int i = 0; student->firstName[i] != 0; i++) {
		hashCode += student->firstName[i]*i;
	}
	for(int i = 0; student->lastName[i] != 0; i++) {
		hashCode += student->lastName[i]*i;
	}
	hashCode += student->studentId;
	return hashCode;
}

//Return slot index
int getSlotIndex(HashTable* hashTable, int hashCode) {
	return hashCode % hashTable->slotCount;
}


void readListFromFile(const char* fileName, vector<string> &names) {
    ifstream file(fileName);
    if (file.is_open()) {
		string name;
		while (file >> name)
		{
			names.push_back(name);
		}
		file.close();
    }
    else {
		cout << "Unable to open file " << fileName << endl; 
    }
}