
# Copyright 2009 Righteous Games
# Created by Jonas Byström, 2009-07-17 for Maya/Righteous Engine tool chain.
# To be used inside of Maya immediately before saving. "Exports" vertices
# and polygons so the rest of the pipeline can produce meshes.

# In Maya 2009 do like this:
# 1. Click on the Script Editor button in the lower-right corner of Maya.
# 2. Make sure the Python tab is active.
# 3. Cut'n'paste this code into the editor.
# 4. Click the "Save Script to Shelf..." button in the Script Editor toolbar.
# 5. Pick any name you like, such as "ExportMesh", click OK.
# 6. Close the editor.
# 7. Click the Py-icon (you just named) in your shelf (below toolbar).


import maya.cmds as cmds


def getnames():
        va = "rgvtx"
        na = "rgn"
        pa = "rgf"
        uva = "rguv0"
        return va, na, pa, uva


def deltime():
        s = cmds.ls('rg_export*')
        if s:
                cmds.delete(*s)


def settime():
        import datetime
        t = datetime.datetime.now().isoformat()
        n = cmds.createNode("script", name="rg_export")
        cmds.addAttr(longName="time", dt="string")
        cmds.setAttr("rg_export.time", t, type="string")


def delmesh(nodename):
        nva, nna, npa, nuva = [nodename+"."+ext for ext in getnames()]
        try: cmds.deleteAttr(nva)
        except TypeError: pass
        try: cmds.deleteAttr(nna)
        except TypeError: pass
        try: cmds.deleteAttr(npa)
        except TypeError: pass
        try: cmds.deleteAttr(nuva)
        except TypeError: pass


def setmesh(nodename):
        cmds.select(nodename)

        exts = getnames()
        va, na, pa, uva = exts
        nva, nna, npa, nuva = [nodename+"."+ext for ext in exts]

        # Vertices first.
        vl = cmds.xform(nodename+'.vtx[:]', q=True, os=True, t=True)
        cmds.addAttr(longName=va, dt="vectorArray")
        vcnt = 0
        vll = []
        vt = []
        for v in vl:
                vt += [v]
                if len(vt) == 3:
                        vll += [vt, ""]
                        vt = []
                        vcnt += 1
        vll = vll[:-1]
        cmds.setAttr(nva, vcnt, type='vectorArray', *vll)

        # Polygons (called faces in Maya).
        pcnt = cmds.polyEvaluate(f=True)
        cmds.addAttr(longName=pa, dt="string")
        pll = []
        for x in range(pcnt):
                fvl = cmds.polyInfo(nodename+".f[%i]" % x, faceToVertex=True)
                p = fvl[0].split()[2:]
                pll += ["[" + ",".join(p) + "]"]
        pll = "[" + ",".join(pll) + "]"
        #print pll
        cmds.setAttr(npa, pll, type="string")
        pl = eval(pll)

        # Normals and UVs (needs polygons).
        ncnt = 0
        nll = []
        uvcnt = 0
        # UVs are only used if user has set a non-default UV-set
        # name (use right-click menu).
        cmds.select(nodename)
        hasUv = (cmds.polyUVSet(q=True, allUVSets=True) != ['map1'])
        #print("Set is: "+str(cmds.polyUVSet(q=True, allUVSets=True)))
        cmds.addAttr(longName=na, dt="vectorArray")
        if hasUv:
                cmds.addAttr(longName=uva, dt="vectorArray")
                uvll = []
        for polyidx in range(pcnt):
                poly = pl[polyidx]
                for vtxidx in poly:
                        sel = nodename+".vtxFace["+str(vtxidx)+"]["+str(polyidx)+"]"
                        cmds.select(sel)
                        n = cmds.polyNormalPerVertex(q=True, xyz=True);
                        nll += [n, ""]
                        ncnt += 1
                        if hasUv:
                                uv_names = cmds.polyListComponentConversion(fromVertexFace=True, toUV=True)
                                if uv_names:
                                        uv_name = uv_names[0]
                                        cmds.select(uv_name)
                                        uv = cmds.polyEditUV(q=True)
                                        uvll += [uv, ""]
                                        uvcnt += 1
        nll = nll[:-1]
        #print "Normals: "+str(nll)
        cmds.setAttr(nna, ncnt, type='vectorArray', *nll)
        if hasUv:
                uvll = uvll[:-1]
                #print "UVs: "+str(uvll)
                cmds.setAttr(nuva, uvcnt, type='vectorArray', *uvll)

        return (1, vcnt+pcnt+ncnt+uvcnt)


def exportmesh():
        deltime()
        s = cmds.ls('*')
        for e in s:
                delmesh(e)
        ocnt = 0
        vpcnt = 0
        s = cmds.ls('m_*', typ="mesh")
        for e in s:
                tocnt, tvpcnt = setmesh(e)
                ocnt += tocnt
                vpcnt += tvpcnt
        settime()
        print "Successfully set %i vertices+polys+normals+UVs in %i objects." % (vpcnt, ocnt)


exportmesh()
