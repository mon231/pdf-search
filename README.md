# pdf-search
Cross platform program used to find which pdf contains given image <br />
Designed to search inside a root of pdf files and subfolders <br />
The project is still in WIP conditions and might be a bit buggy

## POC
We provide a python one-file script as a poc of program usage and features

## cpp project
We provide a cpp project using cross-platform cmake for windows and linux compilations, <br />
In order to support various users using common libs

### algorithm
We'll use a thread which processes images from given queue, and searches for the given image inside them <br />
In addition, we'll use multiple threads which iterates pdf file from the root (rglob), <br />
And add them to queue for further processing by the thread above

### 3rd party libs
We'll use 3rd-party libs as submodules which points to the latest release-tags of these librarys <br />
We'll accept the license of the submodule release pointed by our submodule

#### opencv
We'll use opencv for image file processing (search template in given image)

#### poppler
We'll use poppler for pdf processing (split pdf to it's pages as images) <br />
It doesn't know how to behave as a subproject, so you'll have to install it by yourself, <br />
For example using `vcpkg install poppler:x86-windows` or `apt install`
