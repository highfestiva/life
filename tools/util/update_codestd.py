#!/usr/bin/env python3

import codecs
from collections import defaultdict
from io import BytesIO
import re
import os
import sys
from tokenize import tokenize, NAME, NUMBER, OP, COMMENT, STRING, TokenError

_lookup = {}
_fnames = {'ThirdParty': 'thirdparty'}
_flookup = defaultdict(set)

def _tok(i, tokens):
	if i >= 0 and i < len(tokens):
		return tokens[i][:2]
	return '',''

def read(filename):
	try:
		return open(filename).read()
	except UnicodeDecodeError:
		return codecs.open(filename, encoding='latin-1').read()
	except:
		return None

def is_cpp_file(filename):
	return os.path.isfile(filename) and any(filename.endswith(e) for e in ['.cpp', '.cxx', '.mm', '.c', '.m', '.h', '.hpp', '.inl'])

def is_proj_file(filename):
	return os.path.isfile(filename) and any(filename.endswith(e) for e in ['.project', '.vcxproj', '.vcxproj.filters', '.pbxproj', '.sln', '.workspace', '.plist', '.rc'])

def parse_token(fname, i, tokens):
	global _flookup
	typ,name = _tok(i, tokens)
	if name:
		_flookup[fname].add(name)
	if len(name) <= 1:
		return
	if i==1 and typ == STRING and tokens[0][1] == '#include':
		_lookup[name] = 'loc_inc'
		return
	if typ not in (NAME,):
		return
	typ0,name0 = _tok(i-1, tokens)
	isclass = name0 in ('class','struct')
	if typ == NAME and name in _lookup:
		if isclass:
			_lookup[name] = 'class'
		return
	typ1,name1 = _tok(i+1, tokens)
	if name0 == '#define' and name == name.upper():
		if name1.isdigit() or name1.startswith('"'):
			_lookup[name] = 'const'
		elif '(' not in name:
			_lookup[name] = 'macro'
		return
	istype = (tokens[0][1] == 'typedef')
	iscamel = (name != name.lower() and name != name.upper())
	isvar = (name1 and name1 in ';=[' and not isclass and not istype)
	if name1 in ';,.-><::?()[+=-=*=/=%=&=|=~=^=':
		if name0 == 'namespace' and iscamel:
			_lookup[name] = 'namespace'
		elif (isclass or istype) and iscamel:
			_lookup[name] = 'class'
		elif isvar and name[0] == 'g' and iscamel and 'const' in [t[1] for t in tokens[max(0,i-4):i]]:
			_lookup[name] = 'const'
		elif isvar and name[0] in 'ms' and iscamel:
			_lookup[name] = 'm_var'
		elif isvar and name[0] == 'g' and (name[1] == '_' or name[1].isupper()):
			_lookup[name] = 'g_var'
		elif i == 0 and name == name.upper() and name1 == '=':
			_lookup[name] = 'const'
		elif (i==0 or 'enum' in [t[1] for t in tokens]) and name == name.upper() and name.split('_',1)[0] not in ('PNG','GL','GLX') and name1 in ('',',','}'):
			_lookup[name] = 'const'	# enum
		elif isvar and name[0] in 'lps' and iscamel:
			_lookup[name] = 's_var'
		elif name1 and name1 in '),' and name[0] in 'lp' and iscamel:
			_lookup[name] = 's_var'
		elif name1 == '(':
			# Constructor call or function call. Hard to distinguish.
			if name[0] in 'mlp' and iscamel:
				if name[0] in 'lp':
					_lookup[name] = 's_var'
				else:
					# This is probably a constructor initializer. Type will be found in definition.
					return
			elif typ0 == NAME or '=' in name0 or ':' in name0:
				if name[0].isupper():
					_lookup[name] = 'func'
				elif _tok(len(tokens)-1, tokens)[1] == ';':
					# This is either a prototype or a stack object calling it's constructor.
					# We can't tell without knowing if we're inside a function or not.
					pass

def get_token(i, tokens):
	typ,name = _tok(i, tokens)
	if name in _lookup:
		return _lookup[name],name
	return typ,name

