#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_NAME 50
#define NUM_MODULES 5
#define MAX_MODULE_NAME 30
#define MAX_STUDENTS 100

typedef struct {
    char name[MAX_NAME];
    char student_id[20];
    char module_names[NUM_MODULES][MAX_MODULE_NAME];
    float scores[NUM_MODULES];
    float average;
    const char* classification;
} Student;

Student students[MAX_STUDENTS];
int student_count = 0;

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

const char* get_classification(float average) {
    if (average >= 69.5) return "1st (First-Class)";
    if (average >= 59.5) return "2.1 (Upper Second-Class)";
    if (average >= 49.5) return "2.2 (Lower Second-Class)";
    if (average >= 39.5) return "3rd";
    return "FAIL";
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
    student->classification = get_classification(student->average);
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

    for (int i = 0; i < student_count; i++) {
        fprintf(file, "%s,%s,", students[i].name, students[i].student_id);
        for (int j = 0; j < NUM_MODULES; j++) {
            fprintf(file, "%s,%.2f,", students[i].module_names[j], students[i].scores[j]);
        }
        fprintf(file, "%.2f,%s\n", students[i].average, students[i].classification);
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
    student_count = 0;
    while (fgets(line, sizeof(line), file) && student_count < MAX_STUDENTS) {
        Student* s = &students[student_count];
        char* token = strtok(line, ",");
        if (token == NULL) continue;

        strncpy(s->name, token, MAX_NAME - 1);
        s->name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(s->student_id, token, 19);
        s->student_id[19] = '\0';

        for (int i = 0; i < NUM_MODULES; i++) {
            token = strtok(NULL, ",");
            if (token == NULL) break;
            strncpy(s->module_names[i], token, MAX_MODULE_NAME - 1);
            s->module_names[i][MAX_MODULE_NAME - 1] = '\0';

            token = strtok(NULL, ",");
            if (token == NULL) break;
            s->scores[i] = atof(token);
        }

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        s->average = atof(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        s->classification = strdup(token);

        student_count++;
    }

    fclose(file);
    printf("Records loaded successfully. Total students: %d\n", student_count);
}

void display_all_records() {
    if (student_count == 0) {
        printf("No records to display.\n");
        return;
    }

    for (int i = 0; i < student_count; i++) {
        printf("\nStudent %d:\n", i + 1);
        display_student(&students[i]);
    }
}

int main() {
    int choice;
    load_records();

    while (1) {
        printf("\nUniversity of Suffolk Graduation Prize Determination Program\n");
        printf("-----------------------------------------------------------\n");
        printf("1. Add new student record\n");
        printf("2. Display all records\n");
        printf("3. Save records\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                if (student_count < MAX_STUDENTS) {
                    input_student_info(&students[student_count]);
                    display_student(&students[student_count]);
                    student_count++;
                } else {
                    printf("Maximum number of students reached.\n");
                }
                break;
            case 2:
                display_all_records();
                break;
            case 3:
                save_records();
                break;
            case 4:
                printf("Exiting program. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
