#!/bin/sh

### BEGIN INIT INFO
# Provides:          MQTT2mysql
# Required-Start:    $remote_fs $syslog  mysql
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: takes MQTT messages and adds them to a database
# Description:       takes MQTT messages and adds them to a database
### END INIT INFO

# Project specific
DIR=/usr/local/bin/myservice
DAEMON=$DIR/MQTT2mysql.py
DAEMON_NAME=MQTT2mysql

# command line options
DAEMON_OPTS=""

#run as root 
DAEMON_USER=root

# process ID of script stored here, so it can be stoped on request
PIDFILE=/var/run/$DAEMON_NAME.pid

. /lib/lsb/init-functions

do_start () {
    log_daemon_msg "Starting sysem $DAEMON_NAME daemon"
    start-stop-daemon --start --background --pidfile $PIDFILE --make-pidfile --user $DAEMON_USER --chuid $DAEMON_USER --startas  $DAEMON -- $DAEMON_OPTS
    log_end_msg $?
}

do_stop () {
    log_daemon_msg "Stopping system $DAEMON_NAME daemon"
    start-stop-daemon --stop --pidfile $PIDFILE --retry 10
    log_end_msg $?
}

case "$1" in

    start|stop)
        do_${1}
        ;;

    restart|reload|force-reload)
        do_stop
        do_start
        ;;

    status) 
        status_of_proc "$DAEMON_NAME" "$DAEMON" && exit 0 || exit $?
        ;;
    
    *)
        echo "Usage: /ect/init.d/$DEAMON_NAME {start|stop|restart|status}"
        exit 1
        ;;

esac
exit 0
