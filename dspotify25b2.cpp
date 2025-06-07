// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b2.h"

DSpotify::DSpotify() = default;

DSpotify::~DSpotify() = default;

StatusType DSpotify::addGenre(int genreId) {
    if (genreId <= 0) { return StatusType::INVALID_INPUT; }
    if (genresTable.find(genreId) != nullptr) { return StatusType::FAILURE; }
    Genre newGenre(genreId);
    if (!genresTable.insert(genreId, newGenre)) { return StatusType::ALLOCATION_ERROR; }
    return StatusType::SUCCESS;
}

StatusType DSpotify::addSong(int songId, int genreId) {
    if (songId <= 0 || genreId <= 0) { return StatusType::INVALID_INPUT; }
    if (songsTable.find(songId) != nullptr) { return StatusType::FAILURE; }
    Genre *genre = genresTable.find(genreId);
    if (genre == nullptr) { return StatusType::FAILURE; }

    int *genreIdFieldPtr = genre->getIdPtr();
    // The call to songsUF.addSong now passes the songId
    int newSongUfIdx = songsUF.addSong(songId, genreIdFieldPtr);
    if (newSongUfIdx == -1) { return StatusType::ALLOCATION_ERROR; }
    if (!songsTable.insert(songId, newSongUfIdx)) { return StatusType::ALLOCATION_ERROR; }

    if (genre->getLeadingSongUFIdx() == -1) {
        genre->setLeadingSongUFIdx(newSongUfIdx);
    }
    else {
        songsUF.unionSongs(genre->getLeadingSongUFIdx(), newSongUfIdx);
        int final_leader = songsUF.findLeader(newSongUfIdx).leader_uf_idx;
        genre->setLeadingSongUFIdx(final_leader);
    }
    genre->setSongCount(genre->getSongCount() + 1);
    return StatusType::SUCCESS;
}

StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int newGenreId) {

    if (genreId1 <= 0 || genreId2 <= 0 || newGenreId <= 0 || genreId1 == genreId2) {
        return StatusType::INVALID_INPUT;
    }
    Genre *g1 = genresTable.find(genreId1);
    Genre *g2 = genresTable.find(genreId2);
    if (g1 == nullptr || g2 == nullptr) {
        return StatusType::FAILURE;
    }
    if (genresTable.find(newGenreId) != nullptr) {
        return StatusType::FAILURE;
    }

    if (!genresTable.insert(newGenreId, Genre(newGenreId))) {
        return StatusType::ALLOCATION_ERROR;
    }
    Genre *newGenre = genresTable.find(newGenreId);

    int leader1Idx = g1->getLeadingSongUFIdx();
    int leader2Idx = g2->getLeadingSongUFIdx();
    int final_leader = -1;
    if (leader1Idx != -1 && leader2Idx != -1) {
        songsUF.unionSongs(leader1Idx, leader2Idx);
        final_leader = songsUF.findLeader(leader1Idx).leader_uf_idx;
    }
    else if (leader1Idx != -1 && leader2Idx == -1) {
        final_leader = songsUF.findLeader(leader1Idx).leader_uf_idx;
    }
    else if (leader1Idx == -1 && leader2Idx != -1) {
        final_leader = songsUF.findLeader(leader2Idx).leader_uf_idx;
    }
    songsUF.incrementLeaderChanges(final_leader);

    if (final_leader != -1) {
        newGenre->setLeadingSongUFIdx(final_leader);
        songsUF.setGenreIdPtr(final_leader, newGenre->getIdPtr());
    }
    newGenre->setSongCount(g1->getSongCount() + g2->getSongCount());

    g1->setSongCount(0);
    g1->setLeadingSongUFIdx(-1);
    g2->setSongCount(0);
    g2->setLeadingSongUFIdx(-1);

    return StatusType::SUCCESS;
}

output_t<int> DSpotify::getSongGenre(int songId) {
    if (songId <= 0) { return StatusType::INVALID_INPUT; }
    int *ufIndexPtr = songsTable.find(songId);
    if (ufIndexPtr == nullptr) { return StatusType::FAILURE; }


    int leaderIndex = songsUF.findLeader(*ufIndexPtr).leader_uf_idx;
    int *genreIdPtr = songsUF.getGenreIdPtr(leaderIndex);

    if (genreIdPtr == nullptr) { return output_t<int>(StatusType::FAILURE); }
    return output_t<int>(*genreIdPtr);
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId) {
    if (genreId <= 0) { return output_t<int>(StatusType::INVALID_INPUT); }
    Genre *genre = genresTable.find(genreId);
    if (genre == nullptr) { return output_t<int>(StatusType::FAILURE); }
    return output_t<int>(genre->getSongCount());
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId) {
    if (songId <= 0) { return output_t<int>(StatusType::INVALID_INPUT); }
    int *ufIndexPtr = songsTable.find(songId);
    if (ufIndexPtr == nullptr) { return output_t<int>(StatusType::FAILURE); }

    SongUnionFind::FindResult res = songsUF.findLeader(*ufIndexPtr);

    // This is the number of times the song's genre was MERGED.
    // For a new song, this value is 0.
    int totalChanges = res.stored_changes_at_node + res.distance_to_leader;

    // We add 1 to account for the very first genre the song was created with.
    return output_t<int>(totalChanges);
}
