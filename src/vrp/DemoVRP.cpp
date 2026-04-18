/*****************************************************************************************
 *                                                                                       *
 *          Genetischer Algorithmus zur Lösung des Vehicle Routing Problems              *
 *                                                                                       *
 *                  Erstellt für Kombinatorische Optimierung                             *
 *                          an der Universität ***                                       *
 *                            Wintersemester ***                                         *
 *                                                                                       *
 *                                *** - ***                                              *
 *                                *** - ***                                              *
 *                                *** - ***                                              *
 *                          saschapas - saschapas@proton.me                              *
 *                                                                                       *
******************************************************************************************/

/*****************************************************************************************
 *                                                                                       *
 * Demo wird wie folgt aufgerufen:                                                       *
 *                                                                                       *
 * ./demoVRP <instance-path> <preset-name> <instance-number> <test-number> [custom-seed] *
 *                                                                                       *
******************************************************************************************/

#include <iostream>
#include <fstream>
#include "util/Logger.hpp"
#include "vrp/Instance.hpp"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <util/Timer.hpp>
#include "util/json.hpp"
#include <filesystem>
#include <iomanip>

using namespace std;
using VRPInstance = ko::vrp::Instance;
using VRPNode = ko::vrp::Instance::Node;
using json = nlohmann::json;

int generations_{0};
double generationProgress_{0.0};
unsigned int currentSeed_{0};

bool full2opt_{false};
bool mutate2opt_{false};
bool init2opt_{false};
bool turn2_{false};

/*****************************************************************************************
 *                              Alternativen zur Ausführung                              *
 *                                                                                       *
 * (Jeweilige Stellen sind mit //+++++ markiert)                                         *
 *                                                                                       *
 *   - selectParent() mit Turnier zwischen 2 Individuen oder 5% der Population           *
 *   - Sortieren der Initialpopulation mittels 2Opt-Algorithmus                          *
 *   - mutate() zur lokalen Optimierung mittels 2Opt-Algorithmus                         *
 *                                                                                       *
******************************************************************************************/

// Erstellen von vorgefertigten Konfigurationen
enum class PresetType 
{
    Small1, Small2, Small3,
    Medium1, Medium2, Medium3,
    Large1, Large2, Large3
};

// Definieren der Struktur einer Lösung
struct Solution
{
    vector<vector<VRPInstance::NodeId>> routes;
    double cost;
};

// Definieren der Struktur einer Konfiguration
struct Preset
{
    int populationSize;
    int generations;
    double mutationRate;
};

