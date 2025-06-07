#pragma once

class SongNode {
private:
    int songId;
    int parent;
    int genreChanges;
    int childrenCount;
    int *genreIdPtr;

public:
    SongNode() : songId(0), parent(-1), genreChanges(0), childrenCount(0), genreIdPtr(nullptr) {}

    void initAsLeader(int s_id, int self_idx, int *genrePtr) {
        songId = s_id;
        parent = self_idx;
        genreChanges = 1;
        childrenCount = 1;
        genreIdPtr = genrePtr;
    }
    int getSongId() const { return songId; }
    int getParent() const { return parent; }
    int getGenreChanges() const { return genreChanges; }
    int getChildrenCount() const { return childrenCount; }
    int *getGenreIdPtr() const { return genreIdPtr; }

    void setParent(int newParent) { parent = newParent; }
    void setGenreChanges(int changes) { genreChanges = changes; }
    void setChildrenCount(int count) { childrenCount = count; }
    void setGenreIdPtr(int *ptr) { genreIdPtr = ptr; }
};