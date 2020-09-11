# Strixel

Strixel will convert a grayscale image to a string art pattern. It is
inspired by the amazing work done by
[Petros Vrellis](http://artof01.com/vrellis/). Checkout his projects
and take a look at [this video](https://vimeo.com/175653201) to get an
idea what this software tries to reproduce and accomblish.

## Rewrite
This is a rewrite of the original code in `Rust`. I recently had more
use for `Strixel` but didn't get it to compile without fiddling with my
`OpenCV` installation. So I started to rewrite it in Rust.

## Hacking

```shell
git clone https://github.com/wose/Strixel.git
cd Strixel
cargo run --release
```
