# Two-Dimensional (2D) Intel Integrated Performance Primitives (IPP) Constant False Alarm Rate (CFAR) Detector

Using's Intel's Integrated Performance Primitives (IPP) we make a two-dimensional Constant False Alarm Rate (CFAR) detector.
This is code is useful when detecting relatively bright and small objects in a image.
For example, if you wanted to detect the locations of stars in an image, but that image had a non-uniform background so a simple threshold hold will not work.

## Setup

[IPP](https://www.intel.com/content/www/us/en/developer/tools/oneapi/ipp.html) needs to be installed.
A standalone installation of IPP is used.
A getting started guide is [here](https://www.intel.com/content/www/us/en/develop/documentation/get-started-with-ipp-for-oneapi-linux/top.html) with some example code which is copied to `ipp_getting_started_example.cpp` with the `@` characters removed (g++ did not compile with unicode).
To compile, first in the terminal `source /opt/intel/oneapi/ipp/latest/env/vars.sh`.
Then `g++ ipp_getting_started_example.cpp -o ipptest -I $IPPROOT/include -L $IPPROOT/lib/linux -lippcore`.
Finally, `./ipptest`.
You should see a print out of a table with features and their support.
