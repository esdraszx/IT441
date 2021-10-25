import http.server
import time
import socketserver
import cgi
import os
import RPi.GPIO as IO
import json
import urllib.parse as parser

# References

#1. Server code https://www.askpython.com/python-modules/python-httpserver
#       Basic code for a simple python web server set up with a do_GET and do_POST
#
#2. GPIO Mapping https://maker.pro/raspberry-pi/tutorial/raspberry-pi-4-gpio-pinout
#       Layout of pins in the pi 4 and correct pin number for manipulation
#
#3. GPIO CODE https://sourceforge.net/p/raspberry-gpio-python/wiki/BasicUsage
#       How to use RPi with Python and how to manipulate different pins for desired output
#
#4. Bytes to JSON convertion https://stackoverflow.com/questions/40059654/python-convert-a-bytes-array-into-json-format
#       The data from the post request is received in a bytes format. So I needed a way to convert from bytes to JSON. 
#       The resource listed above was used as a reference, but I still had to do some changes for my code to actually work.

hostName = "localhost"
serverPort = 8080
isAuto = 0

IO.setmode(IO.BCM)
IO.setwarnings(False)

red = 23
yellow = 24
green = 25

def actionAutomatic():
    IO.output(red, True)
    time.sleep(3)
    IO.output(red, False)
    IO.output(green, True)
    time.sleep(2)
    IO.output(green, False)
    IO.output(yellow, True)
    time.sleep(1)
    IO.output(yellow, False)

def actionLights(light, value):
        input = int(value)
        
        if light == "auto" and input == 1:
            IO.output([yellow, green], False)
            actionAutomatic()
        elif light == "red":
            IO.output([yellow, green], False)
            IO.output(red, input)
        elif light == "yellow":
            IO.output([red, green], False)
            IO.output(yellow, input)
        elif light == "green":
            IO.output([red, yellow], False)
            IO.output(green, input)
        else:
            IO.output([red, yellow, green], False)

class Server(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.path = "index.html"
        return http.server.SimpleHTTPRequestHandler.do_GET(self)
    def do_POST(self):
        content = int(self.headers.get('Content-Length'))
        post_body = self.rfile.read(content).decode("utf8")
        
        self.path = '/'
        
        jsonBody = json.dumps(parser.parse_qs(post_body))
        jsonBody = json.loads(jsonBody)
        
        actionLights(jsonBody['light'][0], jsonBody['value'][0])
        return http.server.SimpleHTTPRequestHandler.do_GET(self)

IO.setup([23, 24, 25], IO.OUT)

ServerHandler = Server

with socketserver.TCPServer(("", serverPort), ServerHandler) as httpd:
    IO.output([23, 24, 25], False)
    print("Http serving in port " + str(serverPort))
    httpd.serve_forever()

IO.cleanup()
