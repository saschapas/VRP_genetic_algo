#!/bin/bash

#/***********************************************************************************************************
#*                                                                                                          *
#*                   Created by saschapas@uni-*** for "Kombinatorische Optimierung" WiSe ***                *
#*                                                                                                          *
#*                           @ Computer Science Department @ University of ***                              *
#*                                                                                                          *
#*                                    Project: Vehicle Routing Problem - GA                                 *
#*                                                                                                          *
#************************************************************************************************************/

# Timer für Auswertung (eigentlich nur für uns)
start=$(date +%s)

# Verzeichnis definieren
REPO_DIR="$HOME <insert path>"
cd "$REPO_DIR"
echo -e "\e[1;35m[BASH] Navigiert nach $REPO_DIR\e[0m"

# Alte Verzeichnisse löschen
rm -rf build/ comparison/
echo -e "\e[1;35m[BASH] Cleanup durchgeführt.\e[0m"

# Random Seed generieren
#SEED=4153096453
#echo -e "\e[1;35m[BASH] Seed: $SEED.\e[0m"
SEED=$(od -An -N4 -tu4 /dev/urandom | tr -d ' ')
echo -e "\e[1;35m[BASH] Random Seed generiert: $SEED.\e[0m"

# Build-Prozess
mkdir build && cd build
cmake ..
cmake --build .
echo -e "\e[1;35m[BASH] Build durchgeführt.\e[0m"

# Random Seed generieren
SEED=$(od -An -N4 -tu4 /dev/urandom | tr -d ' ')
echo -e "\e[1;35m[BASH] Random Seed generiert: $SEED.\e[0m"

echo -e "\e[1;35m[BASH] Enter Taste zum Starten der parallelen Tests...\e[0m"
read -r

# Parameter definieren
INSTANCES=(4)
#INSTANCES=(0 1 2 3 4 5 6 7 8 9)
PRESETS=("small1" "small2" "small3")
TEST_IDS=(1) 
#TEST_IDS=({0..10}) 


for instanz in "${INSTANCES[@]}"; do
    for preset in "${PRESETS[@]}"; do
        for test in "${TEST_IDS[@]}"; do
            echo -e "\e[1;35m[BASH] Starte testerVRP mit Instanz=$instanz, Preset=$preset, Test=$test\e[0m"
            sleep 3
            ./testerVRP "$instanz" "$preset" "$SEED" "$test" 
        done
    done
done

# Zurück zum Root für Python-Skripte
cd "$REPO_DIR"
echo -e "\e[1;35m[BASH] Parallele Tests abgeschlossen. Starte post-processing.\e[0m"

python3 visualize_solutions.py
python3 compare_solutions.py
echo -e "\e[1;35m[BASH] All tasks completed.\e[0m"

end=$(date +%s.%N)
dauerSekunden=$(echo "$end - $start" | bc) # bc für Gleitkomma-Artihmetik
dauerSekundenGanz=$(printf "%.0f" "$dauerSekunden")

minuten=$(echo "$dauerSekundenGanz / 60" | bc)
sekunden=$(echo "$dauerSekundenGanz % 60" | bc)
echo -e "\e[1;35m[BASH] Dauer der Tests: ${minuten} Minuten ${sekunden} Sekunden.\e[0m"

