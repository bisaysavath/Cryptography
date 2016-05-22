import socket
import os
import sys
import commands
import getpass
import cPickle
import select
import re
import rsa
from Crypto.Cipher import AES
import base64

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

USER_PRIVATE_KEY = ""
with open("server_public_key.pem") as publicfile:
    pkeydata = publicfile.read()
    
SERVER_PUBLIC_KEY = rsa.PublicKey.load_pkcs1(pkeydata)

RANDOM_KEY = os.urandom(32)
# Create an instance of the RC4 cipher class.
Cipher = AES.new(RANDOM_KEY)

# the block size for the cipher object; must be 16, 24, or 32 for AES
BLOCK_SIZE = 32
MAXIMUM_CHAT_MESSAGE_LEN = 1000

PADDING = '{'

# one-liner to sufficiently pad the text to be encrypted
pad = lambda s: s + (BLOCK_SIZE - len(s) % BLOCK_SIZE) * PADDING

# one-liners to encrypt/encode and decrypt/decode a string
# encrypt with AES, encode with base64
EncodeAES = lambda c, s: base64.b64encode(c.encrypt(pad(s)))
DecodeAES = lambda c, e: c.decrypt(base64.b64decode(e)).rstrip(PADDING)

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
    
    # Encrypt header data
    header = rsa.encrypt(header, SERVER_PUBLIC_KEY)

    return header

# ************************************************
# Function to add header to data
# ************************************************
def preparePacket(data):
    
    # Encrypt data
    data = rsa.encrypt(data, SERVER_PUBLIC_KEY)
    
    dataSize = str(len(data))
    header = prepareHeader(dataSize)
    return header + data


# ************************************************
# Handle log in
# ************************************************
def userLogIn():

    while (True):
        username = raw_input("Username: ")
        password = getpass.getpass("Password: ")

        accountInfo = str(username) + ";" + str(password)
        accountInfoSize = str(len(accountInfo))

        # Encrypt request
        request = rsa.encrypt(LOGIN, SERVER_PUBLIC_KEY)

        sendAll(clientSock, request  + preparePacket(accountInfo))

        returnStatus = clientSock.recv(64)
        
        global USER_PRIVATE_KEY
        USER_PRIVATE_KEY = setUserPrivateRSAKeys(username)
        
        if (USER_PRIVATE_KEY != ""):
            returnStatus = rsa.decrypt(returnStatus, USER_PRIVATE_KEY)
        else:
            returnStatus == FAIL

        if returnStatus == OK:
            print "Welcome back, " + username + "!"
            print ""
            return True, username
        else:
            userChoice = raw_input("Wrong credentials. Try again? (y/n) ")
            if userChoice.lower() == "n":
                return False, ""
            print ""

def recvRSAPacket(sock):
    
    # Recieve header info
    dataSizeBuff = recvAll(sock, 64)
    
    # Decrypt header and get its size
    dataSizeBuff = rsa.decrypt(dataSizeBuff, USER_PRIVATE_KEY)
    dataSize = int(dataSizeBuff)
    
    # Recieve data and decrypt
    data = recvAll(sock, dataSize)
    data = rsa.decrypt(data, USER_PRIVATE_KEY)
    
    return data

# ************************************************
# Handle checking online users
# ************************************************
def getOnlineUser(clientSock):
    
    serializedOnlineList = recvRSAPacket(clientSock)
    onlineUserList = cPickle.loads(serializedOnlineList)
    
    print "Online users:"

    for user in onlineUserList:
        print user
    print ""

def getChatMembers(clientSock):
    serializedChatList = recvRSAPacket(clientSock)
    chatList = cPickle.loads(serializedChatList)
    
    print "Chat members: "

    for user in chatList:
        print user
    print ""
    
def getRandomKey(clientSock):
    global RANDOM_KEY
    key = recvRSAPacket(clientSock)
    if key != RANDOM_KEY:
        RANDOM_KEY = key
        global Cipher
        Cipher = AES.new(RANDOM_KEY)


def decryptChatMessage(clientSock):
    encryptedMessage = clientSock.recv(MAXIMUM_CHAT_MESSAGE_LEN)
    return DecodeAES(Cipher, encryptedMessage)

def encryptChatMessage(message):
    encryptedMessage = EncodeAES(Cipher, message)
    return encryptedMessage

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
        
        inputready,outputready,exceptready = select.select(input,[], [])

        for s in inputready:

            if s == sock:
                # Receive data from the server
                # get the reponse code
                response = s.recv(64)
                
                response = rsa.decrypt(response, USER_PRIVATE_KEY)

                # Server sent the list of online users
                if response == CHECKSTATUS:
                    getOnlineUser(s)

                # Server sent chat message from other users
                if response == CHAT:
                    print decryptChatMessage(s) + "\n"

                # Server sent notification about offline user
                if response == MEMBEROFFLINE:
                    print s.recv(100) + "\n"

                # Server responded to chat invitation
                if response == INVITE:
                    print recvRSAPacket(s) + "\n"

                # Server responded to check chat members
                if response == CHECKCHATMEM:
                    getChatMembers(s)
                    
                # Server sent a random symmetric key to be used for chatting
                if response == KEY:
                    getRandomKey(s)
            
            elif s == sys.stdin:
                # Handle standard input
                msg = sys.stdin.readline().strip()

                # User wants to quit
                if msg == "::quit":
                    running = 0
                    break

                # User wants to check who are online
                elif msg == "::online":
                    # Esncrypt request
                    request = rsa.encrypt(CHECKSTATUS, SERVER_PUBLIC_KEY)
                    sendAll(sock, request)

                # User wants to invite online users to chat
                elif msg.startswith('::invite'):
                    names = ""
                    matchObj = re.match( r'::invite (.*)', msg)
                    if not matchObj:
                        print "Invalid command"
                    else:
                        names = matchObj.group(1)
                        if len(names) > 0:
                            request = rsa.encrypt(INVITE, SERVER_PUBLIC_KEY)
                            sendAll(sock, request + preparePacket(names))

                # User wants to check who are in the chat session
                elif msg == "::chatmem":
                    request = rsa.encrypt(CHECKCHATMEM, SERVER_PUBLIC_KEY)
                    sendAll(sock, request)
                        
                # Chat message
                else:
                    message = msg.strip()
                    msg = username + ": " + msg
                    header = str(len(msg))
                    if message:
                        request = rsa.encrypt(CHAT, SERVER_PUBLIC_KEY)                        
                        sendAll(sock, request + encryptChatMessage(msg))


# ************************************************
# Direction
# ************************************************
def directionMenu():

    print "**************************************************************************"
    print "*****                INSTRUCTION                                     *****"
    print "***** Type ::online to check online users                            *****"
    print "***** Type ::invite name1,name2... to invite online users to chat    *****"
    print "***** Type ::chatmem to check members in the chat session            *****"
    print "***** Type ::quit to quit the program                                *****"
    print "***** Otherwise, any input message will be chat message              *****"
    print "**************************************************************************"
    print "\n"

def setUserPrivateRSAKeys(username):
    try:
        with open(username + "_private_key.pem") as privatefile:
            keydata = privatefile.read()
    except:
        return ""

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
        process(clientSock, username)
    else:
        print "Bye!"

    clientSock.close()
