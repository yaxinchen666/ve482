1.  airqi2.sh
    To run this file, type
	chmod +x airqi2.sh
	./airqi2.sh
    This script uses curl to retrieve information about Shanghai air quality and then uses sed to get and display the aqi and the temperature.

2. activenetwork.sh
    To run this file, type
	chmod +x activenetwork.sh
	./activenetwork.sh
    This script first uses ifconfig to get information about current active network connection. Then pipe the result to awk to get the line containing '/inet /', i.e. containing ip address. Later use awk to print the second word of each line, which is the ip address.