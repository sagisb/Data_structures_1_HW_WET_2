class GenreNode {
public:
public:
    int parent;
    int rank; // This field is essential for Union-by-Rank
    int songCount;
    int genreId;
    int changes_to_parent; // This field is essential for getNumberOfGenreChanges

    GenreNode() : parent(-1), rank(0), songCount(0), genreId(-1), changes_to_parent(0) {}

    void init_new_set(int parentId, int genre_id) {
        parent = parentId;
        rank = 0;
        songCount = 0;
        genreId = genre_id;
        changes_to_parent = 0;
    }
};