# 598APE-HW1

## Tools

This program assumes the following are installed on your machine:
* A working C++ compiler (g++ is assumed in the Makefile)
* make
* ImageMagick (for importing and exporting non-ppm images)
* FFMpeg (for exporting movies from image sequences)

If any of the above is missing, you can run the following commands in a terminal:
* `sudo apt update`
* `sudo apt install build-essential`
* `sudo apt install imagemagick`
* `sudo apt install ffmpeg`

## Instructions

After pulling the code, open a terminal in the folder the code was pulled into.\
Run `make`.\
After main.exe is created, you can either run the program with preset commands or custom commands.\
We used preset commands over 3 runs to benchmark each optimization.

### Preset commands
Usage:
```
./main.exe <test name> <optimization> <runs>
```

Test names:
* pianoroom
* globe
* sphere
* elephant

Optimizations:
* default
* o1
* o2
* o3
* o4
* o5
* o6
* o7
* all

Runs: a positive integer > 0 representing the number of runs the test perform.

Example:
```
./main.exe pianoroom all 3
```

Present commands are run with the following flags by default:
* `./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500`
* `./main.exe -i inputs/globe.ray --ppm -a inputs/globe.animate --movie -F 24`
* `./main.exe -i inputs/sphere.ray --ppm -a inputs/sphere.animate --movie -F 24 -W 100 -H 100 -o output/sphere.mp4`
* `./main.exe -i inputs/elephant.ray --ppm -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/elephant.mp4`

Note that because Piano Room's preset outputs a ppm file, using a custom command is necessary to create and view a png file.

### Custom commands
Usage:
```
./main.exe --help
# Prints the following
# Usage ./main.exe [-H <height>] [-W <width>] [-F <framecount>] [--movie] [--no-movie] [--png] [--ppm] [--help] [-o <outfile>] [-i <infile>] [-a <animationfile>]
```

Example:
```
./main.exe -i inputs/pianoroom.ray --png -o output/pianoroom.png -H 500 -W 500
```

Custom commands accept optimizations flags but do not accept a flag for the number of runs.\
Example:
```
./main.exe -i inputs/pianoroom.ray --png -o output/pianoroom.png -H 500 -W 500 all
```

## Optimizations

Optimization flags correspond to the following:
* default: no optimizations
* o1: first ray hit
* o2: bounding box
* o3: parallelization
* o4: scalar calculations
* o5: function inlining
* o6: mesh BVH
* o7: checkerboard interpolation
* all: o1 to o7 combined
