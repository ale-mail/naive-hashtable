#define HT_PRIME_1 1019
#define HT_PRIME_2 2699
#define HT_INITIAL_BASE_SIZE 50 

typedef struct { 
	char* key; 
	char* value; 
} ht_item; 

typedef struct { 
	int base_size; 
	int size; 
	int count; 
	ht_item** items; //this is a double pointer: items points to a pointer to an ht_item structure 
        // this is an array of pointers to structures of type ht_item 	
} ht_hash_table; 

// in order for the main.c file to see the functions that it calls, it is necessary to declare these functions here 
// otherwise, main.c will be blind to them. other files don't see functions in other files unless explicitly declared
ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* ht);
void ht_insert(ht_hash_table* ht, const char* key, const char* value); 
char* ht_search(ht_hash_table* ht, const char* key); 
void ht_delete(ht_hash_table* h, const char* key); 
