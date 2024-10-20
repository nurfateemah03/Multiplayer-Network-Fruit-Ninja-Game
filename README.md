# README

## CS/IMGD 411x Multiplayer Network Games

## Project 4 - Latency and Ping

## Overview
This project aimed to analyze and implement latency compensation techniques in a multiplayer game similar to Fruit Ninja, 
where players slice moving objects (fruit) using a virtual sword controlled by mouse movements. In multiplayer online games,
latency—the delay between a player's input and the corresponding game response—can severely impact gameplay experience and performance.
The primary focus of this project was to measure how latency affects players’ ability to interact with the game and explore methods to 
mitigate its negative effects using various latency compensation techniques
## how to run
You can run in microsoft visual studio by launching the server first then client with fn +f5. Enter local host when prompted for client.

## Issues
Sometimes if you move your mouse too fast then the trail glitches

## How I implemented things 
Ping Synchronization: The server and client exchange ping messages to measure latency and adjust the game accordingly. 
On the client side, a custom event sends a "PING" message to the server with a timestamp. The server responds by echoing the message back, 
and the client calculates the round-trip time based on the time difference between when the ping was sent and received. The latency is
displayed on the client’s screen, helping the player understand the connection quality.
On the server side, when it receives a "PING" message, it extracts the timestamp, prepends "PING" to it, and sends it back to the
client. The client then calculates the round-trip latency and updates the on-screen display using a Ping object.

Sword Synchronization: Each player controls a sword, and the server maintains authority over the sword’s final position. However,
to reduce input delay for the controlling player, client-side prediction is used, allowing the player to see their sword respond to
mouse movements immediately, without waiting for server confirmation. This involves the client sending sword movement updates to the
server while locally moving the sword for the player. The server then synchronizes the sword's position with all other clients, ensuring 
consistency across all players' screens.

The server uses the sendMessage() function to broadcast the sword’s updated position to all clients except the one controlling the sword, ensuring smooth synchronization for non-controlling players.

Fruit Synchronization: Fruits are created by the server, and their positions are synchronized across all clients. When a new fruit is created, 
the server calculates the latency to the client and extrapolates the fruit’s position based on its velocity and the measured latency. This ensures
that the fruit appears at its predicted location on the client’s screen, reducing the impact of network lag. The server continually updates the fruit’s
position to ensure it remains synchronized across all clients as the game progresses

## link to video
https://drive.google.com/file/d/15I1rQkUliVfMqTkRQBYnWxmL_l4EoyhT/view?usp=sharing 