// Verschiedene Presets zum Evaluieren der am besten geeigneten Presets
/*const std::unordered_map<PresetType, Preset> PRESETS = 
{
    {PresetType::Small1,  {50,  100000, 0.10}},
    {PresetType::Small2,  {50,  100000, 0.25}},
    {PresetType::Small3,  {50,  100000, 0.35}},
    //{PresetType::Small3,  {50,  100000, 0.4}},
    {PresetType::Medium1, {200, 100000, 0.10}},
    {PresetType::Medium2, {200, 100000, 0.25}},
    {PresetType::Medium3, {200, 100000, 0.35}},
    {PresetType::Large1,  {500, 100000, 0.10}},
    {PresetType::Large2,  {500, 100000, 0.25}},
    {PresetType::Large3,  {500, 100000, 0.35}}
};

const std::unordered_map<PresetType, Preset> PRESETS = 
{
    {PresetType::Small1,  {50,  100000, 0.10}},
    {PresetType::Small2,  {50,  100000, 0.25}},
    {PresetType::Small3,  {50,  100000, 0.35}},
    {PresetType::Medium1, {50, 1000000, 0.10}},
    {PresetType::Medium2, {50, 1000000, 0.25}},
    {PresetType::Medium3, {50, 1000000, 0.35}},
    {PresetType::Large1,  {50, 10000000, 0.10}},
    {PresetType::Large2,  {50, 10000000, 0.25}},
    {PresetType::Large3,  {50, 10000000, 0.35}}
};

const std::unordered_map<PresetType, Preset> PRESETS = 
{
    {PresetType::Small1,  {10,  100000, 0.10}},
    {PresetType::Small2,  {10,  100000, 0.25}},
    {PresetType::Small3,  {10,  100000, 0.35}},
    {PresetType::Medium1, {10, 1000000, 0.10}},
    {PresetType::Medium2, {10, 1000000, 0.25}},
    {PresetType::Medium3, {10, 1000000, 0.35}},
    {PresetType::Large1,  {10, 10000000, 0.10}},
    {PresetType::Large2,  {10, 10000000, 0.25}},
    {PresetType::Large3,  {10, 10000000, 0.35}}
};

const std::unordered_map<PresetType, Preset> PRESETS = 
{
    {PresetType::Small1,  {5,  100000, 0.10}},
    {PresetType::Small2,  {5,  100000, 0.25}},
    {PresetType::Small3,  {5,  100000, 0.35}},
    {PresetType::Medium1, {5, 1000000, 0.10}},
    {PresetType::Medium2, {5, 1000000, 0.25}},
    {PresetType::Medium3, {5, 1000000, 0.35}},
    {PresetType::Large1,  {5, 10000000, 0.10}},
    {PresetType::Large2,  {5, 10000000, 0.25}},
    {PresetType::Large3,  {5, 10000000, 0.35}}
};
const std::unordered_map<PresetType, Preset> PRESETS = 
{
    {PresetType::Small1,  {25,  100000, 0.10}},
    {PresetType::Small2,  {25,  100000, 0.25}},
    {PresetType::Small3,  {25,  100000, 0.35}},
    {PresetType::Medium1, {25, 1000000, 0.10}},
    {PresetType::Medium2, {25, 1000000, 0.25}},
    {PresetType::Medium3, {25, 1000000, 0.35}},
    {PresetType::Large1,  {25, 10000000, 0.10}},
    {PresetType::Large2,  {25, 10000000, 0.25}},
    {PresetType::Large3,  {25, 10000000, 0.35}}
};

const std::unordered_map<PresetType, Preset> PRESETS = 
{
    {PresetType::Small1,  {100,  100000, 0.10}},
    {PresetType::Small2,  {100,  100000, 0.25}},
    {PresetType::Small3,  {100,  100000, 0.35}},
    {PresetType::Medium1, {100, 1000000, 0.10}},
    {PresetType::Medium2, {100, 1000000, 0.25}},
    {PresetType::Medium3, {100, 1000000, 0.35}},
    {PresetType::Large1,  {100, 10000000, 0.10}},
    {PresetType::Large2,  {100, 10000000, 0.25}},
    {PresetType::Large3,  {100, 10000000, 0.35}}
};
const std::unordered_map<PresetType, Preset> PRESETS = 
{// Test mit instanz 0 ein Test
    {PresetType::Small1,  {5,  100000, 0.20}},
    {PresetType::Small2,  {5,  100000, 0.25}},// <- Beste 1., 3., 4. Test
    {PresetType::Small3,  {5,  100000, 0.35}},// <- Beste 2. Test
    {PresetType::Medium1, {10, 1000000, 0.35}},
    {PresetType::Medium2, {10, 100000, 0.25}},// <- Beste 2. Test
    {PresetType::Medium3, {10, 100000, 0.35}},// <- Beste 4. Test
    {PresetType::Large1,  {25, 100000, 0.20}},
    {PresetType::Large2,  {25, 100000, 0.25}},// <- Beste 3. Test
    {PresetType::Large3,  {25, 100000, 0.35}}// <- Beste 1. Test
};*/

const std::unordered_map<PresetType, Preset> PRESETS = 
{
    {PresetType::Small1,  {5,  2000000, 0.25}},
    {PresetType::Small2,  {10,  2000000, 0.25}},
    {PresetType::Small3,  {15,  2000000, 0.25}},
    {PresetType::Medium1, {10, 1000000, 0.35}},
    {PresetType::Medium2, {10, 100000, 0.25}},
    {PresetType::Medium3, {10, 100000, 0.35}},
    {PresetType::Large1,  {25, 100000, 0.20}},
    {PresetType::Large2,  {25, 100000, 0.25}},
    {PresetType::Large3,  {25, 100000, 0.35}}
};

