#!/bin/bash

# retrieve information from the website, parse and print
curl -s https://aqicn.org/city/shanghai/ | sed -n 's/.*"p":"t","v":\[\(.*\),[0-9].*,[0-9].*\],"i":"Shanghai  t.*,"aqi":\(.*\),"time":{"v":.*/AQ: \2 Temp: \1 ℃ /p'

