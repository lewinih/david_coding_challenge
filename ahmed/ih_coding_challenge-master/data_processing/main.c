#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <curl/curl.h>

#define MAX_DATA_SIZE 10000
#define someHighVarianceThreshold 100 // threshold
#define someLowVarianceThreshold 20   // threshold
#define minWindowSize 10               // Minimum window size
#define maxWindowSize 200              // Maximum window size


typedef struct {
    int position;
    int height;
    int fwhmLeft;
    int fwhmRight;
} Peak;
int compare(const void * a, const void * b);
int median(int arr[], int n);
int* rollingMedianFilter(int data[], int dataSize, int windowSize);
double calculateLocalVariance(const int data[], int dataSize, int position, int windowSize);
int characterizePeaksDynamic(const int data[], const int position[], int dataSize, Peak *peaks, int *numPeaks);
void plotDataWithGnuplot(const char* dataFilename);



// Function to compare two integers - used in qsort
int compare(const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}

// Function to calculate the median of an array
int median(int arr[], int n) {
    qsort(arr, n, sizeof(int), compare); // Sort the array using qsort algo.
    if (n % 2 == 0) {
        // If even number of elements, return the average of the two middle elements
        return (arr[n/2 - 1] + arr[n/2]) / 2;
    } else {
        // If odd, return the middle element
        return arr[n/2];
    }
}

// Apply rolling median filter
int* rollingMedianFilter(int data[], int dataSize, int windowSize) {
    int *temp = malloc(windowSize * sizeof(int));
    int *filteredData = malloc(dataSize * sizeof(int));
    for (int i = 0; i < dataSize; i++) {
        int tempSize = 0;
        for (int j = -windowSize / 2; j <= windowSize / 2; j++) {
            if (i + j >= 0 && i + j < dataSize) {
                temp[tempSize++] = data[i + j];
            }
        }
        filteredData[i] = median(temp, tempSize);
    }

    free(temp);
    return filteredData;
}

// Function to calculate the local variance of the data
double calculateLocalVariance(const int data[], int dataSize, int position, int defaultWindowSize) {
    int windowSize = defaultWindowSize;
    double sum = 0.0, mean, variance = 0.0;

    // Define boundaries of the local window
    int start = position - windowSize / 2;
    int end = position + windowSize / 2;
    if (start < 0) start = 0;
    if (end >= dataSize) end = dataSize - 1;

    int localWindowSize = end - start + 1;

    // Calculate mean
    for (int i = start; i <= end; i++) {
        sum += data[i];
    }
    mean = sum / localWindowSize;

    // Calculate variance
    for (int i = start; i <= end; i++) {
        variance += pow(data[i] - mean, 2);
    }
    variance /= localWindowSize;

    // Adjust window size based on variance (example logic)
    if (variance > someHighVarianceThreshold) {
        windowSize /= 2; // Reduce window size in noisy data
    } else if (variance < someLowVarianceThreshold) {
        windowSize *= 2; // Increase window size in stable data
    }

    // Ensure window size is within reasonable bounds
    if (windowSize < minWindowSize) windowSize = minWindowSize;
    if (windowSize > maxWindowSize) windowSize = maxWindowSize;

    return windowSize;
}


// Function to characterize peaks and peak detection statically
int characterizePeaks(const int data[], const int position[], int dataSize, int peakDetectionWindow, Peak *peaks, int *numPeaks) {
    printf("\nPeak Characterization:\n");
    printf("%-12s %-12s %-12s %-12s\n", "Position", "Height", "FWHM-Left", "FWHM-Right");
    printf("----------------------------------------------\n");
    *numPeaks = 0; // Initialize peak count

    for (int i = peakDetectionWindow; i < dataSize - peakDetectionWindow; i++) {
        int isPeak = 1;
        for (int j = -peakDetectionWindow; j <= peakDetectionWindow; j++) {
            if (j != 0 && data[i] <= data[i + j]) {
                isPeak = 0;
                break;
            }
        }
        if (isPeak) {
            int peakHeight = data[i];
            int halfMax = peakHeight / 2;
            peaks[*numPeaks].position = position[i];
            peaks[*numPeaks].height = peakHeight;

            // Initialize FWHM bounds to peak position
            int fwhmLeft = position[i];
            int fwhmRight = position[i];

            // Find left bound of FWHM
            for (int j = i; j >= 0 && data[j] > halfMax; j--) {
                fwhmLeft = position[j];
            }

            // Find right bound of FWHM
            for (int j = i; j < dataSize && data[j] > halfMax; j++) {
                fwhmRight = position[j];
            }

            peaks[*numPeaks].fwhmLeft = fwhmLeft;
            peaks[*numPeaks].fwhmRight = fwhmRight;
            (*numPeaks)++;

            // Print peak data
            printf("%-12d %-12d %-12d %-12d\n", position[i], peakHeight, fwhmLeft, fwhmRight);
        }
    }
    return 0;
}


