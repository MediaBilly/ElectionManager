#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/curator.h"
#include "../headers/utilities.h"
#include "../headers/redblacktree.h"
#include "../headers/voter.h"
#include "../headers/bloomfilter.h"

#define BUFFER_SIZE 15

struct curator
{
    FILE *inputfile;
    FILE *outputfile;
    int numofupdates;
    int inserts;
    RedBlackTree RBT;
    BloomFilter BF;
};

string readNextWord(FILE *input) {
    // Ignore whitespace
    char ch = 0;
    while (!feof(input) && ((ch = fgetc(input)) < 'A' || ('Z' < ch && ch < 'a') || ch > 'z'));
    if (feof(input))
        return NULL;
    ungetc(ch,input);
    char *buf;
    int curBufSize = sizeof(char) * BUFFER_SIZE;
    if ((buf = (string)malloc(curBufSize)) == NULL) {
        not_enough_memory();
        return NULL;
    }
    int i = 0;
    // Read word to dynamically allocated buffer letter by letter in order to avoid static array
    while (!feof(input) && (ch = fgetc(input)) != '\n' && ch != ' ')
    {
        // Buffer full so allocate more space
        if (i == curBufSize) {
            if ((buf = (string)realloc(buf,(BUFFER_SIZE + curBufSize) * sizeof(char))) == NULL) {
                not_enough_memory();
                return NULL;
            }
            curBufSize += BUFFER_SIZE * sizeof(char);
        }
        buf[i++] = ch;
    }
    // Buffer full so allocate 1 more byte for end of string character to fit
    if (i == curBufSize) {
        if ((buf = (string)realloc(buf,(1 + curBufSize) * sizeof(char))) == NULL) {
            not_enough_memory();
            return NULL;
        }
        curBufSize += sizeof(char);
    }
    buf[i] = '\0';
    string word;
    if((word = (string)malloc((strlen(buf) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return NULL;
    }
    strcpy(word,buf);
    free(buf);
    return word;
}

char readGender(FILE *input) {
    char ch = 0;
    // Ignore whitespace
    while (!feof(input) && ((ch = fgetc(input)) < 'A' || ('Z' < ch && ch < 'a') || ch > 'z'));
    return ch;
}

unsigned int lines(FILE *file) {
    char ch;
    unsigned int lines = 1;
    // Count lines
    while (!feof(file))
        if ((ch = fgetc(file)) == '\n')
            lines++;
    // Go to the start of the file in order to read it again later
    fseek(file,0,SEEK_SET);
    return lines - 1;
}

int Curator_Initialize(Curator *cur,FILE *inputfile,FILE *outputfile,int numofupdates) {
    // Allocate memory for the curator
    if ((*cur = (Curator)malloc(sizeof(struct curator))) == NULL) {
        not_enough_memory();
        return 0;
    }
    // Initialize variables
    (*cur)->inputfile = inputfile;
    (*cur)->outputfile = outputfile;
    (*cur)->numofupdates = numofupdates;
    (*cur)->inserts = 0;
    // Initialize the RBT
    if (!RBT_Initialize(&((*cur)->RBT))) {
        return 0;
    }
    // Initialize the BF
    if (!BF_Initialize(&((*cur)->BF),lines(inputfile))) {
        return 0;
    }
    // Read the records of all voters and insert them to the data structures
    char gender;
    string idCode,firstname,lastname;
    int age,zip;
    while ((idCode = readNextWord(inputfile)) != NULL)
    {
        if ((firstname = readNextWord(inputfile)) == NULL) {
            return 0;
        }
        if ((lastname = readNextWord(inputfile)) == NULL) {
            return 0;
        }
        fscanf(inputfile,"%d",&age);
        gender = readGender(inputfile);
        fscanf(inputfile,"%d",&zip);
        // Allocate memory for the new voter
        Voter v;
        Voter_Initialize(&v,idCode,firstname,lastname,age,gender,zip);
        // Insert her/him to the RBT
        RBT_Insert((*cur)->RBT,v);
        // Insert her/him to the BF
        BF_Insert((*cur)->BF,idCode);
        // Free unecessary memory
        DestroyString(&idCode);
        DestroyString(&lastname);
        DestroyString(&firstname);
    }
    return 1;
}

void Vote(Curator cur,string idCode) {
    // Search BF first!
    if (BF_Search(cur->BF,idCode)) {
        switch (RBT_Vote(cur->RBT,idCode))
        {
            case VOTE_SUCCESS:
                printf("\t# REC-WITH %s SET-VOTED\n",idCode);
                break;
            case ALREADY_VOTED:
                printf("\t# REC-WITH %s ALREADY-VOTED\n",idCode);
                break;
            case VOTER_NOT_FOUND:
                printf("\t# key %s NOT-in-structs\n",idCode);
                break;
            default:
                break;
        }
    } else {
        // Not in BF so definitely NOT in structs
        printf("\t# KEY %s NOT-in-structs\n",idCode);
    }
}

int insertRecord(Curator cur,string idCode,string firstname,string lastname,int age,int zip) {
    Voter v;
    Voter_Initialize(&v,idCode,firstname,lastname,age,'M',zip);
    // Insert her/him to the RBT
    RBT_Insert(cur->RBT,v);
    // Check if we must increas size of the bloom filter
    if (++cur->inserts == cur->numofupdates) {
        if (BF_Resize(cur->BF)) {
            // Re-insert all id's to the bloom filter
            string id;
            while ((id = RBT_Next_Id(cur->RBT)) != NULL)
                BF_Insert(cur->BF,id);
            cur->inserts = 0;
        } else {
            return 0;
        }
    } else {
        // Insert her/him to the BF
        BF_Insert(cur->BF,idCode);
    }
    printf("\t# REC-WITH %s INSERTED-IN-BF-RBT\n",idCode);
    return 1;
}

void Curator_Run(Curator cur) {
    int run = 1;
    char *cmd = NULL,*param = NULL;
    // Main Loop(Commands)
    while (run) {
        // Read Command
        printf(">");
        cmd = readNextWord(stdin);
        // 1. lbf key
        if (!strcmp(cmd,"lbf")) {
            // Read key
            param = readNextWord(stdin);
            if (BF_Search(cur->BF,param)) {
                printf("\t# KEY %s POSSIBLY-IN REGISTRY\n",param);
            } else {
                printf("\t# KEY %s Not-in-LBF\n",param);
            }
        }
        // 2. lrb key
        else if (!strcmp(cmd,"lrb")) {
            // Read key
            param = readNextWord(stdin);
            if (RBT_Search(cur->RBT,param) != NULL) {
                printf("\t# KEY %s FOUND-IN-RBT\n",param);
            } else {
                printf("\t# KEY %s NOT-IN-RBT\n",param);
            }
        }
        // 3. ins record
        else if (!strcmp(cmd,"ins")) {
            string idCode,firstname,lastname;
            int zip,age;
            idCode = readNextWord(stdin);
            firstname = readNextWord(stdin);
            lastname = readNextWord(stdin);
            scanf("%d %d",&age,&zip);
            // Check if he already exists
            if (!BF_Search(cur->BF,idCode)) {
                insertRecord(cur,idCode,firstname,lastname,age,zip);
            } else {
                if (!RBT_Search(cur->RBT,idCode)) {
                    insertRecord(cur,idCode,firstname,lastname,age,zip);
                } else {
                    printf("\t# REC-WITH %s EXISTS\n",idCode);
                }
            }
            // Free unecessary memory
            DestroyString(&idCode);
            DestroyString(&lastname);
            DestroyString(&firstname);
        }
        // 4. find key
        else if (!strcmp(cmd,"find")) {
            // Read key
            param = readNextWord(stdin);
            // Search BF first
            if (BF_Search(cur->BF,param)) {
                // Then search the RBT
                Voter v;
                if ((v = RBT_Search(cur->RBT,param)) == NULL) {
                    printf("\t# REC-WITH %s NOT-in-structs\n",param);
                } else {
                    printf("\t# REC-IS: ");
                    Voter_Print(v);
                    printf("\n");
                }
            } else {
                // Not in BF so definitely NOT in structs
                printf("\t# REC-WITH %s NOT-in-structs\n",param);
            }
        }
        // 5. delete key
        else if (!strcmp(cmd,"delete")) {
            // Read key
            param = readNextWord(stdin);
            // Search and delete from BF first and then from postal codes
            if (BF_Search(cur->BF,param)) {
                if (RBT_Delete(cur->RBT,param)) {
                    printf("\t# DELETED %s FROM-structs\n",param);
                } else {
                    printf("\t# KEY %s NOT-in-structs\n",param);
                }
            } else {
                // Not in BF so definitely NOT in structs
                printf("\t# KEY %s NOT-in-structs\n",param);
            }
        }
        // 6. vote key
        else if (!strcmp(cmd,"vote")) {
            // Read key
            param = readNextWord(stdin);
            Vote(cur,param);
        }
        // 7. load fileofkeys
        else if (!strcmp(cmd,"load")) {
            // Open fileofkeys
            param = readNextWord(stdin);
            FILE *fileofkeys = fopen(param,"r");
            // Vote all codes contained in the fileofkeys
            string idCode;
            while ((idCode = readNextWord(fileofkeys)) != NULL) {
                Vote(cur,idCode);
                DestroyString(&idCode);
            }
            fclose(fileofkeys);
        }
        // 8. voted
        else if (!strcmp(cmd,"voted")) {
            printf("\t# NUMBER %d\n",RBT_NumVoted(cur->RBT));
        }
        // 9. voted postcode
        // 10. votedperpc
        // 11. exit
        else if (!strcmp(cmd,"exit")) {
            run = 0;
        }
        // Wrong
        else {
            printf("Wrong Command\n");
        }
        DestroyString(&param);
        DestroyString(&cmd);
    }
}

void Curator_Destroy(Curator *cur) {
    RBT_Destroy(&(*cur)->RBT);
    BF_Destroy(&(*cur)->BF);
    free(*cur);
    *cur = NULL;
}
