import json
import requests

# read input
with open("input.json") as f:
    data = json.load(f)

# send request
res = requests.post("http://127.0.0.1:5000/search", json=data)

# save response
with open("output.txt", "w") as f:
    f.write(res.text)