#!/bin/bash
ps -e -o pid,ppid,pcpu,rss,comm --no-headers | pscircle 
