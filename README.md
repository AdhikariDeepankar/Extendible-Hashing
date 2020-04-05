# Extendible-Hashing
Implementation of Extendible Hashing.

Dataset creation:
For this question, you would have to create a synthetic table (simulating sales records of department stores) containing 1
Lakh records. Each record in this file contains four fields: (1) Transaction ID (an integer), (2) Transaction sale amount (an
integer), (3) Customer name (string) and, (4) category of item. Transaction ID is an integer to identify each transaction
uniquely in the dataset. Transaction sale amount is a random integer between 1 and 500000. Customer name is a random 3
letter string. You can model this as a character array of length 3. Category of the item is a random integer between 1
--1500 (uniformly distributed across the table).
After creating this dataset, you need to simulate its storage on a disk. Define a disk block as a file which can store only
300 records of the synthetic table. Assume unspanned organization i.e., records are not allowed to span across two disk
block. Following this store your entire synthetic table as collection of these “disk blocks.” Each disk block (simulated as a
file) should have a unique name, for that you can name them as 1, 2, 3, …, etc. Basically, your original synthetic sales
table would be stored as a series of files. The first disk block (file) would store Row 1 -- Row 300; second disk block
(second file) would store Row 301 – Row 600. For sake of simplicity use text files for simulating the disk blocks. Also
note that each disk block should have an entry at its end which stores the file name of the next disk block for the file.
Additionally, in your code you should store the file name of the first disk block of the synthetic sales table. This would be
needed a later stage to perform a linear scan through the file.

After creating the dataset, you are required to index the file using an Extendible hash. Basically, in your extendible
hash you would be storing the following kinds of records: {TransactionID, <Filename of the disk block which
stores this record>}. Implementation specification of the extendible hash is given below:

Details of Extendible Hash:
● We would be storing index records of the format “{TransactionID, <Filename of the disk block which stores this
record>}” in the extendible hash.
● We would hashing on “TransactionID.”
● Ideally, the buckets in the extendible hash should be stored in the secondary memory. However, for the purpose of
this project, they would be stored in something called “Simulated Secondary Memory (detailed below).”
● The directory or bucket address table of the extendible hash would contain hash prefix and pointer to the bucket
sitting in “Simulated Secondary Memory (detailed below).”

Simulated Secondary Memory:
Following tips would help you achieve this.
(a) The secondary memory can be simulated through an array of the abstract data-type “bucket”. You can fix a very
large size for this array.
(b) The bucket capacity is fixed in terms of number of index records it can contain. Do not hard code this number as it
would be varied in the experiments.
(c) Indices in this array form our “bucket address / hardware address.”
(d) Here, the bucket abstract data-type would have the following information:
a. Number of empty spaces
b. An array of structures to store the index records. Length of this array is fixed according to the parameter
“bucket-size” specified.
c. Link to the next bucket (valid only if this bucket is overflowing)
d. All buckets in the overflow chain must be linked. The last bucket of the overflow chain must have a
special character denoting that it is end of the overflow chain.
(e) Note that in your entire code, there should be only one abstract data type for bucket for all the purposes,
viz., records, overflow buckets for both records and directory entries.
(f) It is advisable to keep a separate area in the secondary memory for storing the overflow buckets.
(g) There should be only one instance of secondary memory running in your code.

Main Memory
(a) You can assume to have enough “main memory” for “bringing” in the buckets to be rehashed.
(b) In addition to item (a) “Main memory” can hold upto 1024 directory entries. The rest resides in “Simulated
Secondary memory.”
(c) Directory entries overflowing into secondary memory would be using the same bucket abstract data type
which was previously declared in item “Simulated Secondary Memory”.

Other Details:
(a) The most significant bits are extracted to find the directory entry.
(b) Only one directory expansion is allowed per record insertion. Following the directory expansion, you may attempt
to resolve the collision (if it still persists) by increasing the local depth (if local depth < global depth). In case the
collision is still not resolved, just create an overflow bucket.
(c) “Main memory” can hold upto 1024 directory entries. The rest resides in “Simulated Secondary Memory.”

