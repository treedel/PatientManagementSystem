#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Patient
typedef struct Patient {
    char name[10];
    int age;
    char gender;
    char condition[10];
    int priority;
} Patient;

Patient* createPatientEntry() {
    Patient* entry = (Patient*) malloc(sizeof(Patient));
    printf("----------------------------------\n");
    printf("Enter patient name: "); scanf("%s", entry->name);
    printf("Enter patient age: "); scanf("%d", &(entry->age));
    printf("Enter patient gender (M/F): "); scanf("%c", &(entry->gender));
    printf("Enter patient condition: "); scanf("%s", entry->condition);
    printf("Enter patient priority: "); scanf("%d", &(entry->priority));
    printf("----------------------------------\n");
    return entry;
}

void displayPatientEntry(Patient* entry) {
    printf("------------ Patient -------------\n");
    printf("Name: %s\n", entry->name);
    printf("Age: %d\n", entry->age);
    printf("Gender: %c\n", entry->gender);
    printf("Condition: %s\n", entry->condition);
    printf("Patient priority: %d\n", entry->priority);
    printf("----------------------------------\n");
}

// Patient queue for same priority nodes
typedef struct ListNodeInt {
    int data;
    struct ListNodeInt* next;
} ListNodeInt;

ListNodeInt* createListNode(int data) {
    ListNodeInt* node = (ListNodeInt*) malloc(sizeof(ListNodeInt));
    node->data = data;
    node->next = NULL;
    return node;
}

void enqueue(ListNodeInt** rear, int key) {
    ListNodeInt* newNode = createListNode(key);
    if (*rear == NULL) {
        newNode->next = newNode;
        *rear = newNode;
        return;
    }
    newNode->next = (*rear)->next;
    (*rear)->next = newNode;
    *rear = newNode;
}

int peek(ListNodeInt* rear) {
    if (rear == NULL) return -1;
    return rear->next->data;
}

int dequeue(ListNodeInt** rear) {
    int key = peek(*rear);
    if (key == -1) return -1;
    
    ListNodeInt* temp = (*rear)->next;
    if ((*rear)->next == *rear) {
        free(temp);
        *rear = NULL;
    }
    else {
        (*rear)->next = temp->next;
        free(temp);
    }
    return key;
}

