#include "GenreNode.cpp"
#include <new>
class GenreUnionFind {
public:
    struct FindResult {
        int representative_idx;
        int path_changes;
    };

    GenreUnionFind();
    ~GenreUnionFind();

    // Returns the new index on success, or -1 on allocation failure.
    int add_set(int genreId);

    void union_sets(int unionFindIndex1, int unionFindIndex2, int newGenreId);
    FindResult find_set(int unionFindIndex);
    int get_song_count(int representative_idx) const;
    void increment_song_count(int representative_idx);
    int get_actual_genre_id(int representative_idx) const;

private:
    GenreNode* nodes;
    int capacity;
    int size;
    // Returns true on success, false on allocation failure.
    bool resize();
};
