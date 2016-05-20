import socket
import os
import sys
import commands
import getpass
import cPickle
import select
import re
import rsa

FAIL = "00"
OK = "01"

LOGIN = "11"
CHECKSTATUS = "12"
INVITE = "13"
CHAT = "14"
CHECKCHATMEM = "15"
MEMBEROFFLINE = "16"

USER_PRIVATE_KEY = ""
with open("server_public_key.pem") as publicfile:
    pkeydata = publicfile.read()
    
SERVER_PUBLIC_KEY = rsa.PublicKey.load_pkcs1(pkeydata)

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
# Handle log in
# ************************************************
def userLogIn():

    while (True):
        username = raw_input("Username: ")
        password = getpass.getpass("Password: ")

        accountInfo = str(username) + ";" + str(password)
        accountInfoSize = str(len(accountInfo))

        header = prepareHeader(accountInfoSize)

        sendAll(clientSock, LOGIN + header + accountInfo)

        if clientSock.recv(2) == OK:
            print "Welcome back, " + username + "!"
            print ""
            return True, username
        else:
            userChoice = raw_input("Wrong credentials. Try again? (y/n) ")
            if userChoice.lower() == "n":
                return False, ""
            print ""

# ************************************************
# Handle checking online users
# ************************************************
def checkOnlineUser(clientSock):
    
    dataSizeBuff = recvAll(clientSock, 10)
    dataSize = int(dataSizeBuff)
    
    serializedOnlineList = recvAll(clientSock, dataSize)
    onlineUserList = cPickle.loads(serializedOnlineList)
    
    print "Online users:"

    for user in onlineUserList:
        print user
    print ""

# ************************************************
# Function to add header to data
# ************************************************
def preparePacket(data):
    
    dataSize = str(len(data))
    header = prepareHeader(dataSize)
    return header + data

# ************************************************
# Process after user successfully logged in
# @param sock: socket used to connect to a server
# *************************************************
def process(sock, username):

    input = [sock, sys.stdin]
    
    # *************************************************
    #           WHILE LOOP
    # *************************************************
    running = 1
    while running:
        
        inputready,outputready,exceptready = select.select(input,[],[])

        for s in inputready:

            if s == sock:
                # Receive data from the server
                # get the reponse code
                response = s.recv(2)

                # Server sent the list of online users
                if response == CHECKSTATUS:
                    checkOnlineUser(s)

                # Server sent chat message from other users
                if response == CHAT:
                    dataSizeBuff = recvAll(s, 10)
                    dataSize = int(dataSizeBuff)
                    msg =  recvAll(s, dataSize)

                    print msg + "\n"

                # Server sent notification about offline user
                if response == MEMBEROFFLINE:
                    print s.recv(100) + "\n"

                # Server responded to chat invitation
                if response == INVITE:
                    messageSizeBuff = recvAll(s, 10)
                    messageSize = int(messageSizeBuff)

                    # Print the message from server
                    print recvAll(s, messageSize)

            
            elif s == sys.stdin:
                # Handle standard input
                msg = sys.stdin.readline().strip()

                # User wants to quit
                if msg == "::quit":
                    running = 0
                    break

                # User wants to check who are online
                elif msg == "::online":
                    sendAll(sock, CHECKSTATUS)

                # User wants to invite online users to chat
                elif msg.startswith('::invite'):
                    names = ""
                    matchObj = re.match( r'::invite (.*)', msg)
                    if not matchObj:
                        print "Invalid command"
                    else:
                        names = matchObj.group(1)
                        if len(names) > 0:
                            sendAll(sock, INVITE + preparePacket(names))
                        
                # Other messages
                else:
                    message = msg.strip()

                    msg = username + ": " + msg
                    
                    if message:
                        sendAll(sock, CHAT + preparePacket(msg))


# ************************************************
# Direction
# ************************************************
def directionMenu():

    print "**************************************************************************"
    print "*****                INSTRUCTION                                     *****"
    print "***** Type ::online to check online users                            *****"
    print "***** Type ::invite name1,name2... to invite online users to chat    *****"
    print "***** Type ::quit to quit the program                                *****"
    print "***** Otherwise, any input message will be chat message              *****"
    print "**************************************************************************"
    print "\n"

def setUserPrivateRSAKeys(username):
    with open(username + "_private_key.pem") as privatefile:
        keydata = privatefile.read()

    return rsa.PrivateKey.load_pkcs1(keydata,'PEM')

if __name__ == "__main__":

    if len(sys.argv) > 1:
        print "Usage: python client.py"
        exit(-1)

    serverAddr = "localhost"
    serverPort = 1234

    # Create a TCP socket
    clientSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect to the server
    clientSock.connect((serverAddr, serverPort))
    
    print "*********************************"
    print "**   Welcome to What's Chat!   **"
    print "*********************************"
    print ""

    successSignIn, username = userLogIn()

    if successSignIn:
        print "Successfully logged in"
        directionMenu()
        setUserPrivateRSAKeys(username)
        USER_PRIVATE_KEY = setUserPrivateRSAKeys(username)
        process(clientSock, username)
    else:
        print "Bye!"

    clientSock.close()
