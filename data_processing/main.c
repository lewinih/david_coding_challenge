#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned MAX_LENGTH = 300;
int FilterPixel_Intensity[300] = {0};
int Trough_Position[300] = {0};
int Trough_Intensity[300] = {0};

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
                }
                else
                {
                    token = strtok(NULL, s);
                    *(inputPixel_Position + i) = atoi(token);                   
                }
            }
        }
    }

    fclose(Fin);
}
//***********************************************************************************************************
void Apply_rolling_median_filter(int *inputPixel_Intensity)
{
    int i = 0, A = 0, B = 0, C = 0, Temp = 0;
    int *F_P;
    F_P = FilterPixel_Intensity;
    for (i = 0; i < MAX_LENGTH - 2; i++)
    {
        A = *(inputPixel_Intensity + i);
        B = *(inputPixel_Intensity + i + 1);
        C = *(inputPixel_Intensity + i + 2);

        if (A > B) // Sorting A and B
        {
            Temp = A;
            A = B;
            B = Temp; // swap A and B
        }
        // compare sorting of 3 signals
        if (C >= A && C <= B)
        {
            *(F_P + i) = C; // if C is middle one
        }
        else if (C <= A)
        {
            *(F_P + i) = A; // if A is middle one
        }
        else if (C >= B)
        {
            *(F_P + i) = B; // if B is middle one
        }
        //printf("filter_intensity = %d\n", *(F_P + i));
    }
}
//****************************************************************************************************************
void Peak_Detection(int *inputPixel_Position, int *Peak_Intensity, int *Peak_Position)
{
    int i = 0, prev = 0;
    char direction = 'u';

    // Start by assuming we're increasing to allow first element peak.
    prev = FilterPixel_Intensity[0];
    direction = 'u';

    for (i = 1; i < MAX_LENGTH - 2; i++)
    {
        // If switching from rising to falling, it is a peak.
        if ((FilterPixel_Intensity[i] < prev) && direction == 'u')
        {
            *(Peak_Position + i - 1) = *(inputPixel_Position + i - 1);
            *(Peak_Intensity + i - 1) = prev;
            direction = 'd';
        }
        // If switching from falling to rising, that is a trough. so Prepare to detect next peak.
        if ((FilterPixel_Intensity[i] > prev) && direction == 'd')
        {
            Trough_Position[i - 1] = *(inputPixel_Position + i - 1); // not used, just for reference
            Trough_Intensity[i - 1] = prev;                          // not used, just for reference
            direction = 'u';
        }
        // Store previous to detect peak / trough.
        prev = FilterPixel_Intensity[i];
        // Note: we are not considering last signal as a peak (we should consider ,if it`s rising )and can be implemented for future work
    }   
}
//********************************************************************************************

int main()
{
    printf("Have fun with the challenge!\n");
    int inputPixel_Position[300] = {0};
    int inputPixel_Intensity[300] = {0};
    int Peak_Position[300] = {0};
    int Peak_Intensity[300] = {0};

    ReadFile_and_StoreInputData(inputPixel_Intensity, inputPixel_Position);
    Apply_rolling_median_filter(inputPixel_Intensity); // step 1: Background noise removal using a rolling median filter
    Peak_Detection(inputPixel_Position, Peak_Intensity, Peak_Position);

    return 0;
}