def filename_convert(line):
	words = [word for word in re.split(r'[^a-zA-Z0-9\._]', line) if word]
	for word in sorted(words, key=lambda w:-len(w)):
		if word in _fnames:
			line = line.replace(word, _fnames[word])
	return line

def namespacename(name):
	return name.lower()

def classname(name):
	return name

def funcname(name):
	return name

def varname(filename, name):
	# Remove hungarian notation.
	n = name.lstrip('abcdefghijklmnopqrstuvwxyz_')
	if len(n)+5 < len(name):
		return name	# Uh-oh. Not even I could stand this many hungarian letters.
	if not n or n[:1].isdigit():
		return name
	# Convert CPUThread to CpuThread.
	n = re.sub('([A-Z])([A-Z]+)([A-Z]|$)', lambda m: m.group(1)+m.group(2).lower()+m.group(3), n)
	# Convert CamelCase to cpp_case.
	n = ''.join('_'+c.lower() if c.isupper() else c for c in n).lstrip('_')
	xlat = {'string':'s', 'double':'d', 'int':'i', 'float':'f', 'char':'c', 'str':'s', 'wstr':'ws', 'true':'if_true', 'false':'if_false',
			'this':'value', 'class':'clazz', 'default':'_default', 'continue':'do_continue', 'const':'constant'}
	n = xlat[n] if n in xlat else n
	hasplain = n in _flookup[filename]
	if name[0] == 'l' and (('p'+name[1:]) in _flookup[filename] or hasplain):
		n = '__'+n if hasplain else '_'+n
	elif name[0] == 'p' and hasplain:
		n = '_'+n
	elif name[:2] in ('ms','sm') and ('m'+name[2:]) in _flookup[filename]:
		n = n+'_'
	return n

def constname(name):
	if '_' in name:
		name = name.replace('_', ' ').title().replace(' ', '')
	if name == name.upper():
		name = name.title()
	return 'k' + name.lstrip('abcdefghijklmnopqrstuvwxyz_')

def incname(name):
	return filename_convert(name)

def balance(s, a, b):
	t = s
	if '"' in s:
		splits = re.split(r'(?<!\\)"', s)[::2]
		if len(splits)%2 == 0:
			t = ' '.join(splits)
	ca,cb = t.count(a),t.count(b)
	while ca < cb:
		s = a+s
		ca += 1
	while ca > cb:
		s += b
		cb += 1
	return s

def treplace(s, src, dst):
	src = r'([^A-Za-z0-9_]+|^)%s([^A-Za-z0-9_]+|$)' % src
	dst = r'\1%s\2' % dst
	return re.sub(src, dst, s)

def update_cpp(fname, src, output):
	newlines = []
	inblock = False
	for line in src.split('\n'):
		line = line.rstrip()
		l = line.lstrip()
		if l == '{':
			prevline,eolcomment = newlines[-1].lstrip(), ''
			if '//' in prevline:
				code = prevline.split('//')[0].rstrip()
				prevline,eolcomment = code,prevline[len(code):]
			if prevline and not prevline.startswith('#') and (prevline[-1] in '):' or prevline[-1].isalnum()):
				s = newlines[-1]
				newlines[-1] = s[:s.index(prevline)+len(prevline)] + ' {' + eolcomment
				continue
		preproc = '#' if l[:1] == '#' else ''
		l = l.replace('#', ' ').lstrip()
		try:
			tokens = list(tokenize(BytesIO(l.encode()).readline))[1:]
		except TokenError as e:
			l = l.split('//')[0]
			l = balance(l, '(', ')')
			l = balance(l, '{', '}')
			l = balance(l, '[', ']')
			l = balance(l, "'", "'")
			try:
				tokens = list(tokenize(BytesIO(l.encode()).readline))[1:]
			except TokenError as e:
				print(e, l)
				continue

		tokens[0] = (tokens[0][0], preproc+tokens[0][1], 0, 0, 0)
		token_len_idx = [i for i,t in sorted(enumerate(tokens), key=lambda it:-len(it[1][1]))]
		for i in range(len(tokens)):
			if not output:
				if _tok(i, tokens) == (OP,'//'):
					break
				if not inblock and _tok(i, tokens) == (OP,'/') and _tok(i+1, tokens) == (OP,'*'):
					inblock = True
				if inblock:
					if _tok(i, tokens) == (OP,'*') and _tok(i+1, tokens) == (OP,'/'):
						inblock = False
					continue
				parse_token(fname, i, tokens)
			else:
				tli = token_len_idx[i]
				typ,name = get_token(tli, tokens)
				#print(' ---', typ, name)
				if typ == 'namespace':
					line = treplace(line, name, namespacename(name))
				elif typ == 'class':
					line = treplace(line, name, classname(name))
				elif typ == 'func':
					line = treplace(line, name, funcname(name))
				elif typ == 'm_var':
					line = treplace(line, name, varname(fname, name)+'_')
				elif typ == 's_var':
					line = treplace(line, name, varname(fname, name))
				elif typ == 'const':
					line = treplace(line, name, constname(name))
				elif typ == 'g_var':
					line = treplace(line, name, 'g_'+varname(fname, name))
				elif typ == 'loc_inc':
					line = treplace(line, name, incname(name))
				elif tli == 0 and (name in ('else','catch', 'break') or (name == 'while' and line.endswith(';'))):
					# Continue else, catch, do...while statements on line with end brace.
					if newlines[-1].endswith('}'):
						line = newlines.pop() + ' ' + line.lstrip()
		newlines.append(line)
	return '\n'.join(newlines)

