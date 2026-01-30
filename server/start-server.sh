#!/bin/bash

app="sensor-server"

echo -n $"Starting $app:"

/usr/bin/$app > /var/log/$app.log 2> /var/log/$app.err < /dev/null &
