#include <stdio.h>

#define MAX_PAGES 50
#define EMPTY -1

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

int runFIFO(int pages[], int totalPages, int frameCount) {
    int frames[frameCount];
    int nextReplace = 0;
    int faults = 0;

    initializeFrames(frames, frameCount);

    for (int i = 0; i < totalPages; i++) {
        int page = pages[i];

        if (pageExists(frames, frameCount, page)) {
            printf("Ref %2d: page %d -> HIT   ", i + 1, page);
        } else {
            faults++;
            frames[nextReplace] = page;
            nextReplace = (nextReplace + 1) % frameCount;
            printf("Ref %2d: page %d -> FAULT ", i + 1, page);
        }

        displayFrames(frames, frameCount);
    }

    return faults;
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
    int fifoFaults = runFIFO(pages, totalPages, frameCount);
    printf("\nTotal FIFO page faults: %d\n", fifoFaults);

    return 0;
}

