#!/usr/bin/env python

import socket
import sys,getopt

def main(argv):
	TCP_IP = '0.0.0.0'
	TCP_PORT = 8787
	COUNT = 0
	BUFFER_SIZE = 1024
	BYE = "BYE\n"
	MSG = "GETMESSAGE\n"





	try:
		opts,args = getopt.getopt(argv,"i:p:c:")
	except getopt.GetoptError:
		print 'pClient.py -i <ip> -p <port> -c <count>'
	for opt,arg in opts:
		if opt == '-i':
			TCP_IP = arg
		elif opt == '-p':
			TCP_PORT = int(arg)
		elif opt == '-c':
			COUNT = int(arg)



			
	print 'IP : ',TCP_IP,' port: ',TCP_PORT,' count: ',COUNT
	
	log = open("log.txt","a");

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((TCP_IP, TCP_PORT))

	for i in range (0,COUNT):
		s.send(MSG)
		data = s.recv(BUFFER_SIZE)
		print "received data:", data
		log.write(data);
		
	
	s.send(BYE)
	s.close()
	log.close()

if __name__ == "__main__":
   main(sys.argv[1:])