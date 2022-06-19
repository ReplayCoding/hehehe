#!/usr/bin/env bash

export LD_LIBRARY_PATH="/home/user/.local/share/Steam/steamapps/common/Team Fortress 2/bin:$LD_LIBRARY_PATH"
export LD_PRELOAD=$PWD/frida-c/test.so
steam-run /home/user/.local/share/Steam/steamapps/common/Team\ Fortress\ 2/hl2.sh -game tf -steam
