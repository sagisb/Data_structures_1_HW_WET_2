## DSpotify (Wet Exercise 2) – System Overview and Data Structures

This repository implements the DSpotify system for the Data Structures course (234218) Wet Exercise #2, Spring 2025B. The system manages songs and genres while supporting operations such as adding genres/songs, merging genres, querying a song's current genre, counting songs in a genre, and tracking how many genre changes a song has experienced.

The core implementation is in:

-   `dspotify25b2.h` / `dspotify25b2.cpp` – the public API and system logic
-   `SongUnionFind.h` / `SongUnionFind.cpp` – a specialized Disjoint Set Union (Union‑Find) structure for songs
-   `HashTable.h` – a minimal separate‑chaining hash table
-   `SongNode.h` – the internal node structure used by the Union‑Find
-   `Genre.h` – per‑genre metadata tracked by the system

### References

-   Assignment PDF: `DS_wet2_Spring_2025.pdf`
-   Dry submission document: `dry_submission.pdf`

### What is DSpotify? (Context)

DSpotify is a simplified music service backend that manages songs and genres over time. New genres can be created; songs are added to existing genres; and two existing genres can be merged into a new genre. After merges, every song that belonged to any merged genre is considered to have experienced one additional “genre change.”

Readers should think of each genre as a dynamic group of songs. The system needs to answer three frequent questions efficiently:

-   What genre does a given song currently belong to?
-   How many songs are currently in a given genre?
-   How many times has a song’s genre changed throughout the history of merges?

To support fast merges and queries at scale, the implementation groups songs with a Union‑Find (disjoint‑set) structure and maintains compact per‑genre metadata (counts and a cached component leader). Hash tables provide O(1) average lookups from external ids to internal structures.

Typical usage flow:

1. Create genres with `addGenre`.
2. Add songs with `addSong` (each song starts in an existing genre).
3. Merge two existing genres into a new genre id with `mergeGenres`. All affected songs now belong to the new genre and their “genre changes” counter increases by exactly one.
4. Query with `getSongGenre`, `getNumberOfSongsByGenre`, and `getNumberOfGenreChanges`.

This repo contains one possible correct solution that satisfies the assignment’s API and complexity requirements.

### High‑Level Design

At a high level, each song belongs to exactly one genre component represented by a Union‑Find set whose root is considered the “leader”. Merging genres corresponds to unioning two sets of songs and re‑labeling the resulting component’s genre id. The system maintains:

-   A Union‑Find over all songs to track component leaders, sizes, and an aggregated “genre changes/merges” counter.
-   Hash tables to map:
    -   `songId → ufIndex` (`songsTable`)
    -   `genreId → Genre` (`genresTable`)
    -   `leaderUfIndex → genreId` (`leaderToGenre`) to ensure a genre’s cached leader is unique and up‑to‑date.

Each `Genre` also caches the UF index of its current leader (`leadingSongUFIdx`) and the number of songs currently in the genre (`songCount`). A small helper `refreshGenreLeader` validates/repairs the cached leader using path compression and the `leaderToGenre` mapping.

### Public API (in `DSpotify`)

-   `StatusType addGenre(int genreId)`

    -   Inserts a new empty genre. Fails if `genreId` already exists or invalid.

-   `StatusType addSong(int songId, int genreId)`

    -   Creates a new song in the specified genre as a singleton UF set then unions it with the genre’s existing leader if present. Updates `songsTable`, `genresTable`, and `leaderToGenre`. Increments the genre’s `songCount`.

-   `StatusType mergeGenres(int genreId1, int genreId2, int newGenreId)`

    -   Merges all songs from `genreId1` and `genreId2` into a new genre labeled `newGenreId`.
    -   If both old genres have leaders, their UF components are unioned. The resulting leader’s “genre changes” counter is incremented exactly once to reflect one global change for all songs in the merged component.
    -   The new `Genre`’s `leadingSongUFIdx` and `songCount` are set; the old genres’ leader caches are cleared and their `songCount` set to 0.

-   `output_t<int> getSongGenre(int songId)`

    -   Returns the current genre id for the song by finding its UF leader and reading the leader’s genre id.

-   `output_t<int> getNumberOfSongsByGenre(int genreId)`

    -   Returns the tracked `songCount` stored in the `Genre` object.

-   `output_t<int> getNumberOfGenreChanges(int songId)`
    -   Returns the number of genre changes the song has experienced. This is computed using a potential‑based scheme in the Union‑Find (see below). In code: `leaderMerges - diff_to_leader + 1`.

### Data Structures and How They’re Used

#### 1) Union‑Find over Songs (`SongUnionFind` + `SongNode`)

Purpose:

-   Track connected components of songs (per genre) and support efficient merges when genres combine.
-   Maintain a per‑component counter of how many genre merges/changes have been applied to that component.

Key features:

-   Path compression and union by size via `childrenCount` on the root for near‑amortized O(α(n)) finds/unions.
-   A potential/difference trick to account for genre changes per node:
    -   Each root stores `genreChanges` = total merges applied to its component.
    -   Each non‑root node stores the difference `(parent.genreChanges - node.genreChanges)`.
    -   During `find` with path compression, distances accumulate and are stored on the node, enabling O(1) retrieval of its total difference to the root.

How genre change counting works:

