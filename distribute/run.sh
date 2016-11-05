#!/bin/bash

PROJPATH=$(cd `dirname $0` && pwd)
EXENAME=${PROJPATH}/bin/w-server

function check_bin() {
	if [ ! -x $EXENAME ]
	then
		echo "ERROR: $EXENAME is not existing"
		exit 1
	fi
}

pid=
pidnum=

function get_pid() {
	pid=`ps -ef | grep "${EXENAME}" | grep -v "\<grep\>" | grep -v "\<vim*\>" | awk '{print $2}'`
	pidnum=`echo $pid | wc | awk '{print $2}'`
}

function start_impl() {
	if [ $pidnum -ne 0 ]
	then
		echo "Process is already running, nothing done"
	else
		echo -n "Starting wserver."
		${EXENAME} -daemon -max_log_size 1000 -log_dir $PROJPATH/log

		sleep 1 && echo -n "." && sleep 1 && echo -n "." && get_pid
		if [ $pidnum -ne 0 ]
		then
			echo " ok,pid = $pid"
		else
			echo " failed"
		fi

	fi
}

function stop_impl() {
	if [ $pidnum -eq 0 ]
	then
		echo "Process is not running, nothing done"
	else
		echo -n "Stopping ..."
		kill -9 $pid >/dev/null 2>&1
		while sleep 1
			get_pid
			[ $pidnum -ne 0 ]
		do
			echo -n "."
		done

		echo " done."
	fi
}

function status_impl() {
	if [ $pidnum -eq 0 ]
	then
		echo "$EXENAME is not running (Need start?)"
	else
		echo "$EXENAME is running, pid = $pid"
	fi
}

function process() {
	get_pid
	case "$1" in
		start)
			start_impl
			;;
		stop)
			stop_impl
			;;
		status)
			status_impl
			;;
		*)
			echo "Please use [start | stop | status] as first argument"
			;;
	esac
}

check_bin
process $1
