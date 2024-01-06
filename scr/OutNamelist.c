
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"
#include "HM_ST.h"
#include "OutNamelist.h"

void Import_Outnamelist(
    char FP[],
    OUT_NAME_LIST *outnl
)
{
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open file %s\n", FP);
        exit(0);
    }
    int i;  // character indexing in a string,
    int j = 0;
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        j++;
        if (row != NULL && strlen(row) > 1 && row[0] != '#')
        {
            for (i = 0; i < strlen(row); i++)
            {
                /* remove all the characters after # */
                if (row[i] == '#')
                {
                    row[i] = '\0'; // replace the '#' with '\0', end sign.
                }
            }
            if (sscanf(row, "%[^,],%s", S1, S2) == 2)
            {
                if (strcmp(S1, "EI_o") == 0)
                {
                    outnl->EI_o = atoi(S2);
                }
                else if (strcmp(S1, "EI_u") == 0)
                {
                    outnl->EI_u = atoi(S2);
                }
                else if (strcmp(S1, "ET_o") == 0)
                {
                    outnl->ET_o = atoi(S2);
                } 
                else if (strcmp(S1, "ET_u") == 0)
                {
                    outnl->ET_u = atoi(S2);
                }
                else if (strcmp(S1, "Es") == 0)
                {
                    outnl->Es = atoi(S2);
                }
                else if (strcmp(S1, "Ep") == 0)
                {
                    outnl->Ep = atoi(S2);
                }
                else if (strcmp(S1, "Interception_u") == 0)
                {
                    outnl->Interception_u = atoi(S2);
                }
                else if (strcmp(S1, "Interception_o") == 0)
                {
                    outnl->Interception_o = atoi(S2);
                }
                else if (strcmp(S1, "Rs") == 0)
                {
                    outnl->Rs = atoi(S2);
                }
                else if (strcmp(S1, "Rno") == 0)
                {
                    outnl->Rno = atoi(S2);
                }
                else if (strcmp(S1, "Rnu") == 0)
                {
                    outnl->Rnu = atoi(S2);
                }
                else if (strcmp(S1, "L_sky") == 0)
                {
                    outnl->L_sky = atoi(S2);
                } 
                else if (strcmp(S1, "SM_Lower") == 0)
                {
                    outnl->SM_Lower = atoi(S2);
                } 
                else if (strcmp(S1, "SM_Upper") == 0)
                {
                    outnl->SM_Upper = atoi(S2);
                }
                else if (strcmp(S1, "SW_Infiltration") == 0)
                {
                    outnl->SW_Infiltration = atoi(S2);
                }
                else if (strcmp(S1, "SW_Percolation_Lower") == 0)
                {
                    outnl->SW_Percolation_Lower = atoi(S2);
                }
                else if (strcmp(S1, "SW_Percolation_Upper") == 0)
                {
                    outnl->SW_Percolation_Upper = atoi(S2);
                }
                else if (strcmp(S1, "SW_Run_Infil") == 0)
                {
                    outnl->SW_Run_Infil = atoi(S2);
                }
                else if (strcmp(S1, "SW_Run_Satur") == 0)
                {
                    outnl->SW_Run_Satur = atoi(S2);
                }
                else
                {
                    printf("Unrecognized field in row %d: %s\n", j, row);
                    exit(0);
                }
            }
        }
    }
    fclose(fp);
}

void Initialize_Outnamelist(
    OUT_NAME_LIST *outnl
)
{

    for (size_t i = 0; i < sizeof(OUT_NAME_LIST) / sizeof(short); i++)
    {
        short *currentMember = ((short*)outnl) + i;
        *currentMember = 0;
    }
}

void malloc_Outnamelist(
    OUT_NAME_LIST outnl,
    int cell_counts_total,
    int time_steps_run,
    int **out_Rs
)
{
    if (outnl.Rs == 1)
    {
        *out_Rs = (int *)malloc(sizeof(int) * time_steps_run * cell_counts_total);
    }
    
}


// int main(int argc, char const *argv[])
// {
//     /* code */
//     OUT_NAME_LIST outnl;
//     Initialize_Outnamelist(&outnl);
//     printf("Ep: %d\n", outnl.Ep);
//     printf("Rs: %d\n", outnl.Rs);
//     printf("SW_Run_Infil: %d\n", outnl.SW_Run_Infil);

//     Import_Outnamelist(
//         "D:/xHM/example_data/OUTPUT_NAMELIST.txt",
//         &outnl);
//     printf("Ep: %d\n", outnl.Ep);
//     printf("Rs: %d\n", outnl.Rs);
//     printf("SW_Run_Infil: %d\n", outnl.SW_Run_Infil);
//     return 0;
// }
