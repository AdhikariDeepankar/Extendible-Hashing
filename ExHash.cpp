#include<bits/stdc++.h>
using namespace std;

// Deepankar Adhikari 2019CSM1004
// Mahip Soni 2019AIM1006
void RecordDisplay(int tid, string fname);
#define MTS 100000 //MemoryTableSize

int countglna=0;
int height = 0;
int calcTheta(int A, int B, int d, int X, int Y);
void Xhashread(char s);
void Xhashcreate(int x);
void XhashC();
int naivecode(int X, int Y);
int XhashSearch(int X, int Y, int myFlag);
int naivecodeEmptyReturn(int X, int Y);
vector< pair < int , string > > forThetaTemp;
vector< pair < int , string > > vectRS;
int METACHAIN=0;    //storing the number of chains

int maxBucketSize= 200;
int maxATSize = 1024;
int countn=0;
int nn = 0;
int GD=0;
int exHash(int tid, int n){
  //countn++;
  //if(countn>=200){ countn=0; if(n!=GD) nn++;  GD=n; }
  //else n = nn;
  tid-=50000;
  if(n==0) return 0;
  //return (tid%(int)(pow(2,n)));
    unsigned int ntid = tid<<13;
    //if(n>=10) n-=10;
    unsigned int hashval = (ntid>>(32-n));


    return hashval;
}

struct Block{
    bool isBucket;
    int localDepth;
    vector<pair<int,string> >bucketEntries; // my records will have this format
    int bucketSize;
    int bucketIndex;    // i realise this got no use
    struct Block* chainedBlock;
    //new Addition
    int pointingAddress;   //Depending on the local depth the bucket will think that only 1 bucket pointing to it tho several may be pointing
    int emptySpace; //useless part created because asked in the specification sheet

    bool isAT; //is address Table
    vector<pair<struct Block*, int> > ATEntries; // Address Table entries will have their index and the address of bucket they pointing at.
    int ATSize; //Address Table block have how many entries currently
    struct Block* followupAT; // after the addressTable block gets filled up to its given limit, we will have to create new block;
};

struct Block createBlock(bool isB,int ld, int bSize, int bIndex, struct Block* chainedB, bool AT, int ATBlockSize, struct Block* nextAT){
    struct Block newBlock;
    newBlock.isBucket = isB;
    newBlock.localDepth = ld;
    newBlock.bucketSize = bSize;
    newBlock.bucketIndex = bIndex;
    newBlock.chainedBlock = chainedB;
    newBlock.emptySpace = maxBucketSize;

    newBlock.isAT = AT;
    newBlock.ATSize = ATBlockSize;
    newBlock.followupAT = nextAT;

    return newBlock;
}

int GlobalDepth = 0;
int addressTableIndex = 0; //1st record in AT will have index 0
struct Block memory[MTS];
int memoryIndex=0;
void Search();
void File();

struct Block* addressTable ;//creating address table block having 0 entries at start
//cant assign in the global so made memory inside the main function and assigned at the value

//made the below type to pointer above so easier work flow of code is felt
//struct Block addressTable= createBlock(0,0,0,0,NULL,1,0,NULL);  //creating address table block having 0 entries at start

struct Block* currentAT = addressTable;  //lastAT
//
//
//
//
//


//LATER I REALISED THAT DURING DOUBLING OF TABLE I AM ALREADY HANDLING POINTERS FROM ATENTRIES VERY WELL SO ACCORDINGLY WHILE DISTRIBUTION THE NEW ENTRIES WILL BE CREATED vector <pair<struct Block*,int> > AddBucketWithNullEntries; // in case the address table is to be doubled then we can easily make new table and copy ATEntries as this

//LATER I REALISED THAT DURING DOUBLING OF TABLE I AM ALREADY HANDLING POINTERS FROM ATENTRIES VERY WELL SO ACCORDINGLY WHILE DISTRIBUTION THE NEW ENTRIES WILL BE CREATED vector <pair<struct Block*,int> > AddBucketWithNullEntries; // in case the address table is to be doubled then we can easily make new table and copy ATEntries as this

