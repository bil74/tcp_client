# tcp_client<br />
simple app sending tcp messages to a specific address, based on MS tcp client socket example:<br />
https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code<br />
<br />
you need a "config.txt" beside the executable with this content:<br />
address=127.0.0.1<br />
port=20000<br />
delay_millisec=1000<br />
restart_after_end=1 (1-yes, 0-no)<br />
text=message1<br />
text=message2<br />
text=...<br />
<br />
program will send all text messages to the address and port given and waits delay_millisecs between them<br />
if param "restart_after_end" is non-zero, after the last text, program will continue with the first one and so on...<br />
sent and received data will be displayed on screen<br />
any key will interrupt the program (after delay)<br />
