# Copyright 2009 Righteous Games# Created by Jonas Byström, 2009-07-17 for Maya/Righteous Engine tool chain.# To be used inside of Maya immediately before saving. "Exports" vertices# and polygons so the rest of the pipeline can produce meshes.import maya.cmds as cmdsdef getnames(nodename):	va = "rgvtx"	nva = nodename+"."+va	pa = "rgf"	npa = nodename+"."+pa	return va, nva, pa, npadef deltime():	s = cmds.ls('rg_export*')	if s:		cmds.delete(*s)def settime():	import datetime	t = datetime.datetime.now().isoformat()	n = cmds.createNode("script", name="rg_export")	cmds.addAttr(longName="time", dt="string")	cmds.setAttr("rg_export.time", t, type="string")def delmesh(nodename):	va, nva, pa, npa = getnames(nodename)	try: cmds.deleteAttr(nva)	except TypeError: pass	try: cmds.deleteAttr(npa)	except TypeError: passdef setmesh(nodename):	cmds.select(nodename)	va, nva, pa, npa = getnames(nodename)	vl = cmds.xform(nodename+'.vtx[:]', q=True, ws=True, t=True)	cmds.addAttr(longName=va, dt="vectorArray")	vcnt = 0	vll = []	vt = []	for v in vl:		vt += [v]		if len(vt) == 3:			vll += [vt, ""]			vt = []			vcnt += 1	vll = vll[:-1]	cmds.setAttr(nva, len(vl)/3, type='vectorArray', *vll)	pcnt = cmds.polyEvaluate(f=True)	cmds.addAttr(longName=pa, dt="string")	pll = "["	for x in range(pcnt):		fnl = cmds.polyListComponentConversion(nodename+".f[%i]" % x, toVertex=True)		fnl = cmds.filterExpand(fnl, sm=31, ex=True)		p = []		for fn in fnl:			vrti = fn.split("[")[1].split("]")[0]			p += [vrti]		pll += "[" + ",".join(p) + "]"	pll += "]"	#print pll	cmds.setAttr(npa, pll, type="string")	return (1, vcnt+pcnt)def exportmesh():	deltime()	s = cmds.ls('*')	for e in s:		delmesh(e)	ocnt = 0	vpcnt = 0	s = cmds.ls('v_*Shape*')	for e in s:		tocnt, tvpcnt = setmesh(e)		ocnt += tocnt		vpcnt += tvpcnt	settime()	print "Successfully set %i vertices+polys in %i objects." % (vpcnt, ocnt)exportmesh()