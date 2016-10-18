from time import time
from trabant.math import lerp
from collections import defaultdict

_t = time()
_last_notice_delay_name = None
_last_normal_time = 0.001
_notice_times = defaultdict(lambda:[0,0.001])

def notice_delay(name, normal_time=0.001, extra=''):
	global _t, _last_notice_delay_name, _last_normal_time
	dt = time() - _t
	_t = time()
	l = _notice_times[_last_notice_delay_name]
	l[0] += 1
	normal_val = max(_last_normal_time, l[1])
	_last_normal_time = normal_time
	if l[0] > 20 and normal_val*2 < dt:
		print('%s overrun by %g%% (%f -> %f). %s' % (_last_notice_delay_name, (dt-normal_val)*100/normal_val, normal_val, dt, str(extra)))
	l[1] = lerp(normal_val, dt, 0.4)
	_last_notice_delay_name = name
