#/***********************************************************************************************************
#*                                                                                                          *
#*                   Created by saschapas@uni-*** for "Kombinatorische Optimierung" WiSe ***                *
#*                                                                                                          *
#*                           @ Computer Science Department @ University of ***                              *
#*                                                                                                          *
#*                                    Project: Vehicle Routing Problem - GA                                 *
#*                                                                                                          *
#************************************************************************************************************/

import json
import os
import pandas as pd
from pathlib import Path
import matplotlib.pyplot as plt
from visualize_solutions import visualize_solution

def plot_preset_comparison(base_dir, output_dir, sol_map=None):
    """Sucht alle Instanzen und plottet {preset}1/2/3 übereinander"""
    base_path = Path(base_dir)
    if not base_path.exists(): return

    for inst_folder in sorted(base_path.glob("Instanz_*")):
        inst_idx = inst_folder.name.replace("Instanz_", "")
        presets = [p for p in inst_folder.iterdir() if p.is_dir()]
        
        groups = {}
        for p in presets:
            base_type = ''.join([i for i in p.name if not i.isdigit()])
            groups.setdefault(base_type, []).append(p)
        
        for base_type, member_paths in groups.items():
            plt.figure(figsize=(10, 6))
            found_data = False
            all_costs = []
            display_name = "Unknown" # Fallback

            for p_path in sorted(member_paths):
                csv_files = list(p_path.rglob("evolution.csv"))
                if csv_files:
                    csv_path = csv_files[0]
                    key = (inst_idx, p_path.name, "0") # Default Test 0
                    meta = sol_map.get(key, {}) if sol_map else {}
                    if meta: display_name = meta.get('inst_name', inst_idx)

                    try:
                        df = pd.read_csv(csv_path)
                        df.columns = df.columns.str.strip()
                        if not df.empty and 'Cost' in df.columns:
                            # Abschneiden bei erreichter Generation
                            if meta and 'generations' in meta:
                                df = df[df['Generation'] <= meta['generations']]

                            line = plt.plot(df['Generation'], df['Cost'], label=f"{p_path.name} (Seed: {meta.get('seed')})", linewidth=2)
                            # Datenpunkt
                            final_val = df['Cost'].iloc[-1]
                            final_gen = df['Generation'].iloc[-1]
                            # Pfeil mit Beschriftung
                            plt.annotate(f'{final_val:.1f}', 
                                        xy=(final_gen, final_val),
                                        xytext=(5, 0), 
                                        textcoords='offset points', 
                                        va='center', 
                                        fontsize=8, 
                                        color=line[0].get_color()) # Nutzt die Farbe der aktuellen Linie
                            all_costs.extend(df['Cost'].tolist())
                            found_data = True
                    except: continue
            
            if found_data:
                y_min, y_max = min(all_costs), max(all_costs)
                plt.ylim(y_min * 0.98, y_max * 1.02)
                # >>> NEU: Instanz-Name im Titel
                plt.title(f"Vergleich {base_type.upper()} | Instanz: {inst_idx}: {display_name}")
                plt.xlabel("Generation")
                plt.ylabel("Kosten")
                plt.legend()
                plt.grid(True, alpha=0.3)
                plt.savefig(os.path.join(output_dir, f"OVERLAY_Instanz_{inst_idx}_{base_type}.png"), bbox_inches='tight')
            plt.close()

def plot_convergence(csv_path, output_png, max_gen=None, inst_name="Unknown", inst_idx="?", seed="N/A"):
    """Erstellt einen Plot des Kostenverlaufs"""
    try:
        df = pd.read_csv(csv_path)
        if df.empty: return
        df.columns = df.columns.str.strip()
        if max_gen: df = df[df['Generation'] <= max_gen]

        plt.figure(figsize=(8,4))
        plt.plot(df['Generation'], df['Cost'], color='#1f77b4', linewidth=2)
        y_min, y_max = df['Cost'].min(), df['Cost'].max()
        plt.ylim(y_min * 0.98, y_max * 1.02)

        # Letzten Datenpunkt bestimmen
        last_gen = df['Generation'].iloc[-1]
        last_cost = df['Cost'].iloc[-1]

        # Pfeil auf Bestwert mit Beschriftung
        plt.annotate(f'{last_cost:.1f}',
                     xy=(last_gen, last_cost),
                     xytext=(last_gen, last_cost + (plt.ylim()[1] - plt.ylim()[0]) * 0.05),
                     arrowprops=dict(facecolor='black', shrink=0.05, width=1, headwidth=5),
                     fontsize=9,
                     fontweight='bold')
        
        # Titel mit Instanz-Name und Preset
        preset_name = Path(csv_path).parts[-3]
        plt.title(f"Konvergenz mit Seed {seed}:\nInstanz {inst_idx} ({inst_name}) - {preset_name}", fontsize=12)
        plt.grid(True, linestyle='--', alpha=0.5)
        plt.tight_layout()
        plt.savefig(output_png, dpi=150)
        plt.close()
    except: pass

