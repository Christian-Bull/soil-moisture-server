#!/bin/sh

APP="/usr/bin/sensor-server"
NAME="sensor-server"
PIDFILE="/var/run/$NAME.pid"
LOG="/var/log/$NAME.log"
ERR="/var/log/$NAME.err"

start() {
    echo "Starting $NAME..."
    mkdir -p /var/run /var/log

    # already running?
    if [ -f "$PIDFILE" ] && kill -0 "$(cat "$PIDFILE")" 2>/dev/null; then
        echo "$NAME already running"
        return 0
    fi

    "$APP" >"$LOG" 2>"$ERR" </dev/null &
    echo $! >"$PIDFILE"
}

stop() {
    echo "Stopping $NAME..."
    if [ -f "$PIDFILE" ]; then
        kill "$(cat "$PIDFILE")" 2>/dev/null || true
        rm -f "$PIDFILE"
    else
        # fallback if pidfile missing
        killall "$NAME" 2>/dev/null || true
    fi
}

case "$1" in
    start) start ;;
    stop) stop ;;
    restart) stop; start ;;
    *)
        # Boot calls scripts with "start"
        start
        ;;
esac

exit 0
