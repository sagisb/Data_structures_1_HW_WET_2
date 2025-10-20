// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b2.h"

DSpotify::DSpotify() = default;

DSpotify::~DSpotify() = default;

// Helper to refresh a genre's cached leader pointer and maintain the leader to genre map.
int DSpotify::refreshGenreLeader(Genre *genre, int genreId) {
    int oldLeader = genre->getLeadingSongUFIdx();
    int newLeader = -1;
    if (oldLeader != -1) {
        SongUnionFind::FindResult chk = songsUF.findLeader(oldLeader);
        int rootIdx = chk.leader_uf_idx;
        if (songsUF.getGenreId(rootIdx) == genreId) {
            newLeader = rootIdx;
            // If root changed because of path compression, update pointer
            genre->setLeadingSongUFIdx(rootIdx);
        }
    }

    if (oldLeader != -1) {
        if (newLeader == -1) {
            // Leader became invalid, remove mapping
            unmapLeader(oldLeader);
            genre->setLeadingSongUFIdx(-1);
        } else if (newLeader != oldLeader) {
            // Root changed, update mapping
            unmapLeader(oldLeader);
            mapLeaderToGenre(newLeader, genreId);
        }
    }
    return newLeader;
}

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

    // Add the song into UnionFind with its current genre id
    int newSongUfIdx = songsUF.addSong(songId, genreId);
    if (newSongUfIdx == -1) {
        return StatusType::ALLOCATION_ERROR;
    }
    if (!songsTable.insert(songId, newSongUfIdx)) {
        return StatusType::ALLOCATION_ERROR;
    }

    int leaderIdxStored = refreshGenreLeader(genre, genreId);

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

    int leader1Idx = refreshGenreLeader(g1, genreId1);
    int leader2Idx = refreshGenreLeader(g2, genreId2);

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

    // Original genres now contain no songs.
    g1->setSongCount(0);
    g2->setSongCount(0);

    // After performing the merge, clear cached leaders and mappings of the old genres
    unmapLeader(g1->getLeadingSongUFIdx());
    unmapLeader(g2->getLeadingSongUFIdx());
    g1->setLeadingSongUFIdx(-1);
    g2->setLeadingSongUFIdx(-1);

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
