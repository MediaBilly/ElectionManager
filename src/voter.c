#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/voter.h"
#include "../headers/utilities.h"

struct voter
{
    char *idCode;
    char *firstname;
    char *lastname;
    int age;
    char gender;
    int zip;
};

int Voter_Initialize(Voter *v,char *idCode,char *firstname,char *lastname,int age,char gender,int zip) {
    if ((*v = (Voter)malloc(sizeof(struct voter))) == NULL) {
        not_enough_memory();
        return 0;
    }
    if (((*v)->idCode = (char*)malloc((strlen(idCode) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return 0;
    }
    strcpy((*v)->idCode,idCode);
    if (((*v)->firstname = (char*)malloc((strlen(firstname) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return 0;
    }
    strcpy((*v)->firstname,firstname);
    if (((*v)->lastname = (char*)malloc((strlen(lastname) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return 0;
    }
    strcpy((*v)->lastname,lastname);
    (*v)->age = age;
    (*v)->gender = gender;
    (*v)->zip = zip;
    return 1;
}

char *Voter_Get_IdCode(Voter v) {
    return v->idCode;
}

char *Voter_Get_FirstName(Voter v) {
    return v->firstname;
}

char *Voter_Get_LastName(Voter v) {
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
