import socket
import threading
host="54.167.124.150"
port=8080
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (host, port)
print('connecting to %s port ' + str(server_address))
s.connect(server_address)
a1='{"den2":14}'
a2='{"quat1":1}'
a3='{"quat2":1}'
a=[a1,a2,a3]

def nhan(x):
    while True:
        data = x.recv(1024)
        print('Server: ', data.decode("utf8"))

try:
    threading.Thread(target=nhan, args=(s,))
    while True:
        x=int(input("nhập vào số <4"))
        s.sendall(bytes(a[x], "utf8"))

finally:
    s.close()

