## VRP solver using a Genetic Algorithm

This repo contains C++ Code to solve the Vehicle Routing Problem using a Genetic Algorithm.
It was build as part of the course 'Kombinatorische Optimierung' at my university.

We were a group of four, so I marked the passages I wrote, the rest stays anonymized. The files lack the code  
for the Timer and Logger, as it was written by someone of the course staff and the License is unknwon.  
This means the Code is **NOT** runnable like this without adaption/change.

These libraries were used, too:

- [fmt](https://github.com/fmtlib/fmt/ "fmtlib github")
- [JSON for modern C++](https://github.com/nlohmann/json "GitHub page")
- [Instances fot testing](https://www.coin-or.org/SYMPHONY/branchandcut/VRP/data/index.htm.old#F)
- Several other libraries (see code)

There are some example visualizations included :)

Below you can see the original Readme, written in german, as it was a full german course.

At some point there will be an english version :).

---

# VRP Solver - Genetischer Algorithmus

Dieses Projekt implementiert einen **Genetischen Algorithmus (GA)** zur Lösung des Vehicle Routing Problems (VRP). Ziel ist es, Kundenbesuche so auf Fahrzeuge zu verteilen, dass die Gesamtdistanz minimiert wird, während die Fahrzeugkapazitäten eingehalten werden.

## Installation von GNU Parallel
Um die Rechenlast auf alle CPU-Kerne zu verteilen, wird `GNU Parallel` benötigt:

**Unter Ubuntu/Debian/Linux Mint:**
```bash
sudo apt update && sudo apt install parallel
```

## Starten der Tests
### 0. Builden des Codes
Im Ordner des Repository befindend:
```bash
cmake ..
cmake --build .
```
Damit wird der build/-Ordner und alle executables etc. erstellt (In den Startskripten ebenfalls automatisiert).
### 1. Einzelne Ausführung (Demo)

Für einen manuellen Testlauf mit detaillierten Konsolen-Logs und visueller Ausgabe:


```bash
./build/demoVRP <instanz_pfad> <preset> <instanz_nr> <test_nr> [seed]
```

### 2. Automatisierte Tests (Tester)

Der Tester führt eine definierte Anzahl an Läufen effizient ohne detaillierte Logs durch:
Bash

```bash
./build/testerVRP <instanz_nr> <preset> <seed> <anzahl_tests>
```

### 3. Automatisierung via Bash-Skripte

Im Hauptverzeichnis liegen Skripte zur Steigerung der Effizienz:

### Linearer Durchlauf (```run_tests.sh```): 
Führt Instanzen und Presets nacheinander aus. Gut für die erste Fehlersuche.
### Paralleler Durchlauf (```run_parallel_tests.sh```): 
Nutzt GNU Parallel, um alle verfügbaren CPU-Kerne gleichzeitig zu belasten. Empfohlen für umfangreiche Benchmarks.

## Konfiguration & Alternativen

Im C++ Code (demoVRP.cpp / testerVRP.cpp) können durch Einkommentieren/Auskommentieren (markiert mit //+++++) verschiedene Strategien aktiviert werden:

### Selektion: 
Turnier zwischen 2 Individuen (milder Selektionsdruck) vs. Turnier mit 5% der Population (aggressiv).

### Initialisierung:
Rein zufällige Verteilung vs. lokale Vor-Optimierung mittels 2-Opt.

### Mutation: 
  Klassischer Swap von Knoten vs. 2-Opt Mutation zur gezielten Verbesserung einzelner Routen.

## Visualisierung & Auswertung

Nach Abschluss der Tests generieren die Python-Skripte automatisch Auswertungen im Ordner build/best_solutions:

### Routen-Plots : 
```visualize_solutions.py```: Zeichnet die physischen Wege der Fahrzeuge. Das Depot wird rot, Kunden blau und die Routen farblich getrennt dargestellt.

### Konvergenz-Graphen: 
Visualisiert, wie die Kosten über die Generationen sinken. Ein Pfeil markiert am Ende die finalen Bestkosten.

## Vergleich von Code-Versionen (Benchmarks)

### Um die Auswirkungen von Code-Änderungen (z. B. neue Mutationsrate oder 2-Opt) zu vergleichen:

Sammlen Sie die erzeugten test_summary.csv Dateien aus verschiedenen Läufen.

Speichern Sie diese im Ordner alternatives/ und benennen Sie sie nach der jeweiligen Version (z. B. base_version.csv, improved_2opt.csv).

Fügen Sie das Skript compare_versions.py in diesen Ordner ein und führen Sie es aus.

### Ergebnis:
Ein Balkendiagramm, das alle Versionen nebeneinander stellt und die BKS (Best Known Solution) als Referenzlinie einzeichnet.

## Ordnerstruktur
### /build: 
Enthält Binaries und die generierten Lösungen (best_solutions/).
### /comparison:
Speicherort für CSV-Statistiken der Testläufe.
### /alternatives:
Manueller Ordner für den Vergleich verschiedener Algorithmus-Stände.

### *.py: Skripte für Visualisierung und Post-Processing.
---
*Erstellt im Rahmen des Moduls Kombinatorische Optimierung, Universität *** (WS ****).*