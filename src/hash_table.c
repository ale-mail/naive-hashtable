#include <stdlib.h> 
#include <string.h> 
#include <math.h>

#include "hash_table.h"
#include "prime.h"

static ht_item HT_DELETED_ITEM = {NULL, NULL}; 

static ht_hash_table* ht_new_sized(const int base_size) { 
	ht_hash_table* ht = malloc(sizeof(ht_hash_table)); 
	ht->base_size = base_size; 
	
	ht->size = next_prime(ht->base_size); 
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
	return ht; 
}

/*given key and value, initializes a pointer to an ht_item  */
// static keyword for function declaration means this function is only visible to other functions IN THE SAME FILE
static ht_item* ht_new_item(const char* k, const char* v) {  //const ensures that the variables pointed to by k and v don't get altered 
	//pointer to chunk of memory the size of ht_item
	ht_item* i = malloc(sizeof(ht_item)); 

	//strdup, taking in a char* returns a pointer to a null-terminated byte string that is a duplicate of the parameter
	i->key = strdup(k); //set key field
	i->value = strdup(v); //set value field

	return i; 
}

ht_hash_table* ht_new() { 
	return ht_new_sized(HT_INITIAL_BASE_SIZE); 
}

static void ht_resize(ht_hash_table* ht, const int base_size) { 
	if (base_size < HT_INITIAL_BASE_SIZE) { // hashtable will never have a size smaller than constant size (50) 
		return; 
	}
	ht_hash_table* new_ht = ht_new_sized(base_size); 
	for (size_t i = 0; i < ht->size; ++i) { // traverse existing items and copy them into new hashtable if they are not NULL and haven't been deleted before 
		ht_item* item = ht->items[i]; 
		if (item != NULL && item != &HT_DELETED_ITEM) { 
			ht_insert(new_ht, item->key, item->value); 
		}
	}

	ht->base_size = new_ht->base_size; // want a pointer to this new base_size so that it can be changed 
	ht->count = new_ht->count; 

	const int tmp_size = ht->size; 
	ht->size = new_ht->size; 
	new_ht->size = tmp_size; 

	ht_item** tmp_items = ht->items; 
	ht->items = new_ht->items; 
	new_ht->items = tmp_items; 

	ht_del_hash_table(new_ht); 
}

static void ht_resize_up(ht_hash_table* ht) { 
	const int new_size = ht->base_size * 2; 
	ht_resize(ht, new_size); 
}

static void ht_resize_down(ht_hash_table* ht) { 
	const int new_size = ht->base_size / 2; 
	ht_resize(ht, new_size);
}

/* THIS IS THE OLD ht_new() CODE PRIOR TO DYNAMIC SIZE IMPLEMENTATION
 initializes a new hash table 
ht_hash_table* ht_new() { 	
	ht_hash_table* ht = malloc(sizeof(ht_hash_table)); //allocates memory for ht_hash_table structure 
	// how does sizeof determine the size of the structure? 
	// ANSWER: 
	// recall that the ht_hash_table structure has three fields, two of which are ints, 
	// and the last is a double pointer to an ht_item structure... the ht_item 
	// structure has two fields that are pointers to chars, so the function 
	// will know that it needs to allocate 2*4 + 2*1 = 10 bytes
	
	ht->size = 53; //determines how many ht_item objects we can store
	ht->count = 0; 
	
	//calloc((size_t)n, (size_t)size) allocates n pieces of memory of the given size, returns pointer to first byte of allocated space
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*)); //is the (size_t) cast necessary? ANSWER: yes
	return ht;
}
*/

/*deletes ht_item - frees the stuff stored at i->key, i->value, and i itself afterwards*/
static void ht_del_item(ht_item* i) { 
	free(i->key); 
	free(i->value); 
	free(i); 
}

/*deletes a given hash_table*/
void ht_del_hash_table(ht_hash_table* ht) { 
	for (int i = 0; i < ht->size; ++i) { 
		ht_item* item = ht->items[i]; 
		if (item != NULL) { 
			ht_del_item(item);
		}
	}
	free(ht->items); 
	free(ht); 
}

/*Pseudo code for generic string hashing function: 
 * function hash(string, a, num_buckets): 
 * 	hash = 0 
 * 	string_len = length(string) 
 * 	for i = 0, 1, ..., string_len: 
 * 		hash += (a ** (string_len - (i+1))) * char_code(string[i])
 * 	hash = hash % num_buckets 
 * 	return hash 
 */ 
 
static int ht_hash(const char* s, const int a, const int m) { 
	long hash = 0; 
	const int len_s = strlen(s); 
	for (int i = 0; i < len_s; ++i) { 
		hash += (long)pow(a, len_s - (i+1)) * s[i]; 
		hash = hash % m; 
	}
	return (int)hash; 
}

