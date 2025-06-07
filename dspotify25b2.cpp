// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b2.h"


DSpotify::DSpotify() = default;

DSpotify::~DSpotify() = default;

StatusType DSpotify::addGenre(int genreId) {
    if (genreId <= 0) return StatusType::INVALID_INPUT;
    if (genresTable.find(genreId) != nullptr) return StatusType::FAILURE;
    Genre newGenre(genreId);
    if (!genresTable.insert(genreId, newGenre)) return StatusType::ALLOCATION_ERROR;
    return StatusType::SUCCESS;
}

StatusType DSpotify::addSong(int songId, int genreId) {
    if (songId <= 0 || genreId <= 0) return StatusType::INVALID_INPUT;
    if (songsTable.find(songId) != nullptr) return StatusType::FAILURE;
    Genre* genre = genresTable.find(genreId);
    if (genre == nullptr) return StatusType::FAILURE;

    // A pointer to the genreId field is passed to the song, which is now the leader.
    int* genreIdFieldPtr = genre->getIdPtr();
    int newSongUfIdx = songsUF.addSong(genreIdFieldPtr);
    if (newSongUfIdx == -1) return StatusType::ALLOCATION_ERROR;
    if (!songsTable.insert(songId, newSongUfIdx)) return StatusType::ALLOCATION_ERROR;

    if (genre->getLeadingSongUFIdx() == -1) {
        genre->setLeadingSongUFIdx(newSongUfIdx);
    } else {
        songsUF.unionSongs(genre->getLeadingSongUFIdx(), newSongUfIdx);
        genre->setLeadingSongUFIdx(songsUF.findLeader(newSongUfIdx).leader_uf_idx);
    }
    genre->setSongCount(genre->getSongCount() + 1);
    return StatusType::SUCCESS;
}

StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int newGenreId) {
    if (genreId1 <= 0 || genreId2 <= 0 || newGenreId <= 0 || genreId1 == genreId2) {
        return StatusType::INVALID_INPUT;
    }
    Genre* g1 = genresTable.find(genreId1);
    Genre* g2 = genresTable.find(genreId2);
    if (g1 == nullptr || g2 == nullptr) {
        return StatusType::FAILURE;
    }
    if (genresTable.find(newGenreId) != nullptr) {
        return StatusType::FAILURE;
    }

    if (!genresTable.insert(newGenreId, Genre(newGenreId))) {
        return StatusType::ALLOCATION_ERROR;
    }
    Genre* newGenre = genresTable.find(newGenreId);

    int leader1Idx = g1->getLeadingSongUFIdx();
    int leader2Idx = g2->getLeadingSongUFIdx();

    if (leader1Idx != -1 && leader2Idx == -1) {
        newGenre->setLeadingSongUFIdx(leader1Idx);
        songsUF.setGenreIdPtr(leader1Idx, newGenre->getIdPtr());
    } else if (leader1Idx == -1 && leader2Idx != -1) {
        newGenre->setLeadingSongUFIdx(leader2Idx);
        songsUF.setGenreIdPtr(leader2Idx, newGenre->getIdPtr());
    } else if (leader1Idx != -1 && leader2Idx != -1) {
        songsUF.unionSongs(leader1Idx, leader2Idx);
        int final_leader = songsUF.findLeader(leader1Idx).leader_uf_idx;
        newGenre->setLeadingSongUFIdx(final_leader);
        songsUF.setGenreIdPtr(final_leader, newGenre->getIdPtr());
    }

    newGenre->setSongCount(g1->getSongCount() + g2->getSongCount());
    genresTable.remove(genreId1);
    genresTable.remove(genreId2);
    return StatusType::SUCCESS;
}

output_t<int> DSpotify::getSongGenre(int songId){
    if (songId <= 0) return StatusType::INVALID_INPUT;
    int* ufIndexPtr = songsTable.find(songId);
    if (ufIndexPtr == nullptr) return StatusType::FAILURE;

    SongUnionFind::FindResult res = songsUF.findLeader(*ufIndexPtr);
    int* genreIdPtr = songsUF.getGenreIdPtr(res.leader_uf_idx);

    if (genreIdPtr == nullptr) return StatusType::FAILURE;
    return output_t<int>(*genreIdPtr);
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId){
    if (genreId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    Genre* genre = genresTable.find(genreId);
    if (genre == nullptr) return output_t<int>(StatusType::FAILURE);
    return output_t<int>(genre->getSongCount());
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId){
    if (songId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    int* ufIndexPtr = songsTable.find(songId);
    if (ufIndexPtr == nullptr) return output_t<int>(StatusType::FAILURE);

    SongUnionFind::FindResult res = songsUF.findLeader(*ufIndexPtr);
    return output_t<int>(1 + res.total_changes);
}
