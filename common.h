#ifndef COMMON_H
#define COMMON_H
#define MAX_ENROLLED_COURSES 6
#define MAX_OFFERED_COURSES 10

struct course
{
char name[100];
char course_id[100];
char department[100];
char off_faculty[100];
int curr_strength;
int max_strength;
};


struct student
{
    char name[100];
    char department[100];
    char password[100];
    bool active;
    int employeeID;
    char enrolled_courses[MAX_ENROLLED_COURSES][100];
    int num_enrolled_courses;
};

struct Faculty
{
char name[100];
char department[100];
char password[100];
bool active;
int employeeID;
char offered_courses[MAX_OFFERED_COURSES][100];
int num_offered_courses;
};

#endif
