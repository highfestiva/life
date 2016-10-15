#!/usr/bin/env python3

from codecs import open
import http.client
from json import dumps as tojson, loads as fromjson
from os.path import split as splitpath
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
	"public": True,
}

headers = {
	'User-agent': fname,
	'Content-type': 'application/json',
}

connection = http.client.HTTPSConnection('api.github.com')
connection.request('POST', '/gists', tojson(data), headers)
response = connection.getresponse().read().decode()
url = 'https://gist.github.com/%s' % fromjson(response)['id']
print('Gist created: %s' % url)
webbrowser.open_new_tab(url)
