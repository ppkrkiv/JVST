# JVST - Juicy VST



## Kuvaus

Tämä ohjelma on tehty TIEA306 -kurssia varten.

JVST on JUCE - ohjelmistokehyksen avulla toteutettu VST -syntetisaattori, joka voidaan ajaa sekä itsenäisesti, että audio-ohjelmistoissa.

JVST sisältää neljä eri ääniaaltoa:

- Siniaalto
- Saha-aalto
- Kolmioaalto
- Neliöaalto

Luotua ääntä voidaan muokata valitsimilla. Lisäksi ohjelma sisältää efektinappulan, jota painamalla voidaan valita efekti. Mod -valitsimella voidaan muokata efektin toimintaa. Filtterin avulla voidaan rajoittaa äänen leikkausta.

## Kääntäminen ja ajaminen

* Varmista, että JUCE on asennettuna
* Avaa projekti JUCE:ssa JVST.jucer -tiedostolla
* Käännä tai aja haluamallasi ympäristöllä

Testattu Visual Studiolla (2017). JUCE tukee myös Visual Studio 2019 versioita sekä Xcodea. Lisäksi sisältää myös Makefilen linuxille (ei testattu) Projektin asetuksia voi tarvittaessa muuttaa Projuce:ssa (HUOM! voi rikkoa projektin).


## English

A simple VST plugin made using JUCE. JVST includes wavetable synthesis (sine, saw, triangle and square), envelope (attack, release etc), fx (tremolo, rise) and a simple cutoff filter.

To build and run, make sure to open the jucer -project file with Projucer.

Tested on Visual Studio 2017. Additional settings may be used in Projucer (may cause issues).

## Tekijä
Pyry Kärki (ppkarkiv)

## License
TBA