void insert(pair<int, string> record){    //record of format vector<pair<int,string>>bucketEntries;
 int index = exHash(record.first,GlobalDepth);
 //
 //
 //
 //
    if(GlobalDepth==0){
            if(addressTable->ATSize == 0 && addressTableIndex==0){   //If addressTable is empty therefore it got no entries, gotcha create a entry and push a bucket into memory corresponding to that entry
                struct Block Bucket = createBlock(1,GlobalDepth,0,0,NULL,0,0,NULL); // whenever we create a new Bucket its local Depth will be equal to the GlobalDepth
                Bucket.bucketEntries.push_back(record); Bucket.bucketSize = 1; Bucket.pointingAddress = 0; Bucket.emptySpace= Bucket.emptySpace -1;



                memory[memoryIndex]=(Bucket);  memoryIndex++; //Push only after we have done with the updates of the struct , afterwards we can only access it via memory references and will need to figure the right references
                //out bucket is pushed in the memory at last!
                addressTable->ATEntries.push_back(make_pair(&(memory[memoryIndex-1]), addressTableIndex));    //to access the location we are storing its address in the corresponding addressTable entry
                addressTableIndex++; //next index will be addressTableIndex
                addressTable->ATSize = 1;    //addressTable got 1 entry now
            }else if(addressTable->ATEntries[0].first->bucketSize < maxBucketSize){     //if the corresponding Bucket has space the push entries there //filling the 1st bucket
                struct Block* currentBucket = addressTable->ATEntries[0].first;
                currentBucket->bucketEntries.push_back(record);
                currentBucket->bucketSize = currentBucket->bucketSize + 1;
                currentBucket->emptySpace = currentBucket->emptySpace -1;
            }else if(addressTable->ATEntries[0].first->bucketSize == maxBucketSize){ //Bucket 0 is full now , also here we know that GD=LD will happen initially
                struct Block* problem = addressTable->ATEntries[0].first;// to be the address of the bucket the record wanted to get in
                //case global depth = local depth of problem
                GlobalDepth = GlobalDepth+1;
               // int newInserts = pow(2,GlobalDepth) - pow(2,GlobalDepth-1);
                //while(newInserts){
                    struct Block newBucket = createBlock(1,GlobalDepth,0,1,NULL,0,0,NULL); // compulsary bucket for index entry 1
                    newBucket.pointingAddress = 1; //newIndex Entry from AT pointing to our new Bucket
                    memory[memoryIndex]=(newBucket); memoryIndex++;

                    addressTable->ATEntries.push_back(make_pair(&(memory[memoryIndex-1]),addressTableIndex)); // Mahip- memory.size() replaced by memoryIndex|| all new records created will be pointing to the block the record wanted to go in but couldn't
                    addressTableIndex++;
                 //   newInserts--;
                //}
                struct Block* prevBlock = addressTable->ATEntries[0].first; //previous block will be the block where record wanted to go but the bucket was full
                //rehashing
                vector<pair<int,string> > tempVector = prevBlock->bucketEntries; tempVector.push_back(record); // copied the whole bucket's record into tempVector and also pushed the new record
                prevBlock->bucketEntries.clear(); prevBlock->localDepth = GlobalDepth; prevBlock->bucketSize = 0;   // cleared the information in that old bucket
                prevBlock->emptySpace = maxBucketSize;
                for(int i=0;i<tempVector.size();i++){
                    int newIndex = exHash(tempVector[i].first,GlobalDepth); //tempVector[i] is old record <<!!!!!!!!!!!NOTENOTENOTENOTENOTENOTENOTE!!!!!!!!!>>
                    struct Block* modBucket = addressTable->ATEntries[newIndex].first;
                       // only proceed further after the check of whether the bucket is created
                    if(modBucket->bucketSize < maxBucketSize){  //if the bucket is created and space in bucket then cool
                        modBucket->bucketEntries.push_back(tempVector[i]);
                        modBucket->bucketSize = modBucket->bucketSize +1;
                        modBucket->emptySpace = modBucket->emptySpace -1;
                    }else if(modBucket->bucketSize == maxBucketSize){// if its full then look for need for chaining
                        struct Block* temp = modBucket;
                        while(temp->chainedBlock != NULL){ // reaching the last chained block if any
                            temp = temp->chainedBlock;
                        }
                        if(temp->bucketSize < maxBucketSize){   // if last chained block has space then push your record
                            temp->bucketEntries.push_back(tempVector[i]);
                            temp->bucketSize = temp->bucketSize +1;
                            temp->emptySpace = temp->emptySpace -1;
                        }else if(temp->bucketSize == maxBucketSize){ // if last chained block is also full then need to create one more chained block and push it in memory after making required connections
                            struct Block chained_Block_new = createBlock(1,GlobalDepth,0,newIndex,NULL,0,0,NULL);
                            chained_Block_new.bucketEntries.push_back(tempVector[i]);
                            chained_Block_new.bucketSize = 1;
                            chained_Block_new.emptySpace = maxBucketSize-1;
                            memory[memoryIndex]=(chained_Block_new); memoryIndex++;
                            temp->chainedBlock = &(memory[memoryIndex-1]);
                            METACHAIN++; //storing the number of chains
                        }
                    }

                }

            }
// int index = exHash(GD,record); could be written just below the function declaration line//there too commented//
    }else{ // going with further cases apart from initial case
   //only when addressTable initial is full     struct Block* currentAT = addressTable;
           //WTF every time index will be less than currentAT.size gotcha check this condition if expansion needed only if(index<currentAT.ATSize){}
            int z=0;
            currentAT = addressTable;
            if(record.first == 7){
                z=1;
            }
             //   if(z==1){
               //   cout<<"sd"<<endl;
              //  }
           int tempIn = index;
           while(tempIn >= maxATSize){
                tempIn= tempIn-maxATSize; // insert at tempIn in the newBucket
                currentAT= currentAT->followupAT;

           }
        if(currentAT->ATEntries[tempIn].first!=NULL ){ //&& currentAT->ATEntries[tempIn].first->bucketSize<maxBucketSize//  our new record is fitting well in the bucket pointed by the corresponding index of the entry in AT // IF ONLY THAT BUCKET EXISTS
            /* I saw the problem in the above if part; it wasn't checking for spaces in the chained buckets if any
            struct Block* currentBucket = currentAT->ATEntries[tempIn].first;
            currentBucket->bucketEntries.push_back(record);
            currentBucket->bucketSize = currentBucket->bucketSize + 1;
            */
            //therefore my editing is as follows and i remove the second half of the above if condition
            struct Block* checkAllChainedBuckets = currentAT->ATEntries[tempIn].first;
            while(checkAllChainedBuckets->chainedBlock!=NULL){ // added a new condition because a case could happen when the chained records
                checkAllChainedBuckets=checkAllChainedBuckets->chainedBlock;
            }
            if(checkAllChainedBuckets->bucketEntries.size()<maxBucketSize){
                checkAllChainedBuckets->bucketEntries.push_back(record);
                checkAllChainedBuckets->bucketSize = checkAllChainedBuckets->bucketSize + 1;
                checkAllChainedBuckets->emptySpace = checkAllChainedBuckets->emptySpace -1;
            }else if(checkAllChainedBuckets->bucketEntries.size()==maxBucketSize){ //correcting the condition where i cross checked that the corresponding bucket to the addressTable Entry is full // earlier i didn't took into consideration the fact that more of the chained buckets are created

            // CASE: LOCAL DEPTH = GLOBAL DEPTH
            if(currentAT->ATEntries[tempIn].first->localDepth==GlobalDepth){ // CASE: LOCAL DEPTH = GLOBAL DEPTH
                //now considering when the table could try to extend beyond it's limit
                GlobalDepth++;
                //making the currentAT to be the last of the current chained address table
                currentAT = addressTable;
                while(currentAT->followupAT!=NULL){
                    currentAT= currentAT->followupAT;
                }
                if(addressTableIndex>=maxATSize){
                    int countOfNewTables = addressTableIndex/maxATSize;
                    while(countOfNewTables){
                        struct Block newAT = createBlock(0,0,0,0,NULL,1,maxATSize,NULL);
                        addressTableIndex = addressTableIndex+maxATSize;
                        countOfNewTables--;
                        memory[memoryIndex]=(newAT); memoryIndex++;
                        currentAT->followupAT = &(memory[memoryIndex-1]);
                        currentAT= currentAT->followupAT;

                    }
                }

                if(addressTableIndex<maxATSize){    //if the address Table index was less than the maxATSize then no need to create a new bucket table

               addressTableIndex=addressTableIndex*2;//addressTable index is doubled and we can ask but we havent pushed any entries into the addresstable
                addressTable->ATSize = 2*addressTable->ATSize; // so in out pointer managing operation while expansion we increase the size there only
                }
               // for handling extendibleAT currentAT = addressTable;
                currentAT = addressTable;
                vector<pair<struct Block*, int> > tempForDouble; //to copy the entries because corresponding to 0 -> after expansion it will become 00 and 01 and both will point to same bucket, hence having the same entries
                 int blah;
                if(addressTableIndex/2 >= maxATSize){//if we got more than the real original addressTable bucket then this will work
                blah = addressTableIndex/(2*maxATSize); //well half of the newly created buckets will have no ATEntries
                }else if(addressTableIndex/2<maxATSize){    // if we got only the real bucket addressTable then this will work as in the max case after expansion the index can jump to 1024, 1024/2 < 1024 but 2048/2 is not less than 1024
                blah = 1;
                }

                while(blah!=0 && currentAT!=NULL){    //copying all the entries from all the address tables and storing it in tempForDouble vector
                    for(int i =0;i<currentAT->ATEntries.size();i++){
                        tempForDouble.push_back(currentAT->ATEntries[i%maxATSize]);
                    }
                    currentAT->ATEntries.clear();//removing the entries from the 1st original addressTable too, coz anyway inserting them
                    blah--;
                    if(currentAT->followupAT!=NULL){
                    currentAT=currentAT->followupAT;
                    }
                }
                currentAT = addressTable; //handling extension of Address Table
               // currentAT->ATEntries.clear(); // removing the entries from the 1st original addressTable too, coz anyway inserting them
                currentAT->ATEntries.insert(currentAT->ATEntries.begin(),tempForDouble[0]);
                currentAT->ATEntries.insert(currentAT->ATEntries.begin(),tempForDouble[0]);
                for(int i=2;i<addressTableIndex;i=i+2){    //earlier before handling for extendibleAT it was i<addressTable->ATEntries.size()
                    if(i%maxATSize==0){
                           //for the current bucket the ATEntries have doubled the maximum capacity so we have to delete that lower half and copy it to the entries of the next bucket and copy the entries of the next bucket further if they exist currentAT->ATEntries
                       // while(currentAT->ATEntries.size()-maxATSize){ //we inserted in the AT ENTRIES table so they got doubled so deeting extra entries
                       //     currentAT->ATEntries.erase(currentAT->ATEntries.begin() + maxATSize);
                        currentAT = currentAT->followupAT;
                        }
                    currentAT->ATEntries.insert(currentAT->ATEntries.begin()+(i%maxATSize),tempForDouble[i/2]);

                    currentAT->ATEntries.insert(currentAT->ATEntries.begin()+(i%maxATSize),tempForDouble[i/2]);

                    }

                }


            //CASE: Local Depth < Global Depth
             // int index = exHash(GD,record); REPITITION BECAUSE WE ARE USING Ld<GD IN CONTINUATION WITH THE LD=GD CASE AS WELL, SO REHASHING OF THAT OLD RECORD FOR WHICH LD=GD CASE WAS APPLIED SHOULD GO TO THE CORRECT ENTRY OF THE EXPANDED ADDRESS TABLE BY THAT CASE
            index = exHash(record.first,GlobalDepth);
            tempIn = index;
            currentAT = addressTable;
               while(tempIn >= maxATSize){
                    tempIn= tempIn-maxATSize; // insert at tempIn in the newBucket
                    currentAT= currentAT->followupAT;
               }
        //CASE: Local Depth < Global Depth
            if(currentAT->ATEntries[tempIn].first->localDepth<GlobalDepth){    //case local depth < Global Depth
                struct Block* currentBucket = currentAT->ATEntries[tempIn].first;
                int realParentstartRange = (currentBucket->pointingAddress)*pow(2,(GlobalDepth-currentBucket->localDepth));
                int numberOfparents = pow(2,(GlobalDepth - currentBucket->localDepth));
                currentBucket->localDepth = currentBucket->localDepth + 1; // 1.increase the current local depth
                currentBucket->pointingAddress = 2*(currentBucket->pointingAddress);    //now the bucket will think the index 2*previousBucketIndex Points to it
                struct Block Bucket = createBlock(1,currentBucket->localDepth,0,tempIn,NULL,0,0,NULL); // 2. create a new Bucket with the updated localDepth
                Bucket.pointingAddress = 2*(currentBucket->pointingAddress)+1;
                //kinda done with the correct storage of what the bucket think is its parent in the pointAddress
                memory[memoryIndex]=(Bucket); memoryIndex++;
                 for(int i=0;i<numberOfparents/2;i++){    // 3.Make second half of the entries pointing to the previous bucket to the newly created bucket
                    if((realParentstartRange+(numberOfparents/2) + i)%maxATSize ==0){
                        currentAT=currentAT->followupAT;
                    }
                    currentAT->ATEntries[(realParentstartRange+(numberOfparents/2) + i)%maxATSize].first = &memory[memoryIndex-1];
                    currentAT->ATEntries[(realParentstartRange+(numberOfparents/2) + i)%maxATSize].second=0;
                }

                //4.RESHASH ALL THE RECORDS
                vector<pair<int,string> > tempVector = currentBucket->bucketEntries;
                struct Block* tempPrevBucket = currentBucket;
                while(tempPrevBucket->chainedBlock!=NULL){   //if the current block has chained buckets then we gotcha rehash them as well
                    tempPrevBucket=tempPrevBucket->chainedBlock;
                    METACHAIN--;
                    for(int l=0;l<tempPrevBucket->bucketEntries.size();l++){
                        tempVector.push_back(tempPrevBucket->bucketEntries[l]);
                    }
                    tempPrevBucket->bucketEntries.clear();//clearing the chained buckets
                    tempPrevBucket->bucketSize=0;
                    tempPrevBucket->emptySpace = maxBucketSize;
                }
                tempVector.push_back(record); // copied the whole bucket's record into tempVector and also pushed the new record
                currentBucket->bucketEntries.clear(); currentBucket->bucketSize = 0; currentBucket->emptySpace = maxBucketSize;  // cleared the information in that old first bucket too

                currentBucket->chainedBlock = NULL; //ADDITION OF CODE AS REHASHED SO ALL THE CHAINS ARE NO MORE THERE
                for(int i=0;i<tempVector.size();i++){
                    int newIndex = exHash(tempVector[i].first,GlobalDepth); //tempVector[i] is old record <<!!!!!!!!!!!NOTENOTENOTENOTENOTENOTENOTE!!!!!!!!!>>
                  //URGENT URGENT NOTE!!!!!!!!!!!!!!!!!!  //make currentAT reach the bucket where the newIndex actually is and convert the newIndex to the index of that bucket
                            tempIn = newIndex;
                            currentAT = addressTable;
                       while(tempIn >= maxATSize){
                            tempIn= tempIn-maxATSize; // insert at tempIn in the newBucket
                            currentAT= currentAT->followupAT;
                       }
                    struct Block* modBucket = currentAT->ATEntries[tempIn].first;
                    /*if(modBucket==NULL){    //if the new index corresponding bucket is not created creating a bucket
                      I think this wont be required as already created the new bucket  struct Block newBucket = createBlock(1,GlobalDepth,0,newIndex,NULL,0,0,NULL);
                     But if again all records go the the old bucket then chaining required and it was a waste of space coz of the newly created bucket newBucket.pointingEntriesIndex.push_back(newIndex); //newIndex Entry from AT pointing to our new Bucket
                        memory.push_back(newBucket);
                        addressTable->ATEntries[newIndex].first = &(memory[memory.size()-1]);
                        modBucket = &(memory[memory.size()-1]);
                    } */  // only proceed further after the check of whether the bucket is created
                    if(modBucket->bucketEntries.size() < maxBucketSize){  //if the bucket is created and space in bucket then cool
                        modBucket->bucketEntries.push_back(tempVector[i]);
                        modBucket->bucketSize = modBucket->bucketSize +1; modBucket->emptySpace = modBucket->emptySpace -1;
                    }else if(modBucket->bucketSize == maxBucketSize){// if its full then look for need for chaining
                        struct Block* temp = modBucket;
                        while(temp->chainedBlock != NULL){ // reaching the last chained block if any or any chained block that has space in it and still has more chained buckets attached to it, because we cleared the chained buckets when we stored the elements in tempVector
                            temp = temp->chainedBlock;

                            if(temp->bucketSize<maxBucketSize){
                                break;
                            }
                        }
                        if(temp->bucketSize < maxBucketSize){   // if last chained block has space then push your record
                            temp->bucketEntries.push_back(tempVector[i]);
                            temp->bucketSize = temp->bucketSize +1;
                            temp->emptySpace = temp->emptySpace -1;
                        }else if(temp->bucketSize == maxBucketSize){ // if last chained block is also full then need to create one more chained block and push it in memory after making required connections
                            struct Block chained_Block_new = createBlock(1,currentBucket->localDepth,0,tempIn,NULL,0,0,NULL);
                            chained_Block_new.bucketEntries.push_back(tempVector[i]);
                            METACHAIN++;
                            chained_Block_new.bucketSize = 1;
                            chained_Block_new.emptySpace = maxBucketSize -1;
                            memory[memoryIndex]=(chained_Block_new); memoryIndex++;
                            temp->chainedBlock = &(memory[memoryIndex-1]);

                        }
                    }

                }
                    //well when LD<GD only one new bucket is created , hence if we chain elements on it we are creating new chained buckets for it
                //NOTE WARNING but if the chained blocks of the original rehashed bucket , elements experienced rehashing into the new bucket such that previous linked buckets space go into waste, I will point the last unwasted bucket ChainedBlock to NULL
                tempPrevBucket = currentBucket;
                while(tempPrevBucket->bucketEntries.size()==maxBucketSize){ // reaching the last chained block of the original hashed bucket
                    if(tempPrevBucket->chainedBlock!=NULL){
                    tempPrevBucket=tempPrevBucket->chainedBlock;
                    }else{
                        break;
                    }
                }
                tempPrevBucket->chainedBlock=NULL; // and making that the chainedBlock entry to NULL thereby creating possible dangling block of chain




            }

        }
        }else if(currentAT->ATEntries[tempIn].first==NULL){ //I THINK THIS WILL NEVER HAPPEN BUT STILL if the entry don't point to any record then need to create a bucket corresponding to it
            struct Block Bucket = createBlock(1,GlobalDepth,0,tempIn,NULL,0,0,NULL); // whenever we create a new Bucket its local Depth will be equal to the GlobalDepth
            Bucket.bucketEntries.push_back(record); Bucket.bucketSize = 1; Bucket.emptySpace = maxBucketSize -1; Bucket.pointingAddress=tempIn;
            memory[memoryIndex]=(Bucket); memoryIndex++;
            currentAT->ATEntries[tempIn].first = &(memory[memoryIndex-1]);   //address of bucket update in the addressTable
        }

    /*
            //if the AT table will become full then if anytime encounter the case when Global Depth = Local Depth with the need  to double the AddressTable and increase the Global Depth,
            // another new whole bucket will be filled with the new indexes coz 1024*2 = 2048 = 2 new buckets, similarly if again we need to expand then 2 more extra buckets are going to be created
            if(addressTable.ATSize == maxATSize && addressTableIndex%maxATSize==0){   //If addressTable is full and there a need to create a new table in continuation then
                struct Block Bucket = createBlock(1,GlobalDepth,0,0,NULL,0,0,NULL); // whenever we create a new Bucket its local Depth will be equal to the GlobalDepth
                Bucket.bucketEntries.push_back(record); Bucket.bucketSize = 1;
                memory.push_back(Bucket);   //Push only after we have done with the updates of the struct , afterwards we can only access it via memory references and will need to figure the right references
                //out bucket is pushed in the memory at last!
                addressTable.ATEntries.push_back({&memory[memory.size()-1], addressTableIndex});    //to access the location we are storing its address in the corresponding addressTable entry
                addressTableIndex++; //next index will be addressTableIndex
                addressTable.ATSize = 1;    //addressTable got 1 entry now
            }else if(addressTable.ATEntries[0].first->bucketSize < maxBucketSize){     //if the corresponding Bucket has space the push entries there //filling the 1st bucket
                struct Block* currentBucket = addressTable.ATEntries[0].first;
                currentBucket->bucketEntries.push_back(record);
                currentBucket->bucketSize = currentBucket->bucketSize + 1;
            }else if(addressTable.ATEntries[0].first->bucketSize == maxBucketSize){ //Bucket 0 is full now , also here we know that GD=LD will happen initially
                struct Block* problem = addressTable.ATEntries[0].first;// to be the address of the bucket the record wanted to get in
                //case global depth = local depth of problem
                GlobalDepth = GlobalDepth+1;
                int newInserts = pow(2,GlobalDepth) - pow(2,GlobalDepth-1);
                while(newInserts){
                    addressTable.ATEntries.push_back({NULL,addressTableIndex}); // all new records created will be pointing to the block the record wanted to go in but couldn't
                    addressTableIndex++;
                    newInserts--;
                }
                struct Block* prevBlock = addressTable.ATEntries[0].first; //previous block will be the block where record wanted to go but the bucket was full
                vector<pair<int,string>> tempVector = prevBlock->bucketEntries; tempVector.push_back(record); // copied the whole bucket's record into tempVector and also pushed the new record
                prevBlock->bucketEntries.clear(); prevBlock->localDepth = GlobalDepth; prevBlock->bucketSize = 0;   // cleared the information in that old bucket
                for(int i=0;i<tempVector.size();i++){
                    int newIndex = exHash(GlobalDepth,tempVector[i]); //tempVector[i] is old record <<!!!!!!!!!!!NOTENOTENOTENOTENOTENOTENOTE!!!!!!!!!>>
                    struct Block* modBucket = addressTable.ATEntries[newIndex].first;
                    if(modBucket==NULL){    //if the new index corresponding bucket is not created creating a bucket
                        struct Block newBucket = createBlock(1,GlobalDepth,0,newIndex,NULL,0,0,NULL);

                        memory.push_back(newBucket);
                        addressTable.ATEntries[newIndex].first = &(memory[memory.size()-1]);
                        modBucket = &(memory[memory.size()-1]);
                    }   // only proceed further after the check of whether the bucket is created
                    if(modBucket->bucketSize < maxBucketSize){  //if the bucket is created and space in bucket then cool
                        modBucket->bucketEntries.push_back(tempVector[i]);
                        modBucket->bucketSize = modBucket->bucketSize +1;
                    }else if(modBucket->bucketSize == maxBucketSize){// if its full then look for need for chaining
                        struct Block* temp = modBucket;
                        while(temp->chainedBlock != NULL){ // reaching the last chained block if any
                            temp = temp->chainedBlock;
                        }
                        if(temp->bucketSize < maxBucketSize){   // if last chained block has space then push your record
                            temp->bucketEntries.push_back(tempVector[i]);
                            temp->bucketSize = temp->bucketSize +1;
                        }else if(temp->bucketSize == maxBucketSize){ // if last chained block is also full then need to create one more chained block and push it in memory after making required connections
                            struct Block chained_Block_new = createBlock(1,GlobalDepth,0,newIndex,NULL,0,0,NULL);
                            chained_Block_new.bucketEntries.push_back(tempVector[i]);
                            chained_Block_new.bucketSize = 1;
                            memory.push_back(chained_Block_new);
                            temp->chainedBlock = &(memory[memory.size()-1]);
                        }
                    }

                }

            }
        */
    }
}
void visualize(){
//freopen("output.txt", "w", stdout);
    struct Block* temp;
    temp = addressTable;
    int j =0;
    struct Block* tryin;
    if(temp->ATEntries.size()==0){
        cout<<"There are no records"<<endl;
    }else{
        while(temp!=NULL){
                cout<<endl<<"Address Table number "<<j<<" has the following content: "<<endl;

            for(int i=0;i<temp->ATEntries.size();i++){
                cout<<"corresponding to "<<i<<" entry, we Have the following:"<<endl;
                    tryin = temp->ATEntries[i].first;
                    if(tryin->bucketEntries.size()==0){
                        cout<<"AN EMPTY BUCKET"<<endl;
                    }else{
                        while(tryin!=NULL){
                            for(int k=0;k<tryin->bucketEntries.size();k++){
                                cout<<" Record ID: "<<tryin->bucketEntries[k].first<<" in file: "<<tryin->bucketEntries[k].second<<" ,";
                            }
                            cout<<" with empty space "<<tryin->emptySpace<<" in one bucket. "<<endl;
                            tryin=tryin->chainedBlock;
                        }
                    }
            }
            j++;
            cout<<endl;
            temp= temp->followupAT;
        }
    }
}