// Zum Speichern/Auswerten einer Generation
struct ProgressEntry
{
    int generation;
    double cost;
};

// Hilfsfunktion für String-Konvertierung der Konfigurationen
PresetType parsePresetType(const std::string& name)
{
    static const std::unordered_map<std::string, PresetType> stringToType =
    {
        {"small1", PresetType::Small1},
        {"small2", PresetType::Small2},
        {"small3", PresetType::Small3},
        {"medium1", PresetType::Medium1},
        {"medium2", PresetType::Medium2},
        {"medium3", PresetType::Medium3},
        {"large1", PresetType::Large1},
        {"large2", PresetType::Large2},
        {"large3", PresetType::Large3}
    };
        
    auto it = stringToType.find(name);
    if (it != stringToType.end())
    {
        return it->second;
    }
    return PresetType::Small1; // Default
}

//Memberariablen, um nicht pro Lösung neu generieren zu müssen
std::vector<int> demands_{0};
std::vector<std::vector<uint32_t>> travelCostLUT_;
std::unordered_set<uint64_t> datenbank;

uint32_t numberNodes_{0};
int numberVehicles_{0};
int vehicleCapacity_{0};
std::string instancePath_ = "default"; // Für den instancename der json
int elitesize_{0};
std::string presetForFolder_ = "default"; // Für den filepath der json
int instanceIdx_{0}; // same
int numTest_{0};

