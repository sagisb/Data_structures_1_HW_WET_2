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
int SongUnionFind::addSong(int* genreIdPtr) {
    if (size == capacity) if (!resize()) return -1;
    int idx = size;
    songs[idx].initAsLeader(idx, genreIdPtr);
    size++;
    return idx;
}
SongUnionFind::FindResult SongUnionFind::findLeader(int udIndex) {
    if (songs[udIndex].getParent() == udIndex) return {udIndex, 0};
    FindResult parentResult = findLeader(songs[udIndex].getParent());
    //FindResult res = findLeader(songs[udIndex].getParent());
    songs[udIndex].setGenreChanges(songs[udIndex].getGenreChanges() + parentResult.distance_to_leader);

    // Now, perform the path compression
    songs[udIndex].setParent(parentResult.leader_uf_idx);

    // Return the final result for the current node
    return {
            parentResult.leader_uf_idx,         // The final leader
            songs[udIndex].getGenreChanges(),   // The NEW stored value at this node
            1                                   // After compression, distance to leader is always 1
    };
}



void SongUnionFind::unionSongs(int uf_idx1, int uf_idx2) {
    int leader1 = findLeader(uf_idx1).leader_uf_idx;
    int leader2 = findLeader(uf_idx2).leader_uf_idx;
    if (leader1 == leader2) return;

    if (songs[leader1].getChildrenCount() < songs[leader2].getChildrenCount()) {
        songs[leader1].setParent(leader2);
        songs[leader2].setChildrenCount(songs[leader2].getChildrenCount() + songs[leader1].getChildrenCount());
       // songs[leader1].setGenreIdPtr(nullptr);
       // songs[leader1].setGenreChanges(1);
    } else {
        songs[leader2].setParent(leader1);
        songs[leader1].setChildrenCount(songs[leader1].getChildrenCount() + songs[leader2].getChildrenCount());
       // songs[leader2].setGenreIdPtr(nullptr);
       // songs[leader2].setGenreChanges(1);
    }
}
int SongUnionFind::getChildrenCount(int leader_uf_idx) const { return songs[leader_uf_idx].getChildrenCount(); }
int* SongUnionFind::getGenreIdPtr(int leader_uf_idx) const { return songs[leader_uf_idx].getGenreIdPtr(); }
void SongUnionFind::setGenreIdPtr(int leader_uf_idx, int* new_ptr) { songs[leader_uf_idx].setGenreIdPtr(new_ptr); }
