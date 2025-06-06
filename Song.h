class SongNode {
public:
    int songId;
    int genreId;
    //int changes_to_parent;

    SongNode(int songId, int genreId) : songId(songId), genreId(genreId) {}
};