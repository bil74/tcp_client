# tcp_client<br />
simple app sending tcp messages to a specific address, based on MS tcp client socket example:<br />
https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code<br />
<br />
you need a "config.txt" beside the executable with this content:<br />
address=127.0.0.1<br />
port=20000<br />
delay_millisec=1000<br />
text=message1<br />
text=message2<br />
<br />
program will send all text messages to the address and port given and waits delay_millisecs between them<br />