void Search(){
  int ano;
  cout<<"Enter Tid to search for index information in HashTable. -1 to stop.\n";
  ano=0;


    int blockread=1;
  	cout<<"T-Id = ";
      //cout<<"hey2";
      cin>>ano;
      //cout<<"hey1";
      int n = exHash(ano,GlobalDepth);
      //cout<<"GD= "<<GlobalDepth<<" ";
      //cout<<"ATSize="<<addressTable->ATEntries.size()<<" ";
      //cout<<"hey";
      int tid=-1;
      string name;
      int whichTable;
      cout<<"HashVal="<<n;
      cout<<" MaxAT="<<maxATSize;
      if(n!=0) whichTable = n/maxATSize;
      else whichTable = 0;
      cout<<" table="<<whichTable;
      struct Block* currAT = addressTable;
      while(whichTable--){
        currAT = currAT->followupAT;
        blockread++;
      }
      int size=currAT->ATEntries.size();

      n = n%maxATSize;
      	//cout<<currAT->ATEntries[n].first->bucketEntries[0].first<<" ";
 	//cout<<currAT->ATEntries[n].first->bucketEntries[1].first<<" ";

	      int k=0;
	      cout<<" CurrentATSize="<<currAT->ATEntries.size()<<" ";
	      int flagAT=0;
	      //struct Block* tempAT = addressTable;


		if(n<currAT->ATEntries.size()&&flagAT==0){

		  struct Block* temp = currAT->ATEntries[n].first;
		  while(tid!=ano&&k<maxBucketSize&&k<temp->bucketSize){
		    tid = temp->bucketEntries[k].first;
		    name= temp->bucketEntries[k].second;
		    k++;
		    //printf("hey");
		    //fflush(stdin);
		  }
		  k=0;
		  while(tid!=ano&&temp->chainedBlock!=NULL){
		  blockread++;
		    int kn=0;
		    //cout<<"hey";
		    temp=temp->chainedBlock;
		    while(tid!=ano&&kn<maxBucketSize&&kn<temp->bucketSize){

		      tid = temp->bucketEntries[kn].first;
		      name= temp->bucketEntries[kn].second;
		      kn++;
		    }
		    if(tid==ano) break;
		  }
		  if(tid==ano) cout<<"TID="<<tid<<" FileName="<<name<<"blockread="<<blockread<<"\n";
		  else cout<<"NOT FOUND\n";
		}

        else cout<<"NOT FOUND "<<"blockread ="<<blockread<<"\n";


}
int totalBlocksRead( int X, int Y ,int exFlag){

int totalReads = 0;
    for(int ano = X; ano<Y;ano++){
    int blockread=1;

      //cout<<"hey1";
      int n = exHash(ano,GlobalDepth);
      //cout<<"GD= "<<GlobalDepth<<" ";
      //cout<<"ATSize="<<addressTable->ATEntries.size()<<" ";
      //cout<<"hey";
      int tid=-1;
      string name;
      int whichTable;

      if(n!=0) whichTable = n/maxATSize;
      else whichTable = 0;
      struct Block* currAT = addressTable;
      while(whichTable--){
        currAT = currAT->followupAT;
        blockread++;
      }
      int size=currAT->ATEntries.size();

      n = n%maxATSize;
      	//cout<<currAT->ATEntries[n].first->bucketEntries[0].first<<" ";
 	//cout<<currAT->ATEntries[n].first->bucketEntries[1].first<<" ";

	      int k=0;
	      int flagAT=0;
	      //struct Block* tempAT = addressTable;


		if(n<currAT->ATEntries.size()&&flagAT==0){

		  struct Block* temp = currAT->ATEntries[n].first;
		  while(tid!=ano&&k<maxBucketSize&&k<temp->bucketSize){
		    tid = temp->bucketEntries[k].first;
		    name= temp->bucketEntries[k].second;
		    k++;
		    //printf("hey");
		    //fflush(stdin);
		  }
		  k=0;
		  while(tid!=ano&&temp->chainedBlock!=NULL){
		  blockread++;
		    int kn=0;
		    //cout<<"hey";
		    temp=temp->chainedBlock;
		    while(tid!=ano&&kn<maxBucketSize&&kn<temp->bucketSize){

		      tid = temp->bucketEntries[kn].first;
		      name= temp->bucketEntries[kn].second;
		      kn++;
		    }
		    if(tid==ano){break;}
		  }
		  if(tid==ano){blockread++; if(exFlag==1){RecordDisplay(tid,name);cout<<endl;}}
            totalReads = totalReads+blockread;
		}

        else totalReads = totalReads+blockread;;

    }

return totalReads;

}

