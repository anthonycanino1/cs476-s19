---
layout: default
---

# Assignment 3: Readers and Writers in Go

#### Due: April 12 (Friday) by 11:00pm

#### Project Template: [p3-template.tar.gz]({{ site.baseurl }}/assignments/p3/p3-template.tar.gz)

#### Table of Contents

1. [Purpose](#1-purpose)
2. [Description](#2-description)
3. [Running and Testing](#3-running-and-testing)
4. [Submission and Grading](#4-submission-and-grading)

## 1. Purpose

- To practice concurrent programming with Go Channels

## 2. Description

The goal of this assignment is to parallelize reading data out of multiple files into a shared data structure using only channels for communication, while at the same time running queries against the data that has been read out so far. The driver file, emerging.go, has been provided in the assignment package. It depends on a file, cmap.go, which you must provide in this assignment. Queries can be of the basic GetCount to retrieve the number of times a word has been read across all files so far, or using a more complex Reduce call.

Detailed guidelines are provided below. Please note that mutexes and similar synchronization primitives are NOT allowed.

### Detailed Guidelines

The emerging.go driver takes as command line arguments: the number of readers, the number of askers, an “ask file” (for the askers to draw words from), an “askdelay,” for the time between asks, a directory containing the files for the readers to use, and a “reducedelay” for the time between reduce calls.  These flags are described in emerging.go.  The details of how emerging.go works can be largely glanced over.

The reader goroutines will read individual words out of a file and send each word to the shared map using AddWord after it is read.  The shared map must keep a count of how many times it has seen each word in a thread-safe manner. The asker goroutines will periodically ask the shared structure for the number of occurrences of a random word in the askfile using GetCount.  The structure cannot add words at the same time it is searching for a word's count. The reducer goroutines will infrequently request a functional reduce from the shared structure.  It will provide a function of type ReduceFunc when doing so. 

The EmergingMap interface is defined as:

```
type EmergingMap interface {
  Listen() // start listening for requests
  Stop() // stop listening, exit goroutine

  AddWord(word string) // increase count for given word
  GetCount(word string) int // retrieve count for given word

  // reduce over current words
  Reduce(functor ReduceFunc, accum_str string, accum_int int) (string, int)
}
```

ReduceFunc is defined in emerging.go as:

```
type ReduceFunc func(key1 string, val1 int, key2 string, val2 int) (string, int)
```

Create your implementation in package main, in the file cmap.go.  The structure will consist of several channels for asking, adding, and reducing, along with map[string]int.  Each of these channels must be buffered to the constant buffer sizes defined in emerging.go:  

```
const (
	ADD_BUFFER_SIZE = 32
	ASK_BUFFER_SIZE = 8
	REDUCE_BUFFER_SIZE = 1
)

```

There will also likely need to be a channel for terminating the ```Listen``` mainloop.  The functions associated with this structure should write requests into the channels, and the ```Listen()``` function will decided what action to take based on what channel has data in it using a ```select``` block.  ```Listen``` should loop infinitely until being told to stop by the ```Stop``` function.  Each case in Listen’s mainloop’s select should perform one task using the information it has read from the channel and possibly provide a response.

Your cmap.go file must include a function NewChannelMap() which returns a pointer to an object of your implemented structure. You may add extra types as needed but you must not change the interface.

As part of your programming tasks, you are asked to add one more query to emerging.go using ```Reduce``` to find the longest word in your ChannelMap. Currently, the ```emerging.go``` driver uses the ```Reduce``` function to find the most ocurring word in the ```EmergingMap```. You will need to follow this usage of ```Reduce``` to add the additional query.

Output will be handled by emerging.go. Regarding your additional query, follow the structure of ```Reduce```, ```reducer```, and ```max_word``` to add it to the output.

### Requirements

- Implement a type ```ChannelMap``` that satisfies the ```EmergingMap``` interface. You must use channels for your synchronization, and cannot use locks. As a reference, ```lmap.go``` builds a ```LockingMap``` that specifically uses locks and not goroutines/channels.

- Add an additional query that uses ```Reduce``` to find the longest word in the dictionary. You must add this to ```emerging.go```.

## 3. Running and Testing

#### Executing the code

You should run ```./emerging --help``` to see a list of available options. The following command will run a simple test of the code using my ```LockingMap``` implementation.

```
./emerging -lock -readers=2 -askers=2 -askdelay=10 -infiles=data/pg1041.txt,data/pg1103.txt
```

You can and should play around with these settings (see the next subsection). 

#### Timing runs

- Time the following runs of your program (along with the two reducers) with an askdelay of 10ms:

  - a. 1 reader, 1 asker

  - b. 16 readers, 2 askers

  - c. 4 readers, 8 askers

  - d. 16 readers, 32 askers

  - e. 64 readers, 64 askers

  An example of running with 2 readers, 2 askers, with askdelay of 10 (and other flags using defaults): ```./emerging -readers=2 -askers=2 -askdelay=10```

- Describe the results and how your implementation scales with more readers and more askers.  Briefly discuss your results.

### Starting from the provided template

I am providing a templated project for you to work in. 

The following files:

- ```Makefile``` : Contains a single ```emerging``` target that uses the go build system.

- ```emerging.go``` : Main driver for the code. 

- ```cmap.go``` : Contains skeleton stubs for you to implement your channel map version of an ```EmergingMap``` implementation.

- ```lmap.go``` : My implementation of a locking map implementation of ```EmergingMap```. You may use it as a reference. 

## 4. Submission and Grading

### Submitting

You will create a tar of all your code (you can simply re-tar up the provided template) to submit on blackboard. The following sequence of commands will make a directory for you to work in, and re-tar up that directory for submission. Note that you should change ``acanino1`` to your PODS username.

```
mkdir project1_acanino3
mv PATH/TO/p3-template.tar.gz project3_acanino1/
cd project3_acanino1
tar -cvvf project3_acanino1.tar *
gzip project3_acanino2.tar
```
You should then submit ``project3_acanino1_tar.gz`` on blackboard.

### Fitting the template

Issuing a ``make`` from your submission should compile all necessary files to run ``emerging``. 

### Grading

Here are some points about grading:

- Make sure your code compiles on ``remote.cs.binghamton.edu``. If it does not compile, I can't assume very much about whether you were running the code and learning from the assignment. If you are that stuck on the assignment, you should come see me during office hours, schedule extra help, and email me. 

- An implementation of ```ChannelMap``` that uses anything other than goroutines and channels for synchronization will receive 0 credit.
