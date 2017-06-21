# Homework #4

### Two-Player Networked Othello (Reversi) Game

In this homework, we are going to implement a simple othello (reversi) game that allows two players play an othello game over the Internet. For the rules of the game, please refer to the links [English](https://en.wikipedia.org/wiki/Reversi) [Chinese](https://zh.wikipedia.org/wiki/%E9%BB%91%E7%99%BD%E6%A3%8B) 

The scenario of this homework is as follows. Suppose our program name is **othello**. We can have player #1 served as the server and player #2 served as the client. Player #1 launches the server using the command

``` sh
$ ./othello -s 12345
```

To launch the server and wait for the connection from player #2 via port number 12345. Player #2 then launches the same program, but acts as a client, to connect to the server. The command for player #2 is:

``` sh
$ ./othello -c <IP-of-Player-1>:12345
```

A simple demo for the initialization phase is shown in the below two figures (left: server; right: client).

<p><img src="http://i.imgur.com/4Okfhi9.png" width="45%"/>
<img src="http://i.imgur.com/VypGNJH.png" width="45%"/></p>

Once connected, the game starts by displaying the game board and the players can move the "cursor" to select the place to put a piece ("disk" or "disc"). As shown in the below two figures.

<p><img src="http://i.imgur.com/7edJtYN.png" width="45%"/>
<img src="http://i.imgur.com/zAhCZpP.png" width="45%"/></p>


To simplify your implementation, the game board can be drawn using the **ncurses** library (libncurses5-dev package on Ubuntu). **We have provided sample codes to draw game board and display messages, so you can focus on the implmentation of program architecture, game logics, and network communications**. The sample codes can be found here: Makefile, othello.h, othello.c, and sample.c. It can be used to generated a simple board as well as handling basic cursor movements and keyboard interactions. If your developing environment is ready (e.g., it works well on linux1.cs.nctu.edu.tw), you can simply type 'make' to build and then run the sample. The sample output is shown in the below figure. For more details, please see the live demo in the class.

### Grading Policy

The grading policy for this homework is listed below:

- [10%] Your program can act as either a server (player #1) or a client (player #2) by using the respective command arguments.
- [10%] A server has to wait for a client connection.
- [10%] A client can connect to the given server (IP address or host name).
- [10%] Once connected, display the game board. The game always starts from player #1 (server).
- [10%] Player can only put pieces (discs) on valid places (see game rule).
- [10%] Display correct number of pieces on the game board for the both players.
- [20%] Implement the rest of game logics.
- [10%] When there is no more moves, display a message to show the player wins or loses.
- [10%] Ensure the both two players have the same view of game board. 
If either the client or the server quits, the peer has to be terminated as well.

### Hints

Here we provide a number of hints for implementing this homework.

- You may use multi-thread, non-blocking I/O, or asynchronous I/O techniques to implement network communications.
- Work with a text-based network protocol design may simplify your implementations.
- Wrap a socket descriptor with FILE data structure may simplify your message processing implementations.
- If your box drawing display is not correct, you may try to launch this program in terminals created by "screen" or "tmux".
