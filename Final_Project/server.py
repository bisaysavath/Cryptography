import socket
import re
import commands
import os
import sys
import select
import cPickle
import rsa

MAXIMUM_CHAT_MESSAGE_LEN = 1000

# Status Codes
FAIL = "00"
OK = "01"
LOGIN = "11"
CHECKSTATUS = "12"
INVITE = "13"
CHAT = "14"
CHECKCHATMEM = "15"
MEMBEROFFLINE = "16"
KEY = "17"

USER_PUBLIC_KEY = ""

# Load server private key
with open("server_private_key.pem") as privatefile:
    keydata = privatefile.read()
    
SERVER_PRIVATE_KEY = rsa.PrivateKey.load_pkcs1(keydata,'PEM')

# List of online users
onlineUsers = {}

# List of chat members
chatMemberList = []

# Random symmetric key used for chatting
randomKey = ""
    
class User:

    def __init__(self, name, password, sock):
        self.__name = name
        self.__password = password
        self.sock = sock
        
        with open(name + "_public_key.pem") as publicfile:
            pkeydata = publicfile.read()
            
        self.__pubKey = rsa.PublicKey.load_pkcs1(pkeydata)
    
    def getPubKey(self):
        return self.__pubKey

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
                return True, self.__pubKey
            elif name == self.__name:
                return False, self.__pubKey
            else:
                return False, ""

    def setSocket(self, sock):
        self.sock = sock
        
    def getSocket(self):
        return self.sock
    
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
def prepareHeader(header, userKey):

    # Prepend '0' to make header 10 bytes
    while len(header) < 10:
        header = "0" + header
        
    # Encrypt header data with a user public key
    header = rsa.encrypt(header, userKey)

    return header

# ************************************************
# Function to add header to data
# ************************************************
def preparePacket(data, userKey):

    # Encrypt data
    data = rsa.encrypt(data, userKey)

    dataSize = str(len(data))
    header = prepareHeader(dataSize, userKey)
    
    return header + data

# ************************************************
# Get the request from client
# @param sock - the socket from which to receive
# @return - the request
# *************************************************
def getRequest(sock):

    # The request command is the first 64 bytes of the data
    request = recvAll(sock, 64)

    if request != 0:
        request = rsa.decrypt(request, SERVER_PRIVATE_KEY)

    return str(request)

# ************************************************
# Get the account information from client
# @param sock - the socket from which to receive
# @return - the account information
# *************************************************
def getAccountInfo(sock):

    # Get fileNameSize
    accountSizeBuff = recvAll(sock, 64)
    
    # Decrypt header data
    accountSizeBuff = rsa.decrypt(accountSizeBuff, SERVER_PRIVATE_KEY)
    
    accountSize = int(accountSizeBuff)

    # Get fileName
    accountInfo = recvAll(sock, accountSize)
    
    # Decrypt userdata
    accountInfo = rsa.decrypt(accountInfo, SERVER_PRIVATE_KEY)
    
    return accountInfo

# ************************************************
# Broadcast message to all of members in chat room
# @param sock - the socket which sent the message
# @message - the message to be sent to all members
# *************************************************
def broadcastMessage(sock):

    getMessage = sock.recv(MAXIMUM_CHAT_MESSAGE_LEN)
    print "User sent: "
    print getMessage
    
    for m in chatMemberList:
        # Send the message only to peers
        if m != sock :
            try :
                
                # Encrypt a request
                request = rsa.encrypt(CHAT, onlineUsers[m].getPubKey())
                
                sendAll(m, request + getMessage)
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
            # Encrypt a request
            request = rsa.encrypt(MEMBEROFFLINE, onlineUsers[m].getPubKey())
            m.send(request + name + " is offline")
        except:
            # Broken socket connection
            m.close()

# ****************************************************
# Handle a client that is trying to log in
# @param sock: client's socket
# @param listOfAccounts: all valid accounts on the system
# ****************************************************
def handleLogin(sock, listOfAccounts):
    status = False
    userAccount = getAccountInfo(sock);
    returnStatus = FAIL
    userKey = ""
    anyUserPubKey = ""
    print "User at socket {0} is trying to log in".format(sock)
    for user in listOfAccounts:
        status, userKey = user.verifyAccount(userAccount)
        anyUserPubKey = user.getPubKey()
        
        if status:
            user.setSocket(sock)
            onlineUsers[sock] = user
            break

        if userKey != "":
            break

    # When fail to verify a user, use any user's key to send back
    if userKey == "":
        userKey = anyUserPubKey

    if status:
        print "Successfully logged in"
        returnStatus = OK
    else:
        print "Failed to log in"

    returnStatus = rsa.encrypt(returnStatus, userKey)
    sock.send(returnStatus)