-   After a merge of two genres into a new one, the system calls `incrementLeaderChanges(final_leader)` exactly once.
-   For a song node queried later, `findLeader` returns the leader and `diff_to_leader` (the accumulated difference stored during compression). The total number of changes for that song is computed as:

```
totalChanges = leaderMerges - diff_to_leader + 1
```

The `+1` accounts for the initial assignment (considered the first “state”).

#### 2) Hash Table (`HashTable<K, V, H = IntHasher>`) – Separate Chaining

Purpose:

-   Provide O(1) average mapping for ids used heavily in the system.

Usage in `DSpotify`:

-   `songsTable: HashTable<int,int>` maps `songId → ufIndex`.
-   `genresTable: HashTable<int,Genre>` maps `genreId → Genre` metadata.
-   `leaderToGenre: HashTable<int,int>` maps `leaderUfIdx → genreId` to ensure a single genre points to a given leader and to help validate/refresh cached leaders.

Implementation highlights:

-   Separate chaining with singly linked lists per bucket.
-   Resizes at 75% load to roughly double capacity (initial capacity 6007).
-   `insert` fails if key exists.
-   `find` returns pointer to value for in‑place updates.
-   `remove` deletes a node from the chain and updates size.
-   `IntHasher` is a simple 32‑bit integer hash mix.

#### 3) Genre Metadata (`Genre`)

Fields:

-   `genreId` – the public identifier
-   `songCount` – number of songs currently attributed to the genre
-   `leadingSongUFIdx` – cached UF index of the component leader for this genre (or −1 if none)

Behavior:

-   When the first song is added to a genre, its UF node becomes the leader and is stored in `leadingSongUFIdx` and `leaderToGenre`.
-   When more songs are added, they are unioned into the cached leader’s component.
-   On merges, the new genre’s leader is set to the resulting UF root; the old genres’ leaders are cleared and unmapped.

### Operation Flow

-   Adding a song:

    1. Create a singleton UF node with the genre id.
    2. If the genre already has a leader, union the new node into that component and update the cached leader to the new root (post compression/union).
    3. Update `songCount` and `leaderToGenre`.

-   Merging genres into a new genre:
    1. Create the new `Genre` record.
    2. Union the two leader components if they exist.
    3. Increment the new root’s `genreChanges` once.
    4. Set the root’s `genreId` to `newGenreId` and point the new genre to it.
    5. Sum counts into the new genre; clear counts and leaders for the old genres and unmap old leader entries.

Invariants:

-   Each UF root has a valid `genreId`; non‑roots’ `genreId` is undefined and unused.
-   `leaderToGenre` never maps one leader to multiple genres. On conflict, the old genre’s cached leader is invalidated.
-   `Genre.songCount` always equals the total size of its component (for non‑empty genres). After merges, only the new genre has the combined count; the old ones are set to 0.

### Time and Space Complexity (per assignment spec)

-   Constructor: `DSpotify()` – O(1).
-   Destructor: `~DSpotify()` – O(n + m) in the worst case (n songs, m unions/links to release).
-   Hash table operations: average O(1) for `insert/find/remove`.
-   Union‑Find operations: amortized O(log\* n) with path compression and union by size.
-   `addGenre`: amortized O(1) on average input.
-   `addSong`: amortized O(log\* n) on average input (UF find/union) + O(1) hash ops.
-   `mergeGenres`: amortized O(log\* n) on average input (at most one union) + O(1) hash ops.
-   `getSongGenre`: amortized O(log\* n) (find leader).
-   `getNumberOfSongsByGenre`: average O(1).
-   `getNumberOfGenreChanges`: amortized O(log\* n) (find leader) + O(1) arithmetic.
-   Space: O(#songs) for the UF array and O(#songs + #genres) across hash tables.

### Building and Notes

-   This code expects the course’s `wet2util.h` header for `StatusType` and `output_t<T>`. Ensure it is available on your include path when compiling.
-   The project uses only standard C++ (new/delete). There is no STL dependency inside the provided structures.
-   Example compile line (adjust paths and files as needed):

```bash
g++ -std=c++17 -O2 -Wall dspotify25b2.cpp SongUnionFind.cpp -o dspotify
```

### File Map

-   `dspotify25b2.h/.cpp` – `DSpotify` API, orchestrates data structures and enforces invariants
-   `SongUnionFind.h/.cpp` – UF with path compression, union by size, and potential tracking
-   `SongNode.h` – node backing the UF array
-   `HashTable.h` – minimal separate‑chaining hash table with resize
-   `Genre.h` – simple metadata holder for genres
-   `DS_wet2_Spring_2025.pdf`, `dry_submission.pdf` – exercise description and dry submission (reference)

### Quick Examples (pseudo‑usage)

```cpp
DSpotify ds;
ds.addGenre(10);
ds.addSong(1, 10);
ds.addSong(2, 10);

ds.addGenre(20);
ds.addSong(3, 20);

// Merge genres 10 and 20 into new genre 30
ds.mergeGenres(10, 20, 30);

int current = ds.getSongGenre(1).ans(); // 30
int count30 = ds.getNumberOfSongsByGenre(30).ans(); // 3
int changes1 = ds.getNumberOfGenreChanges(1).ans(); // 2 (initial + one merge)
```
