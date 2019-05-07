import json
import sys

qtype = int(sys.argv[1])
query = sys.argv[2]

f = open('../Output/bigrams.json' , 'r')
bigram = json.load(f)
bigram = json.loads(bigram)
f = open('../Output/index.json' , 'r')
index = json.load(f)
index = json.loads(index)
if qtype == 1:
    query = query.split(' AND ')
    arr = []
    for x in query:
        if index.get(x) is None:
            print([])
            sys.exit()
        arr.append(index[x])
    arr.sort(key = lambda t : len(t))
    while len(arr) > 1:
        a = arr[0]
        b = arr[1]
        del arr[0]
        arr[0] = [x for x in a if x in b]
    print(sorted(arr[0]))
elif qtype == 2:
    query = query.split(' OR ')
    arr = []
    for x in query:
        if index.get(x) is None:
            continue
        arr.append(index[x])
    arr.sort(key = lambda t : len(t))
    while len(arr) > 1:
        a = arr[0]
        b = arr[1]
        del arr[0]
        arr[0].extend([x for x in a if x not in b])
    print(sorted(arr[0]))
elif qtype == 3:
    orj = query
    if query[0] != '*':
        query = '$' + query
    if query[len(query)-1] != '*':
        query = query + '$'
    query = query.split('*')
    arr = []
    for x in query:
        for i in range(len(x)-1):
            arr.append(x[i:(i+2)])
    arr2 = []
    for x in arr:
        if bigram.get(x) is None:
            print([])
            sys.exit()
        arr2.append(bigram[x])
    arr2.sort(key = lambda t : len(t))
    while len(arr2) > 1:
        a = arr2[0]
        b = arr2[1]
        del arr2[0]
        arr2[0] = [x for x in a if x in b]
    arr2 = arr2[0]
    orjsplit = orj.split('*')
    if '' in orjsplit:
        orjsplit.remove('')
    if len(orjsplit) == 2:
        arr2 = [x for x in arr2 if (x.find(orjsplit[0]) == 0) and (x.rfind(orjsplit[1]) == len(x) - len(orjsplit[1]))]
    elif orj[0] == '*':
        arr2 = [x for x in arr2 if x.rfind(orjsplit[0]) == len(x) - len(orjsplit[0])]
    else:
        arr2 = [x for x in arr2 if x.find(orjsplit[0]) == 0]
    arr = []
    for x in arr2:
        if index.get(x) is None:
            continue
        arr.append(index[x])
    arr.sort(key = lambda t : len(t))
    while len(arr) > 1:
        a = arr[0]
        b = arr[1]
        del arr[0]
        arr[0].extend([x for x in a if x not in b])
    print(sorted(arr[0]))
else:
    print("Wrong query type")