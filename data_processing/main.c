#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned MAX_LENGTH = 300;

void ReadFile_and_StoreInputData(int inputPixel_Intensity[], int inputPixel_Position[])
{

    FILE *Fin = NULL;
    Fin = fopen("C:\\Users\\hardi\\Downloads\\ih_coding_challenge-master\\ih_coding_challenge-master\\data_processing\\data.txt", "r");
    if (!Fin)
    {
        printf("File not found! Exiting...\n");
        return;
    }
    const char s[4] = ", ";
    char *token;
    const unsigned MAX = 5000;
    char buffer[MAX];
    int i, j;

    while (fgets(buffer, MAX, Fin) != NULL)
    {
        token = strtok(buffer, s); // read first extra string "intensity"
        token = strtok(NULL, s);   // read second extra string "position"
        for (i = 0; i < MAX_LENGTH; i++)
        {
            for (j = 0; j < 2; j++)
            {
                if (j == 0)
                {
                    token = strtok(NULL, s);
                    *(inputPixel_Intensity + i) = atoi(token);
                     printf("%d\t", inputPixel_Intensity[i]);
                }
                else
                {
                    token = strtok(NULL, s);
                    *(inputPixel_Position + i) = atoi(token);
                    printf("%d\n", inputPixel_Position[i]);
                }
            }
        }
    }

    fclose(Fin);
}

int main()
{
    printf("Have fun with the challenge!\n");
    int inputPixel_Position[300] = {0};
    int inputPixel_Intensity[300] = {0};

    ReadFile_and_StoreInputData(inputPixel_Intensity, inputPixel_Position);

    return 0;
}