void FileCreate(){
  ofstream file;
  int flag=1;
  string FileName;
  int i=1;
  int j=1;
  int fk;
  cout<<"Enter number of records to create.\n";
  cin>>fk;
  int a[fk+1];

  for (size_t i = 0; i <= fk; i+=1) {
    a[i]=i+50000;
  }

  shuffle(a,a+fk,default_random_engine(0));

  while(flag){
    ostringstream str1;
    str1 << i;
    string number = str1.str();
    number+=".txt";
    i++;
    string currFName = FileName+number;
    file.open (currFName);
    int records=1;
      while(records<=300&&j<=fk){
        records++;
        ostringstream str2;
        str2 << a[j];
        j++;
        string temp = str2.str();
        string insert;
        insert+=temp;
        insert+=' ';

        int transsale = (rand()%500000);
        ostringstream str21;
        str21 << transsale;
        temp = str21.str();
        insert+=temp;
        insert+=' ';

        string tempn;
        int custname = (rand()%(26));
        tempn+=(char)(custname+97);
        custname = (rand()%(26));
        tempn+=(char)(custname+97);
        custname = (rand()%(26));
        tempn+=(char)(custname+97);
        insert+=tempn;
        insert+=' ';

        int category = (rand()%1500);
        ostringstream str23;
        str23 << category;
        temp = str23.str();
        insert+=temp;
        insert+=' ';

        insert+='\n';
        file << insert;
        }

      if(j<fk){ // second edit j<=fk
        ostringstream str11;
        str11 << "-1 ";
        str11 << i;
        number = str11.str();
        number+=".txt";
        currFName = FileName+number;

        file<<currFName;
      }

    //file.close();

    if(j>=fk){ flag=0; ostringstream str12;
    str12 << "-2 ";
    str12 << "end";
    string str = str12.str();
    //number+=".txt";

    //currFName = FileName+number;
    file<<str;
  }
  file.close();
  }
  //return 0;
}

