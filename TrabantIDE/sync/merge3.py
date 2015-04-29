#!/usr/bin/env python3

from codecs import open as codecs_open


class mergefile:
	def __init__(self,name,src=None,preprocess=lambda s:s):
		self.name = name
		self.src = src if src else name
		self.preprocess = preprocess


def diff2_lines(linesa, linesb, ia_norecurse=-1, ib_norecurse=-1):
	'''Generates a bunch of 2-tuples. On the left is original version of each line. To the right are the replacement lines in B.'''
	ia,ib,l,toggleab = 0,0,max(len(linesa),len(linesb)),False
	a,b = None,[]
	while ia<len(linesa) or ib<len(linesb):
		if ia >= len(linesa):
			b += [linesb[ib]]
			ib += 1
		elif ib >= len(linesb):
			if a!=None: yield a,b
			a,b = linesa[ia],[]
			ia += 1
		elif linesa[ia] == linesb[ib]:
			if a!=None: yield a,b
			a,b = linesa[ia], (b if a==None else [])+[linesb[ib]]
			ia,ib = ia+1,ib+1
			#toggleab = False
		# elif not linesa[ia] and ia != ia_norecurse:
			# diff_remove = list(diff2_lines(linesa[ia+1:],linesb[ib:]))
			# diff_replace = list(diff2_lines(linesa[ia:],linesb[ib:],0,-1))
			# if len(diff_remove)+1<len(diff_replace):
				# yield '',None
				# dl = diff_remove
			# else:
				# dl = diff_replace
			# for d in dl:
				# yield d
			# break
		# elif not linesb[ib] and ib != ib_norecurse:
			# diff_add = list(diff2_lines(linesa[ia:],linesb[ib+1:]))
			# diff_replace = list(diff2_lines(linesa[ia:],linesb[ib:],-1,0))
			# if len(diff_add)<len(diff_replace):
				# yield None,''
				# dl = diff_add
			# else:
				# dl = diff_replace
			# for d in dl:
				# yield d
			# break
		else:
			#print(linesa[ia], '!=', linesb[ib], 'so searching...')
			sa,sb = set(linesa[ia:]),set(linesb[ib:])
			if linesa[ia] in sb:
				#print(linesa[ia], '(in A) found in remainder of B, stepping up b.')
				b += [linesb[ib]]
				ib += 1
				#toggleab = False
			elif linesb[ib] in sa:
				#print(linesb[ib], '(in B) found in remainder of A, stepping up a.')
				if a!=None: yield a,b
				a,b = linesa[ia],[]
				ia += 1
			else:
				if a!=None: yield a,b
				a,b = linesa[ia],[linesb[ib]]
				ia,ib = ia+1,ib+1
	if a!=None: yield a,b


