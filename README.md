# project-find-circle
Circular object detection using edge detection and Hough transform

## Description
This code takes a path to a directory and performs Hough Circle detection on each compatible image and writes seperately for each image the accumulator image, the original image with found circle centers, as well as an image with the two images next to each other for comparison.

## Requirements
- Working C++ compiler (C++ 17 or greater)
- CMake (version >= 2.8)
- OpenCV (version >= 4.0.0)

## Usage
- `./findCircle <path to directory containing the images> <output file prefix> <minimum diameter> <maximum diameter> <threshold>`

## Future Works
- Improve edge detection technique to combat noisy edge image
- Implement blob detection to find only one center for each circle

## Example Results

| ![output_blood_combined](example_results/output_blood_combined.png) | 
|:--:| 
| *Circles detected for blood image* |

| ![output_cable_combined](example_results/output_cable_combined.png) | 
|:--:| 
| *Circles detected for cable image* |

| ![output_cells_combined](example_results/output_cells_combined.png) | 
|:--:| 
| *Circles detected for cells image* |

| ![output_circles_combined](example_results/output_circles_combined.png) | 
|:--:| 
| *Circles detected for circles image* |


## How to build
### Linux or MacOS
- Build project
  - create and navigate into a new directory in the cloned project directory: `mkdir build && cd build`
  - execute the following commands: `cmake ..` and `make`
- Run executable
  - to run the executable, execute the following: `./findCircle <path to directory containing the images> <output file prefix> <minimum diameter> <maximum diameter> <threshold>`
    - ex. command: `./findCircle ../3_hough test 20 30 35`
    
### Windows
- ***add the binary directory of OpenCV to System (or User) PATH variable***
  - ex. path: `C:\OpenCV-4.5.3\opencv\build\x64\vc15\bin`
- Build project
  - create and navigate into a new directory in the cloned project directory: `mkdir build && cd build`
  - execute the following command: `cmake -DOpenCV_DIR=<path to 'build' directory of OpenCV> ..`
    - ex. path: `C:\OpenCV-4.5.3\opencv\build`
  - open the build folder of the project and open the *.sln* Visual Studio file
  - change the startup project to *getThreshold* by right clicking on *getThreshold* in the Solution Explorer and selecting *Set as StartUp Project*
  - run code by clicking on the green arrow or pressing F5
- Run executable
  - open Command Prompt (or PowerShell) and navigate to the project build folder, then: `cd Debug`
  - in Debug, run the following command: `getCircle.exe <path to directory containing the images> <output file prefix> <minimum diameter> <maximum diameter> <threshold>`
    - ex. command: `getCircle.exe ../3_hough test 20 30 35`

