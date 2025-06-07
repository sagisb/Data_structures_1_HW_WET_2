#pragma once

class SongNode {
private:
    int parent;
    int genreChanges;
    int childrenCount;
    int *genreIdPtr;

public:
    SongNode() : parent(-1), genreChanges(0), childrenCount(0), genreIdPtr(nullptr) {}

    void initAsLeader(int self_idx, int *genrePtr) {
        parent = self_idx;
        genreChanges = 0;
        childrenCount = 1;
        genreIdPtr = genrePtr;
    }

    int getParent() const { return parent; }
    int getGenreChanges() const { return genreChanges; }
    int getChildrenCount() const { return childrenCount; }
    int *getGenreIdPtr() const { return genreIdPtr; }

    void setParent(int newParent) { parent = newParent; }
    void setGenreChanges(int changes) { genreChanges = changes; }
    void setChildrenCount(int count) { childrenCount = count; }
    void setGenreIdPtr(int *ptr) { genreIdPtr = ptr; }
};