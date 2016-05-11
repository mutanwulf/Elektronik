# ESP8266 ESP-01 Programmier-Adapter #

Dieser Programmieradapter beinhaltet einen Spannungsteiler im TX/RX Bereich damit die 5V vom TX-TTL nicht den RX-Eingang ams ESP zerstören.  

Das Bild esp8266_prog_wo_board zeigt die Beschaltung:
- An J2 (1x6PIN) kommt der RS232 TTL/USB Adapter
- An J1 (2x4PIN) kommt der ESP8266-01
- Zwischen J4 und J6 kann ein LowDown-V-Regler 5V->3.3V gesteckt werden
- An J7 kann eine externe Spannung eingespeist werden für den LowDown Regler
- J5 (unter R2) kann gebrückt werden, falls der TTL Adapter 3.3V liefert
- J8 dient als Anschlussmöglich von GPIO1,GND,GPIO2
 
Zum programmieren den FLASH-Knopf halten, Reset drücken, Flash nach 2 Sek. wieder loslassen.

Dann sollte man die Firmware aufspielen können.