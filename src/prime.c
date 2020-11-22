#include <math.h> 
#include <stdlib.h>

#include "prime.h" 

/* 
 * return whether x is prime or not 
 * 1 - prime 
 * 0 - not prime 
 * -1 - undefined (x<2) 
 *  
 */

int is_prime (const int x) { 
	if (x<2) return -1; 
	if (x<4) return 1; 
	if ((x%2) == 0) return 0; 
	for (size_t i = 3; i <= floor(sqrt((double) x)); i += 2) {
			if ((x%i) == 0) return 0; 
	}

	return 1;
}

int next_prime (int x) { 
	while (is_prime(x) != 1) { 
		x++; 
	} 
	return x; 
}