def find_all_solutions(base_dir="build/best_solutions"):
    solutions = []
    for json_file in Path(base_dir).rglob("best_solution.json"):
        with open(json_file, 'r') as f:
            data = json.load(f)
            parts = json_file.parts
            data['metadata'] = {
                'seed': data.get('seed', "N/A"),
                'inst_idx': parts[-4].replace("Instanz_", ""),
                'preset': parts[-3],
                'test': parts[-2].replace("Test_", ""),
                'path': str(json_file),
                'inst_name': data.get('instance_name', "N/A"),
                'gen_progress': data.get('generation_progress', 0.0),
                'generations': data.get('generations', 0)
            }
            solutions.append(data)
    return solutions

def generate_comparison(solutions, base_dir="build/best_solutions", output_dir="comparison"):
    os.makedirs(output_dir, exist_ok=True)
    sol_map = {(s['metadata']['inst_idx'], s['metadata']['preset'], s['metadata']['test']): s['metadata'] for s in solutions}

    all_data = []
    for csv_file in Path(base_dir).rglob("evolution.csv"):
        parts = csv_file.parts
        key = (parts[-4].replace("Instanz_", ""), parts[-3], parts[-2].replace("Test_", ""))
        meta = sol_map.get(key, {})

        # Plot erstellen
        plot_name = f"Plot_{key[0]}_{key[1]}_{key[2]}.png"
        plot_convergence(csv_file,
                         os.path.join(output_dir, plot_name), 
                         max_gen=meta.get('generations'),
                         inst_name=meta.get('inst_name'),
                         inst_idx=key[0],
                         seed=meta.get('seed'))

        df_temp = pd.read_csv(csv_file)
        if not df_temp.empty:
            all_data.append({
                'Seed': meta.get('seed'),
                '#': int(key[0]),
                'Instanz': meta.get('inst_name'),
                'Preset': key[1],
                'Test': key[2],
                'Start': round(df_temp['Cost'].iloc[0], 1),
                'Final': round(df_temp['Cost'].iloc[-1], 1),
                'Imp %': round((1-df_temp['Cost'].iloc[-1]/df_temp['Cost'].iloc[0])*100, 2),
                'Gen': meta.get('generations'),
                'Gen %': meta.get('gen_progress')
            })

    if all_data:
        summary_df = pd.DataFrame(all_data).sort_values(by=['#', 'Preset'])
        summary_df.to_csv(os.path.join(output_dir, "test_summary.csv"), index=False)
        save_summary_as_png(summary_df, os.path.join(output_dir, "test_summary_table.png"))
        
        print(f"\n==================ZUSAMMENFASSUNG (Seed: {meta.get('seed')})==================")
        print(summary_df.to_string(index=False, justify='center'))
        print("="*71 + "\n")

        # Im Terminal: Winner Analysis
        print("="*52)
        print(f"==========AUSWERTUNG (Seed: {meta.get('seed')})============")
        print("="*52)
        print(f"{'INSTANZ':<20} | {'BEST PRESET':<15} | {'KOSTEN':<10}")
        print("-" * 52)
        idx_best = summary_df.groupby('#')['Final'].idxmin()
        winners = summary_df.loc[idx_best]
        for _, w in winners.iterrows():
            print(f"{w['Instanz']:<20} | {w['Preset']:<15} | {w['Final']:<10}")
        print("-" * 52)
        global_winner = winners['Preset'].value_counts().idxmax()
        print("="*52)
        print(f"GLOBALER SIEGER (Meiste Bestwerte): {global_winner}")
        print("="*52 + "\n")

    plot_preset_comparison(base_dir, output_dir, sol_map=sol_map)

def save_summary_as_png(df, output_path):
    if df.empty: return
    # Breitere Figur für zusätzliche Spalten
    fig, ax = plt.subplots(figsize=(16, len(df) * 0.5 + 1))
    ax.axis('off')
    table = ax.table(cellText=df.values,
                     colLabels=df.columns,
                     cellLoc='center',
                     loc='center',
                     colColours=['#1f77b4'] * len(df.columns))
    
    for (row, col), cell in table.get_celld().items():
        cell.set_text_props(ha='center')
        if row == 0: cell.set_text_props(weight='bold', color='white')

    table.auto_set_font_size(False)
    table.set_fontsize(8)
    table.scale(1.0, 1.5)
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    sols = find_all_solutions()
    if sols: generate_comparison(sols)
    else: print("Keine Daten gefunden.")