def update_proj(src):
	newlines = []
	for line in src.split('\n'):
		line = filename_convert(line)
		newlines.append(line)
	return '\n'.join(newlines)

def cpp_convert(filenames):
	# Traverse all files to pick up all unique symbols.
	print('Parsing C++ source...')
	for filename in filenames:
		if not is_cpp_file(filename):
			continue
		print('%s... ' % filename, flush=True, end='')
		src = read(filename)
		if not src:
			print('ignored.')
			continue
		update_cpp(filename, src, False)
		print('parsed.')

	# Update source code.
	print('Updating C++ source and projects...')
	for filename in filenames:
		print('%s... ' % filename, flush=True, end='')
		src = read(filename)
		if not src:
			print('ignored (unable to read).')
			continue
		if is_proj_file(filename):
			newsrc = update_proj(src)
		else:
			newsrc = update_cpp(filename, src, True)
		if newsrc != src:
			open(filename, 'w').write(newsrc)
			print('updated.')
			#print(newsrc)
		else:
			print('unchanged.')

def cpp_convert_dir_entries(root, entries, level=0):
	global _fnames
	es = []
	for e in entries:
		exempt = any(a==e for a in ['.xcodeproj','Contents','Resources','Debug','Release','Profile'])
		n = e if exempt else e.lower()
		n = n if n != 'source' else 'src'
		e,n = [os.path.join(root,x) for x in (e,n)]
		es += [n]
		if n != e:
			os.rename(e, n)
			ebase, nbase = e.rstrip('/').rsplit('/',1)[-1], n.rstrip('/').rsplit('/',1)[-1]
			assert ebase not in _fnames or _fnames[ebase] == nbase
			_fnames[ebase] = nbase
			print('%s -> %s' % (e, n))
	files = [f for f in es if is_cpp_file(f) or is_proj_file(f)]
	dirs  = [d for d in es if os.path.isdir(d)]
	for d in dirs:
		files += cpp_convert_dir_entries(d, os.listdir(d), level+1)
	if level == 0:
		cpp_convert(files)
	return files


assert varname('?', 'lCPUThreadFTW') == 'cpu_thread_ftw'
_flookup['?'].add('pCPUThreadFTW')
assert varname('?', 'lCPUThreadFTW') == '_cpu_thread_ftw'
_flookup['?'].add('cpu_thread_ftw')
assert varname('?', 'lCPUThreadFTW') == '__cpu_thread_ftw'
assert namespacename('UiLepra') == 'uilepra'
s = 'namespace Apa;\nApa::Bepa::Cepa(Thing pThing);'
update_cpp('?', s, False)
assert update_cpp('?', s, True) == 'namespace apa;\napa::Bepa::Cepa(Thing thing);'

cpp_convert_dir_entries('', sys.argv[1:])

for name,typ in sorted(_lookup.items(), key=lambda nt:nt[1]+nt[0]):
	print(typ, name)
