#pragma once

/**
 * Internal node stored inside `SongUnionFind`'s dynamic array.
 *
 * The invariants mirror those in classic weighted-quick-union with path
 * compression, plus an extra `genreChanges` field that lets us answer
 * "how many times did this song change genre?" in O(1) per query.
 */
class SongNode {
private:
    int songId;
    int parent;        // index of parent in UF array. Root points to itself
    int genreChanges;  // For roots: number of genre merges the set has undergone.
    // For non-roots: difference (parent.merges - my.merges)
    int childrenCount; // size of the subtree (valid only at roots)
    int genreId;       // current genre id for the root (undefined for non-roots)

public:
    SongNode() : songId(0), parent(-1), genreChanges(0), childrenCount(1), genreId(0) {}

    void initAsLeader(int s_id, int self_idx, int g_id) {
        songId = s_id;
        parent = self_idx; // root points to itself
        genreChanges = 0;  // zero merges so far
        childrenCount = 1;
        genreId = g_id;
    }

    int getSongId() const { return songId; }

    int getParent() const { return parent; }

    int getGenreChanges() const { return genreChanges; }

    int getChildrenCount() const { return childrenCount; }

    int getGenreId() const { return genreId; }

    void setParent(int newParent) { parent = newParent; }

    void setGenreChanges(int changes) { genreChanges = changes; }

    void setChildrenCount(int count) { childrenCount = count; }

    void setGenreId(int id) { genreId = id; }
};