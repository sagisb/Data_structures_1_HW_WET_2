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
    return StatusType::FAILURE;
}

StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3) {
    return StatusType::FAILURE;
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
