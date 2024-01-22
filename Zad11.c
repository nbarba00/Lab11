#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH (50)
#define MAX_LINE (1024)
#define TABLE_SIZE (11)
#define FILE_NOT_OPEN (-1)

typedef struct Town {
    char name[MAX_LENGTH];
    int population;
    struct Town* next;
    struct Town* left;
    struct Town* right;
} Town;

typedef struct Country {
    char name[MAX_LENGTH];
    struct Country* next;
    Town* townTreeRoot;
} Country;

typedef struct HashTable {
    Country* table[TABLE_SIZE];
} HashTable;

unsigned int hashFunction(const char* key) {
    unsigned int sum = 0;
    for (int i = 0; i < 5 && key[i] != '\0'; ++i) {
        sum += key[i];
    }
    return sum % TABLE_SIZE;
}

Country* insertNewCountryList(Country* head, Country* newCountry) {
    if (head == NULL || strcmp(head->name, newCountry->name) > 0) {
        newCountry->next = head;
        return newCountry;
    }

    Country* current = head;
    while (current->next != NULL && strcmp(current->next->name, newCountry->name) < 0) {
        current = current->next;
    }

    newCountry->next = current->next;
    current->next = newCountry;

    return head;
}

Country* createNewCountry(const char* countryName) {
    Country* country = (Country*)malloc(sizeof(Country));
    if (!country) {
        printf("Can't allocate memory!\n");
        exit(EXIT_FAILURE);
    }

    strcpy(country->name, countryName);
    country->next = NULL;
    country->townTreeRoot = NULL;

    return country;
}

Town* createNewTown(const char* townName, int townPopulation) {
    Town* town = (Town*)malloc(sizeof(Town));
    if (!town) {
        printf("Can't allocate memory!\n");
        exit(EXIT_FAILURE);
    }

    strcpy(town->name, townName);
    town->population = townPopulation;
    town->next = NULL;
    town->left = NULL;
    town->right = NULL;

    return town;
}

Town* insertNewTownTree(Town* root, Town* newTown) {
    if (root == NULL)
        return newTown;

    if (newTown->population > root->population)
        root->left = insertNewTownTree(root->left, newTown);
    else if (newTown->population <= root->population)
        root->right = insertNewTownTree(root->right, newTown);

    return root;
}

void readAndFillHashTable(HashTable* hashTable) {
    FILE* filePointer = NULL;
    char countryName[MAX_LINE] = { 0 };
    char countryFile[MAX_LINE] = { 0 };

    filePointer = fopen("drzave.txt", "r");
    if (!filePointer) {
        printf("Can't open file!\n");
        exit(FILE_NOT_OPEN);
    }

    while (fscanf(filePointer, " %s %s", countryName, countryFile) == 2) {
        Country* newCountry = createNewCountry(countryName);

        // Read city file, create town structures, and populate country structures
        FILE* cityFilePointer = fopen(countryFile, "r");
        if (!cityFilePointer) {
            printf("\033[31mCan't open city file!\033[0m\n");
            exit(FILE_NOT_OPEN);
        }

        while (fscanf(cityFilePointer, " %s %d", countryName, &townPopulation) == 2) {
            Town* newTown = createNewTown(countryName, townPopulation);
            newCountry->townTreeRoot = insertNewTownTree(newCountry->townTreeRoot, newTown);
        }

        fclose(cityFilePointer);

        unsigned int hashKey = hashFunction(newCountry->name);
        hashTable->table[hashKey] = insertNewCountryList(hashTable->table[hashKey], newCountry);
    }

    fclose(filePointer);
}

void printHashTable(const HashTable* hashTable) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Country* currentCountry = hashTable->table[i];
        while (currentCountry != NULL) {
            printf("\n%s", currentCountry->name);
            Town* townTreeRoot = currentCountry->townTreeRoot;
            printTownTree(townTreeRoot);
            currentCountry = currentCountry->next;
        }
    }
}

int main() {
    HashTable hashTable;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        hashTable.table[i] = NULL;
    }

    readAndFillHashTable(&hashTable);
    printHashTable(&hashTable);

    return EXIT_SUCCESS;
}
