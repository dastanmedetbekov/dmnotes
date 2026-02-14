# dmnotes

A minimalist CLI note-taking utility written in C. Designed for speed, privacy, and adherence to the Linux XDG Base Directory Specification.

## Installation

```bash
gcc -Wall dmnotes.c -o dmnotes
sudo mv dmnotes /usr/local/bin/
sudo chmod +x /usr/local/bin/dmnotes

```

## Usage

* **Add Note**: `dmnotes "Your thought"` (or run `dmnotes` for interactive input).
* **Read**: `dmnotes -r [n]` — View the last `n` notes (default: 10).
* **Search**: `dmnotes -s "keyword"` — Search notes using `grep`.
* **Delete**: `dmnotes -d "keyword"` — Remove all lines containing the keyword.
* **Reset**: `dmnotes --reset` — Permanently clear all notes.

## Storage

* **File Path**: `~/.local/share/dmnotes/notes.txt`
