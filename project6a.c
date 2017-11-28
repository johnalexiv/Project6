//
//  project6a.c
//  Project6
//
//  Created by John Alexander on 11/11/17.
//  Copyright © 2017 John Alexander. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

int sharedValue = 0;
int readCount = 0;
int writeCount = 0;
int numOfWriters = 0;
int numOfReaders = 0;
pthread_mutex_t dbMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readMutex = PTHREAD_MUTEX_INITIALIZER;

struct Reader
{
    int id;
    pthread_t thread;
};

struct Writer
{
    int id;
    pthread_t thread;
};

bool isValidNumOfArguments(int, char*[]);
struct Reader *initializeReaders();
struct Writer *initializeWriters();
void waitForReaders(struct Reader *);
void waitForWriters(struct Writer *);
void readerFunction(void *);
void writerFunction(void *);
void readerAccessDatabase(struct Reader *reader);
void readerReadDatabase(struct Reader *reader);
void readerLeaveDatabase(struct Reader *reader);  
void writerAccessDatabase(struct Writer *writer);
void writeToDatabase(struct Writer *writer);
void writerLeaveDatabase(struct Writer *writer);

int main(int argc, char *argv[])
{
    if( !isValidNumOfArguments(argc, argv) )
    {
        printf("Invalid input.\nProper use: ./a.out <# of readers> <# of writers>\n");
        exit(0);
    }

    struct Writer *writers = initializeWriters();
    struct Reader *readers = initializeReaders();

    waitForReaders(readers);
    waitForWriters(writers);

    return 0;
}

bool isValidNumOfArguments(int argc, char *argv[])
{
    if ( argc == 3 )
    {
        sscanf(argv[1], "%d", &numOfReaders);
        sscanf(argv[2], "%d", &numOfWriters);

        if ( numOfReaders < 0 || numOfWriters < 0 )
            return false;
    }

    return true;
}

struct Reader *initializeReaders()
{
    int i;
    struct Reader *readers = (struct Reader *)calloc(numOfReaders, sizeof(struct Reader));

    for ( i = 0; i < numOfReaders; i++ )
    {
        pthread_t thread;
        readers[i].id = i;
        readers[i].thread = thread;
    }

    for ( i = 0; i < numOfReaders; i++ )
        pthread_create(&readers[i].thread, NULL, (void *)readerFunction, &readers[i]);

    return readers;
}

struct Writer *initializeWriters()
{
    int i;
    struct Writer *writers = (struct Writer *)calloc(numOfWriters, sizeof(struct Writer));

    for ( i = 0; i < numOfWriters; i++ )
    {
        pthread_t thread;
        writers[i].id = i;
        writers[i].thread = thread;
    }

    for ( i = 0; i < numOfWriters; i++ )
        pthread_create(&writers[i].thread, NULL, (void *)writerFunction, &writers[i]);

    return writers;
}

void readerFunction(void *argument)
{
    struct Reader *reader = (struct Reader *)argument;

    readerAccessDatabase(reader);
    readerReadDatabase(reader);
    readerLeaveDatabase(reader);    

    pthread_exit(NULL);
}

void writerFunction(void *argument)
{
    struct Writer *writer = (struct Writer *)argument;
    usleep(250);

    writerAccessDatabase(writer);
    writeToDatabase(writer);
    writerLeaveDatabase(writer);
    
    pthread_exit(NULL);
}

void readerAccessDatabase(struct Reader *reader)
{
    pthread_mutex_lock(&readMutex);
    
    printf("Reader %d is entering the database\n", reader->id);
    readCount++;
    if ( readCount == 1 )
        pthread_mutex_lock(&dbMutex);

    pthread_mutex_unlock(&readMutex);
}

void readerReadDatabase(struct Reader *reader)
{
    usleep(200);
    printf("Reader %d read value from database %d\n", reader->id, sharedValue);
}

void readerLeaveDatabase(struct Reader *reader)
{
    pthread_mutex_lock(&readMutex);

    readCount--;
    if ( readCount == 0 )
        pthread_mutex_unlock(&dbMutex);

    pthread_mutex_unlock(&readMutex);
    printf("Reader %d is leaving database\n", reader->id);
}

void writerAccessDatabase(struct Writer *writer)
{
    pthread_mutex_lock(&dbMutex);
    printf("Writer %d is entering the database\n", writer->id);
}

void writeToDatabase(struct Writer *writer)
{
    sharedValue++;
    printf("Writer %d updade value to %d\n", writer->id, sharedValue);
}

void writerLeaveDatabase(struct Writer *writer)
{   
    pthread_mutex_unlock(&dbMutex);
    printf("Writer %d is leaving database\n", writer->id);
}

void waitForReaders(struct Reader *readers)
{
    int i;
    for ( i = 0; i < numOfReaders; i++ )
        pthread_join(readers[i].thread, NULL);
}

void waitForWriters(struct Writer *writers)
{
    int i;
    for ( i = 0; i < numOfWriters; i++ )
        pthread_join(writers[i].thread, NULL);
}

/*
Most of the time writers will write before readers are initialized and can get into
the critical section to read the variable. At times when readers do get in, multiple 
readers will often get into the critical section to read the variable
before any writers can get in. I am able to produce these results by inserting
usleep times into the writer thread function and reader thread function. Where
the usleep in the writer function is before the writer can access the critical
section, and the reader usleep is inserted after locking the shared variable to
allow multiple readers into the critical section to read the shard variable at 
the same time.

sample output:
Writer 1 is entering the database
Writer 1 updade value to 1
Writer 1 is leaving database
Reader 0 is entering the database
Writer 0 is entering the database
Writer 0 updade value to 2
Writer 0 is leaving database
Writer 2 is entering the database
Writer 2 updade value to 3
Reader 2 is entering the database
Reader 0 read value from database 3
Reader 0 is leaving database
Writer 2 is leaving database
Reader 1 is entering the database
Reader 2 read value from database 3
Reader 2 is leaving database
Reader 1 read value from database 3
Reader 1 is leaving database
Reader 3 is entering the database
Writer 3 is entering the database
Writer 3 updade value to 4
Writer 3 is leaving database
Reader 3 read value from database 4
Reader 3 is leaving database
*/










