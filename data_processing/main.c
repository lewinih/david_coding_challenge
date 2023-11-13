#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const unsigned MAX_LENGTH = 300;
int FilterPixel_Intensity[300] = {0};
int Trough_Position[300] = {0};
int Trough_Intensity[300] = {0};
int Peak_Width[300] = {0};
bool Grating_Binary_Pos[300];

void ReadFile_and_StoreInputData(int inputPixel_Intensity[], int inputPixel_Position[])
{

    FILE *Fin = NULL;
    Fin = fopen("data.txt", "r");
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
            Temp = A;  A = B; B = Temp; // swap A and B
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
        // printf("filter_intensity = %d\n", *(F_P + i));
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
void find_Position_and_half_max_width(int inputPixel_Position[], int Peak_Intensity[])
{
    // Note: there are many ways to find out half-max-width reference to https://de.mathworks.com/help/signal/ref/findpeaks.html and https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.peak_widths.html
    // here I have measured the widths, using the half height as reference. if this does not seem proper for your application let me know, I have 2 other method ((1)we can find width using the half prominence as reference (2)find widths at the relative height of 0.5 (contour line at half the prominence height) and 1 (at the lowest contour line at full prominence height).
    int i = 0, value = 0, j = 0, index1 = 0, index2 = 0;
    index1 = inputPixel_Position[0];
    for (i = 0; i < MAX_LENGTH - 2; i++)
    {
        if (Peak_Intensity[i] > 0) // for every peak we need to find out width
        {
            value = (Peak_Intensity[i]) / 2;                             //
            for (j = i - 1; ((Peak_Intensity[j] == 0) && (j != 0)); j--) // peak left falling edge
            {
                if (FilterPixel_Intensity[j] < value) // if signal cross the value
                {
                    index1 = *(inputPixel_Position + j + 1); // Inten1 = FilterPixel_Intensity[j + 1];
                    break;
                }
                else if (FilterPixel_Intensity[j] < FilterPixel_Intensity[j - 1]) // if trough arrives
                {
                    index1 = *(inputPixel_Position + j); //   Inten1 = FilterPixel_Intensity[j];
                    break;
                }
            }
            for (j = i + 1; ((Peak_Intensity[j] == 0)); j++) // peak right falling edge
            {
                if (FilterPixel_Intensity[j] < value) // if signal cross the value
                {
                    index2 = *(inputPixel_Position + j - 1); // Inten2 = FilterPixel_Intensity[j - 1];
                    break;
                }
                else if (FilterPixel_Intensity[j] < FilterPixel_Intensity[j + 1]) // if trough arrives
                {
                    index2 = *(inputPixel_Position + j); //   Inten2 = FilterPixel_Intensity[j];
                    break;
                }
            }
            *(Peak_Width + i) = index2 - index1; // difference beetween both index position
        }
    }
}
//**************************************************************************************************+
void Grating_binary(int *Peak_Position)
{
    int i = 0, A = 0, B = 0, C = 0, no = 0, j = 0;
    int Temp_Array[300] = {0};
    bool Temp_Binary[300] = {0};

    // remove 0 from Peak_Position array to easy calculation
    for (i = 0; i < MAX_LENGTH; i++)
    {
        if (Peak_Position[i] > 0)
        {
            Temp_Array[no] = Peak_Position[i];
            no++;
        }
    }
    for (i = 0; i < no; i++)
    {
        A = Temp_Array[i];
        B = Temp_Array[i + 1];
        C = Temp_Array[i + 2];
        if ((A - B) == (B - C))
        {
            Temp_Binary[i] = 1;
            Temp_Binary[i + 2] = 1;
        }
        else
        {
            Temp_Binary[i + 2] = 0;
        }
    }
    for (i = 0; i < MAX_LENGTH; i++)
    {
        if (Peak_Position[i] > 0)
        {
            *(Grating_Binary_Pos + i) = *(Temp_Binary + j);
            j++;
        }
    }
}
//*****************************************************************************+
void GNUplot_plotting(int *inputPixel_Intensity, int *inputPixel_Position, int *Peak_Intensity)
{
    int i;
    char *commandsForGnuplot[] = {"set title \"Interherence\"", "plot 'data.temp'  with lines title 'Input Intensity'"};
    //{"set title \"Interherence\"", "plot 'data.temp'  with lines title 'Input Intensity' using 1:2 index 0 \ 'data.temp' with linespoints title 'Series B' using 1:2 index 1 "}; //\ 'data.temp' with linespoints title 'Series B',   using 1:2 with lines, with linespoints,  using 3:0

    FILE *fp = fopen("data.temp", "w");
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    fprintf(gnuplotPipe, "set term qt persist\n");
    for (i = 0; i < MAX_LENGTH - 2; i++)
    {
        fprintf(fp, "%d %d \n", *(inputPixel_Position + i), *(inputPixel_Intensity + i)); // Write the data to a temporary file
    }
    fprintf(fp, " \n");
    for (i = 0; i < MAX_LENGTH - 2; i++)
    {
        fprintf(fp, "%d %d \n", *(inputPixel_Position + i), *(Peak_Intensity + i)); // Write the data to a temporary file
    }
    for (i = 0; i < 2; i++)
    {
        fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); // Send commands to gnuplot one by one.
    }
    // fflush(gnuplotPipe);
     fclose(fp);
    return;
}

//**********************************************************************************+
int main()
{
    printf("Have fun with the challenge!\n");
    int inputPixel_Position[300] = {0};
    int inputPixel_Intensity[300] = {0};
    int Peak_Position[300] = {0};
    int Peak_Intensity[300] = {0};

    ReadFile_and_StoreInputData(inputPixel_Intensity, inputPixel_Position);
    Apply_rolling_median_filter(inputPixel_Intensity);                     // step 1: Background noise removal using a rolling median filter
    Peak_Detection(inputPixel_Position, Peak_Intensity, Peak_Position);    // step 2 : peak detection
    find_Position_and_half_max_width(inputPixel_Position, Peak_Intensity); // step 3: width
    Grating_binary(Peak_Position);                                         // step 4: binary grating
    GNUplot_plotting(inputPixel_Intensity, inputPixel_Position, Peak_Intensity); //plotting using gnuplot

    printf("Input_Ins   |  Input_Pos  |  Filter_Ins  |  Peak_In    |  Peak_Pos    |  Peak_Width  | Grating_Bin \n");
    int i = 0;
    for (i = 0; i < MAX_LENGTH; i++)
    {
        printf("  %5d\t|\t%6d\t|\t%5d\t|\t%4d\t|\t%6d\t|\t%6d\t|\t%d\n", *(inputPixel_Intensity + i), *(inputPixel_Position + i), *(FilterPixel_Intensity + i), *(Peak_Intensity + i), *(Peak_Position + i), *(Peak_Width + i), *(Grating_Binary_Pos + i));
    }
    return 0;
}
