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

read -p "Enter Taste für ersten Test..."


echo "Test mit small1 in .."
echo "3 .."
sleep 1
echo "2 .."
sleep 1
echo "1 .."
sleep 1
./demoVRP ../instances/A-n80-k10.vrp small1 4153096453DD



cd #insert path
echo "[BASH] Navigated back to repo"