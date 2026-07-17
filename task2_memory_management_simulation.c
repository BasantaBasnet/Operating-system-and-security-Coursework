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

    printf("Page Size: %d KB | Frames: %d | Simulated Capacity: %d KB\n",
           pageSizeKB, frameCount, pageSizeKB * frameCount);

    int frames[frameCount];
    initializeFrames(frames, frameCount);
    displayFrames(frames, frameCount);

    return 0;
}
