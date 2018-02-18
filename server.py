#!/bin/python
# Cite from https://docs.python.org/release/2.6.5/library/socketserver.html
# This program serves as the server side for a TCP chatroom

from socket import *
import sys

#call the chatroom
def chatroom(sockfd, clientname, username):
    while 1:
	receiver = sockfd.recv(501)[0:-1]
        # if we received nothing, print connection closed and close connection
        if receiver == "":
            print "Connection closed"
            print "Waiting for new connection"
            break
        # print the clients name with their message
        print "{}> {}".format(clientname, receiver)
        # get server input and send back to client
        sender = ""
        while len(sender) == 0 or len(sender) > 500:
            sender = raw_input("{}> ".format(username))
        # send it to the client if the message is not \quit
        if sender == "\quit":
            print "Connection closed"
            print "Waiting for new connection"
            break
        sockfd.send(sender)


if __name__ == "__main__":

    if len(sys.argv) != 2:
        print "You must specify the port number for the server to run"
        exit(1)
    serverport = sys.argv[1]
    serversocket = socket(AF_INET, SOCK_STREAM)
    serversocket.bind(('', int(serverport)))
    # listen on the port for incoming messages
    serversocket.listen(1)
    username = ""
    while len(username) == 0 or len(username) > 10:
        username = raw_input("Please enter a user name of 10 characters or less: ")
    print "The server is ready to receive incoming messages"
    while 1:
        # create a new socket for a chatroom
        connectionSocket, address = serversocket.accept()
        # print the chatroom address
        print "received connection on address {}".format(address)
        clientname = connectionSocket.recv(1024)
	connectionSocket.send(username)
        chatroom(connectionSocket, clientname, username)
        connectionSocket.close()