# Software Repository for Lecture Computerarchitektur

## Getting started

Make sure, that pico-sdk and picotool are cloned and checked out as well:
```
git clone --recurse https://gitlab.hs-esslingen.de/rakeller/computerarchitektur_arm.git
```

In case you forgot to recursively checkout the sub-modules (i.e. pico-sdk), do this afterwards:
```
  git submodule update --init --recursive
```

XXXXX FIX ME:

Compile picotool into the subdirectory external/usr/picotool:
  cd external/picotool
  cmake -BBUILD -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DPICO_SDK_PATH=../../pico-sdk/ -DCMAKE_INSTALL_PREFIX=../usr -DPICOTOOL_FLAT_INSTALL=1 .
  make -C BUILD install ; cd ../..

