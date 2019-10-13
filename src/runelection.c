#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/curator.h"

int main(int argc, char const *argv[])
{
    // Define variables
    FILE *inputfile = NULL,*outputfile = NULL;
    int numofupdates = 5;
    Curator curator = NULL;
    // Read arguments
    int i;
    for (i = 1;i < argc;i++)
    {
        if (!strcmp(argv[i],"-i")) {
            if((inputfile = fopen(argv[++i],"r")) == NULL) {
                printf("Could not open input file.\n");
                return 1;
            }
        } else if (!strcmp(argv[i],"-o")) {
            if((outputfile = fopen(argv[++i],"w")) == NULL) {
                printf("Could not open output file.\n");
                return 1;
            }
        } else if (!strcmp(argv[i],"-n")) {
            numofupdates = atoi(argv[++i]);
        } else {
            // Wrong arguments.So specify the correct usage and exit normally
            printf("Usage: ./runelection -i inputfile -o outfile -n numofupdates\n");
            return 0;
        }
    }
    if(Curator_Initialize(&curator,inputfile,outputfile,numofupdates)) {
        Curator_Run(curator);
        Curator_Destroy(&curator);
    }
    //Close files and exit
    if (inputfile != NULL)
        fclose(inputfile);
    if (outputfile != NULL)
        fclose(outputfile);
    return 0;
}
