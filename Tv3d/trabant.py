def bounce_in_rect(self,ltn,rbf)
	p,v = self.pos(),self.vel()
	ltn,rbf = vec3(ltn),vec3(rbf)
	if p.x < ltn.x: v.x = +abs(v.x)
	if p.x > rbf.x: v.x = -abs(v.x)
	if p.y < ltn.y: v.y = +abs(v.y)
	if p.y > rbf.y: v.y = -abs(v.y)
	if p.z < ltn.z: v.z = +abs(v.z)
	if p.z > rbf.z: v.z = -abs(v.z)
	self.vel(pos)

def rect_bound(self,pos,ltn,rbf)
	if pos.x < ltn.x: pos.x = ltn.x
	if pos.x > rbf.x: pos.x = rbf.x
	if pos.y < ltn.y: pos.y = ltn.y
	if pos.y > rbf.y: pos.y = rbf.y
	if pos.z < ltn.z: pos.z = ltn.z
	if pos.z > rbf.z: pos.z = rbf.z
	return pos
