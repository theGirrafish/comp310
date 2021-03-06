#define _BSD_SOURCE
#define _SVID_SOURCE
#define _XOPEN_SOURCE 500
#define _XOPEN_SORUCE 600
#define _XOPEN_SORUCE 600

#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <semaphore.h>

#define MAX_ARGS 100
#define BUFF_SIZE 20
#define BUFF_SHM "OS_BUFF_SHM_ABBAS"
#define BUFF_MUTEX_A "/OS_MUTEX_A_ABBAS"
#define BUFF_MUTEX_B "/OS_MUTEX_B_ABBAS"

// Declaring semaphores names for local usage
sem_t *mutexA;
sem_t *mutexB;

// Declaring the shared memory and base address
int shm_fd;
void *base;

// Structure for individual table
struct table {
    int num;
    char name[30];
    char status[30];
};


void initTables(struct table *base) {
    // Capture both mutexes using sem_wait
    sem_wait(mutexA);
    sem_wait(mutexB);

    // Initialise the tables with table numbers
    int sec = 0;
    for(int i = 100; i < 200; i += 90, sec += 10) {
        for (int j = 0 + sec; j < 10 + sec; j++) {
            (base+j)->num = i + j;
            strcpy((base+j)->name, "\0");
            strcpy((base+j)->status, "Free");
        }
    }

    printf("Initialization successful.\n");

    // Perform a random sleep
    sleep(rand() % 5);

    // Release the mutexes using sem_post
    sem_post(mutexA);
    sem_post(mutexB);

    return;
}


void createTableDB(struct table *base) {
    if (base->num < 100)
        initTables(base);
    return;
}


void printTableInfo(struct table *base) {
    // Capture both mutexes using sem_wait
    sem_wait(mutexA);
    sem_wait(mutexB);

    // Print the tables with table numbers and name
    for (int i = 0; i < 20; i++) {
        if (i < 10) {
            if (i == 0)
                printf("\n----- Section A -----\n");
            printf("Table %d -- %s\n", (base+i)->num, (base+i)->name);
        } else {
            if (i == 10)
                printf("\n----- Section B -----\n");
            printf("Table %d -- %s\n", (base+i)->num, (base+i)->name);
        }
    }

    // Perform a random sleep
    sleep(rand() % 5);

    // Release the mutexes using sem_post
    sem_post(mutexA);
    sem_post(mutexB);

    return;
}


void reserveSpecificTable(struct table *base, char *name_hld, char *section, int table_num) {
    int i;

    switch (section[0]) {
        case 'a':
        case 'A':
            // Capture mutex for section A
            sem_wait(mutexA);

            // Check if table number belongs to section specified
            if (table_num < 100 || table_num > 109) {
                printf("Reservation failed: Table %d does not exist for section A.\n", table_num);
                sem_post(mutexA);
                return;
            }

            // Reserve table for the name specified if free
            i = table_num - 100;
            if (strcmp((base+i)->status, "Free") == 0) {
                strcpy((base+i)->name, name_hld);
                strcpy((base+i)->status, "Reserved");
                printf("Reservation successful.\n");
                printf("Table %d A: %s\n", (base+i)->num, (base+i)->name);
            } else {
                printf("Reservation failed: Table %d A already reserved by %s.\n", table_num, (base+i)->name);
            }

            // Perform a random sleep
            sleep(rand() % 5);

            // Release mutex
            sem_post(mutexA);
            break;
        case 'b':
        case 'B':
            // Capture mutex for section B
            sem_wait(mutexB);

            // Check if table number belongs to section specified
            if (table_num < 200 || table_num > 209) {
                printf("Reservation failed: Table %d does not exist for section B.\n", table_num);
                sem_post(mutexB);
                return;
            }

            // Reserve table for the name specified if free
            i = table_num - 190;

            if (strcmp((base+i)->status, "Free") == 0) {
                strcpy((base+i)->name, name_hld);
                strcpy((base+i)->status, "Reserved");
                printf("Reservation successful.\n");
                printf("Table %d B: %s\n", (base+i)->num, (base+i)->name);
            } else {
                printf("Reservation failed: Table %d B already reserved by %s.\n", table_num, (base+i)->name);
            }

            // Perform a random sleep
            sleep(rand() % 5);

            // Release mutex
            sem_post(mutexB);
            break;
        default:
            printf("Reservation failed: Section %s does not exist.", section);
            break;
    }
    return;
}


void reserveRandomTable(struct table *base, char *name_hld, char *section) {
    switch (section[0]) {
        case 'a':
        case 'A':
            // Capture mutex for section A
            sem_wait(mutexA);

            // Look for empty table and reserve it ie copy name to that struct
            for (int i = 0; i < 10; i++) {
                if (strcmp((base+i)->status, "Free") == 0) {
                    strcpy((base+i)->name, name_hld);
                    strcpy((base+i)->status, "Reserved");

                    printf("Reservation successful.\n");
                    printf("Table %d A: %s\n", (base+i)->num, (base+i)->name);
                    sem_post(mutexA);
                    return;
                }
            }

            printf("Reservation failed: All tables are already reserved.\n");

            // Perform a random sleep
            sleep(rand() % 5);

            // Release mutex for section A
            sem_post(mutexA);
            break;
        case 'b':
        case 'B':
            // Capture mutex for section B
            sem_wait(mutexB);

            // Look for empty table and reserve it ie copy name to that struct
            for (int i = 10; i < 20; i++) {
                if (strcmp((base+i)->status, "Free") == 0) {
                    strcpy((base+i)->name, name_hld);
                    strcpy((base+i)->status, "Reserved");

                    printf("Reservation successful.\n");
                    printf("Table %d B: %s\n", (base+i)->num, (base+i)->name);
                    sem_post(mutexB);
                    return;
                }
            }

            printf("Reservation failed: All tables are already reserved.\n");

            // Perform a random sleep
            sleep(rand() % 5);

            // Release mutex for section B
            sem_post(mutexB);
            break;
        default:
            printf("Reservation failed: Section %s does not exist.", section);
            break;
    }
    return;
}


