# Tragic: A Card Game

This is a Command Line Game that was developed for the Procedural Programming exam at the University of Perugia.

## Official Soundtrack

You can find the game's Official Soundtrack (composed by me) [here](https://www.youtube.com/watch?v=AW660BojWzU)

## Building the application

To build the application you will need GCC. Then:

- Compile `main.c` and `gamelib.c` separately

```bash
gcc -c main.c
gcc -c gamelib.c
```

- Link `main.o` and `gamelib.o`

```bash
gcc -o game main.o gamelib.o -std=c11 -Wall
```