// the previous hashing algorithm is subject to collision issues, meaning many different inputs can hash to the same value// which is obviously problematic... so in an effort to fix this, the hashing algorithm is modified to now double hash

/* this is a really basic double hashing scheme, notice that the "second" hash function satisfies all criteria: 
 * is never zero since 1 is always added, 
 * cycles through the whole table since taken modulo num_buckets 
 * fast to compute 
 * needs  a rigorous justification for why is pairwise independent of the other, but the probabilty of collision is exceedingly small
 */
static int ht_get_hash(
		const char* s, const int num_buckets, const int attempt
		) { 
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets); 
	const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets); 
	return (hash_a + (attempt * (hash_b + 1))) % num_buckets; 
}

// UPDATE: when performing insertions, if a deleted node is hit in a chain, a new node can be inserted into the slot
// UPDATE 2: there is still the issue of a collision when the same two keys are to be inserted -- they lead to collision!
// not only that, but the second value is rendered inaccessible!
void ht_insert(ht_hash_table* ht_table, const char* key, const char* value) { 
	const int load = ht_table->count * 100 / ht_table->size; 
	if (load > 70)  { 
		ht_resize_up(ht_table); 
	} 

	ht_item* item = ht_new_item(key, value); //construct new ht_item 
	int index = ht_get_hash(item->key, ht_table->size, 0); //compute index for particular key and size of bucket-list
	ht_item* cur_item = ht_table->items[index]; // find item that is at previously-computed index
	int i = 1; 
	while (cur_item != NULL) { 
		if (cur_item != &HT_DELETED_ITEM) { 
			if (strcmp(item->key, key) == 0) { 
				ht_del_item(cur_item); 
				ht_table->items[index] = item; 
				return; 
			} // if they keys aren't equal, we want to continue traversing the chain
			index = ht_get_hash(item->key, ht_table->size, i); 
			cur_item = ht_table->items[index]; 
			i++; 
		} // if cur-item is HT_DELETED_ITEM, then an insertion can be performed
		ht_table->items[index] = item; // set item at computed index to newly-created ht_item
		ht_table->count++; // increment count of number of ht_items in the ht_table
	}
	ht_table->items[index] = item; // set item at computed index to newly-created ht_item
	ht_table->count++; // increment count of number of ht_items in the ht_table
}

// UPDATE: when searching, deleted nodes are ignored and 'jumped over' 
char* ht_search(ht_hash_table* ht, const char* key) { 
	int index = ht_get_hash(key, ht->size, 0); // compute hash for given key
	ht_item* item = ht->items[index]; // extract item in bucket index of computed hash
	int i = 1; // attempt number
	while (item != NULL) { // if there already was an element at that hash, 
		if (item != &HT_DELETED_ITEM) { // if item at that index hasn't been deleted...  	
			if (strcmp(item->key, key) == 0) { // determine if the extracted item has the same key as passed
				return item->value; // return the value of the item if it is 
			}
		}
		index = ht_get_hash(key, ht->size, i); // keys did not match, so we are in a chain; compute new index 
		item = ht->items[index]; 
		i++; // increase attempt count -- this lets us traverse the chain 
	}
	return NULL; // if the key wasn't entered into the hashtable earlier, the chain will eventually end with a NULL
	// this means that we were incapable of locating a value associated to the passed key
}

/* Deleting from a hashtable is more difficult than the previous operations since deletion of a 
 * key-value pair that is an intermediate in a chain of hashes will render all later pairs inaccessible;
 * to fix this, a global flag HT_DELETED_ITEM is introduced to flag that a bucket is clear, functions will be updated 
 * to allow insertion into such buckets
 */

void ht_delete(ht_hash_table* ht, const char* key) {
       	const int load = ht->count * 100 / ht->size; 
	if (load < 10) { 
		ht_resize_down(ht); 
	} 

	int index = ht_get_hash(key, ht->size, 0); 
	ht_item* item = ht->items[index]; 
	int i = 1; 
	while (item != NULL) { // traversing the chain of hashes
		if (item != &HT_DELETED_ITEM) { // if a value is stored at a given index
			if (strcmp(item->key, key) == 0) { // determine if the keys really are equal 
				ht_del_item(item); // if they are, delete the item at that index
				ht->items[index] = &HT_DELETED_ITEM; // replace it with HT_DELETED_ITEM
			}
		} // if we're here, means the item has already been deleted, generate next hash in chain
		index = ht_get_hash(key, ht-> size, i); 
		item = ht-> items[index]; 
		i++; 
	}
	ht->count--; // after successfully deleting, decrement the count number of elements in hashtable 
}

// having added the HT_DELETED_ITEM, we must now change the insertion and search functions as well 


