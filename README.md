# asciify - Yet another images-to-ascii-art converter

## Why?
Maybe because I can? Why you ever asking that question?
Also I just wanted a reason to learn C and have no other way other than
make something like this.

## So then how it works?
After building everithing as described in section bellow, you should be
left with %d nice binaries in `build` folder. Everyone of them accept
simillar arguments with slight changes. Common syntax for them as follows:
`build/asciify_NAME -W80 -H24 -T -o output.txt image.png`, where
 * `-W80` is maximum width in characters, optional
 * `-H24` is maximum height, optional
 * `-T` (or `-G`) is color mode (`-T` for truetype, `-G` for black-and-white,
    if available, and nothing for default 256-color mode), optional
 * `-o filename.txt` is output filename, optional, defaults to STDOUT
 * `image.png` is input image. Supported all image types that are supported
    by [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)

There are some converter-specific flags/options, so that'd be good to check
out internal help with `-h` flag.

## That's all interesting and stuff, but how can I build it?
Good question. With help of [artsi0m](https://github.com/artsi0m), we now have
more-or-less proper makefile, so you just need to run `make` and it should work!
Also because we're using `stb` libraries, you don't need to install any
dependencies except `make` and `gcc` (`tcc` won't work for some reason, but if
you're able to fix it, PR's are welcome!)

## Can I somehow help you?
Of course! If you have any suggestion or improvement, feel free to open
new [issue](https://github.com/hatkidchan/asciify/issues), also if you made
something already and want to accept those changes by us, you can make
[pull request](https://github.com/hatkidchan/asciify/compare) too!

## Why using `stb`?
Maybe because it was the easiest way to handle popular file types without
external dependencies?

## Bruh there's already `libcaca` and others. Why not use them?
<img src="https://i.imgur.com/dB0RFur.png">
