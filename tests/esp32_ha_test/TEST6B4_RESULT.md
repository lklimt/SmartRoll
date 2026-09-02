# TEST 6B.4 – HA MQTT Cover

**Výsledek: PASS**  
**Datum:** 2026-09-02

## Cíl testu

Ověřit finální ovládání rolety v Home Assistant prostřednictvím MQTT Cover s použitím již ověřených příkazů:

- `UP`
- `STOP`
- `DOWN`

Použit byl již ověřený MQTT command topic:

`smartroll/smartroll_01/cover/command`

## Výsledek

Test 6B.4 proběhl úspěšně.

MQTT Cover v Home Assistant funguje spolehlivě a správně ovládá ESP32/RF část systému.

## Stav

Tímto je laboratorní část **6B** uzavřena jako úspěšná.

Ověřené funkce zahrnují:

- ESP32 → Wi-Fi → MQTT → Home Assistant
- HA → MQTT → ESP32
- MQTT Cover UP / STOP / DOWN
- RF ovládání ERTE
- Hall A/B
- směr otáčení
- relativní počítání polohy
- teplotu
- osvětlení

## Co nebylo součástí testu

Test 6B.4 neřešil absolutní kalibraci polohy rolety:

- skutečnou polohu CLOSED / OPEN
- počet impulsů mezi skutečnými koncovými polohami
- převod impulsů na 0–100 %
- automatické nastavení polohy v procentech
- finální kalibraci na skutečné roletě

Tyto funkce budou řešeny až v etapě **6C**, po dokončení mechanické konstrukce a přípravě kompletního finálního zařízení.

## Závěr

**TEST 6B.4 = PASS**

MQTT Cover v Home Assistant je funkční a spolehlivý. Další vývoj se nyní přesouvá z laboratorního ověřování elektroniky a HA na **mechanickou konstrukci SmartRoll**. 

Při mechanickém vývoji se nemění již ověřené funkční části elektroniky a firmware.
