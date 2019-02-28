---
layout: default
---

# Assignment 2: Parallel 3-SAT with MPI

#### Due: March 12 (Tuesday) by 11:00pm

#### Project Template: [p2-template.tar.gz]({{ site.baseurl }}/assignments/p2/p2-template.tar.gz)

#### Table of Contents

1. [Purpose](#1-purpose)
2. [Description](#2-description)
3. [Hacker Challenge](#3-hacker-challenge)
4. [Running and Testing](#4-running-and-testing)
5. [Submission and Grading](#5-submission-and-grading)
6. [Tips](#6-tips)

## 1. Purpose

- To practice parallel programming with MPI
- To face some of the challenges with encoding data in lower-level languages

## 2. Description

Boolean satisfiability is a famous problem in computer science (you likely have heard of it). In the most general sense, boolean satisfiability asks the question of whether we can assign the variables in a boolean formula with truth values such that the evaluation of formula is true. We call this assignment of variables with truth values an *interpretation* of a boolean formula, and if one exists for a formula F, we say that F is *satisfiable*. For example, consider the boolean formula ``(x /\ y) \/ !z``, where conjunction is represented with ``/\``,  disjunction with ``\/``, and negation with ``!``. There are several interpretations that will satisfy this formula: ``[x:True,y:True,z:Any]`` or ``[x:False,y:False,z:False]``. However, consider the formula ``(x /\ !x)``. There is no assignment of ``x`` that will result in this formula evaluating to true; hence, ``(x /\ !x)`` is not satisfiable.

Boolean satisfiability (SAT for short) was the first problem to be proven to be NP-Complete; we do not know an efficient algorithm for finding an interpretation that satisfies a given boolean formula. In other words, the only way we can find an true interpretation for a formula, or confidently state that one does not exist, is to try every combination of variable assignment (O(2^n) where n is equal to the number of variables). As a result, SAT makes a great candidate for us to tackle with parallel programming and MPI because an efficient serial algorithm does not exist.

### Restricted 3-SAT

Strictly speaking, we will work with 3-SAT, a restricted version of the boolean satisfiability problem---although still NP-complete---where a boolean formula must be in *conjunctive normal form*: 

``(x \/ y \/ z) /\ (a \/ b \/ c)``

We call the variables *literals*, and the group of variables joined by disjuction a *clause*. Essentially, a formula in *conjunctive normal form* consists of clauses joined by conjunection, where each clause consists of at most three literals joined by disjuction. For our purposes, we will restrict a clause to exactly three literals but a formula may consist of an arbitrary number of clauses.

You will use MPI to search for a solution for a boolean formula in parallel. 

### Part 1 : SAT Logic

I am providing a framework for working with 3-SAT formulas. Most of the work you will do for this assignment involves the communication to MPI workers, but you will be exposes to some C data encoding.

#### 3-SAT Formulas in C


The following code will represent a boolean formula in C:

```
typedef enum { AND, OR, NEG, VAR } connective;

typedef struct formula {
  union {
    struct {
      struct formula *f;
      struct formula *next;
    } land; // Represents conjunction (x /\ y)
    struct {
      struct formula *f1;
      struct formula *f2;
      struct formula *f3;
    } lor; // Represents disjunction (x \/ y)
    struct {
      struct formula *f;
    } lneg; // Represents negation (!x)
    struct {
      int lit;
    } lvar;
  };
  connective conn; // Stores the type of actual formula
} formula;
```

If you recall, the union allows us to represent different values as one type in C (you might use inheritance in an object oriented language). The general trend is to perform a switch on ``conn`` which indicates the type of connective, and then operate on the formula using that ``struct`` inside the union...

```
void pretty_print(formula *f) {
  switch (f->conn) {
    case AND:
      pretty_print(f->land.f);
      if (f->land.next != NULL) {
        printf(" /\\ ");
        pretty_print(f->land.next);
      }
      break;
    case OR:
      printf("(");
      pretty_print(f->lor.f1);
      printf(" \\/ ");
      pretty_print(f->lor.f2);
      printf(" \\/ ");
      pretty_print(f->lor.f3);
      printf(")");
      break;
    // ...
}
```

See how I check the type of f with a switch, and then treat f as that type when I use it? Furthermore, I recursively traverse over the sub formulas. For example, if ``f->conn`` is ``AND``,  I know to access the ``land`` field inside the union. If ``f->conn`` is ``OR``, I should access the ``lor`` field inside the union. Of course, there is nothing stopping you from accidently treating an ``AND`` formula as an ``OR`` (reading ``lor`` instead of ``land``) so you need to be careful---welcome to C programming.

#### Assignments to 3-SAT Formulas

We have to have some way to *assigning* truth values to the variables in our boolean formula. The most generic way to do this is to keep a mapping between variable names and boolean values. In Java, you would use a ``Map``, in Python, you would use a ``dictionary``. We would have to manually write a map to do this in C, which is beyond the purpose of this class (although makes for a great general purpose interview question). Instead, we can use the bucket technique discussed in class to hack a map together instead.

Instead of savings boolean variables as strings in our formula, we will just an integer to represent a variable instead.  For example, I can write ``(x /\ y /\ z)`` as ``(0 /\ 1 /\ 2)``. Now, I will simply keep an array of boolean values (ints in C) where each index represents the truth value assigned to the variable of that position, i.e., if I want to assign 0 to T, 1 to False, and 2 to True in the formula ``(0 /\ 1 /\ 2)``, I would create an array of size 3 with values ``int *arr = {0,1,1}``, then ``arr[0]`` gives ``0``, ``arr[1]`` gives ``1`` etc.

We will encapsulate this in a struct called ``assignment``:

```
typedef struct assignment {
  int *map; // map of variable ints to assigned values
  int size; // size of mapping
} assignment;
```
To simply some of this work, I have provided a function ``assignment* make_assignment(formula *f)`` that will traverse a formula, find the highest valued variable, and return an assignment with space allocated for each variable in ``map``, and the size of ``map`` in ``size``. By default, all variables will be mapped to false (zero'd array). The following is some pseudocode for manipulating an assignment:

```
formula *f = decode(some_str);

assignment *a = make_assignment(f);

// Assuming f represents (0 \/ 1 \/ !2), set the assignment to represent (T \/ T \/ F)
a->map[0] = 1;
a->map[1] = 1;
a->map[2] = 0;

```
#### Interpreting a Formula with an Assignment (Searching for Solutions, Requirements)

So far we have discussed how to encode 3-SAT formulas and assignments for them in C. Regarding searching for an assignment that satisfies a given formula, there are two features that you must implement:

1. Given a ``formula *f``, and an ``assignment *a``, **check** if ``a`` satisfies ``f``. We called this interpretation before, so write a function, ``int interpret(formula *f, assignment *a)`` that traverses the formula and interprets (evaluates) ``f``.

2. Given a ``formula *f``, you need to **generate** all possible assignments and check each to see if one satisfies the formula. 


### Part 2 : MPI Search

Once you know that you can check if an assignment satisfies a formula, and you know how to generate assignments, it's time to use MPI to perform this work in parallel.

We discussed a number of functions for communication in class, e.g, ``MPI_Send``, ``MPI_Recv``, ``MPI_Bcast``, ``MPI_Scatter``, ``MPI_Gather``, and there are several more available with a little documentation digging (you won't need to do that, although you are certainly welcome to). The specifics of how you implement your parallel search is up to you, but there is one technical hurdle you will need to overcome.

Our formulas are allocated on the heap using pointers; however, remember that each individual process in the MPI process group **does not share memory**. This means that you can't simply copy a formula into a buffer to MPI_Send, as the pointed to memory does not exist on the workers. This is a very common problem with any kind of non-shared memory model, and one that you need to handle. Essentially, you have to figure out a way to encode a formula into data that can get passed along to a worker.

In reality, this is non-trivial task, so I will help you. One way to do this is to encode the formula into a string, pass the string to the worker, and then decode the formula using the supplied ``decode`` function. One nice side effect of using a string-based encoding is that the encoding is human-readable which helps for debugging. For assignments, the ``map`` field can be sent along just fine (since it is simply an allocated array of int), but sending the whole ``assignment`` struct is a little trickier. If you want to send assignments, I recommend sending the data contained in ``map`` in one message, and then sending the size of ``map`` in a second message.

Here are the two features you must implement for the MPI search part of this assignment.

1. You must implement some form of encoding to encode a ``formula *f`` to something that can be sent from master to worker. Likely, write a function, ``char* encode(formula *f)`` that will return a string representing the encoded formula.

2. You **must** search for assignments that satisfy a formula in parallel. What I am looking for is that you distribute work across workers. A naive implementation might generate all possible assignments on the master, and continuously send an assignment to a worker to get checked. A better solution would push the generation of assignments to each worker, and have the master simply synchronize all workers.

## 3. Running and Testing

Your code will need to search for solutions to a series of formulas. I have provided a templated ``main.c``: It will load a file containing a series of formulas, and return encoded ``forumula`` until none are left for you to search. For each formula, your program (master) should print 1 if its satisfiable, and 0 if its not.

One thing you'll notice is that I have provided a header file "util.h", and a pre-compiled "libutil.a" that implements some utility functions, including ``decode``, and ``nextFormula``. While I have left the formula file in text format for ease of use for testing, you are not guaranteed that the input files to the program will always be in this format. What this means is, you must gather formulas from the ``nextFormula`` function to be solved. Furthermore, if any process other than master calls ``nextFormula``, it will result in the current formula being consumed early, and you will have two processes with two different formulas. Translated: I am forcing you to read formulas only from the master process, and distribute them to workers.

#### Program Output

For each formula received from ``nextFormula``, simply print ``0`` if the formula is not satisfiable, and ``1`` if it is, followed by a newline (each output line contains either a 0 or 1).

For example, a file with the following input:

```
(0 \/ 1 /\ 2)
(0 \/ 0 \/ 0) /\ (!0 \/ !0 \/ !0)
(0 \/ 0 \/ 1)
```

should give the following output:

```
1
0
1
```

For your final submission, please make sure **ONLY** the specified output is printed, and nothing else.

#### Starting from the provided template

I am providing a templated project for you to work in. 

The following files:

- ``sat.h / sat.c`` : Functions for working with the formula and assignment data structures.

- ``util.h / libutil.a`` : Functions for decoding formula from a string representation and parsing a formula file.

- ``main.c`` : Templated main driver for you to work from.

- ``verify`` : Binary that implements a serial version of assignment generation and interpretation for you to compare against. Run ``./verify simple.txt`` to see how it works.

- ``simple.txt`` : A formula file that contains a couple of simple test cases.

The following make targets:

- ``make sat`` : Builds a "library" out of the sat.c code for other programs to link against. ``sat.h`` and ``sat.c`` encapsulate the boolean satisfiability logic that I have provided for you to use.
- ``make main`` : Builds the main program. Inside the main function, I have put the necessary code to read all formulas from a supplied formula file. Without MPI, you may run with ``./main formula.txt``. 

#### Running your code with MPI

When you are ready to convert your application to an MPI one, you will need to compile main.c with ``mpicc`` and run with ``mpirun``.

#### Using the verifier

To help you test your solution, I have provided a compiled program called ``verify``. It functions the same as your program should (except that it is only a serial search, no MPI / mpirun needed). If you would like to check solutions against some expected output, you may use ``verify``. 

## 4. Hacker Challenge

Our formula encoding favors readability over performance. Essentially, we have created an abstract syntax tree in C for 3-SAT formulas. Traversing these abstract syntax trees, while somewhat painful due to C union syntax, is actually quite straightforward. However, restrictions (my 3-SAT requires each clause to have exactly 3 literals) lead to optimizations, and we can certainly optimize our 3-SAT encoding in terms of both space and time.

Because each clause must have exactly 3 literals, we don't really need a full AST (although our full AST is more general). Consider the formula ``(0 \/ 1 \/ 2) /\ (0 \/ 1 \/ 2)``. We could encode this as an array of int, where each entry represents the variable at that location in the formula ``[0,1,2,0,1,2]``. Now, we know that every 3 entries represents an OR clause, and then the next three entries represents the next OR clause, with an AND between them. So if I see ``[0,5,1,0,0,0,1,2,3]``, I know it represents ``(0 \/ 5 \/ 1) /\ (0 \/ 0\/ 0) /\ (1 \/ 2 \/ 3)``. Of course, this current encoding does not allow us to encode negation. For that, we could keep a separate array where each entry represents whether the variable is negated or not.

Whats nice about this encoding is that you can send it to workers AND perform interpretation on it directly. For the hacker challenge, go ahead and implement this encoding in place of the string encoding (you will have to translate from formulas to this encoding) and perform your interpretation using this format directly.

## 5. Submission and Grading

### Submitting

You will create a tar of all your code (you can simply re-tar up the provided template) to submit on blackboard. The following sequence of commands will make a directory for you to work in, and re-tar up that directory for submission. Note that you should change ``acanino1`` to your PODS username.

```
mkdir project1_acanino2
mv PATH/TO/p2-template.tar.gz project2_acanino1/
cd project2_acanino1
tar -cvvf project2_acanino1.tar *
gzip project1_acanino2.tar
```
You should then submit ``project2_acanino1_tar.gz`` on blackboard.

### Fitting the template

Issuing a ``make`` from your submission should compile all necessary files to run ``main``. 

Please include a ``README`` in your submission that contains any notes you have for me about your solution, and if you have attempted the hacker challenge.

### Grading

Here are some points about grading:

- Make sure your code compiles on ``remote.cs.binghamton.edu``. If it does not compile, I can't assume very much about whether you were running the code and learning from the assignment. If you are that stuck on the assignment, you should come see me during office hours, schedule extra help, and email me. 

- (limited) partial credit will be given to a solution  gets serial interpretation working.

- A partially correct solution will use MPI to perform parallel satisfiability search.

- A 100% correct solution will use MPI to perform parallel satisfiability search.

- The project will be graded out of 100 points. Hacker challenge will award up to 20 bonus points (20%).  

## 6. Tips

#### Encoding / Decoding Formulas

Look at the ``pretty_print`` function to more or less see how take a formula and encode it back as a string. This function prints directly to output, so you will have to write an ``encode`` version which prints to some buffer. There are many ways to do this in C. Search ``sprintf``, ``strcat``. Given that this is C, you need to be aware of buffer overflows. I recommend you write a function, ``strcatr(char **dest, char *src)`` which will resize ``dest`` if there is not enough space to safely fit ``dest`` + ``src`` in ``dest`` (the concatenated string).

#### Generating Assignments

This may seem like a complex task, but it is actually rather straightforward. Think back to binary arithmetic: If you continuously add 1 to the first digit in a number, you "generate" all binary numbers. If you instead view the binary number as an array of 1s and 0s, you can use a similar technique to generate all possible assignments---you will have to use a ``for`` loop here however, as there is no way to naturally overflow in an array.

The trick is how to split this generation apart so you can search in parallel. Again, think back to binary. Say we have 4 variables in our formula, that gives 2^4 combinations of assignments, so a max value of 16 (all 1s). Divide by 2, and you get 0-8, and 8-16, or 0000-1000, and 1000-1111. Pass these ranges to the workers and generate and try solutions on the workers.
