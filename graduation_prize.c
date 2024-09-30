#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_MODULES 5

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

int main() {
    const char* modules[NUM_MODULES] = {"Module 1", "Module 2", "Module 3", "Module 4", "Module 5"};
    float scores[NUM_MODULES];

    printf("University of Suffolk Graduation Prize Determination Program\n");
    printf("-----------------------------------------------------------\n");

    for (int i = 0; i < NUM_MODULES; i++) {
        scores[i] = get_valid_score(modules[i]);
    }

    printf("\nStudent's Scores:\n");
    for (int i = 0; i < NUM_MODULES; i++) {
        printf("%s: %.2f\n", modules[i], scores[i]);
    }

    float average = calculate_average(scores, NUM_MODULES);
    const char* classification = get_classification(average);

    printf("\nFinal Average: %.2f\n", average);
    printf("Classification: %s\n", classification);

    if (strcmp(classification, "1st (First-Class)") == 0) {
        printf("Congratulations! The student qualifies for a graduation prize.\n");
    } else {
        printf("The student does not qualify for a graduation prize.\n");
    }

    return 0;
}
