import codecs
import json
import operator
# Placeholder for index.json and biagram.json
index = {}
bigrams = {}
num_tokens_before_stop = 0
num_tokens_after_stop = 0
# Translate table for punctuations
punc = open('../punctuations.txt' , 'r')
punc = punc.readlines()
dic = {}
for x in punc:
    dic[x.strip()] = ' '
dic = str.maketrans(dic)

# Read stopwords
stops = open('../stopwords.txt' , 'r')
stops = [x.strip() for x in stops.readlines()]

for j in range(0,22): #For all 22 sgm files
    f = codecs.open('../reuters21578/reut2-'+ str(j).zfill(3) + '.sgm' , mode='r' , encoding='latin_1')
    f.readline() # Skip first line
    news = f.read().split('\n</REUTERS>\n') # Take <REUTERS> to </REUTERS>
    del news[len(news)-1] # Delete empty line end of the file
    for n in news:
        newid = n.find('NEWID=')
        date = n.find('<DATE>')
        newid = int(n[(newid+7):(date-3)]) # Extract NEWID
        l = n.find('<TITLE>')
        r = n.find('</TITLE>') # Take TITLE part
        if l != -1: # If TITLE exists
            s = n[(l+7):r]
            s = s.translate(dic) # Delete punctuations
            s = s.split() # Tokenize by space
            s = [x.lower() for x in s] # lower case
            num_tokens_before_stop = num_tokens_before_stop + len(s)
            i = 0
            while i < len(s): # Delete stopwords
                if s[i] in stops:
                    del s[i]
                else:
                    i = i + 1
            num_tokens_after_stop = num_tokens_after_stop + len(s)
            for x in s: # Push NEWID s to index.json
                if index.get(x) is None:
                    index[x] = []
                if newid in index[x]: # Make sure all newid s diffirent for each token
                    continue
                index[x].append(newid)
        l = n.find('<BODY>')
        r = n.find('</BODY>') # Take BODY part
        if l != -1: # If BODY exists
            s = n[(l+6):r]
            s = s.translate(dic) # Delete punctuations
            s = s.split() # Tokenize by space
            s = [x.lower() for x in s] # lower case
            num_tokens_before_stop = num_tokens_before_stop + len(s)
            i = 0
            while i < len(s): # Delete stopwords
                if s[i] in stops:
                    del s[i]
                else:
                    i = i + 1
            num_tokens_after_stop = num_tokens_after_stop + len(s)
            for x in s: # Push NEWID s to index.json
                if index.get(x) is None:
                    index[x] = []
                if newid in index[x]: # Make sure all newid s diffirent for each token
                    continue
                index[x].append(newid)

for key in index.keys():
    orj = key
    key = '$' + key + '$'
    for i in range(len(key)-1):
        if bigrams.get(key[i:(i+2)]) is None:
            bigrams[key[i:(i+2)]] = []
        bigrams[key[i:(i+2)]].append(orj)

print('Number of tokens before stopword removal : ' + str(num_tokens_before_stop))
print('Number of tokens after stopword removal : ' + str(num_tokens_after_stop))
print('Number of terms after stopword removal and case-folding : ' + str(len(index.keys())))

print('Top 20 after stopword removal and case-folding :', end = ' ')
print([x[0] for x in sorted(index.items(), key = lambda t : len(t[1]) , reverse = True)[0:20]])

index = json.dumps(index, sort_keys=True) # sort by keys
f = open('../Output/index.json' , 'w') 
json.dump(index , f) # print to file

bigrams = json.dumps(bigrams , sort_keys=True) # sort by keys
f = open('../Output/bigrams.json' , 'w') 
json.dump(bigrams , f) # print to file