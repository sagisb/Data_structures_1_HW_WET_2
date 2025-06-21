#include "SongUnionFind.h"
#include <new>

SongUnionFind::SongUnionFind() : capacity(10), size(0) {
    songs = new(std::nothrow) SongNode[capacity];
    if (songs == nullptr)
        capacity = 0;
}

SongUnionFind::~SongUnionFind() { delete[] songs; }

bool SongUnionFind::resize() {
    int newCap = (capacity > 0) ? capacity * 2 : 10;
    SongNode *newNodes = new(std::nothrow) SongNode[newCap];
    if (newNodes == nullptr)
        return false;
    for (int i = 0; i < size; ++i) {
        newNodes[i] = songs[i];
    }
    delete[] songs;
    songs = newNodes;
    capacity = newCap;
    return true;
}

int SongUnionFind::addSong(int songId, int genreId) {
    if (size == capacity)
        if (!resize())
            return -1;
    int idx = size;
    songs[idx].initAsLeader(songId, idx, genreId);
    size++;
    return idx;
}

SongUnionFind::FindResult SongUnionFind::findLeader(int ufIndex) {
    HelperResult helper = findHelper(ufIndex);
    return {helper.leader_idx, helper.distance};
}

void SongUnionFind::unionSongs(int uf_idx1, int uf_idx2) {
    int leader1 = findLeader(uf_idx1).leader_uf_idx;
    int leader2 = findLeader(uf_idx2).leader_uf_idx;
    if (leader1 == leader2)
        return;

    // Union by size (childrenCount)
    if (songs[leader1].getChildrenCount() < songs[leader2].getChildrenCount()) {
        // Make leader2 the new leader
        songs[leader1].setParent(leader2);
        // diff = parent.merges - my.merges
        int diff = songs[leader2].getGenreChanges() - songs[leader1].getGenreChanges();
        songs[leader1].setGenreChanges(diff);
        int combined = songs[leader2].getChildrenCount() + songs[leader1].getChildrenCount();
        songs[leader2].setChildrenCount(combined);
    } else {
        songs[leader2].setParent(leader1);
        int diff = songs[leader1].getGenreChanges() - songs[leader2].getGenreChanges();
        songs[leader2].setGenreChanges(diff);
        int combined = songs[leader1].getChildrenCount() + songs[leader2].getChildrenCount();
        songs[leader1].setChildrenCount(combined);
    }
}

void SongUnionFind::incrementLeaderChanges(int leader_uf_idx) {
    if (leader_uf_idx != -1) {
        int m = songs[leader_uf_idx].getGenreChanges();
        songs[leader_uf_idx].setGenreChanges(m + 1);
    }
}

int SongUnionFind::getLeaderMerges(int leader_uf_idx) const { return songs[leader_uf_idx].getGenreChanges(); }

int SongUnionFind::getChildrenCount(int leader_uf_idx) const { return songs[leader_uf_idx].getChildrenCount(); }

int SongUnionFind::getGenreId(int leader_uf_idx) const { return songs[leader_uf_idx].getGenreId(); }

void SongUnionFind::setGenreId(int leader_uf_idx, int new_id) {
    songs[leader_uf_idx].setGenreId(new_id);
}

SongUnionFind::HelperResult SongUnionFind::findHelper(int ufIndex) {
    int parentIdx = songs[ufIndex].getParent();
    if (parentIdx == ufIndex) {
        return {ufIndex, 0};
    }
    HelperResult parentRes = findHelper(parentIdx);
    int leader = parentRes.leader_idx;
    int totalDiff = songs[ufIndex].getGenreChanges() + parentRes.distance;

    // Path compression: link directly to leader and store cumulative diff
    songs[ufIndex].setParent(leader);
    songs[ufIndex].setGenreChanges(totalDiff);

    return {leader, totalDiff};
}

void SongUnionFind::addToNodeDiff(int idx, int delta) {
    songs[idx].setGenreChanges(songs[idx].getGenreChanges() + delta);
}