// Function to characterize peaks with dynamic peak detection
int characterizePeaksDynamic(const int data[], const int position[], int dataSize, Peak *peaks, int *numPeaks) {
    printf("\nDynamic Peak Characterization:\n");
    printf("%-12s %-12s %-12s %-12s\n", "Position", "Height", "FWHM-Left", "FWHM-Right");
    printf("----------------------------------------------\n");
    *numPeaks = 0; // Initialize peak count

    for (int i = 0; i < dataSize; i++) {
        int dynamicWindow = calculateLocalVariance(data, dataSize, i, /* default window size */ 100);
        int isPeak = 1;
        for (int j = -dynamicWindow; j <= dynamicWindow; j++) {
            if (j != 0 && data[i] <= data[i + j]) {
                isPeak = 0;
                break;
            }
        }
        if (isPeak) {
            int peakHeight = data[i];
            int halfMax = peakHeight / 2;
            peaks[*numPeaks].position = position[i];
            peaks[*numPeaks].height = peakHeight;

            // Initialize FWHM bounds to peak position
            int fwhmLeft = position[i];
            int fwhmRight = position[i];

            // Find left bound of FWHM
            for (int j = i; j >= 0 && data[j] > halfMax; j--) {
                fwhmLeft = position[j];
            }

            // Find right bound of FWHM
            for (int j = i; j < dataSize && data[j] > halfMax; j++) {
                fwhmRight = position[j];
            }

            peaks[*numPeaks].fwhmLeft = fwhmLeft;
            peaks[*numPeaks].fwhmRight = fwhmRight;
            (*numPeaks)++;

            // Print peak data
            printf("%-12d %-12d %-12d %-12d\n", position[i], peakHeight, fwhmLeft, fwhmRight);
        }
    }
    return 0;
}

void plotDataWithGnuplot(const char* dataFilename) {
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Error opening pipe to Gnuplot.\n");
        exit(EXIT_FAILURE);
    }

    // Gnuplot commands
    fprintf(gnuplotPipe, "set title 'Peak Detection'\n");
    fprintf(gnuplotPipe, "set xlabel 'Position'\n");
    fprintf(gnuplotPipe, "set ylabel 'Intensity'\n");
    fprintf(gnuplotPipe, "plot '%s' using 1:2 with lines title 'Original Data', '%s' using 1:3 with lines title 'Filtered Data'\n", dataFilename, dataFilename);

    pclose(gnuplotPipe);
}




// Function to generate binary positional data from peaks
void generateBinaryPositionalDataFromPeaks(const Peak *peaks, int numPeaks, const int *position, int dataSize) {
    int *binaryData = malloc(dataSize * sizeof(int));
    if (!binaryData) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < dataSize; i++) {
        binaryData[i] = 0; // Initialize to 0 (absence)

        for (int j = 0; j < numPeaks; j++) {
            if (position[i] >= peaks[j].fwhmLeft && position[i] <= peaks[j].fwhmRight) {
                binaryData[i] = 1; // Set to 1 (presence) if within any peak range
                break;
            }
        }
    }

    printf("\nBinary Positional Data:\n");
    for (int i = 0; i < dataSize; i++) {
        printf("%d ", binaryData[i]);
    }
    printf("\n");

    free(binaryData);
}

void exportDataForPlotting(const int *originalData, const int *filteredData, const int *position, int dataSize, const char* filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for export");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Position,Original_Intensity,Filtered_Intensity\n");
    for (int i = 0; i < dataSize; i++) {
        fprintf(file, "%d,%d,%d\n", position[i], originalData[i], filteredData[i]);
    }

    fclose(file);
    printf("Data exported to %s\n", filename);
}


