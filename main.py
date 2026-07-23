import argparse
from pathlib import Path

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


def cfar_2d(path_to_image, guard_halfwidth, local_bkg_halfwidth, deviation):
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

    # default values
    if guard_halfwidth is None:
        guard_halfwidth = 20
    if local_bkg_halfwidth is None:
        local_bkg_halfwidth = 50
    if deviation is None:
        deviation = 2.5
    if DEBUG:
        print("input parameters:")
        print(f"\t{path_to_image=}")
        print(f"\t{guard_halfwidth=}")
        print(f"\t{local_bkg_halfwidth=}")
        print(f"\t{deviation=}")

    # read image
    img = cv2.imread(path_to_image, cv2.IMREAD_GRAYSCALE).astype(float)
    if DEBUG:
        print("input image read:")
        print(f"\tshape: {img.shape}")
        print(f"\ttype: {img.dtype}")
        print(f"\tmin {np.min(img)}, max {np.max(img)}")
        print(f"\tcenter pixel: {img[img.shape[0]//2 + 1, img.shape[1]//2 + 1]}")
        disp_img_mpl(img, "Input Image")

    # the next few chunks of code is going to calculate standard deviation as
    # sqrt(E[x^2] - (E[x])^2) where E is the expectation value (i.e. the mean)
    # see wiki for details https://en.wikipedia.org/wiki/Standard_deviation
    # |
    # |
    # V
    #
    # E[x]
    E_x = doughnut_mean(img, local_bkg_halfwidth, guard_halfwidth)
    if DEBUG:
        print("Expectation Value Calculated")
        print(f"\tmin {np.min(E_x)} max {np.max(E_x)}")
        print(f"\tcenter pixel: {E_x[E_x.shape[0]//2 + 1, E_x.shape[1]//2 + 1]}")
        disp_img_mpl(E_x, "Expectation Value")

    # E[x^2] expectation value of image values squared
    E_xsqrd = doughnut_mean(img**2, local_bkg_halfwidth, guard_halfwidth)
    if DEBUG:
        print("Image**2 Expectation Value Calculated")
        print(f"\t min {np.min(E_xsqrd)} max {np.max(E_xsqrd)}")
        print(f"\tcenter pixel: {E_xsqrd[E_xsqrd.shape[0]//2 + 1, E_xsqrd.shape[1]//2 + 1]}")
        disp_img_mpl(E_xsqrd, "Image**2 Expectation Value")

    # standard deviation 
    std_dev = cv2.sqrt(E_xsqrd - (E_x**2))
    if DEBUG:
        print("Standard Deviation Calculated")
        print(f"\tmin {np.min(std_dev)} max {np.max(std_dev)}")
        print(f"\tcenter pixel: {std_dev[std_dev.shape[0]//2 + 1, std_dev.shape[1]//2 + 1]}")
        disp_img_mpl(std_dev, "Standard Deviation")
    # ^
    # |
    # |
    # end of standard deviation calculation

    # threshold to find pixels of interest
    threshold_values = E_x + deviation*std_dev
    if DEBUG:
        print("Threshold Calculated")
        print(f"\t min {np.min(threshold_values)} max {np.max(threshold_values)}")
        print(f"\tcenter pixel: {threshold_values[threshold_values.shape[0]//2 + 1, threshold_values.shape[1]//2 + 1]}")

        disp_img_mpl(threshold_values, "Threshold Values")
    outliers = img > threshold_values

    fig, axs = plt.subplots(1, 2, figsize=(16, 9), sharex=True, sharey=True)
    axs[0].imshow(img)
    axs[0].set_title("input")
    axs[1].imshow(outliers)
    axs[1].set_title("outliers")
    plt.show()


def main():
    """main entry point, must pass in filepath to image"""
    ap = argparse.ArgumentParser()
    ap.add_argument('-I', '--image', required=True, help='Path to image')
    ap.add_argument('-G', '--guard', required=False, type=int,
                    help='inner guard size halfwidth')
    ap.add_argument('-B', '--bkg', required=False, type=int,
                    help='outer local background halfwidth')
    ap.add_argument('-T', '--threshold', required=False, type=int,
                    help='standard deviation threshold')
    args = ap.parse_args()

    if Path(args.image).exists():
        cfar_2d(args.image, args.guard, args.bkg, args.threshold)
    else:
        print("image filepath does not exist")


if __name__ == '__main__':
    main()