void FileCreateData(){
  ofstream file;
  int flag=1;
  string FileName;
  int i=1;
  int j=0;
  int fk;
  cout<<"Enter number of records to create.\n";
  cin>>fk;
  int a[fk+1];
  int b[300000];
  for (size_t i = 0; i < 300000; i+=1) {
    b[i]=i+50000;
  }
  shuffle(b,b+300000,default_random_engine(0));

  for (size_t i = 0; i < fk; i+=1) {
    a[i]=b[i];
  }


  file.open ("data.txt");
  while(flag){
    if(j>=fk) break;
    //string currFName = FileName+number;

    int records=1;

        records++;
        ostringstream str2;
        str2 << a[j];
        j++;
        string temp = str2.str();
        string insert;
        insert+=temp;
        insert+=' ';

        int transsale = (rand()%500000);
        ostringstream str21;
        str21 << transsale;
        temp = str21.str();
        insert+=temp;
        insert+=' ';

        string tempn;
        int custname = (rand()%(26));
        tempn+=(char)(custname+97);
        custname = (rand()%(26));
        tempn+=(char)(custname+97);
        custname = (rand()%(26));
        tempn+=(char)(custname+97);
        insert+=tempn;
        insert+=' ';

        int category = (rand()%1500);
        ostringstream str23;
        str23 << category;
        temp = str23.str();
        insert+=temp;
        insert+=' ';

        insert+='\n';
        file << insert;
      }
  file.close();


}


