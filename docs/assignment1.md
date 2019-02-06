---
layout: default
---

# Assignment 1: Thread-Safe Memory Allocator

#### Due: February 15 (Friday) by 11:00pm

#### Project Template: [p1-template.tar.gz]({{ site.baseurl }}/assignments/p1/p1-template.tar.gz)

#### Table of Contents

1. [Purpose](#1-purpose)
2. [Description](#2-description)
3. [Requirements](#3-requirements)
4. [Tips](#4-tips)
5. [Hacker Challenge](#5-hacker-challenge)
6. [Submission and Grading](#6-submission-and-grading)

## 1. Purpose

- To practice concurrent / parallel programming in C
- To face some of the challenges associated with manual memory management

## 2. Description

You will write your own custom thread-safe memory allocator in C. While that may sound intimidating, note that the following assumption before panicking:

> Assumption #1: Threads will do not need to free any memory they acquire.

As such, you essentially need to provide a thread-safe bump pointer allocator, where your program grabs a chunk of memory from ``malloc``, and issues allocation requests from that chunk. You do not need to consider block size, fragmentation, etc.

### Bump Pointer Allocation

Bump pointer allocation is a technique for very efficient memory allocation for applications that load a bunch of data at the start of a program run that gets used during the entire application lifetime. An example of this would be a compiler, where parsed source code is transformed into an abstract syntax tree---essentially runtime data of static source code---that the compiler needs to use right up until the very end of program execution. A counter example would be a web server, where requests cause independent processing with relatively short lifetimes; if we did not release memory from these requests, the web server would crash eventually.

To implement a bump pointer allocator, you grab a large chunk of memory from ``malloc`` up front and save it in some ``void*``, lets call it ``top``. When the program needs memory, say for an ``int`` of 8 bytes, you return the memory the ``top`` currently points to, and *bump* ``top`` 8 bytes:

```
void *top = NULL;
size_t remaining = 0;

int main(void) {
  top = malloc(512);
  remaining = 512;
  
  // Allocate a pointer to a single int
  int *ip = top;
  top += sizeof(int);
  remaining -= sizeof(int);

  // Allocate a pointer to 10 ints (an array)
  int *arr = top;
  top += sizeof(int) * 10;
  remaining -= sizeof(int) * 10;
}
```

### Handling Requests of Unknown Size / Resizing Bump Pointer Allocator

One of the requirements (see below) for your allocator is that it can handle requests of any size. This is actually quite simple to implement: If a request comes in to ``my_malloc`` that is larger than the remaining size of the bump pointer block, just allocate a new, larger chunk of memory from ``malloc``. The slightly tricky part is that you need to save the previous top pointer so it can be freed in ``my_malloc_destroy``. Some pseudo code might help:

```
void* my_malloc(size_t s) {
  if (s > remaining) {
    // save top in some linked list, you know how to do this from class
    pushlive();

    top = malloc(s);
    remaining = s;
  }
  // allocate
}
```

### Multi-Threaded Arena Allocation

Writing a thread-safe allocator is easy: just lock the entire function with a mutex! Writing an *efficient* thread-safe allocator is significantly harder. If you look in the ``glibc`` project, the entire code for ``malloc`` (which is thread-safe) is incredibly complex, containing lines upon lines of carefully constructed functions to make ``malloc`` as efficient (and general) as possible. We're not going to do anything nearly as complex, but the general high-level idea that ``glibc`` uses will serve us nicely: divide the allocator into several, small allocators that each allocate from their own pool (top pointer in our case) of memory that are protected by their own locks. 

We will call each of these small allocators an *arena*. You should structure your code so that you create a number of arenas (call it ``num_arenas`` based upon the configuration in the API summary below) that each have their own lock to protect access. By structuring your allocator in this way, you essentially allow up to ``num_areans`` of threads to concurrently grab memory. 

### API Summary

The following functions are defined in the header file associated with your allocator and are part of the public API that you need to provide. 

- ``void my_malloc_init(size_t default_size, size_t num_arena)``
  - Initialize any data structures associated with your library and allocator. The two parameters are a pair of options to your allocator: the first, ``default_size``, defines the default size of memory your allocator grabs from ``malloc``. In the real world, this would be a larger number, for testing purposes, it will be small; the second, ``num_arena`` is the number of arenas of memory (and the level of concurrency) your allocator supports.

- ``void my_malloc_destroy(void)``
  - Free up all memory held by your allocator and reset state. A call to ``my_malloc_destroy`` will allow code to re-initialize your library.

- ``void* my_malloc(size_t s)``
  - Allocate a chunk of memory of size s, returned as a ``void*``. Note that users of your library do not need to and should not free this memory. 


## 3. Requirements

- 1. You must handle an allocation request of any size. If the request size is greater than the memory you have available in your allocator, you should allocate a new chunk of memory with ``malloc`` and save the previous chunk of memory onto some liveness list for your program to later free during ``my_malloc_destroy``.

- 2. Multiple threads must be able to call ``my_malloc`` concurrently. You will have to use strategies we discussed in class, along with ``pthread`` locks and condition variables to make ``my_malloc`` thread safe. Note that simply putting a lock around the ``my_malloc`` function will make it thread safe, it will also make it very slow, which I will test for. I will look for some variation of the arena strategy discussed above in your implementation.

- 3. Your code must release all allocated memory when ``my_malloc_destroy`` is called; furthermore, your code should not contain any *memory errors* when run with ``valgrind``. 

### 1. Starting from the provided template

I am providing a templated project for you to work in. In short, there is a Makefile, ``my_malloc.h``, ``my_malloc.c``, and ``driver.c``. 

The following make targets:

- ``make my_malloc`` : Builds a "library" out of your my_malloc.c code for other programs to link against. Note that you are not actually building a library, instead, it's just the compiled object code that is ready to be linked against other compiled code.

- ``make driver`` : Builds a full driver executable that contains test cases for your code. Execute with ``./driver``. 

- ``make`` : Defaults to ``make driver``.

The test cases are meant to provide both guidance in terms of expected behavior and API design; if your code does not compile and successfully run the test cases, it most certainly is wrong, but passing all the test cases does not guarantee it is correct. I am very likely to test your project with additional cases, and you should do so as well. 

#### A Short Point on Testing

There is some debate as to how much and at what level to test your code, *.i.e.*, should tests be written for each function (unit testing)? I personally believe in testing the *integration* of components within an application. For example, given a program with a known syntax error on line 5, does gcc produce the proper error message, or, if gcc is given a correct program, does the program run as expected? The former will insure proper parsing, the latter will insure proper code generation. What *is* to good testing is *automated testing*. At first, the examples for gcc seem very hard to automate, but once you gain some experience with writing test harnesses, these kind of tests actually get quite easy to write and maintain. I will likely demonstrate this in class at some point.

For this project, note that no matter how often or how many changes you make to your code, you will always know if you have taken a step backwards in that the test will fail. This is the true benefit of automated testing: You can focus on software development worry free because checking its health is just a keyboard click away. I know you (myself many times) have had a project where it was no longer fun to work on because you could no longer grasp what was and wasn't working. Writing and maintain *tests* are boring, I understand, but those boring tests make writing *software* much more fun.

### 2. Running your code with the driver

I have built the tester (``driver``) with 3 separate over-arching tests that are wrapped in the following functions called from ``main``:

1. ``test_sequential`` : Test various allocations with only the single, main thread. 

2. ``test_concurrent`` : Test various concurrent allocations with two threads.

3. ``test_concurrent_contention`` : Heavily load the allocator with 32 threads.

Each test is wrapped with its own pair of ``my_malloc_init`` and ``my_malloc_destroy``. Feel free to comment / uncomment individual tests to build your program up slowly. If the program runs without exiting early or seg faulting you may have the logic correct. The next step is to run things with ``valgrind`` to see if any memory is leaked or you have memory errors.

To run: ``./driver``
To run with valgrind: ``valgrind --leak-check=full -v ./driver``

## 4. Tips

### Getting Started

When I wrote my version of the project, I started by writing a sequential version of the allocator, and did not worry about nor design any of the multi-threaded part. Only after I passed all the ``test_sequential`` tests with valgrind did I move on to the concurrent version. The majority of the work of this project is getting the bump pointer allocation / allocator resizing working. Anytime you work heavily with memory in C, most of the latent bugs wont show until you go and try and free everything (or use valgrind).  

Adding the thread-safety part was fairly straightforward after I fixed all memory errors. My point is: Don't overwhelm yourself by trying to solve the entire problem first. Solve the easier problem which forces you to lay the groundwork for some simple refactoring and one new feature: thread-safety.

### Suggested Structures

I recommend the following types / data structures for your. You are free to do things however you please, so long as you match the API and test cases.

```
// Linked list of "live" memory segments (top pointers)
typedef struct {
} live;

// Track a memory segment available for allocation: use a top pointer
typedef struct {
} arena;

// Should hold all memory arenas, and necessary synchronization primitives
typedef struct {
} allocator;
```

### Global State

You will have to save some global state in your ``my_malloc.c`` that all of your code operates on. Global state is generally bad, and we often discourage it, but in the real world global state is used all the time. The original ``Go`` compiler used global state *extensively*. Newer languages generally have safer versions of global state; Java has class variables, Go has package variables etc. For this project, I want you to understand how C code is sometimes written. Consider the alternative for this project: We would need to make ``my_malloc_init`` return a ``struct`` that would then need to be passed around to each ``my_malloc`` call. Is that really better? Is it better when this kind of allocator is meant for a single program to allocate memory thread-safely anyways? 

You'll likely need something like this in your ``my_malloc.c``:

```
// ...
static allocator alloc;
```

In C, ``static`` declares a variable with *file scope*. It essentially means that only code in ``my_malloc.c`` can access ``alloc``, which offers some protection to the global state.

### Handling Arenas

We discussed some specific solutions in class for handling how threads access arenas. The specifics are up to you, however, I have two general strategies for you to use. 

The key is that multiple threads can *concurrently* access the arenas. Each arena likely needs some form of synchronization primitive (lock). Then, one strategy would be to continiously try all locks until one is grabbed. To do this, you may use ``pthread_try_lock`` which returns 0 if a thread grabs the lock, otherwise, a non-0 value is returned *imediately*, i.e,. ``pthread_try_lock`` is a non-blocking way to atomically grab a lock. For example:

```
pthread_mutex_t lock;

if (pthread_try_lock(&lock) == 0) {
  // have the lock
} else {
  // need to do something else
}
```

Another strategy is to use a threads idea as a form of index into a specific arena. To do this, you can use the following code:

```
pthread_t tid = pthread_self();
int index = tid % num_arenas;
```

This strategy is not platform-independent, but it works fine for the purpose of the assignment (pthread_t is a typedef'd int on ``remote.cs.binghamton.edu``).


## 5. Hacker Challenge

(NOTE: This section is for a challenging extension to the assignment, and will award extra credit points)

Right now the level of parallelism of our allocator is fairly static. For example, ``my_malloc_init(512,4)`` will work good if an application that uses our allocator has roughly 4-8 threads, but an application with 32 threads may start to cause contention on the locks protecting the memory arenas. If we know this ahead of time, we could instead initialize the allocator with ``my_malloc_init(512,32)``, but if the application spawns additional threads, or we use a different application with ``my_malloc``, we reach the same issue.

One solution to this problem is to dynamically increase the number of memory arenas of your allocator. Increasing the number of allocators is actually somewhat straightforward (we discussed two styles of this in class, the first is using a linked list, the second is using a vector that you dynamically resize), the challenging part is *when* to increase the number of arenas, i.e., what condition should cause a resize? For a multi-threaded allocator, *contention* of the locks is a good indicator. If threads continuously wait on memory arenas, it's likely time to fix something.

How you determine if threads *wait* on a lock is up to you, and the wait does not need to track *actual* waiting, but you should somehow track contention or potential contention, and then dynamically adjust your application when you determine that some contention is "too much". Again, "too much" can be your choosing, but a good technique would be to say if one memory arena reaches a certain contention level (say 25% of total contention) then go ahead an allocate a new memory arena.

If you attempt the hacker challenge, make sure to leave your regular solution in place. The simplest way to do this is to create a separate file, ``my_malloc_hacker.c`` and additional make target that outputs ``driver-hacker`` for you and me to run, and indicate in your ``README`` that you have attempted the challenge.

## 6. Submission and Grading

### Submitting

You will create a tar of all your code (you can simply re-tar up the provided template) to submit on blackboard. The following sequence of commands will make a directory for you to work in, and re-tar up that directory for submission. Note that you should change ``acanino1`` to your PODS username.

```
mkdir project1_acanino1
mv PATH/TO/p1-template.tar.gz project1_acanino1/
cd project1_acanino1
tar -cvvf project1_acanino1.tar *
gzip project1_acanino1.tar
```
You should then submit ``project1_acanino1_tar.gz`` on blackboard.

### Fitting the template

Issuing a ``make`` from your submission should compile all necessary files to run the ``driver``. If you attempt the hacker challenge, make sure to leave your working regular solution intact to ``make`` still builds the ``driver``.

Please include a ``README`` in your submission that contains any notes you have for me about your solution, and if you have attempted the hacker challenge.

### Grading

Given you have a tester to run, you'll know if your code works or not; furthermore, using valgrind will inform you if any memory is leaked, or more importantly, any memory errors are in your program. 

Here are some points about grading:

- Make sure your code compiles on ``remote.cs.binghamton.edu``. If it does not compile, I can't assume very much about whether you were running the code and learning from the assignment. If you are that stuck on the assignment, you should come see me during office hours, schedule extra help, and email me. 

- A functionally correct solution will pass all the test cases and show no memory leaks or errors using valgrind. 

- A partially correct solution will get the serial allocator working. 

- A 100% correct solution will use some form of memory arenas to allow for multiple threads to allocate using the allocator at the same time. 

- Any decent attempt at the hacker portion of the assignment will gain bonus points (it must at least run, but is ok if it crashes). 

- The project will be graded out of 100 points. Hacker challenge will award up to 20 bonus points (20%).  
