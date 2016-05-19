import socket
import re
import commands
import os
import sys
import select
import cPickle

FAIL = "0"
OK = "1"

LOGIN = "11"
CHECKSTATUS = "12"
INVITE = "13"
CHAT = "14"
CHECKCHATMEM = "15"
MEMBEROFFLINE = "16"

# List of online users
onlineUsers = {}

# List of chat members
chatMemberList = []

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
# Function to add header to data
# ************************************************
def preparePacket(data):

    dataSize = str(len(data))
    header = prepareHeader(dataSize)
    return header + data

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

# ************************************************
# Broadcast message to all of members in chat room
# @param sock - the socket which sent the message
# @message - the message to be sent to all members
# *************************************************
def broadcastMessage(sock, message):
    for m in chatMemberList:
        # Send the message only to peers
        if m != sock :
            try :
                m.send(CHAT + onlineUsers[sock].getName() + ": " + message)
            except :
                # Broken socket connection
                m.close()

# ****************************************************
# Notify all users in the chat session about an user
# just gone offline
# @param name: name of user that has just gone offline
# ****************************************************
def notifyOfflineMember(name):
    for m in chatMemberList:
        # Send notify to peers
        try:
            m.send(MEMBEROFFLINE + name + " is offline")
        except:
            # Broken socket connection
            m.close()

# ****************************************************
# Handle a client that is trying to log in
# @param sock: client's socket
# @param listOfAccounts: all valid accounts on the system
# ****************************************************
def handleLogin(sock, listOfAccounts):
    accountInfo = getAccountInfo(sock)
    status = False
    print "User at socket {0} is trying to log in".format(sock)
    for user in listOfAccounts:
        status = user.verifyAccount(accountInfo)
        if status:
            user.setSocket(sock)
            onlineUsers[sock] = user
            break

    if status:
        print "Successfully logged in"
        sock.send(OK)
    else:
        print "Failed to log in"
        sock.send(FAIL)

# ****************************************************
# Handle user's request to check all online users
# @param sock: user's socket
# ****************************************************
def handleCheckOnlineUsers(sock):
    print "Check online users..."

    onlineUserList = []

    for u in onlineUsers:
        print onlineUsers[u].getName()
        onlineUserList.append(onlineUsers[u].getName())

    serializedOnlineList = cPickle.dumps(onlineUserList)
    sendAll(sock, CHECKSTATUS + preparePacket(serializedOnlineList))

# ****************************************************
# Handle chat messages sent from an user
# @param sock: user's socket
# ****************************************************
def handelChatMessages(sock):
    print onlineUsers[sock].getName() + " sent: ",
    getMessageSizeBuff = recvAll(sock, 10)
    getMessageSize = int(getMessageSizeBuff)

    # Get the message
    getMessage = recvAll(sock, getMessageSize)
    print getMessage

    # Send the message to everyone in the chat session
    broadcastMessage(sock, getMessage)

# ****************************************************
# Handle chat invitaion
# @param sock: user's socket
# ****************************************************
def handelInvitation(sock):
     # Add user to chatMemberList
    if not sock in chatMemberList:
        chatMemberList.append(sock)

    dataSizeBuff = recvAll(sock, 10)
    dataSize = int(dataSizeBuff)

    # Get list of names
    listOfNames = recvAll(sock, dataSize)

    Names = []
    if len(listOfNames) > 0:
        Names = listOfNames.split(',')

    found = False
    message = ""
    for name in Names:
        print "User invites {0} to join chat session".format(name)
        # Check name with online users
        for u in onlineUsers:
            if onlineUsers[u].getName() == name.strip():
                found = True
                break

        # Found the user
        if found:
            if not u in chatMemberList:
                message = "{0} is added to the chat session".format(onlineUsers[u].getName())
                chatMemberList.append(u)
                sendAll(sock, INVITE +  preparePacket(message))

                # Notify user that is added to session
                sendAll(u, INVITE + preparePacket("{0} invited you to the chat session".format(onlineUsers[sock].getName())))
            else:
                message = "{0} is alreay in the chat session".format(onlineUsers[u].getName())
                print message
                sendAll(sock, INVITE +  preparePacket(message))
        else:
            message = name + " is not registered"
            
            for user in listOfAccounts:
                if user.getName() == name:
                    message = name + " is not online"
                    break

            sendAll(sock, INVITE +  preparePacket(message))

        found = False


    # Print out list of chat members
    print "\nList of chat members:"
    for m in chatMemberList:
        print onlineUsers[m].getName()

# ****************************************************
# Main of the server
# ****************************************************
if __name__ == "__main__":

    # Create some accounts
    listOfAccounts = []
    listOfAccounts.append(User("Duy", "abc", 0))
    listOfAccounts.append(User("Billy", "123", 0))
    listOfAccounts.append(User("Holy", "456", 0))

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
                # Handle the server socket
                client, address = welcomeSock.accept()
                input.append(client)

            elif s == sys.stdin:
                # Handle standard input
                junk = sys.stdin.readline()
                running = 0

            else:
                # Handle all other sockets
                request = getRequest(s)

                # User sent log in information
                if request == LOGIN:
                    handleLogin(s, listOfAccounts)

                # User requested to check who are online
                elif request == CHECKSTATUS:
                    handleCheckOnlineUsers(s)

                # User sent messages to other members
                elif request == CHAT:
                    handelChatMessages(s)

                # User sent invitation to online users to chat
                elif request == INVITE:
                    handelInvitation(s)

                # User's socket is closed
                else:
                    s.close()
                    input.remove(s)
                    if s in chatMemberList:
                        chatMemberList.remove(s)
                        notifyOfflineMember(onlineUsers[s].getName())
                    if s in onlineUsers:
                        del onlineUsers[s]

    # Close server socket
    welcomeSock.close()