void FileSplit(){
      ofstream file;
      int x;
      cout<<"Enter the size of each file record.\n";
      cin>>x;
      ifstream inFile;
      string filename="data.txt";
      cout<<filename;
      int readflag=1;
      inFile.open(filename);
      if (!inFile) {
          cout << "Unable to open filesss";
          exit(1); // terminate with error
      }
      int i=1;
      int tid, tsale, cat;
      string cname;
      string FileName;
      ostringstream str1;
      str1 << i;
      string number = str1.str();
      number+=".txt";
      cout<<number;
      file.open(number);
      int records=-1;

        while (inFile >> tid >> tsale >> cname >> cat ) {
            records++;
            if(records>=x){
              i++;
              records=0;


              ostringstream str12;
              str12 << i;
              string number1 = str12.str();
              number1+=".txt";
              string end = "-1 ";
              file<<end;
              file<<number1;
              file.close();
              file.open(number1);
            }

            ostringstream str2;
            str2 << tid;
            //j++;
            string temp = str2.str();
            string insert;
            insert+=temp;
            insert+=' ';

            int transsale = (tsale);
            ostringstream str21;
            str21 << transsale;
            temp = str21.str();
            insert+=temp;
            insert+=' ';

            string tempn;
            //int custname = (rand()%(26));
            //tempn+=(char)(custname+97);
            //custname = (rand()%(26));
            //tempn+=(char)(custname+97);
            //custname = (rand()%(26));
            //tempn+=(char)(custname+97);
            //insert+=tempn;
            insert+=cname;
            insert+=' ';

            int category = (cat);
            ostringstream str23;
            str23 << category;
            temp = str23.str();
            insert+=temp;
            insert+=' ';

            insert+='\n';
            file << insert;
          }
        inFile.close();
        string endfile = "-2 end";
        file<<endfile;
        file.close();
}


void InsertElement(){
  cout<<"Enter T-Id CustomerName|Spaced\n";
  int Tid;
  string Cname;
  cin>>Tid>>Cname;
  insert(make_pair(Tid, Cname));
  cout<<"\n";
}




