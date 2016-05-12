import socket
import os
import sys
import commands
import getpass

LOGIN = "11"
CHECKSTATUS = "12"
CHAT = "13"

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
# *************************************************
def userLogIn():

    print "*********************************"
    print "**   Welcome to What's Chat!   **"
    print "*********************************"
    print ""

    while (True):
        username = raw_input("Username: ")
        password = getpass.getpass("Password: ")

        accountInfo = str(username) + ";" + str(password)
        accountInfoSize = str(len(accountInfo))

        header = prepareHeader(accountInfoSize)

        sendAll(clientSock, LOGIN + header + accountInfo)

        if clientSock.recv(2) == "1":
            print "Welcome back, " + username + "!"
            return True
        else:
            print "Wrong credentials. Please try again."
            print ""

    # Should never get here
    return False

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

    userLogIn()

    clientSock.close()
