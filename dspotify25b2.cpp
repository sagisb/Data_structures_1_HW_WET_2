// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b2.h"

DSpotify::DSpotify() = default;

DSpotify::~DSpotify() = default;

StatusType DSpotify::addGenre(int genreId) {
    if (genreId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    if (genresTable.find(genreId) != nullptr) {
        return StatusType::FAILURE;
    }
    Genre newGenre(genreId);
    if (!genresTable.insert(genreId, newGenre)) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType DSpotify::addSong(int songId, int genreId) {
    if (songId <= 0 || genreId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    if (songsTable.find(songId) != nullptr) {
        return StatusType::FAILURE;
    }
    Genre *genre = genresTable.find(genreId);
    if (genre == nullptr) {
        return StatusType::FAILURE;
    }

    // Add the song into Union-Find with its current genre id
    int newSongUfIdx = songsUF.addSong(songId, genreId);
    if (newSongUfIdx == -1) {
        return StatusType::ALLOCATION_ERROR;
    }
    if (!songsTable.insert(songId, newSongUfIdx)) {
        return StatusType::ALLOCATION_ERROR;
    }

    int leaderIdxStored = genre->getLeadingSongUFIdx();
    if (leaderIdxStored != -1) {
        // Validate that the stored index is still a root that represents the same genre
        SongUnionFind::FindResult chk = songsUF.findLeader(leaderIdxStored);
        if (chk.leader_uf_idx != leaderIdxStored || songsUF.getGenreId(leaderIdxStored) != genreId) {
            // Outdated pointer – reset
            unmapLeader(leaderIdxStored);
            leaderIdxStored = -1;
            genre->setLeadingSongUFIdx(-1);
        }
    }

    if (leaderIdxStored == -1) {
        genre->setLeadingSongUFIdx(newSongUfIdx);
        mapLeaderToGenre(newSongUfIdx, genreId);
    } else {
        songsUF.unionSongs(leaderIdxStored, newSongUfIdx);
        int final_leader = songsUF.findLeader(newSongUfIdx).leader_uf_idx;
        genre->setLeadingSongUFIdx(final_leader);
        mapLeaderToGenre(final_leader, genreId);
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

    auto validateLeaderEarly = [&](int idx, int expectedGenreId, Genre *g) -> int {
        if (idx == -1)
            return -1;
        SongUnionFind::FindResult chk = songsUF.findLeader(idx);
        if (chk.leader_uf_idx != idx || songsUF.getGenreId(idx) != expectedGenreId) {
            unmapLeader(idx);
            g->setLeadingSongUFIdx(-1);
            return -1;
        }
        return idx;
    };

    leader1Idx = validateLeaderEarly(leader1Idx, genreId1, g1);
    leader2Idx = validateLeaderEarly(leader2Idx, genreId2, g2);

    int final_leader = -1;
    if (leader1Idx != -1 && leader2Idx != -1) {
        songsUF.unionSongs(leader1Idx, leader2Idx);
        final_leader = songsUF.findLeader(leader1Idx).leader_uf_idx;
    } else if (leader1Idx != -1 && leader2Idx == -1) {
        final_leader = songsUF.findLeader(leader1Idx).leader_uf_idx;
    } else if (leader1Idx == -1 && leader2Idx != -1) {
        final_leader = songsUF.findLeader(leader2Idx).leader_uf_idx;
    }

    // All songs in the resulting genre experienced exactly one additional change
    songsUF.incrementLeaderChanges(final_leader);

    if (final_leader != -1) {
        newGenre->setLeadingSongUFIdx(final_leader);
        songsUF.setGenreId(final_leader, newGenreId);
        mapLeaderToGenre(final_leader, newGenreId);
    }
    newGenre->setSongCount(g1->getSongCount() + g2->getSongCount());

    unmapLeader(leader1Idx);
    unmapLeader(leader2Idx);
    g1->setSongCount(0);
    g1->setLeadingSongUFIdx(-1);
    g2->setSongCount(0);
    g2->setLeadingSongUFIdx(-1);

    auto validateLeader = [&](int idx, int expectedGenreId) -> int {
        if (idx == -1)
            return -1;
        // Must still be a root and belong to its genre
        SongUnionFind::FindResult chk = songsUF.findLeader(idx);
        if (chk.leader_uf_idx != idx || songsUF.getGenreId(idx) != expectedGenreId) {
            return -1;
        }
        return idx;
    };

    leader1Idx = validateLeader(leader1Idx, genreId1);
    if (leader1Idx == -1) {
        unmapLeader(g1->getLeadingSongUFIdx());
        g1->setLeadingSongUFIdx(-1);
    }

    leader2Idx = validateLeader(leader2Idx, genreId2);
    if (leader2Idx == -1) {
        unmapLeader(g2->getLeadingSongUFIdx());
        g2->setLeadingSongUFIdx(-1);
    }

    return StatusType::SUCCESS;
}

output_t<int> DSpotify::getSongGenre(int songId) {
    if (songId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    int *ufIndexPtr = songsTable.find(songId);
    if (ufIndexPtr == nullptr) {
        return StatusType::FAILURE;
    }

    int leaderIndex = songsUF.findLeader(*ufIndexPtr).leader_uf_idx;
    int gid = songsUF.getGenreId(leaderIndex);
    return output_t<int>(gid);
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId) {
    if (genreId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    Genre *genre = genresTable.find(genreId);
    if (genre == nullptr) {
        return output_t<int>(StatusType::FAILURE);
    }
    return output_t<int>(genre->getSongCount());
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId) {
    if (songId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    int *ufIndexPtr = songsTable.find(songId);
    if (ufIndexPtr == nullptr) {
        return output_t<int>(StatusType::FAILURE);
    }

    SongUnionFind::FindResult res = songsUF.findLeader(*ufIndexPtr);
    int leaderMerges = songsUF.getLeaderMerges(res.leader_uf_idx);

    int totalChanges = leaderMerges - res.diff_to_leader + 1;

    return output_t<int>(totalChanges);
}

void DSpotify::unmapLeader(int leaderIdx) {
    if (leaderIdx == -1)
        return;
    leaderToGenre.remove(leaderIdx);
}

void DSpotify::mapLeaderToGenre(int leaderIdx, int genreId) {
    if (leaderIdx == -1)
        return;
    // If some other genre already points to this leader, invalidate it
    int *otherGenreIdPtr = leaderToGenre.find(leaderIdx);
    if (otherGenreIdPtr != nullptr && *otherGenreIdPtr != genreId) {
        Genre *otherGenre = genresTable.find(*otherGenreIdPtr);
        if (otherGenre != nullptr) {
            otherGenre->setLeadingSongUFIdx(-1);
        }
        leaderToGenre.remove(leaderIdx);
    }
    leaderToGenre.insert(leaderIdx, genreId);
}
