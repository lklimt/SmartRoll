# SR-HALL-TEST-003 — Nepájivé pole layout

## Použité pole

Test používá nepájivé pole Dratek **400 pinů**, se sloupci **A–J** a 30 řadami. Pole má dvě napájecí větve. urlProdukt Dratek – Nepájivé pole 400 pinůhttps://dratek.cz/arduino-platforma/1226-eses-nepajive-pole-400-pinu.html

Arduino Nano je zasunuté přímo do středové mezery pole tak, aby jeho vývody byly v samostatných kontaktních řadách na obou stranách mezery.

> **Důležité:** přesné číslování řad na konkrétním kusu může být vytištěno jinak. Proto se schéma řídí názvem pinu Arduino a písmeny/sloupci pole, ne pouze číslem řady.

---

## 1. Doporučené rozložení

Použij jednu polovinu pole pro Arduino/Hall signály a horní napájecí lištu pro +5 V a spodní pro GND.

```text
                    NEPÁJIVÉ POLE – POHLED SHORA

        levá kontaktní skupina       pravá kontaktní skupina
             A B C D E       F G H I J
             │ │ │ │ │       │ │ │ │ │
řada 1       ● ● ● ● ●       ● ● ● ● ●
řada 2       ● ● ● ● ●       ● ● ● ● ●
řada 3       ● ● ● ● ●       ● ● ● ● ●
   ...

                 ┌───────────────┐
                 │ STŘEDOVÁ      │
                 │ MEZERA        │
                 └───────────────┘

Arduino Nano je přes středovou mezeru:

       D2/D3  ● ● │ MEZERA │ ● ●  5V/GND
```

### Poznámka k propojení kontaktů

Na běžném nepájivém poli jsou v každé skupině propojené kontakty **A–E** a zvlášť **F–J** v jedné řadě. Levá a pravá skupina nejsou přes středovou mezeru propojené.

---

# 2. Napájení

Arduino je napájeno z USB.

Z Arduino Nano vyveď:

```text
Nano 5V  ── M-M vodič ──> + napájecí lišta pole
Nano GND ── M-M vodič ──> - napájecí lišta pole
```

Pokud jsou na konkrétním poli napájecí lišty rozdělené uprostřed, propoj jejich poloviny krátkým vodičem.

Doporučení:

```text
+ lišta = +5 V
- lišta = GND
```

---

# 3. Hall A

A3144 připoj podle **ověřeného pinoutu konkrétního pouzdra**, které dorazí.

Funkční zapojení je:

```text
A3144 Hall A

VCC ───────────── +5 V lišta
GND ───────────── GND lišta
OUT ───────────── Arduino D2
```

### Pull-up

```text
+5 V lišta
    │
   10 kΩ
    │
    ├──────────── Hall A OUT
    │
    └──────────── Arduino D2
```

### Kondenzátor

100 nF přímo mezi VCC a GND Hall A:

```text
+5 V ─────┬──── A3144 VCC
          │
         100 nF
          │
GND ──────┴──── A3144 GND
```

---

# 4. Hall B

```text
A3144 Hall B

VCC ───────────── +5 V lišta
GND ───────────── GND lišta
OUT ───────────── Arduino D3
```

### Pull-up

```text
+5 V lišta
    │
   10 kΩ
    │
    ├──────────── Hall B OUT
    │
    └──────────── Arduino D3
```

### Kondenzátor

```text
+5 V ─────┬──── A3144 VCC
          │
         100 nF
          │
GND ──────┴──── A3144 GND
```

---

# 5. Jaké vodiče použít

Uživatel má vodiče **M-M** a **F-M**.

## Arduino Nano → pole

Použij **M-M**, pokud vedeš signál přímo z pinu Nano do kontaktní řady nepájivého pole.

```text
Nano D2 ───── M-M ───── Hall A OUT řada
Nano D3 ───── M-M ───── Hall B OUT řada
Nano 5V ───── M-M ───── +5 V lišta
Nano GND ──── M-M ───── GND lišta
```

## Hall modul / senzor

Pokud používáš samostatný A3144 v klasickém pouzdře, vodič se zasouvá do pole stejně jako ostatní součástky.

Pokud by byl senzor na modulu s konektorem, použij podle konektoru **F-M**.

---

# 6. Doporučené fyzické rozložení

Hall senzory dej na jednu část pole, Arduino Nano přes středovou mezeru.

```text
             +5 V LIŠTA
================================================

       10k A                 10k B
        │                     │
        ├── OUT A             ├── OUT B
        │                     │
      [HALL A]              [HALL B]
        │ │ │                 │ │ │
        │ │ └── GND           │ │ └── GND
        │ └──── 5V            │ └──── 5V
        │                     │
        │                     │
        D2                    D3
        │                     │
   ┌─────────── ARDUINO NANO ───────────┐
   │                                     │
   │ USB                                 │
   └─────────────────────────────────────┘

================================================
             GND LIŠTA
```

---

# 7. Kontrola před zapnutím USB

1. Arduino Nano není ještě připojeno k USB.
2. Zkontroluj, že +5 V a GND nejsou propojené zkratem.
3. Zkontroluj oba 10 kΩ rezistory – každý musí být mezi **+5 V a OUT**.
4. Zkontroluj oba 100 nF kondenzátory – každý musí být mezi **VCC a GND**.
5. Hall A OUT musí vést na **D2**.
6. Hall B OUT musí vést na **D3**.
7. Ověř skutečný pinout dodaného A3144 podle označení pouzdra/datasheetu.
8. Teprve potom připoj USB.

---

# 8. Poznámka k pull-up rezistorům

A3144 má open-collector výstup, proto jsou použity dva samostatné pull-up rezistory:

- 10 kΩ: +5 V → Hall A OUT
- 10 kΩ: +5 V → Hall B OUT

Nenahrazovat jedním společným rezistorem.

---

## Stav dokumentu

**REV.A – testovací zapojení**  
**Není výrobní dokumentace.**
