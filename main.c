#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define THREAD_NUM 5
sem_t semaphore;
pthread_mutex_t mutex;
pthread_cond_t condition_variable;

int entryCount = 0;

typedef struct dataFormat {
    int index;
    char *__name;
    char *__fileName;
    struct dataFormat *prev;
    struct dataFormat *next;
} dataFormat_t;

dataFormat_t *head;
dataFormat_t *tail;

dataFormat_t *createData()
{
    char name [32];
    char fileName [64];

    printf("Enter the name of the file owner: ");
    fgets(name, 32, stdin);
    fflush(stdin);
    printf("Enter the name of the file: ");
    fgets(fileName, 64, stdin);

    int name_len = strlen(name), fileName_len = strlen(fileName);

    dataFormat_t *new_data = (dataFormat_t *)malloc(sizeof(dataFormat_t));
    new_data->index = entryCount+1;
    new_data->__name = (char *)malloc(sizeof(name_len)+1);
    new_data->__fileName = (char *)malloc(sizeof(fileName_len)+1);
    
    strcpy(new_data->__name, name);
    strcpy(new_data->__fileName, fileName);
    
    new_data->prev = NULL;
    new_data->next = NULL;

    return new_data;
}

void *searchData(void *args) {
    sem_wait(&semaphore);
    int i = 5;
    while(i > 0) {
   dataFormat_t *temp = head;
   int option;
   printf("select the search options:\n 1) Index\t2) User Name\t3) File Name");
LABEL:   scanf("%d", &option);
    fflush(stdin);

   switch (option)
   {
   case 1:
    {
        int search_index;
        printf("Enter the index of the file: ");
        scanf("%d", &search_index);
        while ((temp->index != search_index) && (temp->next != NULL)) {
            temp = temp->next;
        }
        if (temp->next == NULL) {
            printf("No file with index %d", search_index);
            break;
        } else if (temp->index == search_index) {
            printf("data found\n");
            printf("Index\t\tName\t\t\t\t\tFile Name\n");
            printf("-----\t\t----\t\t\t\t\t---------\n");
            printf("%5d\t\t%32s\t\t\t\t\t%64s\n", temp->index, temp->__name, temp->__fileName);   
        }
    }
    break;
    case 2:
    {
        char name[32];
        printf("Enter the index of the file: ");
        fgets(name, 32, stdin);
        while ((strcmp(temp->__name, name) == 0) && (temp->next != NULL)) {
            temp = temp->next;
        }
        if (temp->next == NULL) {
            printf("No file with index %32s", name);
            break;
        } else if (strcmp(temp->__name, name) == 0) {
            printf("data found\n");
            printf("Index\t\tName\t\t\t\t\tFile Name\n");
            printf("-----\t\t----\t\t\t\t\t---------\n");
            printf("%5d\t\t%32s\t\t\t\t\t%64s\n", temp->index, temp->__name, temp->__fileName);   
        }
    }
    break;
    case 3:
    {
        char filename[32];
        printf("Enter the index of the file: ");
        fgets(filename, 32, stdin);
        while ((strcmp(temp->__name, filename) == 0) && (temp->next != NULL)) {
            temp = temp->next;
        }
        if (temp->next == NULL) {
            printf("No file with index %64s", filename);
            break;
        } else if (strcmp(temp->__name, filename) == 0) {
            printf("data found\n");
            printf("Index\t\tName\t\t\t\t\tFile Name\n");
            printf("-----\t\t----\t\t\t\t\t---------\n");
            printf("%5d\t\t%32s\t\t\t\t\t%64s\n", temp->index, temp->__name, temp->__fileName);   
        }
    }
   default:
   {
    printf("Invalid option, select again: ");
    goto LABEL;
   }
    break;
   }
   i--;
}
}

void *dataEntry(void *args)
{
    int i = 6;
    while (i > 0) {
        dataFormat_t *new_entry = createData();
        
        dataFormat_t *temp = head;
        if (temp == NULL) {
            head = new_entry;
            head->next = NULL;
            head->prev = NULL;
        } else {

            //temp = temp->next;
            while((temp->next != NULL) && (temp->index < new_entry->index)) {
                temp = temp->next;
            }

            if ((temp->next == NULL)) {
                temp->next = new_entry;
                new_entry->prev = temp;
                new_entry->next = NULL;
            } else if (new_entry->index < temp->index) {
                new_entry->next = temp;
                temp->prev = new_entry;
            }
        }
        i--;
        
    }
    sem_post(&semaphore);
    
}

int main(int argc, char *argv[])
{
    sem_init(&semaphore, 0, 0);
    int i;
    head = (dataFormat_t *)malloc(sizeof(dataFormat_t));
    tail = (dataFormat_t *)malloc(sizeof(dataFormat_t));
    head = NULL;
    tail = NULL;   

    pthread_t th[2];
    for(i = 0; i < 2; i++) {
        if (i == 0) {
            if (pthread_create(&th[i], NULL, dataEntry, NULL) != 0) {
                perror("pthread_create");
                exit(1);
            }
        } else if (i == 1) {
            if (pthread_create(&th[i], NULL, searchData, NULL) != 0) {
                perror("pthread_create");
                exit(1);
            }
        }
    }

    for (i = 0; i < 2; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }
    sem_destroy(&semaphore);
    return 0;
}