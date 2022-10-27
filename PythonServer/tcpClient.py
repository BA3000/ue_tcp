import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
server_address = ('localhost', 2333)
sock.connect(server_address)
try:
    # Send data
    message = 'This is the message.  It will be repeated..'
    print('sending "%s"' % message)
    bytes2Send = str.encode(message, "utf-8")
    sock.sendall(bytes2Send)

    # Look for the response
    amount_received = 0
    amount_expected = len(message)
    while amount_received < amount_expected:
        data = sock.recv(16)
        amount_received += len(data)
        print('received "%s"' % data)

finally:
    sock.close()
