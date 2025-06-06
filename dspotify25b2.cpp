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

    int newUnionFindIndex = genreUnionFind.add_set(genreId);
    if (newUnionFindIndex == -1) {
        return StatusType::ALLOCATION_ERROR;
    }

    if (!genresTable.insert(genreId, newUnionFindIndex)) {
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
    int *genre_uf_idx_ptr = genresTable.find(genreId);
    if (genre_uf_idx_ptr == nullptr) {
        return StatusType::FAILURE;
    }
    SongNode new_song_node(songId, *genre_uf_idx_ptr);
    if (!songsTable.insert(songId, new_song_node)) {
        return StatusType::ALLOCATION_ERROR;
    }

    GenreUnionFind::FindResult res = genreUnionFind.find_set(*genre_uf_idx_ptr);
    genreUnionFind.increment_song_count(res.representative_idx);

    return StatusType::SUCCESS;
}

StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3) {
    if (genreId1 <= 0 || genreId2 <= 0 || genreId3 <= 0 || genreId1 == genreId2 ||
        genreId1 == genreId3 || genreId2 == genreId3) {
        return StatusType::INVALID_INPUT;
    }
    int *unionFindIndex1_ptr = genresTable.find(genreId1);
    int *unionFindIndex2_ptr = genresTable.find(genreId2);
    if (unionFindIndex1_ptr == nullptr || unionFindIndex2_ptr == nullptr) {
        return StatusType::FAILURE;
    } //We can split it up and save on searching
    if (genresTable.find(genreId3) != nullptr) {
        return StatusType::FAILURE;
    }
    genreUnionFind.union_sets(*unionFindIndex1_ptr, *unionFindIndex2_ptr, genreId3);

    genresTable.remove(genreId1);
    genresTable.remove(genreId2);

    int new_rep_idx = genreUnionFind.find_set(*unionFindIndex1_ptr).representative_idx;
    if (!genresTable.insert(genreId3, new_rep_idx)) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

output_t<int> DSpotify::getSongGenre(int songId) {
    return 0;
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId) {
    return 0;
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId) {
    return 0;
}
