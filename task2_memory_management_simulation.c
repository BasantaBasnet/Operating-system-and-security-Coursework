#include <stdio.h>

#define MAX_PAGES 50
#define EMPTY -1

typedef struct {
    int pageFaults;
    int hits;
    int totalReferences;
} Result;

// Fill all frames with EMPTY (no page loaded yet)
void initializeFrames(int frames[], int frameCount) {
    for (int i = 0; i < frameCount; i++) {
        frames[i] = EMPTY;
    }
}

// Show the current state of memory frames
void displayFrames(int frames[], int frameCount) {
    printf("Frames: ");
    for (int i = 0; i < frameCount; i++) {
        if (frames[i] == EMPTY) printf("[ ] ");
        else printf("[%d] ", frames[i]);
    }
    printf("\n");
}

// Check whether a page is already loaded in memory
int pageExists(int frames[], int frameCount, int page) {
    for (int i = 0; i < frameCount; i++) {
        if (frames[i] == page) return 1;
    }
    return 0;
}
void printReferenceString(int pages[], int totalPages) {
    printf("Reference String: ");
    for (int i = 0; i < totalPages; i++) {
        printf("%d ", pages[i]);
    }
    printf("\n");
}

Result runFIFO(int pages[], int totalPages, int frameCount) {
    int frames[frameCount];
    int nextReplace = 0;
    Result result = {0, 0, totalPages};

    initializeFrames(frames, frameCount);

    for (int i = 0; i < totalPages; i++) {
        int page = pages[i];

        if (pageExists(frames, frameCount, page)) {
            result.hits++;
            printf("Ref %2d: page %d -> HIT   ", i + 1, page);
        } else {
            result.pageFaults++;
            frames[nextReplace] = page;
            nextReplace = (nextReplace + 1) % frameCount;
            printf("Ref %2d: page %d -> FAULT ", i + 1, page);
        }

        displayFrames(frames, frameCount);
    }

    return result;
}

// LRU: replaces whichever page hasn't been used for the longest time
Result runLRU(int pages[], int totalPages, int frameCount) {
    int frames[frameCount];
    int lastUsed[frameCount];
    Result result = {0, 0, totalPages};
    int clock_time = 0;

    initializeFrames(frames, frameCount);
    for (int i = 0; i < frameCount; i++) lastUsed[i] = -1;

    for (int i = 0; i < totalPages; i++) {
        int page = pages[i];
        clock_time++;

        if (pageExists(frames, frameCount, page)) {
            result.hits++;
            printf("Ref %2d: page %d -> HIT   ", i + 1, page);

            for (int j = 0; j < frameCount; j++) {
                if (frames[j] == page) {
                    lastUsed[j] = clock_time;
                    break;
                }
            }
        } else {
            result.pageFaults++;
            printf("Ref %2d: page %d -> FAULT ", i + 1, page);

            int lruIndex = 0;
            int oldestTime = clock_time;
            for (int j = 0; j < frameCount; j++) {
                if (frames[j] == EMPTY) { lruIndex = j; break; }
                if (lastUsed[j] < oldestTime) {
                    oldestTime = lastUsed[j];
                    lruIndex = j;
                }
            }

            frames[lruIndex] = page;
            lastUsed[lruIndex] = clock_time;
        }

        displayFrames(frames, frameCount);
    }

    return  result;
}

void printStatistics(Result result, const char *algorithm) {
    float hitRatio = (float)result.hits / result.totalReferences * 100;
    float missRatio = (float)result.pageFaults / result.totalReferences * 100;

    printf("\n%s Results:\n", algorithm);
    printf("  Page Faults : %d\n", result.pageFaults);
    printf("  Hits        : %d\n", result.hits);
    printf("  Hit Ratio   : %.2f%%\n", hitRatio);
    printf("  Miss Ratio  : %.2f%%\n", missRatio);
}

void compareAlgorithms(Result fifo, Result lru) {
    float fifoHit = (float)fifo.hits / fifo.totalReferences * 100;
    float lruHit  = (float)lru.hits  / lru.totalReferences  * 100;

    printf("\nAlgorithm     Faults   Hits   Hit Ratio\n");
    printf("\n");
    printf("FIFO          %5d   %4d    %6.2f%%\n", fifo.pageFaults, fifo.hits, fifoHit);
    printf("LRU           %5d   %4d    %6.2f%%\n", lru.pageFaults, lru.hits, lruHit);
    printf("\n");

    if (fifo.pageFaults < lru.pageFaults) {
        printf("FIFO performed better by %d fewer faults.\n", lru.pageFaults - fifo.pageFaults);
    } else if (lru.pageFaults < fifo.pageFaults) {
        printf("LRU performed better by %d fewer faults.\n", fifo.pageFaults - lru.pageFaults);
    } else {
        printf("Both algorithms performed the same.\n");
    }
}

int main(void) {
    int pageSizeKB;
    int frameCount;

    printf("\n");
    printf("Task 2 - Memory Management Simulation\n");
    printf("\n");

    printf("Enter page size (KB): ");
    scanf("%d", &pageSizeKB);

    printf("Enter number of memory frames: ");
    scanf("%d", &frameCount);

    if (pageSizeKB <= 0 || frameCount <= 0) {
        printf("Invalid input.\n");
        return 1;
    }
    printf("Enter number of page references: ");
    int totalPages;
    scanf("%d", &totalPages);

    if (totalPages <= 0 || totalPages > MAX_PAGES) {
        printf("Invalid input.\n");
        return 1;
    }

    int pages[MAX_PAGES];
    printf("Enter the page reference string (space separated):\n");
    for (int i = 0; i < totalPages; i++) {
        scanf("%d", &pages[i]);
    }

    printf("Page Size: %d KB | Frames: %d | Simulated Capacity: %d KB\n",
           pageSizeKB, frameCount, pageSizeKB * frameCount);
    printReferenceString(pages, totalPages);

    printf("\n-- FIFO Simulation --\n");
    Result fifoResult = runFIFO(pages, totalPages, frameCount);

    printf("\n-- LRU Simulation --\n");
    Result lruResult = runLRU(pages, totalPages, frameCount);

    printStatistics(fifoResult, "FIFO");
    printStatistics(lruResult, "LRU");
    compareAlgorithms(fifoResult, lruResult);

    return 0;
}
