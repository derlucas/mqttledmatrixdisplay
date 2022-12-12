# Not used anymore - Nicht mehr in Verwendung.

Diese Firmware wird nicht mehr verwendet, stattdessen ist nun ESPhome im Einsatz.



# Matrix ESP dings

Dieses Projekt besteht aus einem Wemos D1 mini und 8x LED Matrix (8x8 Leds) als Laufschrift-matrix

Es benutzt Homie als ESP8266 Firmware und kann via MQTT angesprochen werden.


## Funktionen
-----------

### Laufschrift

    mosquitto_pub -h MQTT_SERVER -t homie/60019410adc1/scroll/on/set -m "superkalifragisch dingens kirchen"

oder mit scrollspeed:

    mosquitto_pub -h MQTT_SERVER -t homie/60019410adc1/scroll/on/set -m "15|superkalifragisch dingens kirchen"

Die Laufschrift zeigt maximal 200 Zeichen an (alles weitere wird abgeschnitten).

### Text setzen

    mosquitto_pub -h MQTT_SERVER -t homie/60019410adc1/text/on/set -m "bla"

### Bar Graph anzeigen

    mosquitto_pub -h MQTT_SERVER -t homie/60019410adc1/bar/on/set -m "23"

Bar akzeptiert eine Zahl von 0 bis 100 und zeigt einen Bar-Graphen an.
