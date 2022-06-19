#!/usr/bin/env bash

export LD_LIBRARY_PATH="/home/user/.local/share/Steam/steamapps/common/Team Fortress 2/bin:$LD_LIBRARY_PATH"
export LD_PRELOAD=$PWD/frida-gadget-15.1.17-linux-x86.so
steam-run /home/user/.local/share/Steam/steamapps/common/Team\ Fortress\ 2/hl2.sh -game tf -steam
