#/***********************************************************************************************************
#*                                                                                                          *
#*                   Created by saschapas@uni-*** for "Kombinatorische Optimierung" WiSe ***                *
#*                                                                                                          *
#*                           @ Computer Science Department @ University of ***                              *
#*                                                                                                          *
#*                                    Project: Vehicle Routing Problem - GA                                 *
#*                                                                                                          *
#************************************************************************************************************/


import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

# Best Known Solutions (BKS) Mapping basierend auf Instanzen
BKS_MAP = {
    "A-n32-k5": 784,   "A-n48-k7": 1073,  "A-n61-k9": 1034, 
    "A-n69-k9": 1159,  "A-n80-k10": 1763, "M-n121-k7": 1034, 
    "M-n200-k17": 1373, "P-n101-k4": 681,  "P-n16-k8": 450, 
    "P-n76-k5": 627
}

def plot_version_comparison():
    # 1. Alle CSV-Dateien im Ordner finden
    csv_files = glob.glob("*.csv")
    if not csv_files:
        print("Fehler: Keine .csv Dateien im aktuellen Verzeichnis gefunden!")
        return

    all_dfs = []
    for file in csv_files:
        try:
            df = pd.read_csv(file)
            # Dateiname ohne Endung dient als Legenden-Eintrag (z.B. "Version_A")
            df['Version'] = os.path.splitext(file)[0]
            all_dfs.append(df)
        except Exception as e:
            print(f"Konnte {file} nicht lesen: {e}")

    if not all_dfs:
        return

    # Alle Daten zu einem großen DataFrame mergen
    master_df = pd.concat(all_dfs, ignore_index=True)
    
    # Spaltennamen vereinheitlichen (# zu Instanz)
    if '#' in master_df.columns:
        master_df.rename(columns={'#': 'Instanz_ID', 'Instanz': 'Instanz_Name'}, inplace=True)

    # 2. Gruppieren nach Instanz-Name (ein Plot pro Problemstellung)
    # Namen nehmen, da dieser im BKS-Mapping steht
    instance_names = master_df['Instanz_Name'].unique()

    for name in instance_names:
        inst_data = master_df[master_df['Instanz_Name'] == name]
        
        # Plot-Setup
        plt.figure(figsize=(10, 6))
        
        # Daten für das Balkendiagramm vorbereiten (Pivot: Presets als X, Versionen als Balken)
        pivot_df = inst_data.pivot_table(index='Preset', columns='Version', values='Final')
        
        # Balkendiagramm zeichnen
        ax = pivot_df.plot(kind='bar', ax=plt.gca(), zorder=3)

        # --- Achsen & Beschriftung ---
        plt.title(f"Code-Versionsvergleich: {name}", fontsize=14, fontweight='bold')
        plt.ylabel("Finale Kosten", fontsize=12)
        plt.xlabel("Presets", fontsize=12)
        plt.xticks(rotation=0) # Waagerechte Preset-Namen
        
        # --- Y-Achsen Zoom & BKS ---
        bks = BKS_MAP.get(name, 0)
        current_min = inst_data['Final'].min()
        current_max = inst_data['Final'].max()

        if bks > 0:
            # Rote BKS Linie zeichnen
            plt.axhline(y=bks, color='red', linestyle='--', linewidth=2, label=f'BKS ({bks})', zorder=4)
            
            # Zoom: Unterkante knapp unter BKS oder Minimum, Oberkante mit 10% Puffer
            y_min = min(bks, current_min) * 0.98
            y_max = current_max * 1.05
            plt.ylim(y_min, y_max)
        
        # Legende & Grid
        plt.legend(title="Versionen / BKS", loc='upper right', frameon=True, shadow=True)
        plt.grid(axis='y', linestyle=':', alpha=0.7, zorder=0)
        
        plt.tight_layout()

        # Speichern
        output_fn = f"Vergleich_{name}.png"
        plt.savefig(output_fn, dpi=300)
        print(f"Erfolgreich erstellt: {output_fn}")
        plt.close()

if __name__ == "__main__":
    plot_version_comparison()
    print("\nAlle Vergleiche wurden als PNG gespeichert.")