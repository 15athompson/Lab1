#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>


// Test functions
void test_calculate_average() {
    float scores[5] = {70.0, 80.0, 90.0, 85.0, 75.0};
    float avg = calculate_average(scores, 5);
    assert(avg == 80.0);
    printf("Test calculate_average passed.\n");
}

void test_get_classification() {
    assert(strcmp(get_classification(70.0), "1st (First-Class)") == 0);
    assert(strcmp(get_classification(65.0), "2.1 (Upper Second-Class)") == 0);
    assert(strcmp(get_classification(55.0), "2.2 (Lower Second-Class)") == 0);
    assert(strcmp(get_classification(45.0), "3rd") == 0);
    assert(strcmp(get_classification(35.0), "FAIL") == 0);
    printf("Test get_classification passed.\n");
}

void test_add_and_remove_student() {
    // Clear existing records
    sqlite3_exec(db, "DELETE FROM students;", 0, 0, 0);

    // Add a test student
    Student test_student = {
        .name = "Test Student",
        .student_id = "TS001",
        .module_names = {"Module1", "Module2", "Module3", "Module4", "Module5"},
        .scores = {70.0, 80.0, 90.0, 85.0, 75.0},
        .average = 80.0,
        .classification = "1st (First-Class)"
    };

    // adding student
    strcpy(students[0].name, test_student.name);
    strcpy(students[0].student_id, test_student.student_id);
    for (int i = 0; i < NUM_MODULES; i++) {
        strcpy(students[0].module_names[i], test_student.module_names[i]);
        students[0].scores[i] = test_student.scores[i];
    }
    students[0].average = test_student.average;
    strcpy(students[0].classification, test_student.classification);
    student_count = 1;

    add_student();

    // Check if student was added
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM students WHERE student_id = 'TS001';", -1, &stmt, 0);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    assert(count == 1);

    // Remove the student
    remove_student("TS001");

    // Check if student was removed
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM students WHERE student_id = 'TS001';", -1, &stmt, 0);
    sqlite3_step(stmt);
    count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    assert(count == 0);

    printf("Test add_and_remove_student passed.\n");
}

void run_tests() {
    initialize_database();
    test_calculate_average();
    test_get_classification();
    test_add_and_remove_student();
    sqlite3_close(db);
    printf("All tests passed successfully.\n");
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        run_tests();
        return 0;
    }

  
}
