#!/usr/bin/env python3

import sys
import argparse
import subprocess
import os
import hashlib
import re

# dictionary that stores hash values as keys and files/directories as values
hash_dic = {}

# hash of empty string
empty_hash = hashlib.sha256(b'').hexdigest()


def terminal(cmd):
    """
        Executes command 'cmd' 
    """
    return subprocess.check_output(cmd,shell=True).decode()


def hash_of_dir(_str):
    """
        Calculates hash values of directories and stores on a dictionary
    """
    _hash = hashlib.sha256()
    if os.path.isdir(_str):
        for p in sorted([hash_of_dir(_str+"/"+q) for q in os.listdir(_str)]):
            _hash.update ( p.encode() )
    else:
        _hash.update ( open(_str,'rb').read() )
    hex_hash = _hash.hexdigest()
    if not os.path.isdir(_str):
        return hex_hash
    if hash_dic.get(hex_hash) is None:
        hash_dic[hex_hash] = []
    hash_dic[hex_hash].append(_str)
    return hex_hash


def hash_of_files(_str):
    """
        Calculates hash values of only files and stores on a dictionary
    """
    if os.path.isdir(_str):
        for p in os.listdir(_str):
            hash_of_files(_str+"/"+p)
    else:
        _hash = hashlib.sha256( open(_str , 'rb').read() ).hexdigest()
        if hash_dic.get(_hash) is None:
            hash_dic[_hash] = []
        hash_dic[_hash].append(_str)
        return _hash

parser = argparse.ArgumentParser()

group = parser.add_mutually_exclusive_group()

group.add_argument('-c','--command',action='store')
group.add_argument('-p','--print',action='store_true')

group2 = parser.add_mutually_exclusive_group()

group2.add_argument('-f','--file',action='store_true')
group2.add_argument('-d','--dir',action='store_true')

parser.add_argument('regex',nargs='?')
parser.add_argument('directories',nargs='*')

args = parser.parse_args()

if args.directories is None:
    args.directories = []

if (args.regex is None) or (len(args.regex) == 0):
    rr = re.compile('.*')
else:
    if args.regex[0] == '\"':
        rr = re.compile(args.regex[1:len(args.regex)-1])
    else:
        rr = re.compile('.*')
        args.directories.append(args.regex)

# default directory is current directory
if len(args.directories) == 0:
    args.directories.append(os.getcwd())

if args.file:
    for p in args.directories:
        hash_of_files(p)
else:
    for p in args.directories:
        hash_of_dir(p)

# Don't print empty files/directories
if hash_dic.get(empty_hash) is not None:
    del hash_dic[empty_hash]

if args.command is None:
    for p in list( hash_dic.values() ):
        r = [os.path.abspath(q) for q in set(p) if re.search(rr , os.path.basename(q)) is not None]
        if len(r) < 2:
            continue
        for q in sorted(r):
            print(q, end=' ')
        print()
        print()
else:
    for p in list( hash_dic.values() ):
        r = [q for q in set(p) if re.search(rr , os.path.basename(q)) is not None]
        if len(r) < 2:
            continue
        cmd = str(args.command)+" "
        for q in r:
            cmd = cmd + str(os.path.abspath(q)) + " "
        print(terminal(cmd))
        print()




