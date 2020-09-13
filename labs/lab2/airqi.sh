#!/bin/bash

# retrieve information from the website, parse and store temperature to temp.txt
curl -s https://aqicn.org/city/shanghai/ | sed -n 's/.*"p":"t","v":\[\(.*\),[0-9].*,[0-9].*\],"i":"Shanghai  t.*/\1/p' > temp.txt

# retrieve information from the website, parse and store aqi to aqi.txt
curl -s https://aqicn.org/city/shanghai/ | sed -n 's/.*,"aqi":\(.*\),"time":{"v":.*/\1/p' > aqi.txt

aqi=$(cat aqi.txt)

temp=$(cat temp.txt)

echo "AQ: aa Temp: tt ℃" | sed -n "s/aa/$aqi/p" | sed -n "s/tt/$temp/p"

