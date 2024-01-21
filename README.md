# TCP-IP-Hangman-Client-Server-Project

Embarking on a collaborative endeavor, our team successfully implemented a TCP/IP client-server program in C, crafting an engaging hangman game where the client interacts with the server.

Server:

-Processes the received port number and reads a designated .txt file serving as the game dictionary via the command line.

-Randomly selects a word from the dictionary, conveying essential details to the client—such as the word's length, its initial and final letters, and the allotted number of attempts for discovery.

-Listens for letter attempts from the client, providing feedback on the letter's position within the chosen word. For unsuccessful attempts, the server dynamically updates the hangman illustration and manages the remaining user attempts.

Client:

-Acquires the server's IP address and port number from the command line, establishing a connection to the server.

-Displays pertinent information provided by the server, prompting the user to input a letter for the hangman game.

-Dynamically updates the user interface, signaling victory or termination upon discovering the word or exhausting the allowable attempts, respectively.

This project underscores our collaborative proficiency in crafting a robust TCP/IP client-server solution in C, seamlessly integrating interactive hangman gameplay. The adept communication between the server and client, coupled with the dynamic management of game states, reflects our team's commitment to delivering an engaging and interactive user experience.
