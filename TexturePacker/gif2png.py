from PIL import Image


def iter_frames(image):
	"""
	根据打开的gif的image，yield每一帧的图像
	:param image Image类的对象
	"""
	try:
		i = 0
		while True:
			image.seek(i)
			frame = image.copy()
			if i == 0:
				palette = frame.getpalette()
			else:
				frame.putpalette(palette)
			yield frame
			i += 1
	except EOFError:
		pass


if __name__ == '__main__':
	im = Image.open('star.gif')
	for i, frame in enumerate(iter_frames(im)):
		frame.save('star%02d.png' % i)