#pragma once

#include "SongUnionFind.h" // needed for validation helper

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

    // Helper: validate cached root pointer. Returns -1 if invalid.
    int validatedLeader(SongUnionFind &uf) {
        int idx = leadingSongUFIdx;
        if (idx == -1) {
            return -1; // no leader cached
        }
        SongUnionFind::FindResult chk = uf.findLeader(idx);
        int rootIdx = chk.leader_uf_idx;
        // If component no longer belongs to this genre – invalidate
        if (uf.getGenreId(rootIdx) != genreId) {
            leadingSongUFIdx = -1;
            return -1;
        }
        // Otherwise refresh pointer to the (possibly new) root
        leadingSongUFIdx = rootIdx;
        return rootIdx;
    }
};