// Priority implementation using BST
typedef struct TreeNode {
    int priority;
    ListNodeInt* queueRear;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* createTreeNode(int priority, ListNodeInt* queueRear) {
    TreeNode* newNode = (TreeNode*) malloc(sizeof(TreeNode));
    newNode->priority = priority;
    newNode->queueRear = queueRear;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

TreeNode* treeInsertHelper(TreeNode* root, int priority, ListNodeInt* queueRear) {
    if (root == NULL) {
        TreeNode* newTreeNode = createTreeNode(priority, queueRear);
        return newTreeNode;
    }
    
    if (root->priority == priority)
        return root;
    
    else if (root->priority < priority)
        root->right = treeInsertHelper(root->right, priority, queueRear);
        
    else if (root->priority > priority)
        root->left = treeInsertHelper(root->left, priority, queueRear);
    
    return root;
}

void treeInsert(TreeNode** root, int priority, ListNodeInt* queueRear) {
    *root = treeInsertHelper(*root, priority, queueRear);
}

TreeNode* treeFindNext(TreeNode* root) {
    while (root && root->left != NULL)
        root = root->left;
    return root;
}

TreeNode* getSuccessor(TreeNode* curr) {
    curr = curr->right;
    while (curr != NULL && curr->left != NULL)
        curr = curr->left;
    return curr;
}

TreeNode* treeRemoveNextHelper(TreeNode* root, int priority) {
    if (root == NULL)
        return root;

    if (root->priority > priority)
        root->left = treeRemoveNextHelper(root->left, priority);
    else if (root->priority < priority)
        root->right = treeRemoveNextHelper(root->right, priority);
    else {
        if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        }

        if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        TreeNode* succ = getSuccessor(root);
        root->priority = succ->priority;
        root->right = treeRemoveNextHelper(root->right, succ->priority);
    }
    return root;
}

void treeRemoveNext(TreeNode** root) {
    TreeNode* next = treeFindNext(*root);
    if (next != NULL) {
        *root = treeRemoveNextHelper(*root, next->priority);
    }
}

void treeFindNodeHelper(TreeNode* root, int priority, TreeNode** result) {
    if (root == NULL || *result != NULL) return;
    treeFindNodeHelper(root->left, priority, result);
    if (*result == NULL && root->priority == priority) *result = root;
    treeFindNodeHelper(root->right, priority, result);
}

TreeNode* treeFindNode(TreeNode* root, int priority) {
    TreeNode* result = NULL;
    treeFindNodeHelper(root, priority, &result);
    return result;
}

void inOrderDisplayPriorities(TreeNode* root) {
    if (root == NULL) return;
    inOrderDisplayPriorities(root->left);
    printf("%d ", root->priority);
    inOrderDisplayPriorities(root->right);
}

// HashTable patient lookup
typedef struct ListNodeHash {
    int key;
    Patient* data;
    struct ListNodeHash* next;
} ListNodeHash;

typedef struct HashTable {
    ListNodeHash** lookupTable;
    int size;
} HashTable;

HashTable* createHashTable(int size) {
    HashTable* hashtable = (HashTable*) malloc(sizeof(HashTable));
    hashtable->size = size;
    
    hashtable->lookupTable = (ListNodeHash**) malloc(sizeof(ListNodeHash*)*size);
    for (int i = 0; i < size; i++) {
        (hashtable->lookupTable)[i] = NULL;
    }
    return hashtable;
}

ListNodeHash* createListNodeHash(Patient* data, int key) {
    ListNodeHash* node = (ListNodeHash*) malloc(sizeof(ListNodeHash));
    node->key = key;
    node->data = data;
    node->next = NULL;
    return node;
}

int hashCode(Patient* entry) {
    int key = 0;
    for (int i = 0; i < strlen(entry->name); i++) {
        key += (entry->name)[i];
    }
    return key;
}

int hashCodeName(char* name) {
    int key = 0;
    for (int i = 0; i < strlen(name); i++) {
        key += name[i];
    }
    return key;
}

int hashInsert(HashTable* hashtable, Patient* entry) {
    int key = hashCode(entry);
    int idx = key % (hashtable->size);
    
    ListNodeHash* newNode = createListNodeHash(entry, key);
    ListNodeHash* ptr = (hashtable->lookupTable)[idx];
    if (ptr == NULL) {
        (hashtable->lookupTable)[idx] = newNode;
        return key;
    }
    
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    ptr->next = newNode;
    return key;
}

Patient* hashFind(HashTable* hashtable, int key) {
    int idx = key % (hashtable->size);
    ListNodeHash* ptr = (hashtable->lookupTable)[idx];
    
    while (ptr != NULL) {
        if (ptr->key == key) {
            return ptr->data;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void hashRemove(HashTable* hashtable, int key) {
    int idx = key % (hashtable->size);
    ListNodeHash* ptr = (hashtable->lookupTable)[idx];
    ListNodeHash* prev = NULL;
    
    while (ptr != NULL) {
        if (ptr->key == key) {
            if (prev == NULL) hashtable->lookupTable[idx] = ptr->next; 
            else prev->next = ptr->next;
            free(ptr);
            return;
        }
        prev = ptr;
        ptr = ptr->next;
    }
}

// Final datastructure
typedef struct Database {
    TreeNode* root;
    HashTable* hashtable;
} Database;

Database* createEmptyDatabase(int hashTableSize) {
    Database* database = (Database*) malloc(sizeof(Database));
    database->root = NULL;
    database->hashtable = createHashTable(hashTableSize);
    return database;
}

void insertEntry(Database* database, Patient* entry) {
    int key = hashInsert(database->hashtable, entry);
    TreeNode* node = treeFindNode(database->root, entry->priority);
    ListNodeInt* queueRear = (node == NULL) ? NULL : node->queueRear;
    enqueue(&queueRear, key);
    if (node != NULL) {
        node->queueRear = queueRear;
    } else {
        treeInsert(&database->root, entry->priority, queueRear);
    }
}

Patient* findNextEntry(Database* database) {
    TreeNode* node = treeFindNext(database->root);
    if (node == NULL) return NULL;
    int key = peek(node->queueRear);
    return hashFind(database->hashtable, key);
}

Patient* findEntry(Database* database, char* name) {
    int key = hashCodeName(name);
    return hashFind(database->hashtable, key);
}

bool removeNextEntry(Database* database) {
    TreeNode* node = treeFindNext(database->root);
    if (node == NULL) return false;
    ListNodeInt* queueRear = node->queueRear;
    int key = dequeue(&queueRear);
    node->queueRear = queueRear;
    hashRemove(database->hashtable, key);
    if (queueRear == NULL && node->queueRear == NULL) {
        treeRemoveNext(&database->root);
    }
    return true;
}

int main() {
    // Driver code
    Database* database = createEmptyDatabase(10);

    while (true) {
        printf("----------------------------------\n");
        printf("Hospital patient management system\n");
        printf("----------------------------------\n");
        printf("\t1) Add patient (Insert)\n");
        printf("\t2) Remove patient (Remove)\n");
        printf("\t3) Fetch patient status (Find)\n");
        printf("\t4) Fetch next patient\n");
        printf("\t5) Exit\n");
        printf("----------------------------------\n");
        printf("Enter your option: ");
        int option;
        scanf("%d", &option);
        printf("----------------------------------\n");
        
        switch (option) {
            case 1: {
                Patient* entry = createPatientEntry();
                insertEntry(database, entry);
                printf("Inserting %s\n", entry->name);
                break;
            }
                
            case 2: {
                Patient* entry = findNextEntry(database);
                if (entry != NULL) {
                    printf("Removing %s\n", entry->name);
                    removeNextEntry(database);
                    break;
                }
                printf("Database is empty\n");
                break;
            }
                
            case 3: {
                char name[10];
                printf("Enter patient name: "); scanf("%s", name);
                Patient* entry = findEntry(database, name);
                if (entry == NULL) {
                    printf("Entry not found\n");
                    break;
                }
                displayPatientEntry(entry);
                break;
            }
                
            case 4: {
                Patient* entry = findNextEntry(database);
                if (entry != NULL) {
                    displayPatientEntry(entry);
                    break;
                }
                printf("Database is empty\n");
                break;
            }
                
            case 5: {
                printf("Exiting\n");
                return 0;}
                
            default: {
                printf("Invalid command\n");
                break;
            }
        }
    }
    
    return 0;
}
