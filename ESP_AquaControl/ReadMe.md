# ESP AquaControl #

Durch dieses Projekt steuere ich derzeit AquariumLampen über Webaufruf mittels FHEM.

Diese LED-Bars sind an den Pins 12,13,14 über MosFETs angeschlossen.

Aktuell benuze ich dafür dieses Board:
[https://github.com/patrickjahns/esp_rgbww_fhemmodule](https://github.com/patrickjahns/esp_rgbww_fhemmodule "esp_rgbww_fhemmodule")

Editiere <WiFiCredentials.h> für das eigene WLAN.

Über einen Webaufruf kann man steuern was man machen will.

- http://<esp-ip>/?   *JSON-Object Ausgabe*
- .../?status			*andere Statusinfos*
- .../?s=<0-3>			*Arbeitsprogramm ändern*
	- 0 normal Modus
	- 1 FadeUp - wie Sonnenaufgang
	- 2 FadeDown - Sonnenuntergang
	- 3 Aus (alle Lampen aus)
- .../?g=<0...100> Lampe G Intensitivität
- .../?c=<0...100> Lampe C Intensitivität
- .../?w=<0...100> Lampe G Intensitivität
- .../?webupdate Update Funktion übers WEB

Auch kombinierte Anweisungen sind möglich:

- http://<esp-ip>/?g=100&c=110&w=120

Das Modul soll noch um folgende Funktionen erweitert werden:

- MQTT support
- Werte abfragen / melden
	- PH-Sensor
	- Temperatur
	- Wasserleck





