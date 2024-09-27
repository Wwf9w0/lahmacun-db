#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_TABLES 100
#define MAX_COLUMNS 10
#define MAX_ROWS 10000
#define MAX_COLUMN_NAME 64
#define MAX_VALUE_SIZE 256

// Structure for a column
typedef struct {
    char name[MAX_COLUMN_NAME]; // Column name
    char type[MAX_COLUMN_NAME]; // Column data type (e.g., INT, VARCHAR)
} Column;

// Structure for a table
typedef struct {
    char ***data;                     // 2D array to store table data
    size_t row_count;                // Number of rows in the table
    size_t column_count;             // Number of columns in the table
    Column columns[MAX_COLUMNS];     // Array of columns
    char name[MAX_COLUMN_NAME];      // Table name
    pthread_mutex_t lock;            // Mutex for thread-safety
} Table;

// Structure for a database
typedef struct {
    Table *tables[MAX_TABLES];       // Array of tables
    size_t table_count;              // Number of tables in the database
    pthread_mutex_t lock;            // Mutex for the database
} Database;

// Function to create a new database
Database *createDatabase() {
    Database *db = malloc(sizeof(Database));  // Allocate memory for the database
    db->table_count = 0;                     // Initialize table count to 0
    pthread_mutex_init(&db->lock, NULL);      // Initialize the database mutex
    return db;                                // Return the newly created database
}

// Function to create a table in the database
void createTable(Database *db, const char *table_name, Column columns[], size_t column_count) {
    pthread_mutex_lock(&db->lock);  // Lock the database mutex for thread safety

    if (db->table_count >= MAX_TABLES) {  // Check if max table limit is reached
        printf("Max table size error.\n");
        pthread_mutex_unlock(&db->lock);  // Unlock the mutex and return
        return;
    }

    Table *table = malloc(sizeof(Table));      // Allocate memory for the new table
    strncpy(table->name, table_name, MAX_COLUMN_NAME);  // Copy table name
    table->row_count = 0;                      // Initialize row count
    table->column_count = column_count;        // Set column count
    table->data = malloc(MAX_ROWS * sizeof(char **));  // Allocate memory for rows
    for (size_t i = 0; i < MAX_ROWS; i++) {
        table->data[i] = malloc(MAX_COLUMNS * sizeof(char *));  // Allocate memory for each row
        for (size_t j = 0; j < MAX_COLUMNS; j++) {
            table->data[i][j] = malloc(MAX_VALUE_SIZE * sizeof(char));  // Allocate memory for each cell
        }
    }
    memcpy(table->columns, columns, column_count * sizeof(Column));  // Copy columns
    pthread_mutex_init(&table->lock, NULL);  // Initialize the table mutex

    db->tables[db->table_count++] = table;   // Add the new table to the database
    pthread_mutex_unlock(&db->lock);         // Unlock the database mutex
    printf("Created table: %s\n", table_name);  // Print success message
}

// Function to insert data into a table
void insertIntoTable(Database *db, const char *table_name, char *values[]) {
    pthread_mutex_lock(&db->lock);  // Lock the database mutex

    for (size_t i = 0; i < db->table_count; i++) {
        Table *table = db->tables[i];
        if (strcmp(table->name, table_name) == 0) {  // Find the correct table by name
            if (table->row_count >= MAX_ROWS) {  // Check if the table is full
                printf("Database is full, failed to add data: %s\n", table_name);
                pthread_mutex_unlock(&db->lock);  // Unlock the mutex and return
                return;
            }

            // Insert values into the table
            for (size_t j = 0; j < table->column_count; j++) {
                strncpy(table->data[table->row_count][j], values[j], MAX_VALUE_SIZE);
            }
            table->row_count++;  // Increment the row count
            printf("Data is added: %s\n", table_name);  // Print success message
            pthread_mutex_unlock(&db->lock);  // Unlock the mutex
            return;
        }
    }
    pthread_mutex_unlock(&db->lock);  // Unlock the mutex
    printf("Table not found: %s\n", table_name);  // Print error message
}

// Function to select and print data from a table
void selectFromTable(Database *db, const char *table_name) {
    pthread_mutex_lock(&db->lock);  // Lock the database mutex

    for (size_t i = 0; i < db->table_count; i++) {
        Table *table = db->tables[i];
        if (strcmp(table->name, table_name) == 0) {  // Find the table by name
            printf("Table: %s\n", table_name);  // Print the table name
            for (size_t j = 0; j < table->row_count; j++) {  // Loop through rows
                for (size_t k = 0; k < table->column_count; k++) {  // Loop through columns
                    printf("%s ", table->data[j][k]);  // Print each value
                }
                printf("\n");  // Newline after each row
            }
            pthread_mutex_unlock(&db->lock);  // Unlock the mutex
            return;
        }
    }
    pthread_mutex_unlock(&db->lock);  // Unlock the mutex
    printf("Table not found: %s\n", table_name);  // Print error message
}

int main() {
    Database *db = createDatabase();  // Create the database

    // Define columns for the table
    Column columns[] = {{"id", "INT"}, {"name", "VARCHAR"}};
    
    // Create a table called "users"
    createTable(db, "users", columns, 2);

    // Insert rows into the table
    char *row1[] = {"1", "MJ"};
    char *row2[] = {"2", "KJ"};
    insertIntoTable(db, "users", row1);
    insertIntoTable(db, "users", row2);

    // Select and print data from the table
    selectFromTable(db, "users");

    return 0;
}
