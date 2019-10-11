# Okami-Shell 1.5

The Okami Shell is a Unix-like command interpreter for the Atari ST, written 1989-1992.

Shell syntax mimics the Bourne shell. I/O redirection incl. here documents, pipelines and command substitution (simulated with temporary files since the Atari operating system didn't have pipes) is fully supported. Since, even with a harddisk, starting a program on the Atari was slow, most commands (cp, mkdir, grep, and about 100 more) are built into the shell. The shell wasn't designed to be strictly Unix-compatible; instead, some commands were adoped to the specialities of the Atari platform (for example, the escape character in `echo` is `^` instead of `\` because backslash is the path separator, and `chmod` flags match the TOS file system without attempting to recplicate the Unix r/w/x permission system). Later versions of the Okami Shell support the MiNT multitasking system.

Folder `Files` contains the unchanged contents of the Okami Shell 1.5 distribution, released 1992-09-20.

The shell is extensively documented (alas, mostly in German). Begin reading in `Files/DOC/README` (a plain text file), continue with `Files/DOC/OKAMI.DOC`. English readers please read `Files/DOC/ENGLISH.DOC`. Note that in the olden days .DOC was common for plain text files that contained documentation.

The shell was written in C. Unfortunately, the source code of the shell is lost.

---
*Wolfram Rösler • wolfram@roesler-ac.de • https://gitlab.com/wolframroesler • https://twitter.com/wolframroesler • https://www.linkedin.com/in/wolframroesler/*
