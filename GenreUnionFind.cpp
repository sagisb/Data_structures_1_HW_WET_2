GenreUnionFind::GenreUnionFind() : capacity(6007), size(0) {
    nodes = new (std::nothrow) GenreNode[capacity];
    if (nodes == nullptr) {
        // Mark object as invalid if initial allocation fails
        capacity = 0;
    }
}

GenreUnionFind::~GenreUnionFind() {
    delete[] nodes;
}

bool GenreUnionFind::resize() {
    int new_capacity = (capacity > 0) ? capacity * 2 : 10;
    GenreNode* new_nodes = new (std::nothrow) GenreNode[new_capacity];
    if (new_nodes == nullptr) {
        return false; // Allocation failed
    }

    for (int i = 0; i < size; ++i) {
        new_nodes[i] = nodes[i];
    }
    delete[] nodes;
    nodes = new_nodes;
    capacity = new_capacity;
    return true; // Allocation succeeded
}

int GenreUnionFind::add_set(int genreId) {
    if (size == capacity) {
        if (!resize()) {
            return -1; // Signal allocation error
        }
    }
    int new_idx = size;
    nodes[new_idx].init_new_set(new_idx, genreId);
    size++;
    return new_idx;
}

GenreUnionFind::FindResult GenreUnionFind::find_set(int unionFindIndex) {
    if (nodes[unionFindIndex].parent == unionFindIndex) {
        return {unionFindIndex, 0};
    }

    FindResult parent_result = find_set(nodes[unionFindIndex].parent);

    nodes[unionFindIndex].changes_to_parent += parent_result.path_changes;
    nodes[unionFindIndex].parent = parent_result.representative_idx;

    return {parent_result.representative_idx, nodes[unionFindIndex].changes_to_parent};
}

void GenreUnionFind::union_sets(int unionFindIndex1, int unionFindIndex2, int newGenreId) {
    int rep1_idx = find_set(unionFindIndex1).representative_idx;
    int rep2_idx = find_set(unionFindIndex2).representative_idx;

    if (rep1_idx == rep2_idx) {
        nodes[rep1_idx].actual_genreId = newGenreId;
        return;
    }

    if (nodes[rep1_idx].rank < nodes[rep2_idx].rank) {
        nodes[rep1_idx].parent = rep2_idx;
        nodes[rep2_idx].song_count += nodes[rep1_idx].song_count;
        nodes[rep2_idx].actual_genreId = newGenreId;
        nodes[rep1_idx].changes_to_parent = 1;
    } else if (nodes[rep1_idx].rank > nodes[rep2_idx].rank) {
        nodes[rep2_idx].parent = rep1_idx;
        nodes[rep1_idx].song_count += nodes[rep2_idx].song_count;
        nodes[rep1_idx].actual_genreId = newGenreId;
        nodes[rep2_idx].changes_to_parent = 1;
    } else {
        nodes[rep2_idx].parent = rep1_idx;
        nodes[rep1_idx].song_count += nodes[rep2_idx].song_count;
        nodes[rep1_idx].actual_genreId = newGenreId;
        nodes[rep2_idx].changes_to_parent = 1;
        nodes[rep1_idx].rank++;
    }
}

int GenreUnionFind::get_song_count(int representative_idx) const {
    return nodes[representative_idx].song_count;
}

void GenreUnionFind::increment_song_count(int representative_idx) {
    nodes[representative_idx].song_count++;
}

int GenreUnionFind::get_actual_genreId(int representative_idx) const {
    return nodes[representative_idx].actual_genreId;
}
