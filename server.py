import socket
from datetime import datetime
now = datetime.now()
current_time = now.strftime("%H:%M:%S")
host=socket.gethostbyname(socket.gethostname())
port=8080
print(host,":",port)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
s.listen(2)
print("listening.....")
while True:
    client, addr = s.accept()
    try:
        print('Connected by', addr[0])
        while True:
            data = client.recv(1024)
            str_data = data.decode("utf8")
            if str_data == "quit":
                break
            if not data:
                break
            data_out = "{time} [{add}:{port}]: {str}".format(time=current_time,add=addr[0],port=port,str=str_data)
            print(data_out)
            client.send(str.encode(str_data))
    finally:
        client.close()

s.close()