from threading import Thread
import time
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

host='192.168.0.101'
port=8080
print(host,":",port)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
s.listen(2)
print("listening.....")

class myThread(Thread):
	"""docstring for myThread"""
	def __init__(self, name, delay):
		super(myThread, self).__init__()
		self.name= name
		self.delay=delay

	def run(self):
		while True:
			client, addr = s.accept()
			print('Connected by', addr[0])
			phoneThread = myThread("thread 1", 0.25)
			phoneThread.start()
			try:
				while True:
					data = client.recv(1024)
					str_data = data.decode("utf8")
					if str_data == "quit":
						print(addr, "is disconnected")
						print("Listening...")
						break
					data_out = "{time} [{add}:{port}]: {str}".format(time=current_time, add=addr[0], port=port,
																 str=str_data)
					time.sleep(self.delay)
					print(data_out)
			except:
				print(addr, "is disconnected")
				print("Listening...")
class phoneThread(Thread):
	"""docstring for myThread"""
	def __init__(self, name, delay):
		super(myThread, self).__init__()
		self.name= name
		self.delay=delay

	def run(self):
		while True:
			tem = random.randint(0, 100)
			hum = random.randint(0, 100)
			data_send = '"nd":{tem},"da":{hum},"b1":{d1},"b2":{d2},"q1":{q1},"q2":{q2}'.format(tem=tem, hum=hum,
																							   d1=den1, d2=den2,																			   q1=quat1, q2=quat2)
			data_send = '{' + data_send + '}\n'
			client.send(str.encode(data_send))
			print(data_send)
			time.sleep(2)

thread1 = myThread("thread 1", 0.5)
thread1.start()
