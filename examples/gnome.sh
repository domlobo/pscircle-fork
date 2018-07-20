#!/bin/bash

# This script updates the wallpaper in GNOME based DE.
# Add the line: sh <file-path-of-this-file> &
# in the ~/.profile to start the script after login.
# Placing this in crontab or init.d doesn't work as they doesn't
# have necessary environment variables.

TIME_INTERVAL=30 # Seconds

gsettings set org.gnome.desktop.background picture-uri file:///tmp/output.png

while [ 1 ]; do
    # Replace the next line with any parameters given in the examples.
    pscircle --output=/tmp/output.png
    sleep $TIME_INTERVAL
done
