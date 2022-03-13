import socket
import json
from datetime import datetime
import random
from threading import Thread
import time
now = datetime.now()
current_time = now.strftime("%H:%M:%S:")

den1=1
den2=1
quat1=1
quat2=1
stop=0
host=socket.gethostbyname(socket.gethostname())
port=8080
print(host,":",port)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
s.listen(2)
print("listening.....")
def send_phone(client):
    while True:
        tem = random.randint(0, 100)
        hum = random.randint(0, 100)
        data_send = '"nd":{tem},"da":{hum},"b1":{d1},"b2":{d2},"q1":{q1},"q2":{q2}'.format(tem=tem, hum=hum, d1=den1,d2=den2, q1=quat1, q2=quat2)
        data_send = '{' + data_send + '}\n'
        dat_print='SEND: "nd":{tem},"da":{hum},"b1":{d1},"b2":{d2},"q1":{q1},"q2":{q2}'.format(tem=tem, hum=hum, d1=den1,d2=den2, q1=quat1, q2=quat2)
        client.send(str.encode(data_send))
        print(dat_print)
        time.sleep(1)

def handle_threading(client,addr):
    global den2, quat1, quat2
    try:
        while True:
            data = client.recv(1024)
            str_data = data.decode("utf8")
            if str_data == "quit":
                print(addr, "is disconnected")
                print("Listening...")
                break
            data_out = "{time} [{add}:{port}]: {str}".format(time=current_time,add=addr[0],port=port,str=str_data)
            print(data_out)
            js = json.loads(str_data)

            if "den2" in js.keys():
                den2=~den2
                if den2==1:
                    print("Đèn 2 đang bật")
                else:
                    print("Đèn 2 đang tắt")
            if "quat1" in js.keys():
                quat1=~quat1
                if quat1==1:
                    print("Quạt 1 đang bật")
                else:
                    print("Quạt 1 đang tắt")
            if "quat2" in js.keys():
                quat2=~quat2
                if quat2==1:
                    print("Quạt 2 đang bật")
                else:
                    print("Quạt 2 đang tắt")
    except:
        print(addr,"is disconnected")
        print("Listening...")
        a.join()
        print(a.is_alive())
def connect():
    while True:
        client, addr = s.accept()
        print('Connected by', addr[0])
        global a,b
        a=Thread(target=send_phone, args=(client,))
        a.start()
        print(a.is_alive())
        b = Thread(target=handle_threading, args=(client,addr,))
        b.start()
        print(b.is_alive())

if __name__ == "__main__":
    s.listen(5)
    print("Chờ kết nối từ các client...")
    ACCEPT_THREAD = Thread(target=connect())
    ACCEPT_THREAD.start()