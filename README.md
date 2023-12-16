# DrivingDarwin 🚗
Repozytorium projektu "Driving Darwin".

Projekt pokazuje możliwość genetycznego uczenia sieci neuronowych na przykładzie samochodów poruszających się krętą drogą.<br/>
Dokładniejszy opis działania znajduje się w "DrivingDarwinLatex".

## Uruchomienie
1. Można wejść w folder "Builds/Build1" i odpalić plik "DrivingDarwin.exe"
2. Można znaleść plik .sln i odpalić wszystko przez VS(użyta wersja --)

## Krótki Opis <br/>
Na projekt składają się 3 sceny(Każdej odpowiadają klawisze 1-3):

**Scena 1 (solo)**<br/>
W tej scenie można poruszać się niebieskim samochodem po drodze klawiszami WSAD.<br/>
Klawiszem H można włączyć/wyłączyc linie wykrywające krawędź drogi.

**Scena 2 (Nauka)**\n
W tej scenie możemy uczestniczyć w nauce czerwonych pojazdów jak jeździć.<br/>
Dokładniejszy opis działania w "DrivingDarwinLatex".<br/>
Klawisze:
- R resetuje świat z najlepszą siecia.
- C czyści świat.
- S zapisuje najlepszą sięć do pliku.
- L wczytuje sieć z pliku.
- Arrow Left zmniejsza poziom mutacji nowych generacji aut.
- Arrow Right zwiększa poziom mutacji nowych generacji aut;

**Scena 3 (Wyścig)**<br/>
W tej scenie możemy ścigać się z czerwonym przeciwnikiem posiadającym sieć neuronową wczytaną z pliku.<br/>
Klawisze WSAD- poruszanie się.<br/>
Klawiszem H można włączyć/wyłączyc linie wykrywające krawędź drogi.

## Biblioteki(Użyte w projekcie):
- OpenGl
- GLAD
- GLFW
- STB_IMAGE
- GLM

## Dodatkowe Informacje
- Na projekt dodatkowo składa się system projektowania dróg stworzony przezemnie w unity, w razie potrzeby może być załączony do repozytorium.
- Inspiracja [Sebastian League](https://youtu.be/hfMk-kjRv4c?si=wjRjDR6VatjjREgS)
