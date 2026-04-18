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

/****************************************************************************************
 *                                                                                      *
 * Tester wird wie folgt aufgerufen:                                                    *
 *                                                                                      *
 * ./testerVRP <instanceIndex 0-9> <preset small|medium|large> [seed] <anzahlTests>     *
 *                                                                                      *
*****************************************************************************************/

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <random>

#include "util/Logger.hpp"

namespace fs = std::filesystem;

/** demoVRP ist fest */
static const char* DEMO_EXE = "./demoVRP";

/** Erlaubte Presets */
static bool isValidPreset(const std::string& s) {
    return (s == "small1" || s == "small2" || s == "small3" ||
            s == "medium1" || s == "medium2" || s == "medium3" ||
            s == "large1" || s == "large2" || s == "large3" );
}

/*
 * Konvertiert vector<string> in execv()-Argumentliste.
 * execv() erwartet ein NULL-terminiertes char* Array.
 */
static std::vector<char*> toExecArgv(std::vector<std::string>& args) {
    std::vector<char*> out;
    out.reserve(args.size() + 1);
    for (auto& a : args) out.push_back(a.data());
    out.push_back(nullptr);
    return out;
}

// Speichern der Instanz und des Presets um an demoVRP zu übergeben, 
// damit diese dann Ordner/Dateien erstellen kann!
// Welche Instanz wird getestet
int instanceIdx_ = -1; 
// Default preset (egal weil geprüft wird -> nur zur initialisierung)
std::string preset_ = "small1"; 
unsigned int seed_{0};

int main(int argc, char* argv[]) {
    auto log = ko::Logger("TesterVRP");
    if (argc != 4 && argc != 5) {
        std::cerr
            << "[MAIN][ERROR] Usage:\n"
            << "  " << argv[0] << " <instance 0-9> <preset small|medium|large> <anzahlTests>\n"
            << "  " << argv[0] << " <instance 0-9> <preset small|medium|large> <seed> <anzahlTests>\n";
        return 2;
    }

    // --- Parameter parsen ---
    //int instanceIdx = -1;
    try {
        instanceIdx_ = std::stoi(argv[1]);
    } catch (...) {
        std::cerr << "[MAIN][ERROR] <instance> must be an integer in [0..9].\n";
        return 2;
    }
    if (instanceIdx_ < 0 || instanceIdx_ > 9) {
        std::cerr << "Error: <instance> must be in [0..9].\n";
        return 2;
    }

    preset_ = argv[2];
    if (!isValidPreset(preset_)) {
        std::cerr << "Error: <preset> must be one of: small, medium, large.\n";
        return 2;
    }

    bool hasSeed = (argc == 5);
    //unsigned long seed = 0;
    int numTests = 0;

    if (hasSeed) {
        try {
            seed_ = std::stoul(argv[3]);
            //log.INFO("[MAIN] Seed als Argument erhalten: {}", seed_);
        } catch (...) {
            std::cerr << "[MAIN][ERROR] <seed> must be an unsigned integer.\n";
            return 2;
        }
        try {
            numTests = std::stoi(argv[4]);
        } catch (...) {
            std::cerr << "[MAIN][ERROR] <anzahlTests> must be an integer.\n";
            return 2;
        }
    } else {
        try {
            numTests = 1;
            std::random_device rd;
            seed_ = rd();
            log.INFO("[MAIN] Kein Seed als Argument erhalten -> Zufällig generiert.");
        } catch (...) {
            std::cerr << "[MAIN][ERROR] <anzahlTests> must be an integer.\n";
            return 2;
        }
    }

    if (numTests <= 0) {
        std::cerr << "Error: <anzahlTests> must be > 0.\n";
        return 2;
    }

    // --- Instanz-Mapping 0..9 -> Instance-Datei ---
    const std::vector<fs::path> instanceTable = {
        "../instances/A-n32-k5.vrp",    // 0
        "../instances/A-n48-k7.vrp",    // 1
        "../instances/A-n61-k9.vrp",    // 2
        "../instances/A-n69-k9.vrp",    // 3
        "../instances/A-n80-k10.vrp",   // 4
        "../instances/M-n121-k7.vrp",   // 5
        "../instances/M-n200-k17.vrp",  // 6
        "../instances/P-n101-k4.vrp",   // 7
        "../instances/P-n16-k8.vrp",    // 8
        "../instances/P-n76-k5.vrp"     // 9
    };

    fs::path instancePath = instanceTable.at(instanceIdx_);

    // --- Sanity checks ---
    if (!fs::exists(DEMO_EXE)) {
        std::cerr << "Error: demoVRP not found at: " << DEMO_EXE << "\n";
        return 2;
    }
    if (!fs::exists(instancePath)) {
        std::cerr << "Error: instance file not found: " << instancePath << "\n"
                  << "Fix instanceTable in testerVRP.cpp.\n";
        return 2;
    }

    // --- Info ---
    std::cout << "[LOG][INFO] (DemoVRP) [START] Executable : " << DEMO_EXE << "\n"
              << "[LOG][INFO] (DemoVRP) [START] Instanz   : [" << instanceIdx_ << "] " << instancePath << "\n"
              << "[LOG][INFO] (DemoVRP) [START] Preset     : " << preset_ << "\n";
    if (hasSeed) std::cout << "[LOG][INFO] (DemoVRP) [START] Seed       : " << seed_ << " (fixed)\n";
    else         std::cout << "[LOG][INFO] (DemoVRP) [START] Seed       : (none; demoVRP generates it)\n";
    std::cout << "[LOG][INFO] (DemoVRP) [START] Anzahl Tests       : " << numTests << "\n\n";

    // --- Mehrfach ausführen ---
    int okCount = 0;
    int failCount = 0;

    //log.INFO("[MAIN] Seed der übergeben wird: {}", seed_);

    for (int run = 1; run <= numTests; ++run) {
        // demoVRP Aufruf: ./demoVRP <instance-path> <preset-name> [custom-seed]
        std::vector<std::string> args;
        args.emplace_back(DEMO_EXE);               // argv[0]
        args.emplace_back(instancePath.string());  // <instance-path>
        args.emplace_back(preset_);                 // <preset-name>
        args.emplace_back(std::to_string(instanceIdx_));            // wird auch übergeben, um Filesystem für ergebnisse aufzubauen
        args.emplace_back(std::to_string(run - 1));
        args.emplace_back(std::to_string(seed_)); // [custom-seed]
        //log.INFO("[MAIN] Anzahl Argumente für DemoVRP: {}", args.size());
        


        auto cargs = toExecArgv(args);

        std::cout << "Run " << run << "/" << numTests << "...\n";

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            execv(cargs[0], cargs.data());
            // Nur bei Fehler:
            perror("execv");
            _exit(127);
        }

        int status = 0;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            if (code == 0) {
                ++okCount;
                std::cout << "  -> OK (exit 0)\n\n";
            } else {
                ++failCount;
                std::cout << "  -> FAIL (exit " << code << ")\n\n";
            }
        } else if (WIFSIGNALED(status)) {
            ++failCount;
            std::cout << "  -> CRASH (signal " << WTERMSIG(status) << ")\n\n";
        } else {
            ++failCount;
            std::cout << "  -> FAIL (abnormal termination)\n\n";
        }
    }

    std::cout << "Summary: OK=" << okCount << " FAIL=" << failCount << "\n";
    return (failCount == 0) ? 0 : 1;
}