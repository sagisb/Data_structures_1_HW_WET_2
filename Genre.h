#pragma once

class Genre {
private:
    int genreId;
    int songCount;
    int leadingSongUFIdx; // Index of the leading song and -1 if doesn't exist

public:
    Genre() : genreId(0), songCount(0), leadingSongUFIdx(-1) {}

    Genre(int id) : genreId(id), songCount(0), leadingSongUFIdx(-1) {}

    // --- Getters ---
    int getId() const { return genreId; }

    int getSongCount() const { return songCount; }

    int getLeadingSongUFIdx() const { return leadingSongUFIdx; }

    int *getIdPtr() { return &genreId; }

    // --- Setters ---
    void setSongCount(int count) { songCount = count; }

    void setLeadingSongUFIdx(int idx) { leadingSongUFIdx = idx; }
};