/*                         Hilfsfunktion die eine einzelne Route lokal optimiert                           */
/*                                        written by saschapas                                             */
void apply2opt(std::vector<VRPInstance::NodeId>& route)
{
    // Eine Route mit nur 1-2 Kunden kann man nicht optimieren
    if (route.size() < 4) return;

    bool improved = true;
    while (improved)
    {
        improved = false;
        // Loop über alle Kanten (i-1, i) und (j, j+1)
        // Depot am Anfang (0), Ende (Index size-1) bleiben fest
        for (size_t i = 1; i < route.size() -2; ++i)
        {
            for (size_t j = i+1; j < route.size() -1; ++j)
            {
                VRPInstance::NodeId A = route[i-1];
                VRPInstance::NodeId B = route[i];
                VRPInstance::NodeId C = route[j];
                VRPInstance::NodeId D = route[j+1];

                // Aktuelle Distanz der zwei Kanten: (A-B) + (C-D)
                double currentDist =    travelCostLUT_[A][B] + 
                                        travelCostLUT_[C][D];
                
                // Neue Distanz nach Umdrehen: (A-C) + (B-D)
                double newDist =    travelCostLUT_[A][C] + 
                                        travelCostLUT_[B][D];

                if (newDist < currentDist - 0.0001) // Puffer
                {
                    // Magischer Teil: Kante umdrehen
                    std::reverse(route.begin() + i, route.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}

// Hilfsfunktion: Generiert EINE zufällige, aber valide Startlösung
vector<vector<VRPInstance::NodeId>> generateRandomInitialSolution(const ko::vrp::Instance& instance, std::mt19937& rng)
{
    auto log = ko::Logger("DemoVRP");

    // 1. Initialisiere Routen mit Depot (NodeId 0)
    vector<vector<VRPInstance::NodeId>> routes(numberVehicles_, vector<VRPInstance::NodeId>{0});
    vector<int> vehicleLoads(numberVehicles_, 0);
    const auto& allNodes = instance.getAllNodes();
    vector<VRPInstance::NodeId> unassignedNodes;
    // da Liste feste Größe, ist es effizienter wenn wir genau genug Platz reservieren, anstatt 2^n
    unassignedNodes.reserve(allNodes.size() - 1); 
    
    for (size_t i = 1; i < allNodes.size(); ++i)
    {// Annahme: Depot ist immer nodeID(0), daher schneller wenn wir einfach alles kopieren, außer erstes Element
        unassignedNodes.push_back(allNodes[i].m_nodeId);
    }

    // 3. Magie für den GA: Kunden/Knoten zufällig mischen!
    // So entsteht bei jedem Funktionsaufruf eine komplett andere Startlösung.
    std::shuffle(unassignedNodes.begin(), unassignedNodes.end(), rng);

    // 4. Kunden semi-zufällig auf Fahrzeuge verteilen
    for (auto nodeId : unassignedNodes)
    {
        int demand = demands_[nodeId];
        bool assigned = false;

        //zufällige Verteilung auf die Fahrzeuge
        int offset = rng() % numberVehicles_;
        for (int i = 0; i <= numberVehicles_; i++)
        { // First Fit startet für jeden Knoten immer beim ersten Fahrzeug --> ungleichmäßige verteilung
            int random = (offset + i) % numberVehicles_;
            if (vehicleLoads[random] + demand <= vehicleCapacity_)
            {
                routes[random].push_back(nodeId);
                vehicleLoads[random] += demand;
                assigned = true;
                break;
            }
        }
        if (!assigned)
        {// Fehlerhafter Zustand, dürfte nicht passieren
            routes[0].insert(routes[0].end() - 1, nodeId);
            ////log.INFO("[INIT] [WARNING]: Node{} passt in kein Fahrzeug mehr!", nodeId);
        }
    }

    // 5. Schließe alle Routen mit dem Depot ab
    for(auto& route : routes)
    {
        // Wenn die Route nur das Depot enthällt (size == 1) 
        // ODER der letzte Knoten nicht das Depot ist: Depot anfügen
        if (route.size() == 1 || route.back() != 0)
        {
            route.push_back(0);
        }
    }
    // init2opt_ = false;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Optional: 2opt direkt einmal lokal optimieren bei Initialpopulation
    init2opt_ = true;
    for (auto& route : routes)
    {
        apply2opt(route);
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    return routes;
}

// Die Kosten einer Route/Lösung berechnen
float calculateRouteCost(Solution& solution)
{
    float totalDistance = 0.0;
    float totalPenalty = 0.0;
    const float penaltyFactor = 1000.0; // Hohe Strafe pro überladener Einheit
    vector<int> routeLoads;

    routeLoads.assign(numberVehicles_, 0);

    for (int v = 0; v < numberVehicles_; ++v)
    {
        const auto& route = solution.routes[v];
        if (route.empty()) continue;

        for (size_t i = 0; i < route.size() - 1; ++i)
        {
            totalDistance += travelCostLUT_[route[i]][route[i + 1]];
            if (route[i] != 0)
            {
                routeLoads[v] += demands_[route[i]];
            }
        }

        // Wenn das Fahrzeug überladen ist, berechne die Strafe
        if (routeLoads[v] > vehicleCapacity_)
        {
            totalPenalty += (routeLoads[v] - vehicleCapacity_) * penaltyFactor;
        }
    }
    // Formel: $Cost = \text{Distanz} + \text{Strafe}$
    return totalDistance + totalPenalty;
}

double calculateAverageCost(const vector<Solution>& population)
{
    double totalCost = 0.0;
    for (const auto& sol : population)
    {
        totalCost += sol.cost;
    }
    return totalCost / population.size();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*                                       Selection möglichkeiten                                           */
/*                                        written by saschapas                                             */
// Mildes Turnier mit 2 Individuen
/*Solution selectParent(const vector<Solution>& population, std::mt19937& rng)
{
    turn2_ = true;
    std::uniform_int_distribution<size_t> dist(0, population.size() - 1);
    
    // Zwei zufällige Individuen auswählen (verringert frühzeitige Konvergenz)
    size_t idx1 = dist(rng);
    size_t idx2 = dist(rng);

    if (population[idx1].cost < population[idx2].cost)
    {
        return population[idx1];
    }
    return population[idx2];
}*/

/*// Alternatives Turnier mit 5% der Population
Solution selectParent(const vector<Solution>& population, std::mt19937& rng)
{
    turn2_ = false;
    // auto log = ko::Logger("DemoVRP");
    // log.INFO("[ALTERNATIVE] Turnier mit 5% der Population.");
    turn2_ = false;
    // Die Tournament Size (k) bestimmt den Selektionsdruck.
    // k=2 ist sehr mild, k=10 ist sehr aggressiv.
    // Ein guter dynamischer Wert: 5% der Population, aber mindestens 2 und maximal 10.
    int tournamentSize = std::clamp(static_cast<int>(population.size() * 0.05), 2, 10);
    std::uniform_int_distribution<size_t> dist(0, population.size() - 1);
    
    // Ersten Kandidaten als aktuellen Champion wählen
    size_t bestIdx = dist(rng);

    // k-1 weitere Kandidaten gegen den Champion antreten lassen
    for (int i = 1; i < tournamentSize; ++i)
    {
        size_t nextIdx = dist(rng);
        if (population[nextIdx].cost < population[bestIdx].cost)
        {
            bestIdx = nextIdx;
        }
    }
    return population[bestIdx];
}*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


Solution crossover(const Solution& parent1, const Solution& parent2, std::mt19937& rng)
{
    Solution child;
    child.routes.resize(numberVehicles_, {0}); // Alle Routen starten mit Depot
    
    vector<int> allNodesSequence;
    vector<bool> visited(numberNodes_, false);
    visited[0] = true; // Depot ignorieren

    // 1. Zufälligen Bereich aus Parent 1 wählen (als flache Liste)
    int routeIdx = rng() % numberVehicles_;
    for (auto nodeId : parent1.routes[routeIdx])
    {
        if (nodeId != 0) {
            allNodesSequence.push_back(nodeId);
            visited[nodeId] = true;
        }
    }

    // 2. Alle anderen Knoten von Parent 2 in deren Reihenfolge hinzufügen
    for (const auto& route : parent2.routes)
    {
        for (auto nodeId : route) {
            if (nodeId != 0 && !visited[nodeId])
            {
                allNodesSequence.push_back(nodeId);
                visited[nodeId] = true;
            }
        }
    }

    // 3. Die flache Liste (Giant Tour) auf Fahrzeuge verteilen (Greedy)
    int currentVehicle = 0;
    int currentLoad = 0;

    for (int nodeId : allNodesSequence)
    {
        int d = demands_[nodeId];
        // Wenn es passt, ins aktuelle Fahrzeug, sonst ins nächste
        if (currentVehicle < numberVehicles_ - 1 && currentLoad + d > vehicleCapacity_)
        {
            child.routes[currentVehicle].push_back(0); // Route abschließen
            currentVehicle++;
            currentLoad = 0;
        }
        child.routes[currentVehicle].push_back(nodeId);
        currentLoad += d;
    }
    
    // Alle Routen mit Depot schließen
    for (auto& r : child.routes)
    {
        if (r.back() != 0) r.push_back(0);
    }
    return child;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*                                        written by saschapas                                              */
/*                                          Mutate ohne 2opt                                                */

/*void mutate(Solution& mutated, std::mt19937& rng, double mutationRate)
{
    // auto log = ko::Logger("DemoVRP");
    // log.INFO("[ALTERNATIVE] Mutation ohne 2opt.");
    mutate2opt_ = false;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    if (dist(rng) > mutationRate) return;

    // Einfacher Swap: Zwei zufällige Kunden-Positionen tauschen
    int r1 = rng() % numberVehicles_;
    int r2 = rng() % numberVehicles_;

    if (mutated.routes[r1].size() > 2 && mutated.routes[r2].size() > 2)
    {
        int pos1 = 1 + (rng() % (mutated.routes[r1].size() - 2));
        int pos2 = 1 + (rng() % (mutated.routes[r2].size() - 2));
        std::swap(mutated.routes[r1][pos1], mutated.routes[r2][pos2]);
    }
}*/

/*                                           Mutate mit 2opt                                                */
void mutate(Solution& mutated, std::mt19937& rng, double mutationRate)
{
    mutate2opt_ = true;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    if (dist(rng) > mutationRate) return;

    // Zwei zufällige Routen wählen
    int r1 = rng() % numberVehicles_;
    int r2 = rng() % numberVehicles_;

    // Zwei Kunden zwischen oder innerhalb oder zwischen Routen tauschen
    if (mutated.routes[r1].size() > 2 && mutated.routes[r2].size() > 2)
    {
        int pos1 = 1 + (rng() % (mutated.routes[r1].size() - 2));
        int pos2 = 1 + (rng() % (mutated.routes[r2].size() - 2));

        std::swap(mutated.routes[r1][pos1], mutated.routes[r2][pos2]);
        
        // Veränderte Routen lokal optimieren
        apply2opt(mutated.routes[r1]);
        if (r1 != r2) {apply2opt(mutated.routes[r2]);}
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

vector<Solution> generateNewPopulation(vector<Solution>& currentPopulation, 
            std::mt19937& rng, const ko::Timer& timer, double mutationRate = 0.1)
{
    if (!timer.notTimeLimitReached()) return {}; // Signal: Zeitlimit
    uint32_t current_size = currentPopulation.size();
    vector<Solution> newPopulation;
    newPopulation.reserve(current_size);
    auto log = ko::Logger("DemoVRP");

    // --- ELITISM ---
    // Finden der besten lösung der aktuellen Population
    std::nth_element(currentPopulation.begin(), currentPopulation.begin() + elitesize_, currentPopulation.end(), 
    [](const Solution& a, const Solution& b)
    {
        return a.cost < b.cost;
    });
    // Kopiere die k besten Individuen
    for (int i = 0; i < elitesize_; ++i)
    {
        newPopulation.push_back(currentPopulation[i]);
    }

    // Den Rest der Population füllen(Starten bei i = 1)
    for (size_t i = 1; i < currentPopulation.size(); i++)
    {
        const Solution& parent1 = selectParent(currentPopulation, rng);
        const Solution& parent2 = selectParent(currentPopulation, rng);

        Solution child;
        if (parent1.cost < parent2.cost)
        {
            child = crossover(parent1, parent2, rng);
        } else {
            child = crossover(parent2, parent1, rng);
        }

        mutate(child, rng, mutationRate);

        child.cost = calculateRouteCost(child);
        newPopulation.push_back(child);
    }
    
    // Prüfen Zeitlimit am Ende
    if (!timer.notTimeLimitReached()) {
        log.INFO("[NEW POP] Zeitlimit in generateNewPopulation erreicht!");
        return newPopulation;
    }
    return newPopulation;
}

// json Datei für Auswertung erstellen und exportieren
void exportAsJson(
    const Solution& solution,
    const ko::vrp::Instance& instance,
    const std::string& filename,
    const std::string& instanceFilename,
    const int generations,
    const double generationProgress)
{
    // 1. Pfad-Objekt erstellen
    std::filesystem::path fullPath(filename);
    std::filesystem::path dirPath = fullPath.parent_path();

    // 2. Ordner erstellen
    if (!dirPath.empty() && !std::filesystem::exists(dirPath))
    {
        std::filesystem::create_directories(dirPath); // Ordner erstelleb, falls noch nicht vorhanden
    }
    
    // 3. json Instanzen erstellen
    json j;

    j["instance"] = instanceFilename;
    j["instance_name"] = std::filesystem::path(instancePath_).stem().string();
    j["total_cost"] = solution.cost;
    j["seed"] = currentSeed_;    
    j["routes"] = solution.routes;
    j["generations"] = generations;
    j["generation_progress"] = generationProgress;

    // 4. Knotendaten (Koordinaten) speichern, damit Python sie zeichnen kann
    json nodes = json::array();
    for (const auto& node : instance.getAllNodes())
    {
        nodes.push_back({
            {"id", node.m_nodeId},
            {"x", node.m_x},
            {"y", node.m_y}
        });
    }
    j["nodes"] = nodes;

    // 4. Datei schreiben (mit Einrückung von 4 Leerzeichen für Lesbarkeit)
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << j.dump(4);
    }
}

int main(int argc, char** argv)
{
    // Vector für die history der Generationen
    std::vector<ProgressEntry> history;

    // Timer Start 2 Minuten als Timelimit
    ko::Timer timer(120.0);

    Solution globalBestSolution;
    double globalBestCost = std::numeric_limits<double>::max();

    // Create utility objects
    auto log = ko::Logger("DemoVRP");


    // command line args checken
    if (argc < 5 || argc > 6)
    {
        log.ERROR("[MAIN][ERROR] usage: ./demoVRP <instance-path> <preset-name> <instance-number> <test-number> [custom-seed] ");
        return EXIT_FAILURE;
    }

    // command line args holen
    const auto filename = std::string(argv[1]);
    std::string presetName(argv[2]);
    PresetType type = parsePresetType(presetName);
    const Preset& setting = PRESETS.at(type);

    // Instanzname, Instanznummer, Testanzahl und Preset holen
    instancePath_ = argv[1];
    presetForFolder_ = argv[2];
    instanceIdx_ = std::stoi(argv[3]);
    numTest_ = std::stoi(argv[4]);


    // Instanz einlesen und Werte abspeichern
    const auto instance = ko::vrp::Instance(filename);
    numberNodes_ = instance.getNumberNodes();
    numberVehicles_ = instance.getNumberVehicles();
    vehicleCapacity_ = instance.getVehicleCapacity();
    log.INFO("[MAIN] VRP Instanz erhalten: n={} Knoten, m={} Fahrzeuge und Fahrzeugkapazität c={}.", numberNodes_, numberVehicles_, vehicleCapacity_);

    //creates and prints the travel cost matrix
    //log.INFO("The travel cost matrix looks as follows:");
    travelCostLUT_.resize(numberNodes_);
    for (auto nodeFirstId = ko::vrp::Instance::NodeId(0); nodeFirstId < numberNodes_; nodeFirstId++) {
        travelCostLUT_[nodeFirstId].resize(numberNodes_);
        for (auto nodeSecondId = ko::vrp::Instance::NodeId(0); nodeSecondId < numberNodes_; nodeSecondId++) {
            travelCostLUT_[nodeFirstId][nodeSecondId] = instance.getTravelCost(nodeFirstId, nodeSecondId);
        }
    }

    const int populationSize = setting.populationSize;
    const int generations = setting.generations;
    const double mutationRate = setting.mutationRate;
    const int samplingRate = 100; // 1 = nur einmal am Ende, 10 = 10% der Gens, 100 = 1% der Gens

    currentSeed_ = static_cast<unsigned int>(std::stol(argv[5]));
    log.INFO("[MAIN] Seed erhalten: {}", currentSeed_);
    

    if (currentSeed_ == 0)
    {
        std::random_device rd;
        currentSeed_ = rd();
        log.INFO("[MAIN] Random Seed generiert: {}", currentSeed_);
    }

    std::mt19937 rng(currentSeed_);
    demands_.resize(numberNodes_, 0);
    for (const auto& node : instance.getAllNodes())
    {
        demands_[node.m_nodeId] = node.m_demand;
    }

    vector<Solution> population;
    population.reserve(populationSize);
    elitesize_ = std::clamp(populationSize / 10, 1, populationSize);

    // Zufällige Lösung generieren
    for (int i = 0; i < populationSize; ++i)
    {
        Solution sol;
        sol.routes = generateRandomInitialSolution(instance, rng);
        sol.cost = calculateRouteCost(sol); 
        population.push_back(sol);
    }

    log.INFO("[MAIN] Erfolgreich Population mit {} Individuen generiert.", population.size());
    log.INFO("[MAIN] Durchschn. Kosten der Initialpopulation: {:.1f}", calculateAverageCost(population));
    log.INFO("[MAIN] Starten der GA Iterationen...");

    //printBestInPopulation(population, instance, numTest_);   

    //Flag zum Abbrechen, fals Timelimit erreicht
    bool timeHit = false;

    // Wie viele Generationen pro "Block" (Ausgabe-Intervall)
    // Beispiel: 1000 Generationen / 100 SamplingRate = 10 Generationen pro Block
    int batchSize = generations / samplingRate; 
    if (batchSize < 1) batchSize = 1; // Sicherstellen, dass wir mind. 1 Gen machen

log.INFO("[MAIN] {} Generationen in {} Blöcken zu je {} Gen.\n", generations, samplingRate, batchSize);

    for (int j = 0; j < samplingRate && timer.notTimeLimitReached() && !timeHit; j++)
    {
        // Innere Schleife berechnet einen Block
        for (int i = 0; i < batchSize && timer.notTimeLimitReached(); i++)
        {
            auto newPop = generateNewPopulation(population, rng, timer, mutationRate);
            if(newPop.empty())
            {
                timeHit = true; 
                break;
            }
            population = std::move(newPop);
        }

        // --- Statistik und Ausgabe --- (Nur für Ausgabe benötigt)
        int completedGens = (j + 1) * batchSize;
        double progressPercent = (static_cast<double>(j + 1) / samplingRate) * 100.0;

    //log.INFO("[AUSWERTUNG] Generation {} mit durchschn. Kosten = {:.2f}", completedGens, calculateAverageCost(population));
    //log.INFO("[AUSWERTUNG] {}% der Generationen durchlaufen.", static_cast<int>(progressPercent));
    //log.INFO("[AUSWERTUNG] Restzeit: {:.2f} s", timer.remainingSeconds());

    if (generations_ < completedGens && generationProgress_ < progressPercent)
    {
        generations_ = completedGens;
        generationProgress_ = progressPercent;
    }

        // Besten-Check
        auto currentBest = *std::min_element(population.begin(), population.end(),
            [](const Solution& a, const Solution& b) {return a.cost < b.cost;});

        if (currentBest.cost < globalBestCost)
        {
            globalBestCost = currentBest.cost;
            globalBestSolution = currentBest;
            //log.INFO("[MAIN] Neue beste Lösung gefunden! Kosten: {}", globalBestCost);
        }

    // Extrahieren der Generation für die Historie
    int currentGen = (j + 1) * batchSize;
    history.push_back({currentGen, globalBestCost});
    }

    log.INFO("[MAIN] Seed war: {}", currentSeed_);
    
    //Ergebnis der Zeitmessung für die jeweilige Instanz
    log.INFO("[MAIN] Abgelaufen nach: {:.6f} s", timer.elapsedSeconds());
    log.INFO("[MAIN] Restzeit: {:.6f} s", timer.remainingSeconds());

    // best_solution.json exportieren
    std::string exportPath =   "best_solutions/" +
                                std::string("Instanz_") + std::to_string(instanceIdx_) + "/" +
                                presetForFolder_ + "/" +
                                "Test_" + std::to_string(numTest_) + "/" +
                                "best_solution.json";
    exportAsJson(globalBestSolution, instance, exportPath, instance.getFilename(), generations_, generationProgress_);

    // Historie exportieren
    std::string testDir =   "best_solutions/Instanz_" + std::to_string(instanceIdx_) + "/" +
                            presetForFolder_ + "/" + std::to_string(numTest_) + "/";
    std::string csvPath = testDir + "evolution.csv";

    std::filesystem::create_directories(testDir);

    std::ofstream csvFile(csvPath);
    csvFile << "Generation,Cost\n";
    for (const auto& entry : history)
    {
        csvFile << entry.generation << "," << entry.cost << "\n";
    }
    csvFile.close();


    log.INFO("[MAIN] Beste Lösung: {}", globalBestCost);
    //printBestInPopulation(population, instance, log, numTest_);

    // Entscheiden, was ausgegeben wird je nach Testszenario
    if (turn2_)
    {
        log.INFO("[ALTERNATIVE] Turnier mit 2 Individuen.");
    }
    else
    {
        log.INFO("[ALTERNATIVE] Turnier mit 5% der Population.");
    }

    if (full2opt_)
    {
        log.INFO("[ALTERNATIVE] Initialisierung und Mutation mit 2opt-Optimierung.\n");
    }

    if (mutate2opt_)
    {
        log.INFO("[ALTERNATIVE] Mutation mit 2opt-Optimierung.\n");
    }

    if (init2opt_)
    {
        log.INFO("[ALTERNATIVE] Initialisierung mit 2opt-Optimierung.\n");
    }


    return EXIT_SUCCESS;
}
