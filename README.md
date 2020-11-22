## High Level Explanation: 

The purpose of this code is to implement a hash table, which is an `associative data` type, which is a collection of (key, value) pairs in such a way that each key is mapped to a value at most once, or in other words, every possible key appears at most once. With this data structure, new pairs can be added, existing ones can be removed, a value mapped to a particular key may be changed, and a value associated with a particular key can be extracted. 

A hash function is used to assign a hash index/code to a given value. These hash codes are stored in an array of buckets/slots. Then, when a lookup is performed, keys are hashed and the results indicate where the corresponding value is stored. Because hash functions are imperfect, it's impossible to always assign hash codes uniquely, so collision-prevention techniques must be employed. 

The average cost for each look-up is typically independent of the number of elements in the hash table. Additionally, insertion and deletion operations may be performed in constant time. 

The implementation here is distinct from what is known as a HashMap in Java. At a high level, a HashMap is a HashTable that is thread-safe. Using certain locking mechanisms and/or "critical sections", it is possible to make a HashTable thread-safe. **READ UP MORE ON CRITICAL SECTIONS AND IMPLEMENTATION IN C**
## Hashing Specifics:

Given a key, `hash = hashfunc(key)` is an execution of the chosen hash function. The index at which to store the value to be associated to the key is the result of the hashing modulo the size of the array -- `index = hash % array_size`. When a new key is input into the hashfunction, a look-up is performed by the output index, if a value is stored there, that value is returned.  

A hashing function whose results lie along a uniform distribution must be chosen as otherwise, collisions are just statistically more likely. Clustering occurs when more than one key map to consecutive slots, so a probing (searching through alternate locations in the array) is performed. Typically, this means that the complexity of the hashtable increases dramatically since a near-linear search becomes necessary.

Linear probing is the most simple resolution to hash collisions. If a collision occurs, linear probing searches the table for the closest following free location and inserts the new key there. Lookups occur in the same fashion -- the table is searched sequentially starting at the position outputted by the hash function until a cell with a matching key is found. Because of `locality of reference`, linear probing is actually not as bad as it sounds -- constant-time look-ups, insertions, and deletions are preserved! 

`Locality of reference` refers to the tendency of a processor to access the same set of memory locations over a short period of time. This breaks down into two more categories: `spatial` and `temporal`. The former refers to the use of data elements within relatively short storage locations while the latter regards the reuse of specific data over a small duration of time. **TO IMPLEMENT!**

Quadratic hashing is the second method for resolving hash collisions. It operates by taking the original hash index and adding successive values of a quadratic polynomial until an open slot is found. The quadratic polynomial might just be squaring the input, which is the example that is on Wikipedia. It avoids the clustering problem and preserves some locality of reference, although less than linear probing does. **TO IMPLEMENT** 

Another approach to resolving hash collision is double hashing. This technique uses one hash value as an index into the table and then repeatedly steps forward an interval until the desired value is located, an empty location is reachehd, or the entire table has been searched. The step interval is set by a second, independent hash function. Note that unlike in linear probing and quadratic probing, the interval depends on the data, so values mapping to the same location have different bucket sequences. This in turn minimizes repeated collisions and the likelihood of clustering. Given two independent hash functions `h1` and `h2`, the i-th location in the bucket sequence for the value k in a hash table consisting of `|T|` buckets is `h(i,k) = (h1(k) + i h2(k)) mod |T|`. Typically `h1` is chosen to have a range `{0, |T| -1}` and `h2` is chosen to have a range `{1, |T|-1}` so that the interval size is non-zero... 


