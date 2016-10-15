#!/usr/bin/env python3

from codecs import open
from json import dumps as tojson
from os.path import split as splitpath
from requests import post
from sys import argv
import webbrowser

py_filename,description = argv[1],argv[2]
_,fname = splitpath(py_filename)
content = open(py_filename,'r','utf8').read()

data = {
	"files": {
		fname: {
			"content": content
		}
	},
	"description": description,
	"public": True
}

headers = {}

r = post('https://api.github.com/gists', data=tojson(data), headers=headers)
if r.status_code == 201:
	url = 'https://gist.github.com/%s' % r.json()['id']
	print('Gist created: %s' % url)
	webbrowser.open_new_tab(url)
else:
	print('GIST CREATE ERROR %s: %s' % (r.status_code, r.text))
