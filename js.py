import json
a='{"thang":30}'
js=json.loads(a)
print(type(js))
if "thang" in js.keys():
    print(js["thang"])
else:
    print("jeje")