int main(){
  cout<<"Enter Max Address Table Size.\n";
  cin>>maxATSize;
  cout<<"Enter Bucket Size.\n";
  cin>>maxBucketSize;
	//cout<<maxATSize<<maxBucketSize;

  struct Block* trial = new Block;//creating address table block having 0 entries at start
  trial->isAT = 1;
  trial->ATSize = 0;
  trial->followupAT = NULL;
  addressTable = trial;

  vector< pair < int , string > > vectR;
  pair<int, string> record;
 // int x;
  //cout<<"Enter 0 to Create files from 0 to 100000?\nEnter 1 to Read from file \"data.txt\"\n";
  //cin>>x;
FileCreateData();
  //if(x==0) FileCreate();
  FileSplit();
  ifstream inFile;
  string filename="1.txt";
  int readflag=1;
  while(readflag){

    inFile.open(filename);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int tid, tsale, cat;
    string cname;
    while (inFile >> tid ) {
    	if(tid==-1){inFile >> filename;  }
        else if(tid==-2){ readflag=0;  break; }
        else{
        inFile >> tsale >> cname >> cat;
        //cout<<tid<<tsale<<cname<<cat<<"\n";

        record = make_pair(tid,filename);
        vectR.push_back(record);
        //insert(record);
        //cout<<GlobalDepth;
    	}
    }
    inFile.close();

  }

  sort(vectR.begin(), vectR.end());

  for(int i=0; i<vectR.size(); i++){insert(vectR[i]);}

  forThetaTemp = vectR;
  vectRS = vectR;
  //exit(0);

  //for(int i=0; i<100000; i+=1){
    //cout<<GlobalDepth;
    //int jk = rand()%100000;
    //cout<<jk<<" ";
    //record =  make_pair(jk,"m");
    //insert(record);
    //record =  make_pair(250000-i,"marhip");
    //insert(record);
  //}
    cout<<"\nGD="<<GlobalDepth<<"\n\n";

//cout<<"number of chained overflow buckets: "<<METACHAIN<<endl;
int omega = (pow(2,GlobalDepth)/maxATSize) - 1;
//cout<<"number of chained address Tables "<<omega<<endl;
float bucketsAccessedAvg = (float)METACHAIN/addressTableIndex + 1;
//cout<<"average number of chained overflow buckets per hash prefix + 1: "<<bucketsAccessedAvg<<endl;



//LOADING THE SECONDARY INTO CODE
cout<<"\nEnter the Block Factor for Secondary memory implementation: "<<endl;
int SecondaryBlockFactor;
cin>>SecondaryBlockFactor;
Xhashcreate(SecondaryBlockFactor);  //WILL CREATE SECONDARY MEMORY STRUCTURE

cout<<"------------------For Histogram and evaluating Theta---------------"<<endl;
cout<<"Enter the minimum Element Possible(starting value of histogram range): "<<endl;
int A;
cin>>A;
cout<<"Enter the maximum Element Possible(ending value of histogram range): "<<endl;
int B;
cin>>B;
cout<<"Enter the width of records for histogram(gap between 2 entries of the graph): "<<endl;
int d;
cin>>d;
cout<<"Inputs for Query: Select * From Transaction Where TID is between X and Y"<<endl;
int X,Y;
int wannaGoOut;
int predictedExtendible;
int predictedSecondary;
int predictedNaive;
//LOOP for range query Testing
while(1){

cout<<"Enter the value of X : "<<endl;
cin>>X;
cout<<"Enter the Value of Y: "<<endl;
cin>>Y;
int theta;
theta = calcTheta(A,B,d,X,Y);
cout<<"actual number of records as per Histogram "<<theta<<endl;
predictedExtendible = theta+((Y-X+1)*(omega/2))+((Y-X+1)*bucketsAccessedAvg);
cout<<"Predicted Weight of Extendible hashing on Query is: "<<predictedExtendible<<endl<<endl<<endl;

predictedSecondary= height+(theta/SecondaryBlockFactor)+1+theta;
cout<<"Predicted Weight of Secondary Memory Implementation on Query is: "<<predictedSecondary<<endl<<endl<<endl;

predictedNaive = naivecodeEmptyReturn(X,Y);
cout<<"Predicted Weight of Naive Implementation on Query is: "<<predictedNaive<<endl<<endl<<endl;

        cout<<"press 1 to see Block Accesses of from all the implemented Methods Or Press 2 to go with our Predictor's result"<<endl;
        cin>>wannaGoOut;
if(wannaGoOut==2){//if pressed 2 then go out of while loop
    break;
}
cout<<totalBlocksRead(X,Y,0)<<" is the total number of block accesses for Entendible Hashing "<<endl;
cout<<XhashSearch(X,Y,0)<<" is the total number of block accesses for Secondary Implementation"<<endl;
cout<<naivecodeEmptyReturn(X,Y)<<" is the total number of block accesses for Naive Implementation"<<endl;

}//infiniteWhile loop ends

if(predictedExtendible<predictedNaive && predictedExtendible<predictedSecondary){
cout<<totalBlocksRead(X,Y,1)<<" is the total number of block accesses for Entendible Hashing "<<endl;
}

if(predictedSecondary<predictedNaive && predictedSecondary<predictedExtendible){
cout<<XhashSearch(X,Y,1)<<" is the total number of block accesses for Secondary Implementation"<<endl;
}

if(predictedNaive<=predictedExtendible && predictedNaive<=predictedSecondary){
cout<<naivecode(X,Y)<<" is the total number of block accesses for Naive Implementation"<<endl;
}



    //visualize();
  /*int cases=1;
  while(cases){


    cout<<"Choices: 1. Search. 2. Visualize 3. Insert \n";
    cin>>cases;

    switch(cases){
      case 1:
      Search();
      break;

      case 2:
        cout<<GlobalDepth<<"\n";
      visualize();
      break;

      case 3:
      InsertElement();
      cout<<"GD= "<<GlobalDepth<<"\n";
      break;

      default:
      break;
    }
  }
  */

}



int calcTheta(int A, int B, int d, int X, int Y){

int z;
int maximumElementPossible = B;
int minimumElementPossible = A;
if(Y>B){
    Y= B;
}
if(X>B){
    X=B;
}
if(X<A){
    X=A;
}
if(Y<A){
    Y=A;
}
if(X>Y){
    int swaptemp = X;
    Y = X;
    X = swaptemp;
}
int widthOfHisto = d;
int numDivisions = (B-A)/d;

int plot[numDivisions]= {};

int plotIndex;
int TID;
for(int i=0; i<forThetaTemp.size(); i++){  //looping thru file line by line
    //TID has the TID value of the file line
    TID = forThetaTemp[i].first;
    plotIndex = (TID - A)/d;
    plot[plotIndex] = plot[plotIndex] + 1;
}
int Start,End;
int theta = 0;
if((X-A)%d == 0)
    Start = (X-A)/d;
else
    Start = ((X-A)/d) +1;

End = (Y-A)/d;

int b = (Start*d) + A;
int c = (End*d)+ A;

for(int i=Start; i<End; i++){
    theta = theta+plot[i];
}
theta = theta+ (plot[End]*(Y-c))/d;

theta = theta + (plot[(X-A)/d]*(b-X))/d ;

return theta;

}

//for Secondary inplementation

void XhashC(){
  ofstream file;
  file.open("dataS.txt", std::ofstream::out | std::ofstream::trunc);
  file.close();
  file.open("dataS.txt");

  for(int i=0; i<vectRS.size(); i++){
    ostringstream str1;
    str1 << vectRS[i].first;;
    string s = str1.str();
    s+=" ";
    s+= vectRS[i].second;
    s+="\n";
    file<<s;
  }
  file.close();

}

