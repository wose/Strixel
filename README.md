# Strixel

Strixel will convert a grayscale image to a knitting pattern. It is
inspired by the amazing work done by
[Petros Vrellis](http://artof01.com/vrellis/). Checkout his projects
and take a look at [this video](https://vimeo.com/175653201) to get an
idea what this software tries to reproduce and accomblish.

## Current State
:warning: very early stage / messy code

![Screenshot](doc/screenshot.png)

![Dinklage](doc/dinklage.png)

## Usage

Works only with square images at the moment.

```shell
% ./strixel image.png
```

## Hacking

```shell
% git clone --recursive -j4 https://github.com/wose/Strixel.git
% cd Strixel
% mkdir build && cd build
% cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
```

## Ideas

*(will add some in the next days)*
