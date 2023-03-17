# Two-Dimensional (2D) Intel Integrated Performance Primitives (IPP) Constant False Alarm Rate (CFAR) Detector

Using's Intel's Integrated Performance Primitives (IPP) we make a two-dimensional Constant False Alarm Rate (CFAR) detector.
This is code is useful when detecting relatively bright and small objects in a image.
For example, if you wanted to detect the locations of stars in an image, but that image had a non-uniform background so a simple threshold hold will not work.

## Setup

[IPP](https://www.intel.com/content/www/us/en/developer/tools/oneapi/ipp.html) needs to be installed.
A standalone installation of IPP is used.
A getting started guide is [here](https://www.intel.com/content/www/us/en/develop/documentation/get-started-with-ipp-for-oneapi-linux/top.html) with some example code which is copied to `ipp_getting_started_example.cpp` with the `@` characters removed (g++ did not compile with unicode).

To compile, first in the terminal `source /opt/intel/oneapi/ipp/latest/env/vars.sh`.
Then 

```bash
g++ ipp_getting_started_example.cpp -o ipp_getting_started_example -I $IPPROOT/include -L $IPPROOT/lib/linux -lippcore
```

Finally, execute `./ipp_getting_started_example`.
You should see a print out of a table with features and their support.

[OpenCV](https://opencv.org/) is used to read in images.
It's built from source.
A function to test loading and displaying an image can be built and ran with

```bash
 g++ open_image.cpp -o open_image `pkg-config --cflags --libs opencv4` && ./open_image 
```

The above code uses [this stary image](https://pixabay.com/photos/astronomy-bright-constellation-dark-1867616/) the open source image from pixabay.

## Main Program

add IPP headers and libraies to path

```bash
source /opt/intel/oneapi/ipp/latest/env/vars.sh 
```

Build and run with

```bash
g++ main.cpp -o main -I $IPPROOT/include -L $IPPROOT/lib/intel64 -lippi -lipps -lippcore -lippcv `pkg-config --cflags --libs opencv4` && ./main 
```

Build and check for memory leaks with:

```bash
g++ main.cpp -g -oo -Wextra -pedantic -Wshadow -I $IPPROOT/include -L $IPPROOT/lib/intel64 -lippi -lipps -lippcore -lippcv `pkg-config --cflags --libs opencv4` -o main && valgrind --leak-check=full --show-leak-kinds=all -s ./main 
```