// creating the json file for server plotting
char* create_json(const int *x_data, const int *intensity_data, int dataSize) {
    // Estimateed buffer size
    int estimatedSize = dataSize * 40; 
    char *json = malloc(estimatedSize * sizeof(char));
    if (json == NULL) {
        perror("Failed to allocate memory for JSON data");
        exit(EXIT_FAILURE);
    }

    strcpy(json, "{\"title\":\"MyDataPlot\",\"x_data\":[");
    for (int i = 0; i < dataSize; i++) {
        char buffer[20]; // Increase buffer size for safety
        snprintf(buffer, sizeof(buffer), "%d", x_data[i]);
        strncat(json, buffer, estimatedSize - strlen(json) - 1);
        if (i < dataSize - 1) {
            strncat(json, ",", estimatedSize - strlen(json) - 1);
        }
    }
    strncat(json, "],\"intensity_data\":[", estimatedSize - strlen(json) - 1);
    for (int i = 0; i < dataSize; i++) {
        char buffer[20]; 
        snprintf(buffer, sizeof(buffer), "%d", intensity_data[i]);
        strncat(json, buffer, estimatedSize - strlen(json) - 1);
        if (i < dataSize - 1) {
            strncat(json, ",", estimatedSize - strlen(json) - 1);
        }
    }
    strncat(json, "]}", estimatedSize - strlen(json) - 1);

    return json;
}

//secding the data to the server
void send_data_to_server(const char *json_data, const char *url) {
    CURL *curl;
    CURLcode res;


    // Initialize a libcurl handle
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(json_data)); // Explicitly set the POST field size
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // Enable verbose for debug


        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else {
            // If the request was successful, print out the response code
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            printf("Success: Server responded with code %ld\n", response_code);
        }
        // cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}


int main() {
    FILE *file;
    char buffer[MAX_DATA_SIZE];
    int intensity[MAX_DATA_SIZE / 2]; // Half the size, as data is split between intensity and position
    int position[MAX_DATA_SIZE / 2];
    int index = 0;

    // Open the data file
    file = fopen("data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    
    // Read the data
    if (fgets(buffer, MAX_DATA_SIZE, file) != NULL) {
        // Parse the data
        char *token = strtok(buffer, ",");
        token = strtok(NULL, ","); // Skip "Intensity" header
        token = strtok(NULL, ","); // Skip "Position" header
        while (token != NULL) {
            if (index % 2 == 0) {
                // Even index: Intensity
                intensity[index / 2] = atoi(token);
            } else {
                // Odd index: Position
                position[index / 2] = atoi(token);
            }
            index++;
            token = strtok(NULL, ",");
        }
    }

    fclose(file);

    // Calculate data size (half of index because data is split between intensity and position)
    int dataSize = index / 2;
    int windowSize = 230; 
    // the data has one spike it might be real value or noise, the larger the windowsize the higher tendency to consider it as outlier (might be a significant feature rather than noise; I consider it as noise)

    // Apply the rolling median filter and capture the filtered data
    int* filteredData = rollingMedianFilter(intensity, dataSize, windowSize);

    printf("%-12s %-12s %-12s\n", "Original Int.", "Filtered Int.", "Position");
    printf("-------------------------------------------------\n");
    // Print the data in a table format
    for (int i = 0; i < dataSize; i++) {
        printf("%-12d %-12d %-12d\n", intensity[i], filteredData[i], position[i]);
    }

    // Save the filtered data to a new file
    FILE *outputFile = fopen("filtered_data.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        return 1;
    }

    for (int i = 0; i < dataSize; i++) {
        fprintf(outputFile, "%d,", filteredData[i]);
    }

    // Close the output file
    fclose(outputFile);

    

    // Allocate memory for peaks array
    int maxNumPeaks = 50; // assuming number of peaks
    Peak *peaks = malloc(maxNumPeaks * sizeof(Peak));
    if (!peaks) {
        perror("Failed to allocate memory for peaks");
        exit(EXIT_FAILURE);
    }

    int numPeaks = 0;

    int peakDetectionWindow = 50; // value represents the number of data points on either side of a given point that will be considered when determining if the point is a peak
    
    //static detection
    characterizePeaks(filteredData, position, dataSize, peakDetectionWindow, peaks, &numPeaks);
    //dynamic detection
    characterizePeaksDynamic(filteredData, position, dataSize, peaks, &numPeaks);

    // After generating binary positional data
    generateBinaryPositionalDataFromPeaks(peaks, numPeaks, position, dataSize);

    free(peaks);

    //plotting in a seperate file, results are in plot_data.ipynb
    // After filtering the data
    exportDataForPlotting(intensity, filteredData, position, dataSize, "plot_data.csv");
    
    // Send data to server
    char *json_data = create_json(position, filteredData, dataSize);
    send_data_to_server(json_data, "http://127.0.0.1:5000/receive_data");

    free(json_data);
    
    //free(filteredData);
    
    return 0;
}
