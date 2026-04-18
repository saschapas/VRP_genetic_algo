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

cd #insert path
echo "[BASH] Navigated to repo"

rm -r build/
echo "[BASH] Deleted old /build/"

mkdir build/
echo "[BASH] Created /build/"

cd build/
echo "[BASH] I am in /build/ now"

cmake ..
cmake --build .
echo "[BASH] Cmake build executed"

read -p "Enter Taste zum Starten Tests..."


echo "2 Tests auf Instanz 0 mit small1:"
sleep 1

./testerVRP 0 small1 2


echo "2 Tests auf Instanz 1 mit small1:"
sleep 1

./testerVRP 1 small1 2

echo "2 Tests auf Instanz 2 mit small1:"
sleep 1

./testerVRP 2 small1 2


cd # insert path
echo "[BASH] Navigated back to repo"