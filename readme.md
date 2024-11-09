[![Build](https://github.com/kassoulet/quasar-lv2/actions/workflows/meson.yml/badge.svg)](https://github.com/kassoulet/quasar-lv2/actions/workflows/meson.yml)

```

      ___                                  _       ____  
     / _ \ _   _  __ _ ___  __ _ _ __     | |_   _|___ \ 
    | | | | | | |/ _` / __|/ _` | '__|____| \ \ / / __) |
    | |_| | |_| | (_| \__ \ (_| | | |_____| |\ V / / __/ 
     \__\_\\__,_|\__,_|___/\__,_|_|       |_| \_/ |_____|

```

# Quasar - Integer Overflow Distortion 

Quasar is:

- *An experimental digital audio effect that reduces audio resolution through bitcrushing and then pushes the signal into severe distortion, resulting in a chaotic and abrasive sound.*

- **A sonic black hole, collapsing audio into a digital abyss of distortion and noise.**

---

> It is strongly recommended to surround Quasar by limiters. One before to adjust the level more easily, and one after to prevent your monitors to fry 😇.

- Plugin format: LV2



# Building

Meson, LV2 and a C compiler are required to build this project. 
Setup the build directory as follows:

	meson setup build

	# How to use a custom LV2 plugin directory:
	meson setup build -Dlv2dir=$HOME/.lv2/

Compile and install the plugins:

	meson compile -C build
	meson install -C build

# Test

Use the docker file to run [plugin-torture](https://github.com/cth103/plugin-torture)  on Quasar-lv2.

	docker build -t quasar-test . && docker run -it quasar-test