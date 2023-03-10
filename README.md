# tcp_client
simple app sending tcp messages to a specific address, based on MS tcp client socket example:
https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code

you need a "config.txt" beside the executable with this content:
address=127.0.0.1
port=20000
delay_millisec=1000
text=message1
text=message2

program will send all text messages to the address and port given and waits delay_millisecs between them

