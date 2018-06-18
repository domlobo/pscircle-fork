#!/bin/bash
ps -e -o pid,ppid,pcpu,rss,comm --no-headers | valgrind --leak-check=full pscircle
#feh --bg-fill pscircle.png
