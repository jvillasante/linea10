#! /bin/sh

echo ""
echo "------------------------------------------"
echo "Installing driver udev rule"

AS_EUID=`whoami`
if [ $AS_EUID != "root" ]; then
	echo
	echo "================================================================"
	echo
	echo "Error: this script must have root privilege to do its job."
	echo
	echo "Example usage: sudo $0"
	echo
	exit 1
fi


rm -f /etc/udev/rules.d/88-Lumidigm.rules.save
mv /etc/udev/rules.d/88-Lumidigm.rules /etc/udev/rules.d/88-Lumidigm.rules.save > /dev/null 2>&1


install    --mode 644 --owner root    --group root ./88-Lumidigm.rules /etc/udev/rules.d

echo "Done."
