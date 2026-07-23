#include <stdio.h>
#include <string.h>

#define MAX_USERS 10
#define MAX_USERNAME 20
#define MAX_PASSWORD 20

typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} User;

User users[MAX_USERS];
int userCount = 0;
User *currentUser = NULL;

void initializeUsers(void) {
    strcpy(users[0].username, "admin");
    strcpy(users[0].password, "admin123");
    strcpy(users[1].username, "user");
    strcpy(users[1].password, "user123");
    userCount = 2;
}

User *findUser(const char *username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) return &users[i];
    }
    return NULL;
}

void login(void) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    User *u = findUser(username);
    if (u == NULL || strcmp(u->password, password) != 0) {
        printf("Error: Incorrect username or password.\n");
        return;
    }

    currentUser = u;
    printf("Login successful. Welcome, %s.\n", username);
}

void logout(void) {
    if (currentUser == NULL) { printf("You are not logged in.\n"); return; }
    printf("Goodbye, %s.\n", currentUser->username);
    currentUser = NULL;
}

int main(void) {
    char line[500];
    char command[100];

    printf(" \n");
    printf("Task 3 - File System & Security\n");
    printf(" \n");
    printf("Default accounts: admin/admin123, user/user123\n");
    printf("Type 'help' for commands.\n");

    initializeUsers();

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
            printf("  login\n");
            printf("  logout\n");
            printf("  help\n");
            printf("  exit\n");
        } else if (strcmp(command, "login") == 0) {
            login();
        } else if (strcmp(command, "logout") == 0) {
            logout();
        } else { 

            printf("Unknown command. Type 'help' for commands.\n");
        }
    }

    return 0;
}