def merge3_lines(baselines, linesa, linesb, srca='A', srcb='B'):
	'''Generates a bunch of 2-tuples. The tuple contains previous version and current version.'''
	if baselines == ['']:
		# If this is a first merge we assume it's started from a common denominator where the head is the same.
		baselines = []
		for a,b in zip(linesa,linesb):
			if a != b:
				break
			baselines.append(a)
		if not baselines:	# Possibly starts with conflict?
			baselines = ['']
	da,db = list(diff2_lines(baselines,linesa)),list(diff2_lines(baselines,linesb))
	#print(da)
	#print(db)
	ca,cb = [],[]
	assert len(da) == len(db)
	def use_next_line(line, nextidx, otherdiffs):
		if nextidx < len(otherdiffs):
			return line in otherdiffs[nextidx][1]
		return False
	output = []
	for i,ra in enumerate(da):
		oa,la = ra
		ob,lb = db[i]
		assert oa==ob
		asame = (len(la)>=1 and oa==la[0])
		bsame = (len(lb)>=1 and ob==lb[0])
		if asame or bsame:
			output += [('c',ca,cb)] if ca or cb else []
			ca,cb = [],[]
			if not asame: lb = lb[1:]	# Modified in A, but unchanged in B. Skip B.
			if not bsame: la = la[1:]	# Modified in B, but unchanged in A. Skip A.
			if asame and bsame:
				output += [('=',[oa],[])]
				la,lb = la[1:],lb[1:]
		if la==lb or (asame and not la) or (bsame and not lb):
			output += [('c',ca,cb)] if ca or cb else []
			ca,cb = [],[]
			if la:
				output += [('a', [a for a in la if not use_next_line(a, i+1, db)], [])]
			else:
				output += [('b', [], [b for b in lb if not use_next_line(b, i+1, da)])]
		else:
			ca,cb = ca+la,cb+lb
	output += [('c',ca,cb)] if ca or cb else []

	# Touch-up conflicts.
	i = 0
	for t,ca,cb in output:
		if t == 'c':
			# One-sided change+conflict is probably remove+conflict.
			if not cb and i > 0 and output[i-1][0] == 'b' and len(output[i-1][2])==1:
				cb = [output[i-1][2][-1]] + cb
				output[i] = ('c', ca, cb)
				output[i-1] = ('>', [], [])
			elif not cb and i > 0 and output[i-1][0] == 'a' and len(output[i-1][1])==1:
				ca = [output[i-1][1][-1]] + ca
				output[i] = ('c', ca, cb)
				output[i-1] = ('>', [], [])

			# Conflict ending in same way means conflict stopped some lines back.
			same_ending = []
			for a,b in zip(reversed(ca),reversed(cb)):
				if a != b:
					break
				same_ending += [a]
				ca,cb = ca[:-1],cb[:-1]
			if same_ending:
				same_ending = list(reversed(same_ending))
				if not ca or not cb:
					# One turned out empty after we ejected the similarities at the end.
					# This means no conflict, only one-sided add.
					output[i] = ('>', ca+cb, [])
				else:
					output[i] = ('c', ca, cb)
				output = output[:i+1] + [('>', same_ending, [])] + output[i+1:]
				i += 1
				ca,cb = [],[]
		i += 1
	# Compact output
	output = [(t,la,lb) for t,la,lb in output if (la or lb)]
	# Join conflicts.
	i = 0
	for t,ca,cb in output:
		if t == 'c':
			# Join conflicts.
			if i > 0 and output[i-1][0] == 'c':
				ca = output[i-1][1] + ca
				cb = output[i-1][2] + cb
				output[i] = ('c', ca, cb)
				output[i-1] = ('>', [], [])
		i += 1

	# Yield it.
	for t,la,lb in output:
		if t == 'c' and la and lb:
			yield '<<<<< '+srca
			for a in la: yield a
			yield '====='
			for b in lb: yield b
			yield '>>>>> '+srcb
		else:
			for l in la+lb:
				yield l


def merge3_str(base, a, b, srca='A', srcb='B'):
	if a == b:
		return a
	return '\n'.join(merge3_lines(base.split('\n'), a.split('\n'), b.split('\n'), srca, srcb))


def merge3(basefile, filea, fileb):
	'''Input files are mergefiles with .name=filename and .src=merge-diff-name. Returns the resulting merged string.'''
	baselines,linesa,linesb = [_read(n.name,n.preprocess) for n in (basefile,filea,fileb)]
	return merge3_str(baselines,linesa,linesb,filea.src,fileb.src)


def merge3_full(basefile, filea, fileb):
        '''Input files are mergefiles with .name=filename and .src=merge-diff-name. Returns the base, local, remote and merged string.'''
        baselines,linesa,linesb = [_read(n.name,n.preprocess) for n in (basefile,filea,fileb)]
        return baselines,linesa,linesb,merge3_str(baselines,linesa,linesb,filea.src,fileb.src)


def _read(name, massage):
	try:
		with codecs_open(name,'rb','utf-8') as f:
			return massage(f.read().replace('\r\n','\n'))
	except Exception as e:
		#print(e)
		return ''

if __name__ == '__main__':
	import sys
	store,local,remote = mergefile(sys.argv[-3]), mergefile(sys.argv[-2]), mergefile(sys.argv[-1])
	print(merge3(store,local,remote))
