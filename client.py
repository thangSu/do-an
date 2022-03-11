import socket

host=socket.gethostbyname(socket.gethostname())
port=8080
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (host, port)
print('connecting to %s port ' + str(server_address))
s.connect(server_address)
begin="{nd:30}"
s.sendall(bytes(begin, "utf8"))
try:
    while True:
        msg = input('Client: ')
        s.sendall(bytes(msg, "utf8"))
        if msg == "quit":
            break
        data = s.recv(1024)
        print('Server: ', data.decode("utf8"))
finally:
    s.close()

