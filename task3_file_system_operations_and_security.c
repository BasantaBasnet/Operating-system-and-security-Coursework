#include <stdio.h>
#include <string.h>

int main(void) {
    char line[500];
    char command[100];

    printf(" \n");
    printf("Task 3 - File System & Security\n");
    printf(" \n");
    printf("Type 'help' for commands.\n");

    while (1) {
        printf("\n> ");
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, " ");
        if (token == NULL) continue;
        strcpy(command, token);

        if (strcmp(command, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        } else if (strcmp(command, "help") == 0) {
            printf("\nCommands:\n");
            printf("  help\n");
            printf("  exit\n");
        } else {
            printf("Unknown command. Type 'help' for commands.\n");
        }
    }

    return 0;
}
