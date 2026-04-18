#!/bin/bash

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
mkdir build && cd build
cmake ..
cmake --build .
echo -e "\e[1;35m[BASH] Build durchgeführt.\e[0m"

echo -e "\e[1;35m[BASH] Enter Taste zum Starten der parallelen Tests...\e[0m"
read -r

# Parameter definieren
#INSTANCES=(4)
INSTANCES=(0)
#PRESETS=("small1" "small2" "small3")
#PRESETS=("small1" "small2" "small3" "medium1" "medium2" "medium3" "large1" "large2" "large3")
PRESETS=("small2" "small3" "medium2" "medium3" "large2" "large3")
#PRESETS=("small1" "small2" "medium1" "medium2" "large1" "large2")
# Wenn TESTS=(2) bedeutet, dass Test-ID 0 und 1 laufen sollen, 
# nutzen wir {0..1}. Wenn es nur die ID 2 ist, lassen wir es so.
TEST_IDS=(1) 
#SEED=4153096453

# --- PARALLEL AUSFÜHRUNG ---
# --tag: Zeigt an, welcher Job gerade schreibt
# --jobs 80%: Nutzt 80% deiner Kerne (schont das System)
# ::: füllt nacheinander die Platzhalter {1} {2} {3}
parallel --tagstring "Instanz:{1} | Preset:{2} | Test:{3}" --line-buffer --jobs 80% \
    ./testerVRP {1} {2} {3} \
    ::: "${INSTANCES[@]}" \
    ::: "${PRESETS[@]}" \
    ::: "${TEST_IDS[@]}"

#parallel --tagstring "Instanz:{1} | Preset:{2} | Test:{3} | Seed:{4}" --line-buffer --jobs 80% \
 #   ./testerVRP {1} {2} {3} \
  #  ::: "${INSTANCES[@]}" \
   # ::: "${PRESETS[@]}" \
    #::: "${TEST_IDS[@]}" \
    #::: "${SEED[@]}"

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
