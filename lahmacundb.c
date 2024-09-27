#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <pthread.h> 

#define MAX_TABLES 100
#define MAX_COLUMNS 10
#define MAX_ROWS 10000
#define MAX_COLUMN_NAME 64
#define MAX_VALUE_SIZE 256

typedef struct {
    char name[MAX_COLUMN_NAME];
    char type[MAX_COLUMN_NAME];
} Column;


typedef struct {
    char **data;
    size_t row_count;
    size_t column_count;
    Column columns[MAX_COLUMNS];
    char name[MAX_COLUMN_NAME]
    pthread_mutex_t lock;
} Table;


typedef struct {
    Table *tables[MAX_TABLES];
    size_t table_count;
    pthread_mutex_t lock;
} Database;


Database *createDatabase() {
    Database *db = malloc(sizeof(Database));
    db->table_count = 0;
    pthread_mutex_init(&db->lock, NULL);
    return db;
}

void createTable(Database *db, const char *table_name, Column columns[], size_t column_count) {
    pthread_mutex_lock(&db->lock);

    



}