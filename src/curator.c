#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/curator.h"
#include "../headers/utilities.h"
#include "../headers/redblacktree.h"
#include "../headers/voter.h"
#include "../headers/bloomfilter.h"
#include "../headers/postcodes.h"

#define BUFFER_SIZE 15

struct curator
{
    FILE *inputfile;
    FILE *outputfile;
    int numofupdates;
    int updates;
    RedBlackTree RBT;
    BloomFilter BF;
    PostCodes PC;
};

string readNextWord(FILE *input) {
    // Ignore whitespace
    char ch = 0;
    while (!feof(input) && ((ch = fgetc(input)) < 33));
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
    // If last character was new line feed return it for potential future use in voted command
    if (ch == '\n')
        ungetc(ch,input);
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
    // Ignore other characters
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

int readPostCode(FILE *input) {
    // Ignore other characters than numbers and stop at new line feed(enter)
    char ch;
    while ((ch = fgetc(input)) != 10 && (ch < '0' || ch > '9'));
    if (ch != 10)
        ungetc(ch,input);
    else
        return -1;
    int num = 0;
    while ((ch = fgetc(input)) != 10 && ch >= '0' && ch <= '9') {
        num *= 10;
        num += ch - '0';
    }
    // If last character when reading digits was not new line feed skip rest characters until new line feed(enter)
    if (ch != 10)
        while ((ch = fgetc(input)) != 10);
    return num;
}

int Curator_Initialize(Curator *cur,FILE *inputfile,FILE *outputfile,int numofupdates) {
    // Allocate memory for the curator
    if ((*cur = (Curator)malloc(sizeof(struct curator))) == NULL) {
        not_enough_memory();
        return 0;
    }
    // Initialize variables
    (*cur)->inputfile = inputfile != NULL ? inputfile : stdin;
    (*cur)->outputfile = outputfile != NULL ? outputfile : stdout;
    (*cur)->numofupdates = numofupdates;
    (*cur)->updates = 0;
    // Initialize the RBT
    if (!RBT_Initialize(&((*cur)->RBT))) {
        return 0;
    }
    // Initialize the BF
    if (!BF_Initialize(&((*cur)->BF),lines((*cur)->inputfile))) {
        return 0;
    }
    // Initialize the PostCodes Data Structure
    if (!PostCodes_Initialize(&((*cur)->PC))) {
        return 0;
    }
    // Read the records of all voters and insert them to the data structures
    char gender;
    string idCode,firstname,lastname;
    int age,zip;
    while ((idCode = readNextWord((*cur)->inputfile)) != NULL)
    {
        if ((lastname = readNextWord((*cur)->inputfile)) == NULL) {
            return 0;
        }
        if ((firstname = readNextWord((*cur)->inputfile)) == NULL) {
            return 0;
        }
        fscanf((*cur)->inputfile,"%d",&age);
        gender = readGender((*cur)->inputfile);
        fscanf((*cur)->inputfile,"%d",&zip);
        // Check if a voter with the same id already exists.If not, insert him to the data structures
        if (RBT_Search((*cur)->RBT,idCode) == NULL) {
            // Allocate memory for the new voter
            Voter v;
            Voter_Initialize(&v,idCode,firstname,lastname,age,gender,zip);
            // Insert her/him to the RBT
            RBT_Insert((*cur)->RBT,v);
            // Insert her/him to the BF
            BF_Insert((*cur)->BF,idCode);
            // Insert her/him to the PostCodes data structure
            PostCodes_InsertVoter((*cur)->PC,v);
        }
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
                printf("# REC-WITH %s SET-VOTED\n",idCode);
                break;
            case ALREADY_VOTED:
                printf("# REC-WITH %s ALREADY-VOTED\n",idCode);
                break;
            case VOTER_NOT_FOUND:
                printf("- KEY %s NOT-in-structs\n",idCode);
                break;
            default:
                break;
        }
    } else {
        // Not in BF so definitely NOT in structs
        printf("- KEY %s NOT-in-structs\n",idCode);
    }
}

int updateBF(Curator cur) {
    if (++cur->updates == cur->numofupdates) {
        if (BF_Resize(cur->BF)) {
            // Re-insert all id's to the bloom filter
            Voter v;
            while ((v = RBT_Next_Record(cur->RBT)) != NULL)
                BF_Insert(cur->BF,Voter_Get_IdCode(v));
            cur->updates = 0;
        } else {
            return 0;
        }
    }
    return 1;
}

int insertRecord(Curator cur,string idCode,string firstname,string lastname,int age,char gender,int zip) {
    Voter v;
    Voter_Initialize(&v,idCode,firstname,lastname,age,gender,zip);
    // Insert her/him to the RBT
    RBT_Insert(cur->RBT,v);
    // Insert her/him to the BF
    BF_Insert(cur->BF,idCode);
    // Insert her/him to the PostCodes data structure
    PostCodes_InsertVoter(cur->PC,v);
    // Check if we must increase size of the bloom filter
    if (!updateBF(cur))
        return 0;
    printf("# REC-WITH %s INSERTED-IN-BF-RBT\n",idCode);
    return 1;
}

