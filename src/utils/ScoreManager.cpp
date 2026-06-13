#include "ScoreManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdio>
#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

static const char* SCORE_FILE = "data/scores.csv";

ScoreManager::ScoreManager() {
    loadHistory();
}

void ScoreManager::startSession(int cubeSize) {
    active = true;
    moveCount = 0;
    currentSize = cubeSize;
    startTime = std::chrono::steady_clock::now();
}

void ScoreManager::cancelSession() {
    active = false;
    moveCount = 0;
}

void ScoreManager::recordMove() {
    if (active) {
        moveCount++;
    }
}

double ScoreManager::getElapsedSeconds() const {
    if (!active) return 0.0;
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - startTime).count();
}

int ScoreManager::computeScore(int cubeSize, int moves, double timeSeconds) {
    // Base pool of points available for a "perfect" (instant, zero extra move) solve.
    const double basePool = 10000.0;

    // Larger cubes are intrinsically harder, so they get a multiplier boost.
    // 2x2 -> x1, 3x3 -> x2, 4x4 -> x3, 5x5 -> x4, 6x6 -> x5, 7x7 -> x6 ...
    double sizeMultiplier = std::max(1, cubeSize - 1);

    // Penalties: each move costs points, each second costs points.
    double movePenalty = moves * 12.0;
    double timePenalty = timeSeconds * 8.0;

    double raw = (basePool - movePenalty - timePenalty) * sizeMultiplier;
    if (raw < 0.0) raw = 0.0;

    return static_cast<int>(raw + 0.5);
}

int ScoreManager::getLiveScore() const {
    if (!active) return 0;
    return computeScore(currentSize, moveCount, getElapsedSeconds());
}

ScoreEntry ScoreManager::finishSession() {
    ScoreEntry entry;
    entry.cubeSize = currentSize;
    entry.moves = moveCount;
    entry.timeSeconds = getElapsedSeconds();
    entry.score = computeScore(entry.cubeSize, entry.moves, entry.timeSeconds);

    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    entry.timestamp = buf;

    history.push_back(entry);
    appendToFile(entry);

    lastResult = entry;
    hasResult = true;
    active = false;
    moveCount = 0;

    return entry;
}

int ScoreManager::getBestScore(int cubeSize) const {
    int best = 0;
    for (const auto& e : history) {
        if (e.cubeSize == cubeSize && e.score > best) {
            best = e.score;
        }
    }
    return best;
}

std::vector<ScoreEntry> ScoreManager::getHistory(int cubeSize, int maxCount) const {
    std::vector<ScoreEntry> result;
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        if (it->cubeSize == cubeSize) {
            result.push_back(*it);
            if ((int)result.size() >= maxCount) break;
        }
    }
    return result;
}

void ScoreManager::loadHistory() {
    std::ifstream file(SCORE_FILE);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string sizeStr, movesStr, timeStr, scoreStr, timestamp;

        if (!std::getline(ss, sizeStr, ',')) continue;
        if (!std::getline(ss, movesStr, ',')) continue;
        if (!std::getline(ss, timeStr, ',')) continue;
        if (!std::getline(ss, scoreStr, ',')) continue;
        std::getline(ss, timestamp); // rest of line (may contain no commas)

        try {
            ScoreEntry e;
            e.cubeSize = std::stoi(sizeStr);
            e.moves = std::stoi(movesStr);
            e.timeSeconds = std::stod(timeStr);
            e.score = std::stoi(scoreStr);
            e.timestamp = timestamp;
            history.push_back(e);
        } catch (...) {
            // Skip malformed lines (e.g. header row)
            continue;
        }
    }
}

void ScoreManager::appendToFile(const ScoreEntry& entry) {
    // Ensure the "data" directory exists (best-effort, ignored on failure)
#if defined(_WIN32)
    _mkdir("data");
#else
    mkdir("data", 0755);
#endif

    bool needsHeader = false;
    {
        std::ifstream check(SCORE_FILE);
        needsHeader = !check.good();
    }

    std::ofstream file(SCORE_FILE, std::ios::app);
    if (!file.is_open()) return;

    if (needsHeader) {
        file << "cube_size,moves,time_seconds,score,timestamp\n";
    }

    file << entry.cubeSize << ","
          << entry.moves << ","
          << entry.timeSeconds << ","
          << entry.score << ","
          << entry.timestamp << "\n";
}
