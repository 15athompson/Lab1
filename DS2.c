#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_NAME 50
#define NUM_MODULES 5
#define MAX_MODULE_NAME 30
#define HASH_SIZE 1000

typedef struct {
    char name[MAX_NAME];
    char student_id[20];
    char module_names[NUM_MODULES][MAX_MODULE_NAME];
    float scores[NUM_MODULES];
    float average;
    const char* classification;
} Student;

typedef struct ClassificationNode {
    float lower_bound;
    const char* classification;
    struct ClassificationNode *left, *right;
} ClassificationNode;

typedef struct StudentNode {
    Student student;
    struct StudentNode *next;
} StudentNode;

StudentNode* student_list_head = NULL;
int student_count = 0;
ClassificationNode* classification_root = NULL;
StudentNode* hash_table[HASH_SIZE] = {NULL};

// Linked List functions
void insert_student_list(Student* student) {
    StudentNode* new_node = (StudentNode*)malloc(sizeof(StudentNode));
    new_node->student = *student;
    new_node->next = student_list_head;
    student_list_head = new_node;
    student_count++;
}

void remove_student_list(const char* student_id) {
    StudentNode* current = student_list_head;
    StudentNode* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->student.student_id, student_id) == 0) {
            if (prev == NULL) {
                student_list_head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            student_count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Binary Search Tree functions for classifications
ClassificationNode* create_classification_node(float lower_bound, const char* classification) {
    ClassificationNode* node = (ClassificationNode*)malloc(sizeof(ClassificationNode));
    node->lower_bound = lower_bound;
    node->classification = classification;
    node->left = node->right = NULL;
    return node;
}

ClassificationNode* insert_classification(ClassificationNode* root, float lower_bound, const char* classification) {
    if (root == NULL) return create_classification_node(lower_bound, classification);
    
    if (lower_bound < root->lower_bound)
        root->left = insert_classification(root->left, lower_bound, classification);
    else if (lower_bound > root->lower_bound)
        root->right = insert_classification(root->right, lower_bound, classification);
    
    return root;
}

const char* get_classification_bst(ClassificationNode* root, float average) {
    if (root == NULL) return "FAIL";
    if (average >= root->lower_bound)
        return root->classification;
    return get_classification_bst(root->left, average);
}

// Hash table functions
unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

void insert_student_hash(Student* student) {
    unsigned int index = hash(student->student_id);
    StudentNode* new_node = (StudentNode*)malloc(sizeof(StudentNode));
    new_node->student = *student;
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

Student* find_student(const char* student_id) {
    unsigned int index = hash(student_id);
    StudentNode* current = hash_table[index];
    while (current != NULL) {
        if (strcmp(current->student.student_id, student_id) == 0)
            return &(current->student);
        current = current->next;
    }
    return NULL;
}


float get_valid_score(const char* module_name) {
    float score;
    char input[100];
    while (1) {
        printf("Enter score for %s (0-100): ", module_name);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }
        
        char* endptr;
        score = strtof(input, &endptr);
        
        if (endptr == input || *endptr != '\n') {
            printf("Error: Please enter a valid number.\n");
        } else if (score < 0 || score > 100) {
            printf("Error: Score must be between 0 and 100.\n");
        } else {
            return score;
        }
    }
}

float calculate_average(float scores[], int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += scores[i];
    }
    return sum / n;
}

void input_student_info(Student* student) {
    printf("Enter student name: ");
    fgets(student->name, MAX_NAME, stdin);
    student->name[strcspn(student->name, "\n")] = 0;

    printf("Enter student ID: ");
    fgets(student->student_id, 20, stdin);
    student->student_id[strcspn(student->student_id, "\n")] = 0;

    for (int i = 0; i < NUM_MODULES; i++) {
        printf("Enter name for Module %d: ", i + 1);
        fgets(student->module_names[i], MAX_MODULE_NAME, stdin);
        student->module_names[i][strcspn(student->module_names[i], "\n")] = 0;
        student->scores[i] = get_valid_score(student->module_names[i]);
    }

    student->average = calculate_average(student->scores, NUM_MODULES);
    student->classification = get_classification_bst(classification_root, student->average);
    insert_student_list(student);
    insert_student_hash(student);
}

void display_student(const Student* student) {
    printf("\nStudent Name: %s\n", student->name);
    printf("Student ID: %s\n", student->student_id);
    printf("Module Scores:\n");
    for (int i = 0; i < NUM_MODULES; i++) {
        printf("%s: %.2f\n", student->module_names[i], student->scores[i]);
    }
    printf("Final Average: %.2f\n", student->average);
    printf("Classification: %s\n", student->classification);
    if (strcmp(student->classification, "1st (First-Class)") == 0) {
        printf("Congratulations! The student qualifies for a graduation prize.\n");
    } else {
        printf("The student does not qualify for a graduation prize.\n");
    }
}

void save_records() {
    FILE* file = fopen("student_records.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    StudentNode* current = student_list_head;
    while (current != NULL) {
        fprintf(file, "%s,%s,", current->student.name, current->student.student_id);
        for (int j = 0; j < NUM_MODULES; j++) {
            fprintf(file, "%s,%.2f,", current->student.module_names[j], current->student.scores[j]);
        }
        fprintf(file, "%.2f,%s\n", current->student.average, current->student.classification);
        current = current->next;
    }

    fclose(file);
    printf("Records saved successfully.\n");
}

void load_records() {
    FILE* file = fopen("student_records.txt", "r");
    if (file == NULL) {
        printf("No existing records found.\n");
        return;
    }

    char line[1000];
    while (fgets(line, sizeof(line), file)) {
        Student s;
        char* token = strtok(line, ",");
        if (token == NULL) continue;

        strncpy(s.name, token, MAX_NAME - 1);
        s.name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(s.student_id, token, 19);
        s.student_id[19] = '\0';

        for (int i = 0; i < NUM_MODULES; i++) {
            token = strtok(NULL, ",");
            if (token == NULL) break;
            strncpy(s.module_names[i], token, MAX_MODULE_NAME - 1);
            s.module_names[i][MAX_MODULE_NAME - 1] = '\0';

            token = strtok(NULL, ",");
            if (token == NULL) break;
            s.scores[i] = atof(token);
        }

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        s.average = atof(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        s.classification = strdup(token);

        insert_student_list(&s);
        insert_student_hash(&s);
    }

    fclose(file);
    printf("Records loaded successfully. Total students: %d\n", student_count);
}

void display_all_records() {
    if (student_count == 0) {
        printf("No records to display.\n");
        return;
    }

    StudentNode* current = student_list_head;
    int count = 1;
    while (current != NULL) {
        printf("\nStudent %d:\n", count++);
        display_student(&(current->student));
        current = current->next;
    }
}

void initialize_classifications() {
    classification_root = insert_classification(classification_root, 69.5, "1st (First-Class)");
    classification_root = insert_classification(classification_root, 59.5, "2.1 (Upper Second-Class)");
    classification_root = insert_classification(classification_root, 49.5, "2.2 (Lower Second-Class)");
    classification_root = insert_classification(classification_root, 39.5, "3rd");
}

int main() {
    initialize_classifications();
    load_records();

    int choice;
    char search_id[20];
    Student* found_student;

    while (1) {
        printf("\nUniversity of Suffolk Graduation Prize Determination Program\n");
        printf("-----------------------------------------------------------\n");
        printf("1. Add new student record\n");
        printf("2. Display all records\n");
        printf("3. Save records\n");
        printf("4. Search for a student\n");
        printf("5. Remove a student\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: {
                Student new_student;
                input_student_info(&new_student);
                display_student(&new_student);
                break;
            }
            case 2:
                display_all_records();
                break;
            case 3:
                save_records();
                break;
            case 4:
                printf("Enter student ID to search: ");
                fgets(search_id, sizeof(search_id), stdin);
                search_id[strcspn(search_id, "\n")] = 0;
                found_student = find_student(search_id);
                if (found_student) {
                    display_student(found_student);
                } else {
                    printf("Student not found.\n");
                }
                break;
            case 5:
                printf("Enter student ID to remove: ");
                fgets(search_id, sizeof(search_id), stdin);
                search_id[strcspn(search_id, "\n")] = 0;
                remove_student_list(search_id);
                printf("Student removed (if found).\n");
                break;
            case 6:
                printf("Exiting program. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}