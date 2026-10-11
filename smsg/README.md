**smsg** - send ipc messages to vindle (forked from [this repository](https://codeberg.org/notchoc/dwlmsg))

```
usage:	smsg [-OTLPF]
	smsg [-o <output>] -s [-t <tags>] [-l <layout>] [-c <tags>]
	smsg [-o <output>] (-g | -w) [-FOotlcvmf]
```

```
options:
-g	get
-s	set
-w	watch
-O	get all outputs
-T	get number of tags
-L	get all available layouts
-P	get compositor pid
-F	get focused output
-o	select output
-t	get/set selected tags (set with [+-^.], overwrite with ! prefix)
-l	get/set current layout
-c	get title and appid of focused client
-v	get visibility of statusbar
-m	get fullscreen status
-f	get floating status
```

```
examples:
	# act like dwl stdout
	smsg -w
	# watch focused client appid and title
	smsg -w -c
	# get currently focused output
	smsg -F
	# watch for focused output changes
	smsg -w -F
	# get all available outputs
	smsg -O
	# watch available outputs
	smsg -w -O
	# select tag 1, deselect tag 2, toggle tag 4 on output eDP-1
	smsg -o eDP-1 -s -t +-.^
	# toggle tag 3, overwriting current tagset (yes, zero-indexed)
	smsg -t !2^
	# select tag 8 on current output
	smsg -s -t 7
	# deselect tag 8 on current output
	smsg -s -t 7-
	# switch to first layout (order given by smsg -L)
	smsg -l 0
	# switch to floating layout
	smsg -l '><>'
```

