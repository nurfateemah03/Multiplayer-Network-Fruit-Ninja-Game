#
# Makefile for game using Dragonfly
#
# Copyright Mark Claypool and WPI, 2024
#
# 'make depend' to generate new dependency list
# 'make clean' to remove all constructed files
# 'make' to build executable
#
# Variables of interest:
#   GAMESRC is the source code files for the game
#   GAME is the game main() source
#

#### Adjust these as appropriate for build setup. ###

HOME= /home/claypool
DF= $(HOME)/proj/dragonfly/dragonfly-src

### Uncomment only 1 of the below! ###

# 1) Uncomment below for Linux (64-bit)
DFLIB= -ldragonfly
SFMLLIB= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
LINKLIB= $(DFLIB) $(SFMLLIB) 
LINKDIR= -L$(DF) -L$(HOME)/src/SFML/lib   
INCDIR= -I$(DF) -I$(HOME)/src/SFML/include 

# 2) Uncomment below for Mac (64-bit)
#DFLIB= -ldragonfly-mac64
#SFMLLIB= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
#LINKLIB= $(DFLIB) $(SFMLLIB) 
#LINKDIR= -L/usr/local/Cellar/sfml/2.4.0/lib/ -L$(DF)
#INCDIR= -I/usr/local/Cellar/sfml/2.4.0/include/ -I$(DF)

######

CC= g++
CFLAGS= -std=c++17

LIBSRC= \
	util.cpp \

GAMSRC= \
	Fruit.cpp \
	GameOver.cpp \
	Grocer.cpp \
	Kudos.cpp \
	Points.cpp \
	Splash.cpp \
	Sword.cpp \
	Timer.cpp \

CLISRC= \
	Client.cpp \
	ServerEntry.cpp \

SRVSRC= \
	Server.cpp \

ENG= $(DF)/libdragonfly.a
CLI= fruit-client.cpp
SRV= fruit-server.cpp
CLIEXE= client
SRVEXE= server
CLIOBJ= $(CLISRC:.cpp=.o)
SRVOBJ= $(SRVSRC:.cpp=.o)
LIBOBJ= $(LIBSRC:.cpp=.o)
GAMOBJ= $(GAMSRC:.cpp=.o)

all: $(CLIEXE) $(SRVEXE)

$(CLIEXE): $(ENG) $(CLI) $(CLIOBJ) $(GAMOBJ) $(LIBOBJ) Makefile 
	$(CC) $(CFLAGS) -o $@ $(CLI) $(CLIOBJ) $(LIBOBJ) $(GAMOBJ) $(INCDIR) $(LINKDIR) $(LINKLIB) 

$(SRVEXE): $(ENG) $(SRV) $(SRVOBJ) $(GAMOBJ) $(LIBOBJ) Makefile
	$(CC) $(CFLAGS) -o $@ $(SRV) $(SRVOBJ) $(LIBOBJ) $(GAMOBJ) $(INCDIR) $(LINKDIR) $(LINKLIB) 

.cpp.o: 
	$(CC) -c $(INCDIR) $(CFLAGS) $< -o $@

clean:
	rm -f $(CLIEXE) $(SRVEXE) $(GAMOBJ) $(SRVOBJ) $(CLIOBJ) $(LIBOBJ) core *.log Makefile.bak *~

depend: 
	makedepend *.cpp 2> /dev/null

# DO NOT DELETE

Client.o: /usr/include/string.h Client.h ServerEntry.h util.h
Server.o: /usr/include/string.h Server.h
ServerEntry.o: ServerEntry.h
Sword.o: Sword.h util.h
fruit-client.o: Client.h
fruit-server.o: State.h util.h
util.o: Sword.h
