import socket
import json
from datetime import datetime
import random
from threading import Thread
import time
now = datetime.now()
current_time = now.strftime("%H:%M:%S:")
tb1=1
tb2=1
tb3=1
tb4=1
tem=0
hum=0
stop=0
cont=0
host="192.168.0.101"
port=80
phone=[]
equipment=[]
print(host,":",port)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
s.listen(3)
print("listening.....")
def xu_ly(js):
    global tb2, tb1, tb3,tb4,tem,hum
    if "tb1" in js.keys():
        tb1=js["tb1"]
        if js["tb1"]==0:
            print("tb1 on")
        else:
            print("tb1 off")
    if "tb2" in js.keys():
         tb2=js["tb2"]
        if js["tb2"]==0:
            print("tb2 on")
        else:
            print("tb2 off")
    if "tb3" in js.keys():
        tb2=js["tb2"]
        if js["tb3"]==0:
            print("tb3 on")
        else:
            print("tb3 off")
    if "tb4" in js.keys():
        tb4=js["tb4"]
        if js["tb4"]==0:
            print("tb4 on")
        else:
            print("tb4 off")
    if "nd" in js.keys():
        tem =js["nd"]
    if "da" in js.keys():
        hum =js["da"]
def send_phone(client):
        data_send = '"nd":{tem},"da":{hum},"b1":{d1},"b2":{d2},"q1":{q1},"q2":{q2}'.format(tem=tem, hum=hum, d1=tb1,d2=tb2, q1=tb3, q2=tb4)
        dat_print= "SEND:" + data_send
        data_send = '{' + data_send + '}\n'
        client.send(str.encode(data_send))
        print(dat_print)
def equip_threading(client,addr):
    while True:
        data = client.recv(1024)
        str_data = data.decode("utf8")
        if str_data == "quit":
            print(addr, "is disconnected")
            break
        data_out = "{time} [{add}:{port}]: {str}".format(time=current_time,add=addr[0],port=port,str=str_data)
        print("equip: ",data_out)
        js = json.loads(str_data)
        xu_ly(js)
        if len(phone) == 0:
            print("không có thiết bị nào")
        else:
            for i in range(0,len(phone)):
                try:
                    send_phone(phone[i])
                except:
                     del phone[i]
def phone_threading(client,addr):
    global tb2, tb1, tb3,tb4,tem,hum
    while True:
        data = client.recv(1024)
        str_data = data.decode("utf8")
        if str_data == "quit":
            print(addr, "is disconnected")
            del phone[0]
            break
        else:
            data_out = "{time} [{add}:{port}]: {str}".format(time=current_time,add=addr[0],port=port,str=str_data)
            print("phone: ",data_out)
            if len(phone) == 0:
                print("không có thiết bị nào")
            else:
                equipment[cont].send(str.encode(str_data+"\r\n"))

def connect():
    while True:
        client, addr = s.accept()
        print('Connected by', addr[0])
        data = client.recv(1024)
        str_data = data.decode("utf8")
        print(str_data)
        if str_data == "phone":
            phone.append(client)
            Thread(target=phone_threading, args=(client,addr,)).start()
            print("điện thoại đã kết nối")
        if str_data == "equip":
            cont+=1
            equipment.append(client)
            Thread(target=equip_threading, args=(client,addr,)).start()
            print("thiết bị đã kết nối")

if __name__ == "__main__":
    s.listen(5)
    print("Chờ kết nối từ các client...")
    ACCEPT_THREAD = Thread(target=connect())
    ACCEPT_THREAD.start()