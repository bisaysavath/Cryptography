
import socket
import re
import commands
import os
import sys
import select

FAIL = "0"
OK = "1"

LOGIN = "11"
CHECKSTATUS = "12"
CHAT = "13"

class User:

    def __init__(self, name, password, sock):
        self.__name = name
        self.__password = password
        self.sock = sock
        
    def getName(self):
        return self.__name

    def verifyAccount(self, info):
        a = info.split(";", 1)

        if len(a) != 2:
            return False
        else:
            name = a[0]
            password = a[1]
            if name == self.__name and password == self.__password:
                return True
            else:
                return False

            
    def setSocket(self, sock):
        self.sock = sock

# ************************************************
# Receives the specified number of bytes
# from the specified socket
# @param sock - the socket from which to receive
# @param numBytes - the number of bytes to receive
# @return - the bytes received
# *************************************************

def recvAll(sock, numBytes):

    # The buffer
    recvBuff = ""

    # The temporary buffer
    tmpBuff = ""

    # Keep receiving till all is received
    while len(recvBuff) < numBytes:

        # Attempt to receive bytes
        tmpBuff =  sock.recv(numBytes)

        # The other side has closed the socket
        if not tmpBuff:
            return 0

        # Add the received bytes to the buffer
        recvBuff += tmpBuff

    return recvBuff

# ************************************************
# Send data to the specified socket
# @param sock - the socket from which to send
# @param fileData - the data will be sent
# @return - Total bytes sent
# *************************************************
def sendAll(sock, fileData):

    # The number of bytes sent
    totalSent = 0

    # Send the data
    while len(fileData) > totalSent:
        sent = sock.send(fileData[totalSent:])
        if sent == 0:
            # socket broken
            return 0
        totalSent += sent
    return totalSent

# ************************************************
# Create a header with 10 bytes
# *************************************************
def prepareHeader(header):

    # Prepend '0' to make header 10 bytes
    while len(header) < 10:
        header = "0" + header

    return header

# ************************************************
# Get the request from client
# @param sock - the socket from which to receive
# @return - the request
# *************************************************
def getRequest(sock):

    # The request command is the first 2 bytes of the data
    request = recvAll(sock, 2)    
    return str(request)

# ************************************************
# Get the account information from client
# @param sock - the socket from which to receive
# @return - the account information
# *************************************************
def getAccountInfo(sock):

    # Get fileNameSize
    accountSizeBuff = recvAll(sock, 10)    
    accountSize = int(accountSizeBuff)

    # Get fileName
    accountInfo = recvAll(sock, accountSize)
    return accountInfo

    
if __name__ == "__main__":

    # Create some accounts
    listOfAccounts = []
    listOfAccounts.append(User("Duy", "abc", 0))
    listOfAccounts.append(User("Eric", "123", 0))

    # List of online users
    onlineUsers = {}
    
    #if len(sys.argv) != 2:
    #    print "Usage: python server.py <PORT NUMBER>"
    #    exit(-1)
    
    # The port on which to listen
    #if not sys.argv[1].isdigit():
    #    print "Port number is not valid"
    #    exit(-2)
    
    #listenPort = int(sys.argv[1])

    listenPort = 1234

    # Create a welcome socket
    welcomeSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Bind the socket to the port
    welcomeSock.bind(('', listenPort))

    # Start listening on the socket
    welcomeSock.listen(10)
    print "Server is listening at port: " + str(listenPort)

    input = [welcomeSock, sys.stdin]
    
    running = 1
    # *************************************************
    #           MAIN LOOP
    # *************************************************
    while running:
    
        inputready,outputready,exceptready = select.select(input,[],[])

        for s in inputready:

            if s == welcomeSock:
                # handle the server socket
                client, address = welcomeSock.accept()
                input.append(client)

            elif s == sys.stdin:
                # handle standard input
                junk = sys.stdin.readline()
                running = 0

            else:
                # handle all other sockets
                request = getRequest(s)
                if request == LOGIN:
                    accountInfo = getAccountInfo(s)
                    print accountInfo
                    status = False
                    for user in listOfAccounts:
                        status = user.verifyAccount(accountInfo)
                        if status:
                            user.setSocket(s)
                            onlineUsers[s] = user
                            break

                    for u in onlineUsers:
                        print u
                        print onlineUsers[u].getName()
                    print status
                    if status:
                        s.send(OK)
                    else:
                        s.send(FAIL)
             
                else:
                    s.close()
                    input.remove(s)
                    if s in onlineUsers:
                        del onlineUsers[s]

    # Close server socket
    welcomeSock.close() 

