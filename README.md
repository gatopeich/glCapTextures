# glCapTextures
## OpenGL wrapper or how to run some 3D games on really old 3D chipsets

A library wrapper that downsamples textures as they are dealt to OpenGL system, enabling my old 3D card to bear with some modern Linux games that are unusable otherwise.

Some history: After years of patient waiting, my Savage 3D chipset (an MX/IX that comes with most Thinkpads T2x) is supported in Linux (X11/Xorg/etc.) with direct rendering! (DRI). Just in time, as everything seems to come in 3-D now. Only my card has grown a little bit old (by consumerist standards) -- most software (read "games" here!) come with 'big' textures that just won't fit, while few will even check for errors. So I spent some time studying OpenGL (horrible learning curve!) and found that a simple workaround around "glTexImage2D" could solve the issue. Then coded a wrapper library for that call with 'the ole handy LD_PRELOAD trick'.

### What does it do?

Checks if a texture passed to OpenGL core is greater than "max_texture_size". If so, downsamples it.

### How it works?

you just preload it setting LD_PRELOAD=glWrap.so before running your favourite 3D games.

### Where can you get it?

Here: glWrap.c. Build & run instructions are embedded in the first lines of comments. You may want to comment out the printouts, change "max_texture_size" parameter or whatever. It is just 88 lines of straight-forward code, don't be scared!

### What if I am too lazy to build it?

Let's see what I can do for you... Have glWrap.so, and run your games with the following command line:
```sh
$ LD_PRELOAD=/path_to_wrapper/glWrap.so OpenGL application here
```
Enjoy!
