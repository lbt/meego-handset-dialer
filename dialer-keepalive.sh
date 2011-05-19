#!/bin/bash

# Until we have working systemd and are integrated with it
# we are defering to crude hackery like this instead of relying
# on applifed, which is rumored to be going the way of the dodo.

# Call this script with any parameters supported by dialer (or
# any Qt std args), for example:
#
#   dialer-keepalive.sh -prestart -output-level debug

while true
do
	/usr/bin/dialer "$@"
done
