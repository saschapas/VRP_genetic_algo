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

# Build-Prozess
mkdir -p build && cd build
cmake ..
cmake --build .
echo -e "\e[1;35m[BASH] Build durchgeführt.\e[0m"

# Random Seed generieren
SEED=$(od -An -N4 -tu4 /dev/urandom | tr -d ' ')
echo -e "\e[1;35m[BASH] Random Seed generiert: $SEED.\e[0m"

#echo -e "\e[1;35m[BASH] Enter Taste zum Starten der parallelen Tests...\e[0m"
#read -r

# Parameter definieren
INSTANCES=(0)
#INSTANCES=(0 1 2 3 4 5 6 7 8 9)
PRESETS=("small1")
#PRESETS=("small1" "small2" "small3")
#PRESETS=("small1" "small2" "small3" "medium1" "medium2" "medium3" "large1" "large2" "large3")

# TEST_IDS=(1) -> einzelner Test 
# TEST_IDS={0..} -> Test 0-9
TEST_IDS=(1)
#TEST_IDS={0..3}
#TEST_IDS={0..9}
#SEED=4153096453

# --- PARALLEL AUSFÜHRUNG ---
# --tag: Zeigt an, welcher Job gerade schreibt
# --jobs 80%: Nutzt 80% deiner Kerne (schont das System)
# ::: füllt nacheinander die Platzhalter {1} {2} {3} [{4}]

#parallel --tagstring "Instanz:{1} | Preset:{2} | Test:{3}" --line-buffer --jobs 80% \
#    ./testerVRP {1} {2} {3} \
#    ::: "${INSTANCES[@]}" \
#    ::: "${PRESETS[@]}" \
#    ::: "${TEST_IDS[@]}"

parallel --tagstring "Instanz:{1} | Preset:{2} | Seed:{3} | Test:{4}" --line-buffer --jobs 80% \
    ./testerVRP {1} {2} {3} {4} \
    ::: "${INSTANCES[@]}" \
    ::: "${PRESETS[@]}" \
    ::: "${SEED}" \
    ::: "${TEST_IDS[@]}" 

# Zurück zum Root für Python-Skripte
cd "$REPO_DIR"
    

# Zurück zum Root für Python-Skripte
cd "$REPO_DIR"
echo -e "\e[1;35m[BASH] Parallele Tests abgeschlossen. Starte post-processing.\e[0m"

# Prüfe vor dem Ausführen, ob der Pfad existiert
if [ -d "build/best_solutions" ]; then
    python3 visualize_solutions.py
    python3 compare_solutions.py
else
    echo -e "\e[1;31m[ERROR] Keine Ergebnisdaten in build/best_solutions gefunden!\e[0m"
fi

end=$(date +%s.%N)
dauerSekunden=$(echo "$end - $start" | bc) # bc für Gleitkomma-Artihmetik
dauerSekundenGanz=$(printf "%.0f" "$dauerSekunden")

minuten=$(echo "$dauerSekundenGanz / 60" | bc)
sekunden=$(echo "$dauerSekundenGanz % 60" | bc)
echo -e "\e[1;35m[BASH] Dauer der Tests: ${minuten} Minuten ${sekunden} Sekunden.\e[0m"
