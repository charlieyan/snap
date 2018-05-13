#!/usr/bin/env python

import os
import numpy as np
import cv2

from ego_atheris.interaction.utils import Util

def yuv_to_rgb(y, u, v):
  b = 1.164*(y - 16) + 2.018*(u - 128)
  g = 1.164*(y - 16) - 0.813*(v - 128) - 0.391*(u - 128)
  r = 1.164*(y - 16) + 1.596*(v - 128)
  return r, g, b

def h(l):
  l_int = int(l.strip())

  yuv_lowest_2 = l_int & 3 # 0b11 is lower 2 bits
  v = yuv_lowest_2

  yuv_middle_2 = (l_int >> 2) & 3 # 0b11 is lower 2 bits after right shifting
  u = yuv_middle_2

  yuv_top_4 = (l_int >> 4) & 15 # 0b1111 is 15, lower 4 bits
  y = yuv_top_4

  # r, g, b = yuv_to_rgb(y, u, v)
  # return [r, g, b]

  return [y, u, v]

pixels = Util.read_file_into_list("./ali42_first_frame.txt", h)

img = np.zeros([480, 640, 3])

# img[:,:,0] = np.ones([5,5])*64/255.0
# img[:,:,1] = np.ones([5,5])*128/255.0
# img[:,:,2] = numpy.ones([5,5])*192/255.0

# cv2.imwrite('color_img.jpg', img)
# cv2.imshow("image", img);
# cv2.waitKey();


for i in xrange(480):
  for j in xrange(640):
    p = pixels[i*480+j]
    img[i, j, 0] = p[0] / 255.0
    img[i, j, 1] = p[1] / 255.0
    img[i, j, 2] = p[2] / 255.0

while(True):
  cv2.imshow("test", img)

  k = cv2.waitKey(2)
  if k == 113:
    break

cv2.destroyAllWindows()