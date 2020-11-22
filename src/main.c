#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h> 

#include "hash_table.h" 
#include "prime.h"

static int ht_hash(const char* s, const int a, const int m) { 
	long hash = 0; 
	const int len_s = strlen(s); 
	for (int i = 0; i < len_s; ++i) { 
		hash += (long)pow(a, len_s - (i+1)) * s[i]; 
		hash = hash % m; 
	}
	return (int)hash; 
}

int main() { 

	ht_hash_table* ht = ht_new(); 
	ht_del_hash_table(ht); 
}

