#!/usr/bin/env bash

LD_LIBRARY_PATH="/home/user/.local/share/Steam/steamapps/common/Team Fortress 2/bin:$LD_LIBRARY_PATH" LD_PRELOAD=$PWD/frida-gadget-15.1.17-linux-x86.so steam
