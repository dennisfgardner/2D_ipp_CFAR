import argparse

import cv2
import numpy as np
import matplotlib.pyplot as plt

DEBUG = True


def disp_img_cv(img, win_name="Display Window"):
    """display image with OpenCV"""
    cv2.namedWindow(win_name, cv2.WINDOW_NORMAL)
    cv2.resizeWindow(win_name, 1024, 1024)
    cv2.imshow(win_name, img)
    cv2.waitKey(0)


def disp_img_mpl(img, window_title="Display Window"):
    """display image with Matplotlib"""
    fig, ax = plt.subplots(1, 1, figsize=(16, 9))
    ax.imshow(img)
    ax.set_title(window_title)
    plt.show()


def doughnut_mean(img, window_halfwidth, guard_halfwidth):
    """calculate the mean excluding the guard region"""
    win_width = 2*window_halfwidth + 1
    grd_width = 2*guard_halfwidth + 1
    win = cv2.boxFilter(img, cv2.CV_32F, (win_width, win_width),
                        normalize=False, borderType=cv2.BORDER_REPLICATE)
    grd = cv2.boxFilter(img, cv2.CV_32F, (grd_width, grd_width),
                        normalize=False, borderType=cv2.BORDER_REPLICATE)
    return (win - grd)/(win_width**2 - grd_width**2)


def cfar_2d(path_to_image):
    """Constant False Alarm Rate Detector in Two-Dimensions

    The local statistics is calculated around the pixel-under-test excluding a
    guard band.

    _________________________
    |                       |
    |           <-local hw->| local halfwidth
    |       __________      |
    |      |     <-g>|      | guard halfwidth
    |      |    _    |      |
    |      |   |_|   |      | pixel-under-test
    |      |   put   |      |
    |      |_________|      |
    |                       |
    | local stats calculated|
    |_______________________|

    """

    img = cv2.imread(path_to_image, cv2.IMREAD_GRAYSCALE).astype(float)
    if DEBUG:
        print(f"img input min {np.min(img)} max {np.max(img)}")
    # calculate the standard deviation
    local_bkg_halfwidth = 50
    guard_halfwidth = 20
    # E[x], expectation value, i.e., the mean
    E_x = doughnut_mean(img, local_bkg_halfwidth, guard_halfwidth)
    if DEBUG:
        print(f"E_x min {np.min(E_x)} max {np.max(E_x)}")
    # E[x^2] expectation value of image values squared
    E_xsqrd = doughnut_mean(img**2, local_bkg_halfwidth, guard_halfwidth)
    if DEBUG:
        print(f"E_xsqrd min {np.min(E_xsqrd)} max {np.max(E_xsqrd)}")
    # standard deviation is sqrt(E[x^2] - (E[x])^2), see wiki for more details
    std_dev = cv2.sqrt(E_xsqrd - (E_x**2))
    if DEBUG:
        print(f"std_dev min {np.min(std_dev)} max {np.max(std_dev)}")
    # threshold to find pixels of interest
    deviation = 2.5
    threshold_values = E_x + deviation*std_dev
    if DEBUG:
        print(f"threshold_values min {np.min(threshold_values)} max {np.max(threshold_values)}")
    outliers = img > threshold_values

    # disp_img_mpl(img, "Original")
    # disp_img_mpl(E_x, "E_x")
    # disp_img_mpl(std_dev, "std_dev")
    # disp_img_mpl(threshold_values, "threshold_values")
    # disp_img_mpl(outliers, "outliers")

    fig, axs = plt.subplots(2, 1, figsize=(16, 9), sharex=True, sharey=True)
    axs[0].imshow(img)
    axs[0].set_title("input")
    axs[1].imshow(outliers)
    axs[1].set_title("outliers")
    plt.show()


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('-i', '--image', required=True, help='Path to image')
    args = ap.parse_args()

    cfar_2d(args.image)
