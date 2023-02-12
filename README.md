# K-Compress
Image compression with K-means clustering.

# Description
K-Compress compresses 24-bit uncompressed BMP 4.0 files using K-means clustering. The compressed images take up only about 33% of the initial space! (For smaller images, the effects may be less profound.)

# How to run
1. Compile `compress.c`. Eg. `gcc compress.c -o compress.exe`
2. Compile `decompress.c`. Eg. `gcc decompress.c -o decompress.exe`
3. To compress an image, run the compiled `compress.c` file, passing in the path to the input image and the path to the compressed output. 
Eg. `compress.exe path/to/image compressed.out`.
4. To decompress an image, run the compiled `decompress.exe` file, passing in the path to the compressed image and a path for the decompressed output. 
Eg. `decompress.c compressed.out decompressed.bmp`.

# Credits
`bmp.h` was taken from [CS50's Introduction to Computer Science](https://www.edx.org/course/introduction-computer-science-harvardx-cs50x).
