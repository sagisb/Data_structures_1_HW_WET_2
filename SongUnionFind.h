#pragma once
#include "SongNode.h"

class SongUnionFind {
public:
    struct FindResult { int leader_uf_idx; int total_changes; int distance_to_leader;};
    SongUnionFind();
    ~SongUnionFind();
    int addSong(int* genreIdPtr);
    FindResult findLeader(int udIndex);
    void unionSongs(int uf_idx1, int uf_idx2);
    int getChildrenCount(int leader_uf_idx) const;
    int* getGenreIdPtr(int leader_uf_idx) const;
    void setGenreIdPtr(int leader_uf_idx, int* new_ptr);
private:
    SongNode* songs;
    int capacity;
    int size;
    bool resize();
};