#include "SongUnionFind.h"
#include <new>

SongUnionFind::SongUnionFind() : capacity(10), size(0) {
    songs = new (std::nothrow) SongNode[capacity];
    if (songs == nullptr) capacity = 0;
}
SongUnionFind::~SongUnionFind() { delete[] songs; }
bool SongUnionFind::resize() {
    int new_cap = (capacity > 0) ? capacity * 2 : 10;
    SongNode* new_nodes = new (std::nothrow) SongNode[new_cap];
    if (new_nodes == nullptr) return false;
    for (int i = 0; i < size; ++i) {
        new_nodes[i] = songs[i];
    }
    delete[] songs;
    songs = new_nodes;
    capacity = new_cap;
    return true;
}
int SongUnionFind::addSong(int* genre_id_ptr) {
    if (size == capacity) if (!resize()) return -1;
    int idx = size;
    songs[idx].initAsLeader(idx, genre_id_ptr);
    size++;
    return idx;
}
SongUnionFind::FindResult SongUnionFind::findLeader(int uf_idx) {
    if (songs[uf_idx].getParent() == uf_idx) return {uf_idx, 0};
    FindResult res = findLeader(songs[uf_idx].getParent());
    songs[uf_idx].setGenreChanges(songs[uf_idx].getGenreChanges() + res.total_changes);
    songs[uf_idx].setParent(res.leader_uf_idx);
    return {res.leader_uf_idx, songs[uf_idx].getGenreChanges()};
}
void SongUnionFind::unionSongs(int uf_idx1, int uf_idx2) {
    int leader1 = findLeader(uf_idx1).leader_uf_idx;
    int leader2 = findLeader(uf_idx2).leader_uf_idx;
    if (leader1 == leader2) return;

    if (songs[leader1].getChildrenCount() < songs[leader2].getChildrenCount()) {
        songs[leader1].setParent(leader2);
        songs[leader2].setChildrenCount(songs[leader2].getChildrenCount() + songs[leader1].getChildrenCount());
        songs[leader1].setGenreIdPtr(nullptr);
        songs[leader1].setGenreChanges(1);
    } else {
        songs[leader2].setParent(leader1);
        songs[leader1].setChildrenCount(songs[leader1].getChildrenCount() + songs[leader2].getChildrenCount());
        songs[leader2].setGenreIdPtr(nullptr);
        songs[leader2].setGenreChanges(1);
    }
}
int SongUnionFind::getChildrenCount(int leader_uf_idx) const { return songs[leader_uf_idx].getChildrenCount(); }
int* SongUnionFind::getGenreIdPtr(int leader_uf_idx) const { return songs[leader_uf_idx].getGenreIdPtr(); }
void SongUnionFind::setGenreIdPtr(int leader_uf_idx, int* new_ptr) { songs[leader_uf_idx].setGenreIdPtr(new_ptr); }
