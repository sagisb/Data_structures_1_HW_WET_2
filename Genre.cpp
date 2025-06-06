class GenreNode {
public:
    int parent;
    int songCount;
    int genreId;
    //int changes_to_parent;

    GenreNode() : parent(-1), songCount(0), genreId(-1) {}

    void init_new_set(int parentId, int genre_id) {
        parent = parentId;
        songCount = 0;
        genreId = genre_id;
        //changes_to_parent = 0;
    }
};