int processCmd(char *cmd, struct table *base) {
    int table_num;
    char *token;
    char *name_hld;
    char *section;
    char *table_char;
    token = strtok(cmd, " ");

    switch (token[0]) {
        case 'r':
        case 'R':
            name_hld = strtok(NULL, " ");
            section = strtok(NULL, " ");
            table_char = strtok(NULL, " ");

            if (name_hld == NULL) {
                printf("Command failed: A name is required for reservation.\n");
                return 1;
            }

            if (section == NULL) {
                printf("Command failed: A valid section is required for reservation.\n");
                return 1;
            } else if (strcmp(section, "A") != 0 && strcmp(section, "a") != 0  &&
                       strcmp(section, "B") != 0 && strcmp(section, "b") != 0) {
                printf("Command failed: Must use valid section a/A or b/B.\n");
                return 1;
            }

            if (table_char != NULL)
                reserveSpecificTable(base, name_hld, section, atoi(table_char));
            else
                reserveRandomTable(base, name_hld, section);

            break;
        case 's':
        case 'S':
            printTableInfo(base);
            break;
        case 'i':
        case 'I':
            initTables(base);
            break;
        case 'e':
        case 'E':
            return 0;
    }
    return 1;
}


// Clear all semaphores on Ctrl+C
void ctrlCHandler() {
    signal(SIGINT, ctrlCHandler);
    printf("Killed process.\n");
    sem_post(mutexA);
    sem_post(mutexB);
    sem_close(mutexA);
    sem_close(mutexB);
    fflush(stdout);
    exit(-1);
}


int main(int argc, char *argv[]) {
    int fd_stdin = STDIN_FILENO;
    int mem_size = sizeof(struct table) * BUFF_SIZE;

    signal(SIGINT, ctrlCHandler);


    char *name1 = "somefiletxt";
    char *ext = strdup(name1);
    char *file = strsep(&ext, ".");
    printf("%s, %s\n", file, ext);
    if (strlen(file) > 2)
        printf("YAY");
    if (strlen(ext) > 2)
        printf("WOO");




    // File name is specifed
    if (argc > 1) {
        // Perform stdin rewiring if input file exists
        if (access(argv[1], R_OK) == 0) {
            close(0);
            open(argv[1], O_RDONLY);
        } else {
            printf("The specified input file does not exist.\n");
            exit(-1);
        }
    }

    // Open mutex BUFF_MUTEX_A and BUFF_MUTEX_B with inital value 1 using sem_open
    mutexA = sem_open(BUFF_MUTEX_A, O_CREAT, S_IRWXU, 1);
    mutexB = sem_open(BUFF_MUTEX_B, O_CREAT, S_IRWXU, 1);

    if (mutexA == (void *)-1 || mutexB == (void *)-1) {
        printf("Semaphore failed: %s.\n", strerror(errno));
        exit(-1);
    }

    // Opening the shared memory buffer ie BUFF_SHM using shm open
    shm_fd = shm_open(BUFF_SHM, O_CREAT | O_RDWR, S_IRWXU);

    if (shm_fd == -1) {
        printf("Shared memory failed: %s.\n", strerror(errno));
        exit(-1);
    }

    // Configuring the size of the shared memory to sizeof(struct table) * BUFF_SIZE using ftruncate
    ftruncate(shm_fd, mem_size);

    // Map this shared memory to kernel space
    base = mmap(NULL , mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (base == MAP_FAILED) {
        // Close and shm_unlink
        printf("Memory map failed: %s.\n", strerror(errno));
        shm_unlink(BUFF_SHM);
        exit(-1);
    }

    // Create the table database if empty
    createTableDB(base);

    // Initializing random number generator
    time_t now;
    srand((unsigned int)(time(&now)));

    // Array in which the user command is held
    char cmd[MAX_ARGS];
    int retStatus = 1;

    while (retStatus) {
        printf("\n>> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[sizeof(cmd) - 1] = '\0';
        cmd[strcspn(cmd, "\r\n")] = 0;

        if (cmd[0] != '\0') {
            if (argc > 1)
                printf("Executing command: %s\n", cmd);

            retStatus = processCmd(cmd, base);
        }
    }

    // Close the semphores
    sem_close(mutexA);
    sem_close(mutexB);

    // Reset the standard input
    if (argc > 1) {
        close(0);
        dup(fd_stdin);
    }

    // Unmap the shared memory
    munmap(base, mem_size);
    close(shm_fd);
    return 0;
}
