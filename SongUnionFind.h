#pragma once

#include "SongNode.h"

/**
 * Disjoint-set data structure with path compression and union-by-size.
 *
 * Each set represents a "genre component"; the root (leader) node stores
 * per-component metadata such as current `genreId`, the number of children,
 * and how many times the entire component has changed genre (`genreChanges`).
 *
 * Non-root nodes store the **difference** between their parent's
 * `genreChanges` and their own, enabling O(1) queries for
 * `getNumberOfGenreChanges` after a single `findLeader`.
 */
class SongUnionFind {
  public:
    /** Result tuple returned from `findLeader`. */
    struct FindResult {
        int leader_uf_idx;  ///< UF index of the root after path compression
        int diff_to_leader; ///< cumulative diff (leader.merges - node.merges)
    };

    SongUnionFind();

    ~SongUnionFind();

    int addSong(int songId, int genreId);

    /** Find the root of `ufIndex` while applying path compression. */
    FindResult findLeader(int ufIndex);

    /** Merge the two components containing the given nodes. */
    void unionSongs(int uf_idx1, int uf_idx2);

    /** Increment the `genreChanges` counter stored at a leader node. */
    void incrementLeaderChanges(int leader_uf_idx);

    int getChildrenCount(int leader_uf_idx) const;

    int getGenreId(int leader_uf_idx) const;

    void setGenreId(int leader_uf_idx, int new_id);

    int getLeaderMerges(int leader_uf_idx) const;

    void addToNodeDiff(int idx, int delta);

private:
    SongNode *songs = nullptr; ///< dynamic array of UF nodes
    int capacity = 0;          ///< total allocated slots in `songs`
    int size = 0;              ///< number of songs currently stored

    bool resize();

    struct HelperResult {
        int leader_idx;
        int distance;
    };

    HelperResult findHelper(int udIndex);
};