void Xhashread(char s){ //read from file and put in vector
  ifstream inFile;
  pair<int, string> record;
  string filename(1, s);
  //cout<<"fname="<<filename;
  //string filename=(string)s;
  filename+="1.txt";
  int readflag=1;
  //cout<<"fname="<<filename;
  vectRS.clear();
  while(readflag){

    inFile.open(filename);
    if (!inFile) {
        cout << "Unable to open fileblabla";
        exit(1); // terminate with error
    }

    int tid;
    string fname;
    int it=0;
    while (inFile >> tid ) {
      it++;
    	if(tid==-1){inFile >> filename;  }
        else if(tid==-2){ readflag=0;  break; }
        else{
        inFile >> fname;
        //cout<<tid<<tsale<<cname<<cat<<"\n";

        record = make_pair(tid,filename);
        if(it==1) vectRS.push_back(record);

        //insert(record);
        //cout<<GlobalDepth;
    	}
    }
    inFile.close();

  }

}

void Xhashcreate(int x){ // x is split value
    ofstream file;
      //cout<<"Enter the size of each file record.\n";
      //cin>>x;
    ifstream inFile;
    XhashC();
    char whichchar = 'a';
    while(1){

      string filename="dataS.txt";
      //cout<<filename;
      int readflag=1;
      inFile.open(filename);
      if (!inFile) {
          cout << "Unable to open files1";
          exit(1); // terminate with error
      }
      int i=1;
      int tid;
      string fname;
      string FileName;
      ostringstream str1;
      str1 << i;
      string number(1, whichchar);
      number+=str1.str();
      number+=".txt";
      //cout<<number;
      file.open(number);
      int records=-1;

        while (inFile >> tid >> fname) {
            records++;
            if(records>=x){
              i++;
              records=0;


              ostringstream str12;
              str12 << i;
              string number1(1, whichchar);
              number1+=str12.str();
              number1+=".txt";
              string end = "-1 ";
              file<<end;
              file<<number1;
              file.close();
              file.open(number1);
            }

            ostringstream str2;
            str2 << tid;
            //j++;
            string temp = str2.str();
            string insert;
            insert+=temp;
            insert+=' ';



            string tempn;
            //int custname = (rand()%(26));
            //tempn+=(char)(custname+97);
            //custname = (rand()%(26));
            //tempn+=(char)(custname+97);
            //custname = (rand()%(26));
            //tempn+=(char)(custname+97);
            //insert+=tempn;
            insert+=fname;
            insert+=' ';



            insert+='\n';
            file << insert;
          }
        inFile.close();
        string endfile = "-2 end";
        file<<endfile;
        file.close();

        if(vectRS.size()<=x) break;
        Xhashread(whichchar);
        whichchar++;
        height++;
        XhashC();

       }

}
int XhashSearch(int X, int Y, int myFlag){  //will print on myFlag set to 1 and will not when set to 0
   int countrecords=0;
  char c = 'a' + height;
  string s(1,c);
  //cout<<"S is "<<s<<" ";
  int ffl0=0; //flag for level 0
  int prevtid;
  string prevfname;
  string fname;
  int pfnames = 0;
  int count=1;
  int readuloop=1;
  int ffll=1; //flag for last level
  while(readuloop){
    ifstream inFile;
    string filename=s+"1.txt";
    if(pfnames) filename = prevfname;
    int readflag=1;
    while(readflag){
      //cout<<filename;
      ffll=1;
      inFile.open(filename);
      if (!inFile) {
          cout << "Unable to open file";
          exit(1); // terminate with error
      }

      int tid;

      while (inFile >> tid ) {
        //cout<<filename;

          if(filename[0]=='a'){ ffl0=1; }
      	  if(tid==-1&&filename[0]=='a'){inFile >> filename; count++;}
          else if(tid==-1&&filename[0]!='a'){ pfnames = 1; readflag=0;}
          else if(tid==-2&&filename[0]=='a'){ readflag=0; readuloop=0; break; }
          else if(tid==-2&&filename[0]!='a'){ readflag=0; pfnames=1; }
          else{

          inFile >> fname;
          if(ffll){
            prevtid = tid;
            prevfname = fname;
            ffll=0;
            //cout<<"prevtid = "<<prevtid<<" here";
           }

          if(ffl0==0){
            if(tid>X){pfnames = 1; readflag=0;  }
            else{
              prevtid = tid;
              prevfname = fname;
            }
          }

          else{
            //cout<<"lal";
            if(X<=tid&&tid<=Y){ if(myFlag!=0){RecordDisplay(tid,fname);cout<<endl;} countrecords++; count++; }
            else if(tid>Y){ readflag=0; readuloop=0; break; }
          }
          if(readuloop==0) break;
      	}
      }
      inFile.close();

    }
  }
  //  cout<<"\nActual number of records found = "<<countrecords<<"\n";
  return (count+height);
}

int naivecode(int X, int Y){
  ifstream inFile;
  string filename="1.txt";
  int readflag=1;
  int nob=0; //no. of blocks
  while(readflag){

    inFile.open(filename);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int tid, x,y;
    string fname;
    while (inFile >> tid ) {
    	if(tid==-1){inFile >> filename; nob++; }
        else if(tid==-2){ readflag=0;  break; }
        else{
        inFile >> x >> fname >> y;
        //cout<<tid;
        if(tid>=X&&tid<=Y){RecordDisplay(tid,filename);cout<<endl;  }
        //cout<<tid<<tsale<<cname<<cat<<"\n";

        //record = make_pair(tid,filename);
        //vectR.push_back(record);
        //insert(record);
        //cout<<GlobalDepth;
    	}
    }
    inFile.close();

  }
  return nob+1;
}

int naivecodeEmptyReturn(int X, int Y){
countglna=0;
  ifstream inFile;
  string filename="1.txt";
  int readflag=1;
  int nob=0; //no. of blocks
  while(readflag){

    inFile.open(filename);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int tid, x,y;
    string fname;
    while (inFile >> tid ) {
    	if(tid==-1){inFile >> filename; nob++; }
        else if(tid==-2){ readflag=0;  break; }
        else{
        inFile >> x >> fname >> y;
        //cout<<tid;
        if(tid>=X&&tid<=Y){ countglna++; }
        //cout<<tid<<tsale<<cname<<cat<<"\n";

        //record = make_pair(tid,filename);
        //vectR.push_back(record);
        //insert(record);
        //cout<<GlobalDepth;
    	}
    }
    inFile.close();

  }
  cout<<"\n Actual records found for extendible = "<<countglna<<"\n";
  cout<<"\n Actual records found for Secondary = "<<countglna<<"\n";
  cout<<"\n Actual records found for Naive = "<<countglna<<"\n";
  return nob+1;
}

//function to cout info

void RecordDisplay(int tid, string fname){
  ifstream inFile;
  string filename=fname;


    inFile.open(filename);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int tidcurr, x,y;
    string cname;
    while (inFile >> tidcurr ) {
    	if(tid==-1){ break; }
        else if(tid==-2){ break; }
        else{
        inFile >> x >> cname >> y;
        if(tidcurr==tid) cout<<"\n"<<tid<<" "<<x<<" "<<cname<<" "<<y;
    	}
    }
    inFile.close();
}

