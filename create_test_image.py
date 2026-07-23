"""create a simple image to verify 2D cfar detector"""

import cv2
import numpy as np
import matplotlib.pyplot as plt

# image size
N = 128

# create blank image
img = np.zeros((N, N), dtype=np.uint8)

# put a detection at the center
# surrounded by elevated intensity pixels and background
det_val = 255
elv_val = 127
bkg_val = 63

cen_loc = N//2 + 1


grd_size = 3
bkg_size = 7

# add background
img[cen_loc - bkg_size: cen_loc + bkg_size + 1, 
    cen_loc - bkg_size: cen_loc + bkg_size + 1] = bkg_val

# add elevated pixels
img[cen_loc - grd_size: cen_loc + grd_size + 1,
    cen_loc - grd_size: cen_loc + grd_size + 1] = elv_val

# add detection
img[cen_loc, cen_loc] = det_val

# view image
clr_map = "gray"
font_sz = 20

fig, axs = plt.subplots(1, 2, sharex=True, sharey=True, figsize=(16, 9))
axs[0].set_title('PColor Plot with Grid Lines', fontsize=font_sz)
axs[0].pcolor(img, edgecolors='w', lw=0.25, cmap=clr_map)
axs[0].set_aspect('equal')
axs[1].imshow(img, cmap=clr_map)
axs[1].set_title('Image Plot', fontsize=font_sz)

plt.show()

# save test image
success = cv2.imwrite("test_image.png", img)

if success:
    print("Image saved successfully.")
else:
    print("Failed to save image.")
