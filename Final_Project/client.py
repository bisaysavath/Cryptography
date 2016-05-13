import socket
import os
import sys
import commands
import getpass
import cPickle
import select

FAIL = "0"
OK = "1"

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
            return True
        else:
            userChoice = raw_input("Wrong credentials. Try again? (y/n) ")
            if userChoice.lower() == "n":
                return False
            print ""

            # ************************************************
# Handle checking online users
# ************************************************
def checkOnlineUser():
    
    dataSizeBuff = recvAll(clientSock, 10)
    dataSize = int(dataSizeBuff)
    
    serializedOnlineList = recvAll(clientSock, dataSize)
    onlineUserList = cPickle.loads(serializedOnlineList)
    
    print "Online users:"

    for user in onlineUserList:
        print user
    print ""
    
# ************************************************
# Process after user successfully logged in
# *************************************************
def process(sock):

    input = [sock, sys.stdin]
    
    # *************************************************
    #           WHILE LOOP
    # *************************************************
    running = 1
    while running:
        
        inputready,outputready,exceptready = select.select(input,[],[])

        for s in inputready:

            if s == sock:
                # receive data from the server
                # get the reponse code
                response = s.recv(2)
                
                if response == CHECKSTATUS:
                    checkOnlineUser()
                    

            elif s == sys.stdin:
                # handle standard input
                msg = sys.stdin.readline().strip()

                # user wants to quit
                if msg == "::quit":
                    running = 0
                    break

                # user wants to check who are online
                elif msg == "::online":
                    sendAll(clientSock, CHECKSTATUS)

                # Other messages
                else:
                    print msg

# ************************************************
# Direction
# ************************************************
def directionMenu():

    print "**************************************************************************"
    print "*****                DIRECTION                                       *****"
    print "***** Type ::online to check online users                            *****"
    print "***** Type ::invite name1,name2... to invite online users to chat    *****"
    print "***** Type ::quit to quit the program                                *****"
    print "***** Otherwise, any input message will be chat message              *****"
    print "**************************************************************************"
    print "\n"
    
# ************************************************
# Handle chat
# ************************************************
def chat():

    while (True):
        # Wait for input from stdin & socket
        inputready, outputready, exceptready = select.select([0, clientSock], [], [])
        
        for i in inputready:
            if i == 0:
                # grab message
                userMsg = raw_input("Your message: ")

                if (userMsg == "quit()"):
                    break;
                
                userMsgSize = str(len(userMsg))

                header = prepareHeader(userMsgSize)

                sendAll(clientSock, CHAT + header + userMsg)
                # try:
                #     # encrypt
                #     data = self.encryptor.encrypt(data, 0)
                #     data = data[0]

                #     # append signature
                #     signkey = self.decryptor
                #     message_hash = SHA.new()
                #     message_hash.update(data)

                #     signer = PKCS1_PSS.new(signkey)
                #     signature = signer.sign(message_hash)
                #     data = '%s#^[[%s' % (data, signature)

                # except ValueError:
                #     print 'Too large text, cannot encrypt, not sending.'
                #     data = None

                # if data:
                #     send(self.sock, data)

            elif i == clientSock:
                dataSizeBuff = recvAll(clientSock, 10)
                dataSize = int(dataSizeBuff)
                
                getMessage = recvAll(clientSock, dataSize)
                print getMessage
                # data = receive(self.sock)

                # if not data:
                #     print 'Shutting down.'
                #     # self.flag = True
                #     break

                # else:
                #     if 'PLAIN:' in data:
                #         data = data.strip('PLAIN:').strip()
                #     else:
                #         data = self.decryptor.decrypt(data)

                #     sys.stdout.write(data + '\n')
                #     sys.stdout.flush()

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

    if userLogIn():
        print "Successfully logged in"
        directionMenu()
        process(clientSock)
    else:
        print "Bye!"

    clientSock.close()