# ****************************************************
# Handle user's request to check all online users
# @param sock: user's socket
# ****************************************************
def handleCheckOnlineUsers(sock):
    print "Check online users..."

    onlineUserList = []
    userPubKey = ""
    
    for u in onlineUsers:
        print onlineUsers[u].getName()
        onlineUserList.append(onlineUsers[u].getName())
        
        if onlineUsers[u].getSocket() == sock:
            userPubKey = onlineUsers[u].getPubKey()

    serializedOnlineList = cPickle.dumps(onlineUserList)
    
    # Encrypt a request
    request = rsa.encrypt(CHECKSTATUS, userPubKey)
    
    sendAll(sock, request + preparePacket(serializedOnlineList, userPubKey))

# ****************************************************
# Handle user's request to check chat members
# @param sock: user's socket
# ****************************************************
def handleCheckChatMembers(sock):
    print "Check chat members..."

    chatList = []
    for m in chatMemberList:
        chatList.append(onlineUsers[m].getName())

    serializedChatList = cPickle.dumps(chatList)

    userPubKey = onlineUsers[sock].getPubKey()
    
    request = rsa.encrypt(CHECKCHATMEM, userPubKey)
    sendAll(sock, request +  preparePacket(serializedChatList, userPubKey))
        

# ****************************************************
# Decrypt the data using server's private key
# @param sock: user's socket
# ****************************************************
def recvRSAPacket(sock):
    
    # Recieve header info
    dataSizeBuff = recvAll(sock, 64)
    
    # Decrypt header and get its size
    dataSizeBuff = rsa.decrypt(dataSizeBuff, SERVER_PRIVATE_KEY)
    dataSize = int(dataSizeBuff)
    
    # Recieve data and decrypt
    data = recvAll(sock, dataSize)
    data = rsa.decrypt(data, SERVER_PRIVATE_KEY)
    
    return data

# ****************************************************
# Handle chat invitaion
# @param sock: user's socket
# ****************************************************
def handleInvitation(sock):
    
    # Add user to chatMemberList
    if not sock in chatMemberList:
        chatMemberList.append(sock)

    # Generate the symmetric key with size of 32 bytes (128 bits) to use for chatting
    if len(chatMemberList) == 1:
        global randomKey
        randomKey = os.urandom(32)

    listOfNames = recvRSAPacket(sock)

    Names = []
    if len(listOfNames) > 0:
        Names = listOfNames.split(',')

    found = False
    message = ""
    for name in Names:
        print "User invites {0} to join chat session".format(name)
        userPubKey = ""
        
        # Check name with online users
        for u in onlineUsers:
            if onlineUsers[u].getName() == name.strip():
                found = True
                break

        for user in listOfAccounts:
            if user.getSocket() == sock:
                userPubKey = user.getPubKey()
                break

        request1 = rsa.encrypt(INVITE, userPubKey)
        request3 = rsa.encrypt(KEY, userPubKey)

        # Found the user
        if found:
            if not u in chatMemberList:
                message = "{0} is added to the chat session".format(onlineUsers[u].getName())
                chatMemberList.append(u)
                sendAll(sock, request1 + preparePacket( message, userPubKey))
                sendAll(sock, request3 + preparePacket(randomKey, userPubKey))
                

                # Notify user that is added to session
                inviteUserPubKey = onlineUsers[u].getPubKey()
                    
                request2 = rsa.encrypt(INVITE, inviteUserPubKey)
                request4 = rsa.encrypt(KEY, inviteUserPubKey)
                    
                sendAll(u, request2 + preparePacket("{0} invited you to the chat session".format(onlineUsers[sock].getName()), inviteUserPubKey))
                sendAll(u, request4 + preparePacket(randomKey, inviteUserPubKey))
                
            else:
                message = "{0} is already in the chat session".format(onlineUsers[u].getName())
                print message
                sendAll(sock, request1 +  preparePacket(message, userPubKey))
                sendAll(sock, request3 + preparePacket(randomKey, userPubKey))
        else:
            message = name + " is not registered"
            
            for user in listOfAccounts:
                if user.getName() == name:
                    message = name + " is not online"
                    
                    break
            
            sendAll(sock, request1 +  preparePacket(message, userPubKey))

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
    listOfAccounts.append(User("Tevin", "abc", 0))
    listOfAccounts.append(User("Billy", "abc", 0))
    listOfAccounts.append(User("Holly", "abc", 0))

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
                    broadcastMessage(s)

                # User sent invitation to online users to chat
                elif request == INVITE:
                    handleInvitation(s)
        
                elif request == CHECKCHATMEM:
                    handleCheckChatMembers(s)
             
                # User's socket is closed
                else:
                    print "Socket closed"
                    s.close()
                    input.remove(s)
                    if s in chatMemberList:
                        chatMemberList.remove(s)
                        username = onlineUsers[s].getName()
                        notifyOfflineMember(username)
                        print username + " logged out."
                    if s in onlineUsers:
                        del onlineUsers[s]

    # Close server socket
    welcomeSock.close()
