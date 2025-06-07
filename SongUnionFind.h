#pragma once
#include "SongNode.h"

class SongUnionFind {
public:
    struct FindResult {
        int leader_uf_idx;
        int stored_changes_at_node;
        int distance_to_leader;
    };
    SongUnionFind();
    ~SongUnionFind();
    int addSong(int songId, int* genreIdPtr);
    FindResult findLeader(int udIndex);
    void unionSongs(int uf_idx1, int uf_idx2);
    void incrementLeaderChanges(int leader_uf_idx);
    int getChildrenCount(int leader_uf_idx) const;
    int* getGenreIdPtr(int leader_uf_idx) const;
    void setGenreIdPtr(int leader_uf_idx, int* new_ptr);
private:
    SongNode* songs;
    int capacity;
    int size;
    bool resize();

    struct HelperResult {
        int leader_idx;
        int distance;
    };
    HelperResult findHelper(int udIndex);
};