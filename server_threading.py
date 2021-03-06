import socket
from threading import Thread
from datetime import datetime
import random
import time
den1=1
den2=1
quat1=1
quat2=1

now = datetime.now()
current_time = now.strftime("%H:%M:%S:")
clients = {}
addresses = {}

HOST = '192.168.0.101'
PORT = 8080
BUFSIZ = 1024
ADDR = (HOST, PORT)
SERVER =socket.socket(socket.AF_INET, socket.SOCK_STREAM)
SERVER.bind(ADDR)

def ket_noi():
    while True:
        client, client_addr = SERVER.accept()
        print("%s:%s has connected." % client_addr)
        addresses[client] = client_addr
        if client_addr in addresses:
            pass
        else:
            Thread(target=handle_client, args=(client,client_addr,)).start()
            Thread(target=send_phone).start()
def handle_client(client,client_addr):  # Takes client socket as argument.
    name=client_addr[0]
    while True:
        msg = client.recv(BUFSIZ)
        if msg != bytes("[quit]", "utf8"):
            str_data = msg.decode("utf8")
            data_out = "{time} [{add}:{port}]: {str}".format(time=current_time, add=name, port=PORT, str=str_data)
            print(data_out)
        else:
            client.send(bytes("[quit]", "utf8"))
            client.close()
            del clients[client]
            broadcast(bytes("%s đã thoát phòng chat." % name, "utf8"))
            break

def broadcast(msg):  # prefix is for name identification.
    for sock in clients:
        sock.send(str.encode(msg))

def send_phone():
    while True:
        tem = random.randint(0, 100)
        hum = random.randint(0, 100)
        data_send = '"nd":{tem},"da":{hum},"b1":{d1},"b2":{d2},"q1":{q1},"q2":{q2}'.format(tem=tem, hum=hum, d1=den1,
                                                                       d2=den2, q1=quat1, q2=quat2)
        data_send = '{' + data_send + '}\n'
        print(data_send)
        broadcast(data_send)
        time.sleep(1)


if __name__ == "__main__":
    SERVER.listen(5)
    print("Chờ kết nối từ các client...")
    ACCEPT_THREAD = Thread(target=ket_noi)
    ACCEPT_THREAD.start()
    ACCEPT_THREAD.join()
    SERVER.close()