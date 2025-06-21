#pragma once

#include "SongNode.h"

class SongUnionFind {
public:
    struct FindResult {
        int leader_uf_idx;
        int diff_to_leader; // cumulative diff (leader.merges - node.merges)
    };

    SongUnionFind();

    ~SongUnionFind();

    int addSong(int songId, int genreId);

    FindResult findLeader(int ufIndex);

    void unionSongs(int uf_idx1, int uf_idx2);

    void incrementLeaderChanges(int leader_uf_idx);

    int getChildrenCount(int leader_uf_idx) const;

    int getGenreId(int leader_uf_idx) const;

    void setGenreId(int leader_uf_idx, int new_id);

    int getLeaderMerges(int leader_uf_idx) const;

    void addToNodeDiff(int idx, int delta);

private:
    SongNode *songs;
    int capacity;
    int size;

    bool resize();

    struct HelperResult {
        int leader_idx;
        int distance;
    };

    HelperResult findHelper(int udIndex);
};