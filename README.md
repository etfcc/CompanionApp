# CompanionApp
**Disclaimer: This project is only a demo. It cuts corners everywhere that is possible and probably should not be used anywhere in production, although I found it usefull in my particular setup. You may fork it the way you want it. Good hunting!

## Problem:
You want to do some external actions on a certain events generated by FCC Desktop applciaton.<br> 
**Example:**
You want your external light to go ON when you join to a video call and OFF when you are done. 
<br>OR<br>
You want certain script to run after you finished your 9am meeting. 

## Solution:
FCC desktop app fires an AF_UNIX datagram upon a certain event. Currently it only supports CAMERA ON|OFF events. Request more |HERE|.<br>
Event has the following format :
<pre>
[version 1 byte ] [ type 1 byte ] [ payload size  2 bytes ] [ payload ... ] 
</pre>

## Implementation:
This particular demo turns ON/OFF external light https://www.elgato.com/en/gaming/key-light when an FCC application turns its camera ON/OFF. To find the address of the light I used zeroconf project from here https://github.com/yvz/zeroconf

## Deployment:
This is a console applciation for the mac that can be run by launchd and will sit in the background. 
Put the followng file to 
<pre>~/Library/LaunchAgents/com.freeconferencecall.companionapp.plist</pre>

```xml
 <?xml version="1.0" encoding="UTF-8"?>
 <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
 <plist version="1.0">
   <dict>
     <key>Label</key>
     <string>com.freeconferencecall.companionapp</string>
     <key>RunAtLoad</key>
     <true/>
       <key>StartInterval</key>
     <integer>10</integer>
     <key>Program</key>
     <string>PATH/CompanionApp</string>
   </dict>
 </plist>
```
to load the configuration 
<pre>launchctl load ~/Library/LaunchAgents/com.freeconferencecall.companionapp.plist</pre>

 
 
   

