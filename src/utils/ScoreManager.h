#pragma once
#include <chrono>
#include <string>
#include <vector>

// Represents one saved solve result (used for the local history scoreboard)
struct ScoreEntry {
    int cubeSize;
    int moves;
    double timeSeconds;
    int score;
    std::string timestamp; // human-readable
};

// Tracks the live solving session (move count + stopwatch) and converts
// a completed solve into a score. Persists results to data/scores.csv
// so the HUD history panel survives between runs.
class ScoreManager {
public:
    ScoreManager();

    // Call when a fresh scramble finishes animating -> starts the stopwatch
    // and resets the move counter for a new attempt.
    void startSession(int cubeSize);

    // Call every time the user performs a manual face turn while a
    // session is active (solver/auto moves do not count).
    void recordMove();

    // Call once activeCube.isSolved() becomes true after a real solve.
    // Computes the score, stores it in history, persists to disk and
    // returns the entry that was just created.
    ScoreEntry finishSession();

    // Cancels the current session without scoring it (e.g. cube size changed)
    void cancelSession();

    bool isActive() const { return active; }
    int getMoveCount() const { return moveCount; }
    double getElapsedSeconds() const;

    // Live score preview for the in-progress attempt
    int getLiveScore() const;

    // Last finished result (valid after finishSession() has been called at least once)
    const ScoreEntry& getLastResult() const { return lastResult; }
    bool hasLastResult() const { return hasResult; }

    // Best score recorded so far for a given cube size (0 if none)
    int getBestScore(int cubeSize) const;

    std::vector<ScoreEntry> getHistory(int cubeSize, int maxCount = 5) const;

    // Standard WCA average of N (drops best and worst time, averages the rest)
    double getAverageOfN(int cubeSize, int n) const;

private:
    bool active = false;
    bool hasResult = false;
    int moveCount = 0;
    int currentSize = 3;
    std::chrono::steady_clock::time_point startTime;

    ScoreEntry lastResult{};
    std::vector<ScoreEntry> history;

    // Computes the score for a given size/time/move combination.
    // Rewards fewer moves and lower time, scaled up for bigger (harder) cubes.
    static int computeScore(int cubeSize, int moves, double timeSeconds);

    void loadHistory();
    void appendToFile(const ScoreEntry& entry);

public:
    void togglePause();
    bool isPaused() const { return paused; }

private:
    bool paused = false;
    double pausedAccumulated = 0.0; // seconds accumulated before the current pause
    std::chrono::steady_clock::time_point pauseStartTime;
};
