#pragma once
#include <vector>
#include "cube/Move.h"

class SolutionPlayer {
private:
    std::vector<Move> moves;
    int currentIndex;
    bool playing;
    float delayTimer;
    float moveDelay; // Time delay in seconds between moves in autoplay mode

public:
    SolutionPlayer();
    ~SolutionPlayer();

    // Sets the sequence of moves for playback and resets play state
    void setMoves(const std::vector<Move>& newMoves);
    const std::vector<Move>& getMoves() const { return moves; }
    
    int getCurrentIndex() const { return currentIndex; }
    int getMoveCount() const { return static_cast<int>(moves.size()); }
    
    bool isPlaying() const { return playing; }
    void setPlaying(bool play) { playing = play; }

    float getSpeed() const;
    void adjustSpeed(float delta); // adjusts speed (increases or decreases delay)

    // Autoplay step forward and step backward
    void stepForward(class Animation& anim, class RubiksCube& cube);
    void stepBackward(class Animation& anim, class RubiksCube& cube);
    
    // Updates autoplay timer
    void update(float deltaTime, class Animation& anim, class RubiksCube& cube);
    void reset();
};
