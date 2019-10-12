#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../headers/voter.h"
#include "../headers/utilities.h"

struct voter
{
    string idCode;
    string firstname;
    string lastname;
    int age;
    char gender;
    int zip;
};

int Voter_Initialize(Voter *v,string idCode,string firstname,string lastname,int age,char gender,int zip) {
    if ((*v = (Voter)malloc(sizeof(struct voter))) == NULL) {
        not_enough_memory();
        return 0;
    }
    if (((*v)->idCode = (string)malloc((strlen(idCode) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return 0;
    }
    strcpy((*v)->idCode,idCode);
    if (((*v)->firstname = (string)malloc((strlen(firstname) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return 0;
    }
    strcpy((*v)->firstname,firstname);
    if (((*v)->lastname = (string)malloc((strlen(lastname) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return 0;
    }
    strcpy((*v)->lastname,lastname);
    (*v)->age = age;
    (*v)->gender = gender;
    (*v)->zip = zip;
    return 1;
}

string Voter_Get_IdCode(Voter v) {
    return v->idCode;
}

string Voter_Get_FirstName(Voter v) {
    return v->firstname;
}

string Voter_Get_LastName(Voter v) {
    return v->lastname;
}

int Voter_Get_Age(Voter v) {
    return v->age;
}

char Voter_Get_Gender(Voter v) {
    return v->gender;
}

int Voter_Get_Zip(Voter v) {
    return v->zip;
}

void Voter_Print(Voter v) {
    // Get year of birth
    // Source:https://www.codevscolor.com/c-print-current-time-day-month-year/
    time_t s = time(NULL);
    struct tm *curTime = localtime(&s);
    int year = curTime->tm_year + 1900 - v->age;
    // Print record info
    printf("%s %s %s %d %d",v->idCode,v->firstname,v->lastname,v->zip,year);
}

void Voter_Destroy(Voter *v) {
    if (*v != NULL) {
        if ((*v)->idCode != NULL)
            free((*v)->idCode);
        if ((*v)->firstname != NULL)
            free((*v)->firstname);
        if ((*v)->lastname != NULL)
            free((*v)->lastname);
        free(*v);
        *v = NULL;
    }
}
