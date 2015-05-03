Steps to build and setup the server:

1) ./build.sh
   this will generate executable of oracleserver
2) ./setup.sh
   this will place the executable in /opt/OracleServer
   script file OracleServer placed at /etc/init.d
3) service OracleServer start
    if -p is not provided default portnumber of 8787 is picked by daemon
    to change port number edit the line in script file to corresponding port number
     /opt/OracleServer/oracleserver -p <port number>
4) python pClient.py -p<port number> -i<ip> -c<count>
    use this command to start the client.
    if parameters are not provided default values are ip(0.0.0.0),count(1),port(8787)
5) service OracleServer stop
    use this command to stop the daemon server
   


Issuues if found:

1)build is done by g++ command. If it is not available then install it by command
sudo apt-get install g++
 
2)if scripts dont run/service OracleServer is not recognized
then please make them executable using chmod command
