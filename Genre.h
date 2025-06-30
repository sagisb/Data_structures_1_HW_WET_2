#pragma once

/**
 * Lightweight value-object that tracks per-genre metadata maintained by
 * DSpotify.
 *
 * It intentionally stores *only* what the assignment requires:
 *   • unique `genreId`  (positive integer)
 *   • `songCount`       (number of songs currently belonging to this genre)
 *   • cached UF index of the component leader (`leadingSongUFIdx`)
 *
 * `leadingSongUFIdx` is set to −1 when the genre has no songs or the cached
 * pointer becomes invalid after merges.  Higher-level code is responsible for
 * validating/refreshing it via `DSpotify::refreshGenreLeader`.
 */
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