#!/usr/bin/env python3

import codecs
from io import BytesIO
import re
import os
import sys
from tokenize import tokenize, NAME, NUMBER, OP, COMMENT, STRING, TokenError

_lookup = {}

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
	if not os.path.isfile(filename):
		return False
	return any(filename.endswith(e) for e in ['.cpp', '.cxx', '.mm', '.c', '.m', '.h', '.hpp', '.inl'])

def parse_token(i, tokens):
	global _lookup
	typ,name = _tok(i, tokens)
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
		if '(' not in name:
			_lookup[name] = 'macro'
		return
	istype = (tokens[0][1] == 'typedef')
	iscamel = (name != name.lower() and name != name.upper())
	isvar = (name1 and name1 in ';=[' and not isclass and not istype)
	if name1 in ';,.-><:?()[+=-=*=/=%=&=|=~=^=':
		if name0 == 'namespace' and iscamel:
			_lookup[name] = 'namespace'
		elif (isclass or istype) and iscamel:
			_lookup[name] = 'class'
		elif isvar and name[0].islower() and iscamel and 'const' in [t[1] for t in tokens[max(0,i-4):i]]:
			_lookup[name] = 'const'
		elif isvar and name[0] in 'ms' and iscamel:
			_lookup[name] = 'm_var'
		elif isvar and name[0] == 'g' and iscamel:
			_lookup[name] = 'g_var'
		elif isvar and name == name.upper():
			_lookup[name] = 'const'
		elif (i==0 or 'enum' in [t[1] for t in tokens]) and name == name.upper() and name1 in ('',',','}'):
			_lookup[name] = 'const'	# enum
		elif isvar and name[0].islower() and iscamel:
			_lookup[name] = 's_var'
		elif name1 and name1 in '),' and name[0] == 'p' and iscamel:
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

def namespacename(name):
	# Convert CPUThread to CpuThread.
	name = re.sub('([A-Z])([A-Z]+)([A-Z]|$)', lambda m: m.group(1)+m.group(2).lower()+m.group(3), name)
	# Convert CamelCase to cpp_case.
	return ''.join('_'+c.lower() if c.isupper() else c for c in name).lstrip('_')

def classname(name):
	return name

def funcname(name):
	return name

def varname(name):
	# Remove hungarian notation.
	n = name.lstrip('abcdefghijklmnopqrstuvwxyz_')
	if not n or n.isdigit():
		return name
	n = namespacename(n)
	xlat = {'string':'s','double':'d','int':'i','float':'f','str':'s','wstr':'ws'}
	n = xlat[n] if n in xlat else n
	return n

def constname(name):
	if '_' in name:
		name = name.replace('_', ' ').title().replace(' ', '')
	if name == name.upper():
		name = name.title()
	return 'k' + name.lstrip('abcdefghijklmnopqrstuvwxyz_')

def incname(name):
	return name.lower()

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

def update(src, output):
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
			if prevline and (prevline[-1] in '):' or prevline[-1].isalnum()):
				s = newlines[-1]
				newlines[-1] = s[:s.index(prevline)+len(prevline)] + ' {' + eolcomment
				continue
		preproc = '#' if l[:1] == '#' else ''
		l = l.replace('#', ' ').lstrip()
		l = balance(l, '(', ')')
		l = balance(l, '{', '}')
		l = balance(l, '[', ']')
		try:
			tokens = list(tokenize(BytesIO(l.encode()).readline))[1:]
			tokens[0] = (tokens[0][0], preproc+tokens[0][1], 0, 0, 0)
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
					parse_token(i, tokens)
				else:
					typ,name = get_token(i, tokens)
					#print(typ, name, i, l, line)
					if typ == 'namespace':
						line = line.replace(name, namespacename(name))
					elif typ == 'class':
						line = line.replace(name, classname(name))
					elif typ == 'func':
						line = line.replace(name, funcname(name))
					elif typ == 'm_var':
						line = line.replace(name, varname(name)+'_')
					elif typ == 's_var':
						line = line.replace(name, varname(name))
					elif typ == 'const':
						line = line.replace(name, constname(name))
					elif typ == 'g_var':
						line = line.replace(name, 'g_'+varname(name))
					elif typ == 'loc_inc':
						line = line.replace(name, incname(name))
					elif i == 0 and (name in ('else','catch', 'break') or (name == 'while' and line.endswith(';'))):
						# Continue else, catch, do...while statements on line with end brace.
						if newlines[-1].endswith('}'):
							line = newlines.pop() + ' ' + line.lstrip()
		except TokenError as e:
			print(e, l)
		newlines.append(line)
	return '\n'.join(newlines)

def cpp_convert(filenames):
	# Traverse all files to pick up all unique symbols.
	print('Parsing C++ source...')
	for filename in filenames:
		print('%s... ' % filename, flush=True, end='')
		src = read(filename)
		if not src:
			print('ignored.')
			continue
		update(src, False)
		print('parsed.')

	# Update source code.
	print('Updating C++ source...')
	for filename in filenames:
		print('%s... ' % filename, flush=True, end='')
		src = read(filename)
		if not src:
			print('ignored.')
			continue
		newsrc = update(src, True)
		if newsrc != src:
			#open(filename, 'w').write(newsrc)
			print('updated.')
			print(newsrc)
		else:
			print('unchanged.')

def cpp_convert_dir_entries(root, entries, level=0):
	es = []
	for e in entries:
		l = e.lower()
		exempt = any(a==e for a in ['.xcodeproj','Contents','Resources','Debug','Release','Profile'])
		n = e if exempt else e.lower()
		n = n if n != 'source' else 'src'
		e,n = [os.path.join(root,x) for x in (e,n)]
		es += [n]
		if n != e:
			#os.rename(e, n)
			print('%s -> %s' % (e, n))
			es.pop()
			es += [e]
	files = [f for f in es if is_cpp_file(f)]
	dirs  = [d for d in es if os.path.isdir(d)]
	for d in dirs:
		files += cpp_convert_dir_entries(d, os.listdir(d), level+1)
	if level == 0:
		cpp_convert(files)
	return files


assert varname('CPUThreadFTW') == 'cpu_thread_ftw'

cpp_convert_dir_entries('', sys.argv[1:])

for name,typ in sorted(_lookup.items(), key=lambda nt:nt[1]+nt[0]):
	print(typ, name)
