import socket
host=socket.gethostbyname(socket.gethostname())
port=8080
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (host, port)
print('connecting to %s port ' + str(server_address))
s.connect(server_address)
a1='{"nd":14}'
a2='{"den":1}'
a3='{"quat":1}'
a4='{"gio":1}'
a=[a1,a2,a3,a4]
try:
    while True:
        x=int(input("nhập vào số <4"))
        s.sendall(bytes(a[x], "utf8"))
        data = s.recv(1024)
        print('Server: ', data.decode("utf8"))
finally:
    s.close()

