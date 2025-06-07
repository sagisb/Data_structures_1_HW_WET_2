#include "SongUnionFind.h"
#include <new>

SongUnionFind::SongUnionFind() : capacity(10), size(0) {
    songs = new (std::nothrow) SongNode[capacity];
    if (songs == nullptr) capacity = 0;
}
SongUnionFind::~SongUnionFind() { delete[] songs; }
bool SongUnionFind::resize() {
    int newCap = (capacity > 0) ? capacity * 2 : 10;
    SongNode* newNodes = new (std::nothrow) SongNode[newCap];
    if (newNodes == nullptr) return false;
    for (int i = 0; i < size; ++i) {
        newNodes[i] = songs[i];
    }
    delete[] songs;
    songs = newNodes;
    capacity = newCap;
    return true;
}
int SongUnionFind::addSong(int songId, int* genreIdPtr) {
    if (size == capacity) if (!resize()) return -1;
    int idx = size;
    songs[idx].initAsLeader(songId, idx, genreIdPtr);
    size++;
    return idx;
}
SongUnionFind::FindResult SongUnionFind::findLeader(int udIndex) {
    HelperResult helperResult = findHelper(udIndex);
    return {
            helperResult.leader_idx,
            songs[udIndex].getGenreChanges(),
            helperResult.distance
    };
}

void SongUnionFind::unionSongs(int uf_idx1, int uf_idx2) {
    int leader1 = findLeader(uf_idx1).leader_uf_idx;
    int leader2 = findLeader(uf_idx2).leader_uf_idx;
    if (leader1 == leader2) return;

    if (songs[leader1].getChildrenCount() < songs[leader2].getChildrenCount()) {
        songs[leader1].setParent(leader2);
        int combinedCount = songs[leader2].getChildrenCount() + songs[leader1].getChildrenCount();
        songs[leader2].setChildrenCount(combinedCount);
    } else {
        songs[leader2].setParent(leader1);
        int combinedCount = songs[leader1].getChildrenCount() + songs[leader2].getChildrenCount();
        songs[leader1].setChildrenCount(combinedCount);
    }
}
void SongUnionFind::incrementLeaderChanges(int leader_uf_idx) {
    if (leader_uf_idx != -1) {
        int currentChanges = songs[leader_uf_idx].getGenreChanges();
        songs[leader_uf_idx].setGenreChanges(currentChanges + 1);
    }
}

int SongUnionFind::getChildrenCount(int leader_uf_idx) const { return songs[leader_uf_idx].getChildrenCount(); }
int* SongUnionFind::getGenreIdPtr(int leader_uf_idx) const { return songs[leader_uf_idx].getGenreIdPtr(); }
void SongUnionFind::setGenreIdPtr(int leader_uf_idx, int* new_ptr) { songs[leader_uf_idx].setGenreIdPtr(new_ptr); }
SongUnionFind::HelperResult SongUnionFind::findHelper(int udIndex) {
    int p_idx = songs[udIndex].getParent();
    if (p_idx == udIndex) {
        return {udIndex, 0};
    }
    HelperResult parentRes = findHelper(p_idx);
    int leader = parentRes.leader_idx;
    int distFromParentToLeader = parentRes.distance;
    int myOldDist = 1 + distFromParentToLeader;

    // Using getter and setter
    int currentChanges = songs[udIndex].getGenreChanges();
    songs[udIndex].setGenreChanges(currentChanges + myOldDist - 1);

    // Using setter
    songs[udIndex].setParent(leader);

    return {leader, 1};
}