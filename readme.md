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

- Plugin format: LV2



# Building

Meson, LV2 and a C compiler are required to build this project. 
Setup the build directory as follows:

```sh
meson setup build

# How to use a custom LV2 plugin directory:
meson setup build -Dlv2dir=$HOME/.lv2/
```

Compile and install the plugins:

```sh
meson compile -C build
meson install -C build
```

# Test

Use the docker file to run [plugin-torture](https://github.com/cth103/plugin-torture) and lv2_validate on Quasar-lv2.

```
$ docker build -t quasar-test . && docker run -it quasar-test
LOG: Turning on denormal detection.
LOG: Running `Quasar Distortion' (/root/.lv2/quasar.lv2/manifest.ttl)
LOG: Inputs:
LOG: 	0 Input Gain => default 1
LOG: 	1 Bits => default 16
LOG: 	2 Overflow => default 1
LOG: 	3 Dry/Wet => default 0.3
LOG: silence
LOG: impulse
LOG: pulse
LOG: sine
LOG: ardour-dc-bias
```
