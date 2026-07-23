#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <ctype.h>

#define MAX_USERS 10
#define MAX_USERNAME 20
#define MAX_PASSWORD 20

#define MAX_FILES 100
#define MAX_FILENAME 50
#define MAX_CONTENT 500

#define PERM_READ    4
#define PERM_WRITE   2
#define PERM_EXECUTE 1

typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} User;

User users[MAX_USERS];
int userCount = 0;
User *currentUser = NULL;

typedef struct {
    char name[MAX_FILENAME];
    char owner[MAX_USERNAME];
    int permissions;
    char content[MAX_CONTENT];
    bool exists;
} File;

File files[MAX_FILES];
int fileCount = 0;

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

int ownerPerm(int perm)  { return (perm / 100) % 10; }
int groupPerm(int perm)  { return (perm / 10) % 10; }
int othersPerm(int perm) { return perm % 10; }

void printPermissions(int perm) {
    int owner = ownerPerm(perm);
    int group = groupPerm(perm);
    int others = othersPerm(perm);
    printf("%c%c%c", (owner & PERM_READ) ? 'r' : '-', (owner & PERM_WRITE) ? 'w' : '-', (owner & PERM_EXECUTE) ? 'x' : '-');
    printf("%c%c%c", (group & PERM_READ) ? 'r' : '-', (group & PERM_WRITE) ? 'w' : '-', (group & PERM_EXECUTE) ? 'x' : '-');
    printf("%c%c%c", (others & PERM_READ) ? 'r' : '-', (others & PERM_WRITE) ? 'w' : '-', (others & PERM_EXECUTE) ? 'x' : '-');
}

void createFile(const char *name, int permissions) {
    if (currentUser == NULL) { printf("Error: You must be logged in.\n"); return; }
    if (fileCount >= MAX_FILES) { printf("Error: Maximum files reached.\n"); return; }

    for (int i = 0; i < fileCount; i++) {
        if (files[i].exists && strcmp(files[i].name, name) == 0) {
            printf("Error: File '%s' already exists.\n", name);
            return;
        }
    }

    File *f = &files[fileCount++];
    strcpy(f->name, name);
    strcpy(f->owner, currentUser->username);
    f->permissions = permissions;
    f->content[0] = '\0';
    f->exists = true;

    printf("File '%s' created (permissions: ", name);
    printPermissions(permissions);
    printf(")\n");
}

void printHelp(void) {
    printf("\nCommands:\n");
    printf("  login\n");
    printf("  logout\n");
    printf("  create <filename> <permissions>   e.g. create notes.txt 755\n");
    printf("  help\n");
    printf("  exit\n");

    printf("\nPermissions (3 digits, owner/others):\n");
    printf("  4 = read (r), 2 = write (w), 1 = execute (x)\n");
    printf("  755 = rwxr-xr-x (owner: all, others: read+execute)\n");
    printf("  644 = rw-r--r-- (owner: read+write, others: read)\n");
    printf("  700 = rwx------ (only owner can access)\n");
    printf("  Default if omitted: 754\n");
}

int main(void) {
    char line[500];
    char command[100], arg1[100], arg2[500];

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

        arg1[0] = '\0';
        arg2[0] = '\0';
        token = strtok(NULL, " ");
        if (token != NULL) {
            strcpy(arg1, token);
            char *rest = strtok(NULL, "");
            if (rest != NULL) strcpy(arg2, rest);
        }

        if (strcmp(command, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        } else if (strcmp(command, "help") == 0) {
            printHelp();
        } else if (strcmp(command, "create") == 0) {
            if (strlen(arg1) == 0) { printf("Usage: create <filename> <permissions>\n"); continue; }

            int perms = 754; // NOT 0754 - a leading 0 in C means octal, which would break our decimal-digit permission scheme

            if (strlen(arg2) == 3 && isdigit((unsigned char)arg2[0]) &&
                isdigit((unsigned char)arg2[1]) && isdigit((unsigned char)arg2[2])) {
                perms = (arg2[0] - '0') * 100 + (arg2[1] - '0') * 10 + (arg2[2] - '0');
            }
            createFile(arg1, perms);
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
