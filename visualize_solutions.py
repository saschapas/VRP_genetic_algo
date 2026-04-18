#/***********************************************************************************************************
#*                                                                                                          *
#*                   Created by saschapas@uni-*** for "Kombinatorische Optimierung" WiSe ***                *
#*                                                                                                          *
#*                           @ Computer Science Department @ University of ***                              *
#*                                                                                                          *
#*                                    Project: Vehicle Routing Problem - GA                                 *
#*                                                                                                          *
#************************************************************************************************************/

import matplotlib.pyplot as plt
import json
import os
import numpy as np
from pathlib import Path

def visualize_solution(json_path, output_png):
    """Visualisiert eine VRP-Lösung aus einer JSON-Datei."""
    try:
        with open(json_path, 'r') as f:
            data = json.load(f)
    except Exception as e:
        print(f"Fehler beim Laden von {json_path}: {e}")
        return

    nodes = data.get('nodes', [])
    routes = data.get('routes', [])
    
    if not nodes or not routes:
        print(f"Warnung: Keine validen Daten in {json_path}")
        return

    plt.figure(figsize=(12, 12))
    
    # 1. Alle Knoten zeichnen
    for node in nodes:
        if node['id'] == 0:
            plt.scatter(node['x'], node['y'], c='red', marker='s', s=200, label='Depot', zorder=5)
        else:
            plt.scatter(node['x'], node['y'], c='blue', marker='o', s=50, alpha=0.6, zorder=3)

    # 2. Routen zeichnen
    colors = plt.get_cmap('tab20').colors
    for i, route in enumerate(routes):
        if not route: continue
        
        # IDs in Koordinaten auflösen
        try:
            route_nodes = [next(n for n in nodes if n['id'] == node_id) for node_id in route]
            
            # WICHTIG: Sicherstellen, dass die Route beim Depot startet und endet
            depot = next(n for n in nodes if n['id'] == 0)
            
            if route[0] != 0:
                route_nodes.insert(0, depot)
            if route[-1] != 0:
                route_nodes.append(depot)

            x_coords = [node['x'] for node in route_nodes]
            y_coords = [node['y'] for node in route_nodes]
            
            plt.plot(x_coords, y_coords, color=colors[i % len(colors)],
                     label=f'Route {i+1}', linewidth=2, alpha=0.8, zorder=4)
        except StopIteration:
            print(f"Warnung: Node ID in Route {i+1} nicht in Knotenliste gefunden.")

    # Formatierung
    cost = data.get("total_cost", "N/A")
    if isinstance(cost, (int, float)):
        plt.title(f'VRP Solution - Instanz: {data.get("instance_name", "Unknown")}\nTotal Cost: {cost:.2f}', fontsize=14)
    else:
        plt.title(f'VRP Solution (Cost: {cost})')

    plt.xlabel('X Coordinate')
    plt.ylabel('Y Coordinate')
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(True, linestyle='--', alpha=0.5)
    
    # Speichern
    plt.tight_layout()
    plt.savefig(output_png, dpi=300, bbox_inches='tight')
    plt.close()

def find_all_json_files(base_dir):
    """Findet rekursiv alle relevanten JSON-Lösungsdateien."""
    # Suche nach beiden möglichen Namenskonventionen
    patterns = ["best_solution.json", "solution.json"]
    found_files = []
    for pattern in patterns:
        found_files.extend(list(Path(base_dir).rglob(pattern)))
    return list(set(found_files)) # Duplikate entfernen

if __name__ == "__main__":
    # Pfad-Logik (Prüfe build-Ordner und Root-Fallback)
    base = Path("build/best_solutions")
    if not base.exists():
        base = Path("best_solutions")

    if not base.exists():
        print(f"Fehler: Verzeichnis {base} existiert nicht.")
    else:
        json_files = find_all_json_files(base)
        print(f"[VISUALIZE] {len(json_files)} Lösungsdateien gefunden in {base}")

        for json_file in json_files:
            output_png = json_file.with_suffix('.png')
            print(f"[VISUALIZE] Plotting: {json_file.name} ...")
            visualize_solution(str(json_file), str(output_png))