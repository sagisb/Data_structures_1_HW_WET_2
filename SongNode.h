class SongNode {
public:
    int songId;
    int originalUnionFindIndexGenre;

    SongNode() : songId(0), originalUnionFindIndexGenre(0) {}

    SongNode(int song_id, int originalUnionFindIndex) :
            songId(song_id), originalUnionFindIndexGenre(originalUnionFindIndex) {}
};