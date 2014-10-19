#!/usr/bin/env python3

def drop_empty_head_tail(shape):
	o,shape = None,list(shape)
	while o != shape:
		o = shape
		if not list(filter(lambda s:s.strip(),shape[0])):
			shape = shape[1:]
		if not list(filter(lambda s:s.strip(),shape[-1])):
			shape = shape[:-1]
	return shape


def crop_shape(shape):
	tallest_layer = max(len(layer) for layer in shape)
	longest_line = max(len(row) for layer in shape for row in layer)
	shape = [layer+['']*(tallest_layer-len(layer)) for layer in shape]	# Make all layers equally tall.
	shape = [[line.ljust(longest_line) for line in layer] for layer in shape]	# Make all rows equally long.
	shape = drop_empty_head_tail(shape)		# Drop empty layers in front and back.
	xpose = drop_empty_head_tail(zip(*shape))	# Transpose layers ('deep' instead of 'high'), drop empty rows on top and bottom.
	xpose = [[''.join(s) for s in zip(*layer)] for layer in zip(*xpose)]	# Transpose back to front-to-back layers, then turn rows into columns.
	xpose = drop_empty_head_tail(zip(*xpose))	# Transform to place all columns together, drop empty left-most columns.
	shape = [[''.join(s) for s in zip(*cols)] for cols in zip(*xpose)]	# Transform back to layers, columns turn back into rows.
	return shape


def load_shape(ascii):
	shape = []	# List of layers.
	layer = []	# List of strings. One string per row.
	for line in open(ascii+'.txt'):
		if '---' in line:
			shape += [layer]
			layer = []
		else:
			layer += [line.rstrip('\n')]
	shape += [layer]
	return crop_shape(shape)

def save_shape(shape):
	splitter = '\n' + '-'*max(3,len(shape[0][0])) + '\n'
	s = splitter.join(['\n'.join(layer) for layer in shape])
	print(s)
