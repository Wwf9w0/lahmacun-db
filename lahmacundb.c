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
    char ***data;                     
    size_t row_count;               
    size_t column_count;           
    Column columns[MAX_COLUMNS];    
    char name[MAX_COLUMN_NAME];    
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
    if (db->table_count >= MAX_TABLES) {  
        printf("Max table size error.\n");
        pthread_mutex_unlock(&db->lock);  
        return;
    }

    Table *table = malloc(sizeof(Table));      
    strncpy(table->name, table_name, MAX_COLUMN_NAME);  
    table->row_count = 0;                    
    table->column_count = column_count;    
    table->data = malloc(MAX_ROWS * sizeof(char **)); 
    for (size_t i = 0; i < MAX_ROWS; i++) {
        table->data[i] = malloc(MAX_COLUMNS * sizeof(char *)); 
        for (size_t j = 0; j < MAX_COLUMNS; j++) {
            table->data[i][j] = malloc(MAX_VALUE_SIZE * sizeof(char)); 
        }
    }
    memcpy(table->columns, columns, column_count * sizeof(Column)); 
    pthread_mutex_init(&table->lock, NULL);  
    db->tables[db->table_count++] = table;  
    pthread_mutex_unlock(&db->lock);         
    printf("Created table: %s\n", table_name);  

void insertIntoTable(Database *db, const char *table_name, char *values[]) {
    pthread_mutex_lock(&db->lock); 

    for (size_t i = 0; i < db->table_count; i++) {
        Table *table = db->tables[i];
        if (strcmp(table->name, table_name) == 0) {
            if (table->row_count >= MAX_ROWS) { 
                printf("Database is full, failed to add data: %s\n", table_name);
                pthread_mutex_unlock(&db->lock);
                return;
            }

            for (size_t j = 0; j < table->column_count; j++) {
                strncpy(table->data[table->row_count][j], values[j], MAX_VALUE_SIZE);
            }
            table->row_count++;  
            printf("Data is added: %s\n", table_name);  
            pthread_mutex_unlock(&db->lock);  
            return;
        }
    }
    pthread_mutex_unlock(&db->lock);  
    printf("Table not found: %s\n", table_name);  
}

void selectFromTable(Database *db, const char *table_name) {
    pthread_mutex_lock(&db->lock); 

    for (size_t i = 0; i < db->table_count; i++) {
        Table *table = db->tables[i];
        if (strcmp(table->name, table_name) == 0) {  
            printf("Table: %s\n", table_name); 
            for (size_t j = 0; j < table->row_count; j++) { 
                for (size_t k = 0; k < table->column_count; k++) {  
                    printf("%s ", table->data[j][k]);  
                }
                printf("\n");  
            }
            pthread_mutex_unlock(&db->lock);  
            return;
        }
    }
    pthread_mutex_unlock(&db->lock); 
    printf("Table not found: %s\n", table_name);  
}

int main() {
    Database *db = createDatabase(); 

    Column columns[] = {{"id", "INT"}, {"name", "VARCHAR"}};
    
    createTable(db, "users", columns, 2);

    char *row1[] = {"1", "MJ"};
    char *row2[] = {"2", "KJ"};
    insertIntoTable(db, "users", row1);
    insertIntoTable(db, "users", row2);

    selectFromTable(db, "users");

    return 0;
}
