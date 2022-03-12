import codecs
f=codecs.open("index.html", 'r')
print(f.read())
file = open("index.html","w")
file.write("yen xinh gai")
file.close()