---
layout: default
---

# Assignment 0

## Purpose

To familiarize yourself with remote work for during class sessions and testing and running assignments. For those of you who have a POSIX-like operating systems (any linux distro, macOS) feel free to install any necessary software and work locally; however, I will use remote.cs.binghamton.edu as the reference environment for grading.

## Description

I expect you to follow along with code examples and work on short problems during class. This includes *compiling* and *running* code, not simply writing things down. I want you to make mistakes and ask questions as you are learning the material in front of me, not only when working on the assignments.

The easiest way to follow along during class---working locally or remotely---is by using some form of terminal. At this point in your CS careers, you should be familiar with working in a terminal with some form of text editor. Interactive Development Environments (IDEs) like Visual Studio or XCode have their place, but so does rapid development inside the linux console. 

### 1. Establish ssh connection

This step will differ depending upon the operating system of your home machine. Regardless, you will need access to your CS LDAP account (username/password) to log in to the remote machines. What follows is the link to the [Binghamton CS sysadmin support page](https://www2.cs.binghamton.edu/~sysadmin/). If you can't access your account, you need to take steps to remedy this situation ASAP which pretty much means tell me and I will help, but that means tell me right away over email, not during class on Thursday.

You will likely see terms such as "host machine" or "hostname" tossed around. In general, for the host machine and host name, use ``remote.cs.binghamton.edu``. The username and password to log in to remote will simply be your LDAP username and password.

#### Windows Machines

If you're working from a windows machine, [PuTTY](https://www.putty.org/) is a well-known ssh client for sshing into a remote machine. You'll have to [download and install](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html). If you have never used PuTTY, [go ahead and follow these instruction](https://the.earth.li/~sgtatham/putty/0.70/htmldoc/Chapter2.html#gs-insecure) and shoot me an email if you have trouble logging on to ``remote``.

I will likely start classes with some form of templated code for you to download and ``push`` to the remote machine. On windows, [WinSCP](https://winscp.net/eng/index.php) serves this purpose.

#### Unix-like Machines

If you're working from macOS or linux, ``ssh`` should be installed on your system by default. You will need to work from a terminal regardless of your specific machine. On macOS, you want ``Terminal.app`` although you [may install / use any of the macOS terminal alternatives](https://www.macobserver.com/news/mac-terminal-app-alternatives/). On linux, you'll likely use the default terminal as well, [but feel free to install an alternative terminal](http://www.linuxandubuntu.com/home/10-best-linux-terminals-for-ubuntu-and-fedora).

Once inside the terminal, try sshing into ``remote`` with ``ssh YOUR_NAME@remote.cs.binghamton.edu`` and enter your password. For me, ``ssh acanino1@remote.cs.binghamton.edu`` will log me in. 

I will likely start classes with some form of templated code for you to download and ``push`` to the remote machine. On linux and macOS, ``scp`` serves this purpose. If you want to push ``file.c`` onto the remote machine under your home directoy, the following should work: ``scp file.c YOUR_NAME@remote.cs.binghamton.edu://home/YOUR_NAME/``, so for me, ``scp acanino1@remote.cs.binghamton.edu://home/acanino1/`` will do the trick.

### 2. Editor choice

For this class, you should get comfortable editing source *in the terminal you compile and run code from*. Sending files back and forth to a remote server each time you need to make an edit and recompile is inefficient, and I highly recommend against that for this class. If nothing else, treat this sort of terminal-focused work as alternative development style---most software development companies expect you to be able to work this way anyways.

Everyone should have *some* text editor that they are familiar and efficient with. It also helps to have working knowledge of some editor that runs natively in a terminal for remote work. Some of your options on ``remote`` are ``vim``, ``emacs``, and ``pico``. I recommend either ``vim`` or ``emacs``. I will make it a point to teach tricks for working effectively in linux, but for now, pick something and stick with it to learn it enough to navigate and work during class---you don't need to become an expert. I am familiar with both ``vim`` and ``emacs``, so feel free to ask questions about both during class as you work.

### 3. Quick check

As a sanity check, make sure you can compile a simple C hello world example on ``remote``:

(1) Create a file name ``hello.c``.

(2) Enter the following code:

```
#include <stdio.h>

int main(int argc, char **argv) {
  printf("Hello, world!\n");
  return 0;
}
```

(3) Compile the code with gcc. This will create a file called ``a.out``.

```
gcc hello.c
./a.out
```

(4) You should see ``Hello, world!`` on the terminal.

## Submission

Nothing to submit, just make sure you have followed the steps in the description, and be ready to work during class on Thursday.