Additional Implementation Specifications
Secondary Memory based index structure : This is multi-level
index structure stored across a collection of “disk-blocks.” Assume that gamma number of index records can fit in each
“disk-block” As done in phase A, these “disk-blocks” are actually files on your hard drives. In your implementation, you
can create this index structure before hand and store in your hard drive. Following is a brief description of the levels of
this index structure:
Level 0 (leaf level): For each record in the transaction file, include a record {TransactionID, <Filename of the disk
block which stores this record>} in the level 0 of the index. In other words, for every record in file, we would have a
corresponding record in the level 0 of this index. These records are stored in files simulating “disk-blocks” containing
gamma records each. All “disk-blocks” in this level are linked, ie., each file simulating the “disk-block” would have the
name of the previous and the next “disk-block” in this level. It is important to note that index records of this level are
sorted (on transaction ID) across the “disk-blocks.” This property is very important to ensure the correctness of the range
query.
Level 1: Using the previously described procedure, level 0 would span across several “disk-blocks.” We pick the first
record from each of the level 0 blocks to create entries in level 1. Note that records in this level would be of the form:
{TransactionID, <Filename of the disk block which stores this entry in Level 0>} Similar to level 0, the records of
this level are also stored in files simulating “disk-blocks” containing gamma records each. And once again, index records
stored in this level are sorted (on transaction ID) across the “disk-blocks.” This property is very important to ensure the
correctness of the range query.
..
..
..
Continue until all records fit in just one “disk-block”
Implement a correct and efficient range query algorithm on this index structure. For our given sample range
query, this algorithm should first search for the smaller transaction id “<X>” in the index structure and then
perform a leaf scan and retrieve all the corresponding records from the transaction file. No other range query
algorithm would be accepted for secondary memory index structure.

Cost Modeling:
Given a range query R, the cost of a particular range query algorithms is defined as the estimated number of
“block” transfers made while answering the query R. Use the cost expressions covered in the lecture for calculating the
estimated number of blocks transferred. Note that in case of extendible hash, the number of blocks transferred would
correspond to the number of “buckets” read from the “simulated secondary memory.” Whereas, in the case of secondary
memory index and the naive algorithm, the number of blocks transferred would be number of “files simulating
disk-blocks” accessed.

Query Optimization Routine:
Given a range query R, the goal of this routine is to determine the most efficient range query algorithm (amongst
naive, extendible hash and secondary memory index) for this particular range query. For this purpose, the routine uses the
cost expressions (covered in the lecture) of different algorithms and chooses the algorithm with the least cost. Recall that
the expressions internally use the parameter theta which denotes the number of records which are likely to be present the
given range between <X> and <Y> . For estimating theta, you should use a histogram built over the dataset.

Experimental Analysis
Compute the following for each of the following queries: (a) estimated number of blocks for naive search, extendible hash
and secondary memory index; (b) choice of your query optimizer and actual blocks accessed by your chosen range query
algorithm; (c) Actual number of blocks accessed by each of the other range query algorithms.
Assume the following parameters:
1. Create 1.5 lakh records in the transaction file.
2. The transaction IDs are random (unique) integers between 50,000 and 350,000.
3. While storing the transaction file in “disk-blocks” — Store 100 records per “disk-block”
4. 1 bucket in “simulated secondary memory” of extendible hash can store
● 200 records of the form {TransactionID, <Filename of the disk block which stores this record>}
● 300 entries of the bucket address table
5. Gamma — 200 index records for the secondary memory index “disk-blocks” (of the form {TransactionID,
<Filename>})
Query 1: Select * From Transaction Where TID is between 50,000 and 51,000
Query 2: Select * From Transaction Where TID is between 50,000 and 55,000
Query 3: Select * From Transaction Where TID is between 50,000 and 60,000
Query 4: Select * From Transaction Where TID is between 50,000 and 70,000
Query 5: Select * From Transaction Where TID is between 50,000 and 90,000
Query 6: Select * From Transaction Where TID is between 100,000 and 180,000
Query 7: Select * From Transaction Where TID is between 100,000 and 260,000
Query 8: Select * From Transaction Where TID is between 100,000 and 300,000
Query 9: Select * From Transaction Where TID is between 100,000 and 350,000