void Curator_Run(Curator cur) {
    int run = 1;
    char *cmd = NULL,*param = NULL;
    // Main Loop(Commands)
    while (run) {
        // Read Command
        printf(">");
        if ((cmd = readNextWord(stdin)) == NULL) {
            return;
        }
        // 1. lbf key
        if (!strcmp(cmd,"lbf")) {
            // Read key
            if ((param = readNextWord(stdin)) == NULL) {
                return;
            }
            if (BF_Search(cur->BF,param)) {
                printf("# KEY %s POSSIBLY-IN REGISTRY\n",param);
            } else {
                printf("# KEY %s Not-in-LBF\n",param);
            }
        }
        // 2. lrb key
        else if (!strcmp(cmd,"lrb")) {
            // Read key
            if ((param = readNextWord(stdin)) == NULL) {
                return;
            }
            if (RBT_Search(cur->RBT,param) != NULL) {
                printf("# KEY %s FOUND-IN-RBT\n",param);
            } else {
                printf("# KEY %s NOT-IN-RBT\n",param);
            }
        }
        // 3. ins record
        else if (!strcmp(cmd,"ins")) {
            // Input format: key firstname lastname age gender postcode
            string idCode,firstname,lastname;
            int zip,age;
            if ((idCode = readNextWord(stdin)) == NULL) {
                return;
            }
            if ((firstname = readNextWord(stdin)) == NULL) {
                return;
            }
            if ((lastname = readNextWord(stdin)) == NULL) {
                return;
            }
            scanf("%d",&age);
            char gender = readGender(stdin);
            scanf("%d",&zip);
            // Check if he already exists
            if (!BF_Search(cur->BF,idCode)) {
                insertRecord(cur,idCode,firstname,lastname,age,gender,zip);
            } else {
                if (!RBT_Search(cur->RBT,idCode)) {
                    insertRecord(cur,idCode,firstname,lastname,age,gender,zip);
                } else {
                    printf("- REC-WITH %s EXISTS\n",idCode);
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
            if ((param = readNextWord(stdin)) == NULL) {
                return;
            }
            // Search BF first
            if (BF_Search(cur->BF,param)) {
                // Then search the RBT
                Voter v;
                if ((v = RBT_Search(cur->RBT,param)) == NULL) {
                    printf("# REC-WITH %s NOT-in-structs\n",param);
                } else {
                    printf("# REC-IS: ");
                    Voter_Print(v,stdout);
                }
            } else {
                // Not in BF so definitely NOT in structs
                printf("# REC-WITH %s NOT-in-structs\n",param);
            }
        }
        // 5. delete key
        else if (!strcmp(cmd,"delete")) {
            // Read key
            if ((param = readNextWord(stdin)) == NULL) {
                return;
            }
            // Search and delete from BF first and then from postal codes
            if (BF_Search(cur->BF,param)) {
                Voter v = RBT_Search(cur->RBT,param);
                if (v != NULL) {
                    // Delete him from postcodes too
                    PostCodes_DeleteVoter(cur->PC,v);
                    // Delete him from the RBT
                    RBT_Delete(cur->RBT,param);
                    // Delete him from BF too
                    BF_Delete(cur->BF);
                    // Check if we must decrease size of the bloom filter
                    updateBF(cur);
                    printf("# DELETED %s FROM-structs\n",param);
                } else {
                    printf("- KEY %s NOT-in-structs\n",param);
                }
            } else {
                // Not in BF so definitely NOT in structs
                printf("- KEY %s NOT-in-structs\n",param);
            }
        }
        // 6. vote key
        else if (!strcmp(cmd,"vote")) {
            // Read key
            if ((param = readNextWord(stdin)) == NULL) {
                return;
            }
            Vote(cur,param);
        }
        // 7. load fileofkeys
        else if (!strcmp(cmd,"load")) {
            // Open fileofkeys
            if ((param = readNextWord(stdin)) == NULL) {
                return;
            }
            FILE *fileofkeys = fopen(param,"r");
            // Vote all codes contained in the fileofkeys
            string idCode;
            while ((idCode = readNextWord(fileofkeys)) != NULL) {
                Vote(cur,idCode);
                DestroyString(&idCode);
            }
            fclose(fileofkeys);
        }
        else if (!strcmp(cmd,"voted")) {
            int postcode;
            if ((postcode = readPostCode(stdin)) == -1) {
                // 8. voted
                printf("# NUMBER %d\n",RBT_NumVoted(cur->RBT));
            } else {
                // 9. voted postcode
                PostCodes_PrintPostCode(cur->PC,postcode);
            }
        }
        // 10. votedperpc
        else if (!strcmp(cmd,"votedperpc")) {
            PostCodes_PrintAll(cur->PC);
        }
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
    // Print final registry
    PostCodes_PrintFinal(cur->PC,cur->outputfile);
}

void Curator_Destroy(Curator *cur) {
    RBT_Destroy(&(*cur)->RBT);
    BF_Destroy(&(*cur)->BF);
    PostCodes_Destroy(&(*cur)->PC);
    free(*cur);
    *cur = NULL;
}
