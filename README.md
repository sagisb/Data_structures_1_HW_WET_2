# DSpotify – Genre/Song Management System

A data structures implementation of a music service back‑end for managing songs and genres using a Union‑Find structure and custom hash tables.

## Overview

This project implements a system called "DSpotify" that allows you to:

-   Create and manage genres
-   Add songs into existing genres
-   Merge two genres into a new genre
-   Query a song’s current genre
-   Get the number of songs in a genre
-   Track how many times a song’s genre has changed

To support fast merges and queries at scale, songs are grouped with a Union‑Find (disjoint‑set) structure; compact per‑genre metadata tracks counts and a cached component leader; and custom hash tables provide O(1) average id lookups.

## Assignment Documentation

-   **Assignment Specification**: `DS_wet2_Spring_2025.pdf`
-   **Dry Submission**: `dry_submission.pdf`

## Project Structure

```
wet_2/
├── dspotify25b2.h               # DSpotify API
├── dspotify25b2.cpp             # DSpotify implementation
├── SongUnionFind.h/.cpp         # Union-Find over songs with potentials
├── SongNode.h                   # Internal UF node
├── HashTable.h                  # Separate-chaining hash table
├── Genre.h                      # Per-genre metadata (count, leader uf index)
├── DS_wet2_Spring_2025.pdf      # Assignment specification
└── dry_submission.pdf           # Dry submission / design notes
```

## Key Features

### Core Operations

-   `addGenre(genreId)`: Create an empty genre
-   `addSong(songId, genreId)`: Add a song into an existing genre
-   `mergeGenres(g1, g2, g3)`: Merge two genres into a new genre id
-   `getSongGenre(songId)`: Current genre of a song
-   `getNumberOfSongsByGenre(genreId)`: Song count in a genre
-   `getNumberOfGenreChanges(songId)`: Number of genre changes a song experienced

## Data Structures Implemented

### 1. Union‑Find over Songs

-   Path compression + union by size for efficient merges.
-   Potential/difference trick for tracking per‑song genre‑change offsets.
-   Root stores the component’s current `genreId` and a `merges` counter; non‑roots store a diff to parent.

How it works in this project:

-   When two song components are united (via adding a song to an existing genre or merging genres), we attach the smaller root under the larger root and adjust the child root’s stored diff so existing paths keep the correct potential.
-   After `mergeGenres`, we increment the resulting root’s `merges` counter exactly once to represent “one more genre change for all songs in this component”.
-   `findLeader(idx)` returns both the root index and the cumulative diff from `idx` to the root, while compressing the path.
-   The number of genre changes for a song is computed as:

```
changes(song) = leader.merges - diff_to_leader + 1
```

This gives amortized O(log\* n) time for unions/finds with precise per‑song change counts.

### 2. Hash Table (Separate Chaining)

-   Custom separate‑chaining table maps: `songId → ufIndex`, `genreId → Genre`, and `leaderUfIdx → genreId`.
-   Node structure: `{ key, value, next }` singly‑linked lists per bucket.
-   Resizes at ~75% load by doubling capacity and re‑inserting nodes.
-   Average O(1) `insert/find/remove`; `IntHasher` provides a fast 32‑bit mix for integer keys.

### 3. Genre Metadata

-   Stores `genreId`, `songCount`, and a cached `leadingSongUFIdx` (−1 if empty).
-   The cached leader lets us union new songs directly into the correct component without scanning.
-   A `leader→genre` mapping ensures a leader is associated with a single genre; when merges happen or roots change due to path compression, we refresh/update this cache so it stays valid.

## Time Complexities

| Operation                 | Time Complexity      | Notes                        |
| ------------------------- | -------------------- | ---------------------------- |
| `DSpotify()`              | O(1)                 | Initialize structures        |
| `~DSpotify()`             | O(n + m) worst‑case  | Release arrays/lists         |
| `addGenre`                | O(1) average         | Hash insert                  |
| `addSong`                 | O(log\* n) amortized | UF find/union + hash ops     |
| `mergeGenres`             | O(log\* n) amortized | At most one union + hash ops |
| `getSongGenre`            | O(log\* n) amortized | UF find leader               |
| `getNumberOfSongsByGenre` | O(1)                 | Read stored count            |
| `getNumberOfGenreChanges` | O(log\* n) amortized | UF find + arithmetic         |

Where n is the number of songs in the system and m the number of UF links.

## Implementation Notes

-   Each merge that forms a new genre increments the resulting leader’s merges counter once; all songs in that component implicitly gain one genre change.
-   The query for a song’s number of changes uses: `leaderMerges - diff_to_leader + 1`.
-   Input validation and exact return codes follow the assignment spec.

## Course Information

-   **Course**: 234218 Data Structures 1
-   **Semester**: 2025B (Spring)
-   **Exercise**: Wet Exercise #2
-   **Institution**: Technion – Israel Institute of Technology
