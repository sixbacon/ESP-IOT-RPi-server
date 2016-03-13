#!/usr/bin/env python
# line above needed to run as background service

# takes messages from ESP12 dual electricty meter counter sent via a mosquitto MQTT brocker
# and then adds them to a Mysql database
# records are time stamped with the time this program reads them
# run as a service using info from http://blog.scphillips.com/posts/2013/07/getting-a-python-script
# -to-run-in-the-backround-as-a-service-on-boot/      6/3/15 

import logging                  # for service logging
import logging.handlers         # for service logging                    
import argparse                 # for service logging

import paho.mqtt.client as mqtt
import string
import MySQLdb
import time
import sys

topic1 ="ESP178/PVWh"           # PROJECT SPECIFIC - MQTT topic 1   
topic2 ="ESP178/gridWh"         # PROJECT SPECIFIC - MQTT topic 2   

def savetoPVgriddB(csource,creading):
    # open database connection, mysql will auto closeafter a few hours so open each time just to be sure
    # PROJECT SPECIFIC next line
    db = MySQLdb.connect("localhost","bill","password","PVgriddb" )  # set to appropriate host, user, password, database
    cursor = db.cursor()                     # prepare a cursor object using cursor() method
    now = time.strftime('%Y-%m-%d %H:%M:%S') # get current time in Mysql format
    # Prepare SQL query to INSERT a record into the database.
    sql=("INSERT INTO data (timestamp,source,count) VALUES ('%s','%s','%s')" % (now,csource,creading))
    try:
       # Execute the SQL command
       cursor.execute(sql)
       # Commit your changes in the database
       db.commit()
       print "added record"
    except:
       # Rollback in case there is any error
       db.rollback()
       print " failed record"
    #disconnect from server
    db.close()

def on_connect(client, userdata, rc):       # subscribe to the appropriate topics at start and on reconnection to server
    print(topic1+ " subscribing and code "+ str(rc))
    client.subscribe([(topic1,0),(topic2,0)])

def on_message(client, userdata, msg):      # runs when the message server publishes a messages on any subscriber topic
    subtopic = msg.topic                    # collect the message
    submess = str(msg.payload)              # breakout the data
    submess=submess[0:len(submess)-1]       # lose non printing characy=ter at end
    savetoPVgriddB(subtopic,int(submess))   # create a record and add to the database
    print(subtopic + "  " + submess)        # echo for debug         
                       
# This block for running as a service
# Deafults
LOG_FILENAME = "/tmp/myservice.log"
LOG_LEVEL = logging.INFO  # Could be e.g. "DEBUG" or "WARNING"

# Define and parse command line arguments
parser = argparse.ArgumentParser(description="My simple Python service")
parser.add_argument("-l", "--log", help="file to write log to (default '" + LOG_FILENAME + "')")

# If the log file is specified on the command line then override the default
args = parser.parse_args()
if args.log:
	LOG_FILENAME = args.log

# Configure logging to log to a file, making a new file at midnight and keeping the last 3 day's data
# Give the logger a unique name (good practice)
logger = logging.getLogger(__name__)
# Set the log level to LOG_LEVEL
logger.setLevel(LOG_LEVEL)
# Make a handler that writes to a file, making a new file at midnight and keeping 3 backups
handler = logging.handlers.TimedRotatingFileHandler(LOG_FILENAME, when="midnight", backupCount=3)
# Format each log message like this
formatter = logging.Formatter('%(asctime)s %(levelname)-8s %(message)s')
# Attach the formatter to the handler
handler.setFormatter(formatter)
# Attach the handler to the logger
logger.addHandler(handler)

# Make a class we can use to capture stdout and sterr in the log
class MyLogger(object):
	def __init__(self, logger, level):
		"""Needs a logger and a logger level."""
		self.logger = logger
		self.level = level

	def write(self, message):
		# Only log if there is a message (not just a new line)
		if message.rstrip() != "":
			self.logger.log(self.level, message.rstrip())

# Replace stdout with logging to file at INFO level
sys.stdout = MyLogger(logger, logging.INFO)
# Replace stderr with logging to file at ERROR level
sys.stderr = MyLogger(logger, logging.ERROR)
#end of service block

client = mqtt.Client()                      # set up an MQTT client
client.on_connect = on_connect              # subscribe to topics on server when it responds
client.on_message = on_message              # respond to any any published messages 
client.connect("localhost", 1883, 60)       # make the connection to the server

client.loop_forever()                       # wait for messages from server
