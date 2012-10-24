#!/bin/sh

export QT_DEBUG_PLUGINS=1
export QT_PLUGIN_PATH=./qtuiotouch/plugins

./qtuiotouch/demo/src/demo -plugin TuioTouch
