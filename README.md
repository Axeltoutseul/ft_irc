*This project has been created as part of the 42 curriculum by axbaudri, quelefev, linehrha.*
 
# ft_irc
 
## Description
 
ft_irc is a fully functional IRC server written in C++98, built as part of the 42 school curriculum. The goal of the project is to implement a real IRC server that can handle multiple simultaneous client connections, following the IRC protocol (RFC 1459 / RFC 2812).
 
The server supports user authentication, channel management, and a set of standard IRC commands. It is compatible with real IRC clients such as irssi or WeeChat.
 
Key features:
- Non-blocking I/O using `poll()`
- Multiple simultaneous client connections on a single port
- User registration (PASS, NICK, USER)
- Channel creation and management
- Channel operator privileges
- Full support for KICK, INVITE, TOPIC, and MODE commands
 
## Instructions
 
### Compilation
 
```bash
make
```
 
### Execution
 
```bash
./ircserv <port> <password>
```
 
**Example:**
```bash
./ircserv 6667 mypassword
```
 
### Connecting with irssi
 
Install irssi:
```bash
sudo apt install irssi
```
 
Connect to the server:
```bash
irssi -c 127.0.0.1 -p 6667 -n <nickname> -w <password>
```
 
### Supported commands
 
| Command   | Description                                          |
|-----------|------------------------------------------------------|
| `PASS`    | Authenticate with the server password                |
| `NICK`    | Set or change nickname                               |
| `USER`    | Register user information                            |
| `JOIN`    | Join or create a channel                             |
| `PRIVMSG` | Send a message to a user or channel                  |
| `KICK`    | Eject a user from a channel (operators only)         |
| `INVITE`  | Invite a user to a channel                           |
| `TOPIC`   | View or change the topic of a channel                |
| `MODE`    | Change channel modes (i, t, k, o, l)                 |
 

## Resources
 
### IRC Protocol
- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) 
 