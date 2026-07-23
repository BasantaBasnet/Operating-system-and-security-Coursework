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

// Note: enforcement uses two tiers (owner vs everyone else) for
// simplicity - the group digit is displayed but not separately
// enforced, since there's no group-membership system here.
bool hasPermission(File *file, User *user, int required) {
    if (user == NULL) return false;

    if (strcmp(user->username, file->owner) == 0) {
        return (ownerPerm(file->permissions) & required) == required;
    }
    return (othersPerm(file->permissions) & required) == required;
}

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

File *findFile(const char *name) {
    for (int i = 0; i < fileCount; i++) {
        if (files[i].exists && strcmp(files[i].name, name) == 0) return &files[i];
    }
    return NULL;
}

void readFile(const char *name) {
    if (currentUser == NULL) { printf("Error: You must be logged in.\n"); return; }

    File *f = findFile(name);
    if (f == NULL) { printf("Error: File '%s' not found.\n", name); return; }

    if (!hasPermission(f, currentUser, PERM_READ)) {
        printf("Error: Permission denied (read).\n");
        return;
    }

    printf("\n  %s \n", f->name);
    printf("Owner: %s | Permissions: ", f->owner);
    printPermissions(f->permissions);
    printf("\nContent: %s\n", f->content);
}

void writeFile(const char *name, const char *content) {
    if (currentUser == NULL) { printf("Error: You must be logged in.\n"); return; }

    File *f = findFile(name);
    if (f == NULL) { printf("Error: File '%s' not found.\n", name); return; }

    if (!hasPermission(f, currentUser, PERM_WRITE)) {
        printf("Error: Permission denied (write).\n");
        return;
    }

    strncpy(f->content, content, MAX_CONTENT - 1);
    f->content[MAX_CONTENT - 1] = '\0';
    printf("File '%s' updated.\n", name);
}
void deleteFile(const char *name) {
    if (currentUser == NULL) { printf("Error: You must be logged in.\n"); return; }

    File *f = findFile(name);
    if (f == NULL) { printf("Error: File '%s' not found.\n", name); return; }

    if (strcmp(currentUser->username, f->owner) != 0) {
        printf("Error: Only the owner can delete this file.\n");
        return;
    }

    f->exists = false;
    printf("File '%s' deleted.\n", name);
}

void listFiles(void) {
    if (currentUser == NULL) { printf("Error: You must be logged in.\n"); return; }

    printf("\n%-20s %-10s %-12s %s\n", "Name", "Owner", "Permissions", "Octal");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < fileCount; i++) {
        if (files[i].exists) {
            printf("%-20s %-10s ", files[i].name, files[i].owner);
            printPermissions(files[i].permissions);
            printf("    %03d\n", files[i].permissions);
        }
    }
}

void printHelp(void) {
    printf("\nCommands:\n");
    printf("  login\n");
    printf("  logout\n");
    printf("  create <filename> <permissions>   e.g. create notes.txt 755\n");
    printf("  read <filename>\n");
    printf("  write <filename> <content>\n");
    printf("  delete <filename>\n");
    printf("  list\n");
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
        } else if (strcmp(command, "read") == 0) {
            if (strlen(arg1) == 0) { printf("Usage: read <filename>\n"); continue; }

            readFile(arg1);
        } else if (strcmp(command, "write") == 0) {
            if (strlen(arg1) == 0 || strlen(arg2) == 0) { printf("Usage: write <filename> <content>\n"); continue; }

            writeFile(arg1, arg2);
        } else if (strcmp(command, "delete") == 0) {
            if (strlen(arg1) == 0) { printf("Usage: delete <filename>\n"); continue; }
            deleteFile(arg1);
        } else if (strcmp(command, "list") == 0) {
            listFiles();
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
