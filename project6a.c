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
pthread_mutex_t dbMutex, readMutex;

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

int main(int argc, char *argv[])
{
    if( !isValidNumOfArguments(argc, argv) )
    {
        printf("Invalid input.\nProper use: ./a.out <# of readers> <# of writers>\n");
        exit(0);
    }

    struct Reader *readers = initializeReaders();
    struct Writer *writers = initializeWriters();

    waitForReaders(readers);
    waitForWriters(writers);

    return 0;
}

bool isValidNumOfArguments(int argc, char *argv[])
{
    if ( argc == 3 )
    {
        sscanf(argv[1], "%d", &readCount);
        sscanf(argv[2], "%d", &writeCount);

        if ( readCount < 0 || writeCount < 0 )
            return false;
    }

    return true;
}

struct Reader *initializeReaders()
{
    int i;
    struct Reader *readers = (struct Reader *)calloc(readCount, sizeof(struct Reader));

    for ( i = 0; i < readCount; i++ )
    {
        pthread_t thread;
        readers[i].id = i;
        readers[i].thread = thread;
    }

    for ( i = 0; i < readCount; i++ )
        pthread_create(&readers[i].thread, NULL, (void *)readerFunction, &readers[i]);

    return readers;
}

struct Writer *initializeWriters()
{
    int i;
    struct Writer *writers = (struct Writer *)calloc(writeCount, sizeof(struct Writer));

    for ( i = 0; i < writeCount; i++ )
    {
        pthread_t thread;
        writers[i].id = i;
        writers[i].thread = thread;
    }

    for ( i = 0; i < writeCount; i++ )
        pthread_create(&writers[i].thread, NULL, (void *)writerFunction, &writers[i]);

    return writers;
}

void readerFunction(void *argument)
{
    struct Reader *reader = (struct Reader *)argument;

    printf("Reader: %d\n", reader->id);
    pthread_exit(NULL);
}

void writerFunction(void *argument)
{
    struct Writer *writer = (struct Writer *)argument;

    printf("Writer: %d\n", writer->id);
    pthread_exit(NULL);
}

void waitForReaders(struct Reader *readers)
{
    int i;
    for ( i = 0; i < readCount; i++ )
        pthread_join(readers[i].thread, NULL);
}

void waitForWriters(struct Writer *writers)
{
    int i;
    for ( i = 0; i < writeCount; i++ )
        pthread_join(writers[i].thread, NULL);
}













