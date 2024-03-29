#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int hasvoted;
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
    (*v)->hasvoted = 0;
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

int Voter_HasVoted(Voter v) {
    return v != NULL ? v->hasvoted : 0;
}

void Voter_Vote(Voter v) {
    if (v != NULL) {
        v->hasvoted = 1;
    } 
}

void Voter_Print(Voter v,FILE *output) {
    // Print record info
    fprintf(output,"%s %s %s %d %d\n",v->idCode,v->firstname,v->lastname,v->zip,v->age);
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
