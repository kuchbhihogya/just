#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include "common.h"

bool admin_operation_handler(int clientSocket);
bool add_faculty(int clientSocket);
bool add_student(int clientSocket);
bool deactivate_student(int clientSocket);
bool deactivate_faculty(int clientSocket);
bool update_student(int clientSocket);
bool update_faculty(int clientSocket);
bool exit_function(int clientSocket);

bool authenticateadmin(int clientSocket);

struct UserCredentials
{
char username[100];
char pass[100];
};

struct UserCredentials adminCredentials={"Jass","123"};

bool admin_operation_handler(int clientSocket)
{
if(authenticateadmin(clientSocket))
{
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readbuff[1000], writebuff[1000]; // A buffer used for reading & writing to the client
        while (1)
        {
            send(clientSocket,"Login Successfully\n",strlen("Login Successfully\n"),0);
            char leMenu[] = "Do you want to:\n - 1.Add Student\n - 2.Add Faculty\n - 3.Deactivate Student\n - 4. Deactivate Faculty\n - 6. Update Student\n - 7. Update Faculty\n - 8.Exit from this menu ";

            send(clientSocket, leMenu, strlen(leMenu),0);
            readBytes = recv(clientSocket, readbuff, sizeof(readbuff),0);
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }

            int choice = atoi(readbuff);
           switch (choice)
            {
            case 1: 
                 if(add_student(clientSocket))
                    {
                     send(clientSocket,"Successfully added the student", strlen("Successfully added the student"),0);
                    }
                      break;
            case 2:
               if(add_faculty(clientSocket))
                 {
                   send(clientSocket,"Successfully added the faculty!!", strlen("Successfully added the faculty!!"),0);
                 }
                 break;
 
           case 3:deactivate_student(clientSocket);
                break;
            case 4:
                deactivate_faculty(clientSocket);
                break;
        /*    case 5:
                update_student(clientSocket);
                break;
            case 6:
                update_faculty(clientSocket);
                break;
           case 7:
                exit_function(clientSocket);*/
            default:
                send(clientSocket,"Sorry...Had to logout" ,strlen("Sorry...Had to logout"),0);
                return false;
            }
        }
 /*   else
    {
     
        return false;
    }*/
}

}
bool authenticateadmin(int clientSocket)
{
    char username[100];
    char pass[100];

    // Send a prompt for the username
    const char *userPrompt = "Enter username: ";
    send(clientSocket, userPrompt, strlen(userPrompt), 0);

    // Receive the username from the client
    ssize_t bytesRead = recv(clientSocket, username, sizeof(username), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    if (username[bytesRead - 1] == '\n')
    {
        username[bytesRead - 1] = '\0';
    }
    else
    {
        username[bytesRead] = '\0';
    }

    // Send a prompt for the password
    const char *passPrompt = "Enter password: ";
    send(clientSocket, passPrompt, strlen(passPrompt), 0);

    // Receive the password from the client
    bytesRead = recv(clientSocket, pass, sizeof(pass), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    if (pass[bytesRead - 1] == '\n')
    {
        pass[bytesRead - 1] = '\0';
    }
    else
    {
        pass[bytesRead] = '\0';
    }
     
    // Compare the received username and password with stored credentials
    if (strcmp(username, adminCredentials.username) == 0 &&
        strcmp(pass, adminCredentials.pass) == 0)
    {
        // Authentication successful
        const char *successMessage = "Authentication successful";
        send(clientSocket, successMessage, strlen(successMessage), 0);
        return true;
    }
    else
    {
        // Authentication failed
        const char *failureMessage = "Authentication failed";
        send(clientSocket, failureMessage, strlen(failureMessage), 0);
        close(clientSocket);
        return false;
    }
}

bool add_faculty(int clientSocket) {
    struct Faculty faculty;

    // Receive faculty name from the client
 char namePrompt[] = "Enter faculty name: ";
    send(clientSocket, namePrompt, strlen(namePrompt), 0);
    int readResult = recv(clientSocket, faculty.name, sizeof(faculty.name)-1, 0);
    if (readResult <= 0) {
        send(clientSocket,"Error receiving faculty name from client",strlen("Error receiving faculty name from client"),0);
        return false;
    }
  faculty.name[readResult]='\0';


   if (faculty.name[readResult - 1] == '\n') {
        faculty.name[readResult - 1] = '\0';
    }
    char passPrompt[] = "Enter password: ";
    send(clientSocket, passPrompt, strlen(passPrompt), 0);
    readResult = recv(clientSocket, faculty.password, sizeof(faculty.password)-1, 0);
    if (readResult <= 0) {
        send(clientSocket,"Error receiving faculty password from client",strlen("Error receiving faculty password from client"),0);
        return false;
    }
  faculty.password[readResult]='\0';


   if (faculty.password[readResult - 1] == '\n') {
        faculty.password[readResult - 1] = '\0';
    }

    // Receive faculty department from the client
    char departmentPrompt[] = "Enter faculty department: ";
    send(clientSocket, departmentPrompt, strlen(departmentPrompt), 0);
    readResult = recv(clientSocket, faculty.department, sizeof(faculty.department)-1, 0);
    if ( readResult <= 0) {
        perror("Error receiving faculty department from client");
        return false;
    }
      faculty.department[readResult]='\0';

   if (faculty.department[readResult - 1] == '\n') {
        faculty.department[readResult - 1] = '\0';
    }
    // Receive faculty employee ID from the client
    char employeeIDPrompt[] = "Enter faculty employee ID: ";
    send(clientSocket, employeeIDPrompt, strlen(employeeIDPrompt), 0);
    readResult = recv(clientSocket, &faculty.employeeID, sizeof(faculty.employeeID), 0);
    if (readResult == -1 || readResult < sizeof(faculty.employeeID)) {
        perror("Error receiving faculty employee ID from client");
        return false;
    }
     faculty.active=true;
    // Open the database file
    const char *dbFileName = "prof_db.txt";
int dbFile = open(dbFileName, O_RDWR | O_CREAT, 0644);

    if (dbFile == -1) {
        perror("Error opening or creating the database file");
        return false;
    }

    // Calculate the offset for the specific record based on its size
    off_t recordOffset = lseek(dbFile, 0, SEEK_END);

    // Lock the specific record for writing (advisory lock)
    if (flock(dbFile, LOCK_EX) == -1) {
        perror("Error locking the record for writing");
        close(dbFile);
        return false;
    }

    // Write the faculty information to the file at the calculated offset
    ssize_t writeResult = pwrite(dbFile, &faculty, sizeof(struct Faculty), recordOffset);
    if (writeResult == -1 || writeResult < sizeof(struct Faculty)) {
        perror("Error writing faculty record to database file");
        flock(dbFile, LOCK_UN); // Unlock the record before closing
        close(dbFile);
        return false;
   }

    // Unlock the specific record
    if (flock(dbFile, LOCK_UN) == -1) {
        perror("Error unlocking the record");
        close(dbFile);
        return false;
    }

    // Close the database file
    close(dbFile);

    return true;
}
bool add_student(int clientSocket)
{

   struct student stud;

    // Receive faculty name from the client
char namePrompt[] = "Enter student name: ";
    send(clientSocket, namePrompt, strlen(namePrompt), 0);
    int readResult = recv(clientSocket, stud.name, sizeof(stud.name)-1, 0);
    if (readResult <= 0) {
        send(clientSocket,"Error receiving student name from client",strlen("Error receiving student name from client"),0);
        return false;
    }
  stud.name[readResult]='\0';


   if (stud.name[readResult - 1] == '\n') {
        stud.name[readResult - 1] = '\0';
    }
char passPrompt[] = "Enter password: ";
    send(clientSocket, passPrompt, strlen(passPrompt), 0);
    readResult = recv(clientSocket, stud.password, sizeof(stud.password)-1, 0);
    if (readResult <= 0) {
        send(clientSocket,"Error receiving faculty password from client",strlen("Error receiving faculty password from client"),0);
        return false;
    }
  stud.password[readResult]='\0';


   if (stud.password[readResult - 1] == '\n') {
        stud.password[readResult - 1] = '\0';
    }

    // Receive faculty department from the client
    char departmentPrompt[] = "Enter student department: ";
    send(clientSocket, departmentPrompt, strlen(departmentPrompt), 0);
    readResult = recv(clientSocket, stud.department, sizeof(stud.department)-1, 0);
    if ( readResult <= 0) {
        perror("Error receiving faculty department from client");
        return false;
    }
      stud.department[readResult]='\0';

   if (stud.department[readResult - 1] == '\n') {
        stud.department[readResult - 1] = '\0';
    }
    // Receive faculty employee ID from the client
    char employeeIDPrompt[] = "Enter student employee ID: ";
    send(clientSocket, employeeIDPrompt, strlen(employeeIDPrompt), 0);
    readResult = recv(clientSocket, &stud.employeeID, sizeof(stud.employeeID), 0);
    if (readResult == -1 || readResult < sizeof(stud.employeeID)) {
        perror("Error receiving student ID from client");
        return false;
    }
    stud.active=true;
    // Open the database file
    const char *dbFileName = "student_db.txt";
    int dbFile = open(dbFileName, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (dbFile == -1) {
        perror("Error opening or creating the database file");
        return false;
    }

    // Calculate the offset for the specific record based on its size
    off_t recordOffset = lseek(dbFile, 0, SEEK_END);

    // Lock the specific record for writing (advisory lock)
    if (flock(dbFile, LOCK_EX) == -1) 
     {
        perror("Error locking the record for writing");
        close(dbFile);
        return false;
    }

    // Write the faculty information to the file at the calculated offset
    ssize_t writeResult = pwrite(dbFile, &stud, sizeof(struct student), recordOffset);
    if (writeResult == -1 || writeResult < sizeof(struct Faculty)) {
        perror("Error writing faculty record to database file");
        flock(dbFile, LOCK_UN); // Unlock the record before closing
        close(dbFile);
        return false;
   }

    // Unlock the specific record
    if (flock(dbFile, LOCK_UN) == -1) {
        perror("Error unlocking the record");
        close(dbFile);
        return false;
    }

    // Close the database file
    close(dbFile);

    return true;
}
bool deactivate_student(int clientSocket)
{
int empid;
send(clientSocket,"Enter the student ID to deactivate it",strlen("Enter the student ID to deactivate it"),0);
ssize_t bytesRead = recv(clientSocket, &empid, sizeof(empid), 0);
    if (bytesRead == -1 || bytesRead < sizeof(empid))
    {
        perror("Error receiving student ID from client");
        return false;
    }
const char *dbFileName = "student_db.txt";
    int dbFile = open(dbFileName, O_RDWR);
    if (dbFile == -1)
    {
        perror("Error opening the database file");
        return false;
    }

    // Lock the file for writing
    if (flock(dbFile, LOCK_EX) == -1)
    {
        perror("Error locking the file for writing");
        close(dbFile);
        return false;
    }

    // Iterate through the file to find the student record
    struct student studRecord;
    bool found = false;

    while (read(dbFile, &studRecord, sizeof(struct student)) > 0)
    {
        if (studRecord.employeeID == empid)
        {
            found = true;

            // Mark the student as inactive
            studRecord.active = false;

            // Move the file pointer back to the beginning of the record and update it
            lseek(dbFile, -sizeof(struct student), SEEK_CUR);
            write(dbFile, &studRecord, sizeof(struct student));
            break;
        }
    }

    if (found)
    {
        // Student deactivated successfully
        const char *deactivationMessage = "Student deactivated successfully";
        send(clientSocket, deactivationMessage, strlen(deactivationMessage), 0);
         return true;
    }
    else
    {
        // Student not found
        const char *notFoundMessage = "Student not found";
        send(clientSocket, notFoundMessage, strlen(notFoundMessage), 0);
        return false;
    }

    // Unlock the file
    if (flock(dbFile, LOCK_UN) == -1)
    {
        perror("Error unlocking the file");
     }

    // Close the file
    close(dbFile);

    return true;
}
bool deactivate_faculty(int clientSocket)
{
int empid;
send(clientSocket,"Enter the faculty ID to deactivate it",strlen("Enter the student ID to deactivate it"),0);
ssize_t bytesRead = recv(clientSocket, &empid, sizeof(empid), 0);
    if (bytesRead == -1 || bytesRead < sizeof(empid))
    {
        perror("Error receiving student ID from client");
        return false;
    }
const char *dbFileName = "faculty_db.txt";
    int dbFile = open(dbFileName, O_RDWR);
    if (dbFile == -1)
    {
        perror("Error opening the database file");
        return false;
    }

    // Lock the file for writing
    if (flock(dbFile, LOCK_EX) == -1)
    {
        perror("Error locking the file for writing");
        close(dbFile);
        return false;
    }

    // Iterate through the file to find the student record
    struct Faculty facultyRecord;
    bool found = false;

    while (read(dbFile, &facultyRecord, sizeof(struct Faculty)) > 0)
    {
        if (facultyRecord.employeeID == empid)
        {
            found = true;

            // Mark the student as inactive
            facultyRecord.active = false;

            // Move the file pointer back to the beginning of the record and update it
            lseek(dbFile, -sizeof(struct student), SEEK_CUR);
            write(dbFile, &facultyRecord, sizeof(struct Faculty));
            break;
        }
    }

    if (found)
    {
        // Student deactivated successfully
        const char *deactivationMessage = "Student deactivated successfully";
        send(clientSocket, deactivationMessage, strlen(deactivationMessage), 0);
         return true;
    }
    else
    {
        // Student not found
        const char *notFoundMessage = "Student not found";
        send(clientSocket, notFoundMessage, strlen(notFoundMessage), 0);
        return false;
    }

    // Unlock the file
    if (flock(dbFile, LOCK_UN) == -1)
    {
        perror("Error unlocking the file");
     }

    // Close the file
    close(dbFile);

    return true;
}
#endif
