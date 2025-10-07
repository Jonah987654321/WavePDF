# WavePDF

**WavePDF** is supposed to a lightweight **PDF viewer and editor** written in **C++**.  
Development is still in an **early stage**.  


## Features (in progress)

- Open and parse PDF files  
- Display PDF metadata and structure  
- Planned: editing, annotations, and rendering


## Requirements

Before building WavePDF, make sure you have the following installed:

- **C++17 (or newer)** compiler  
- **CMake** (≥ 3.20 recommended)  
- **Conan** (C++ package manager)


## Installation & Setup

For the initial setup, simply run:

```bash
helper/setup.sh
```

This will:
* Install dependencies via Conan
* Generate the CMake build files
* Configure the project for development

You can re-run setup at any time using: ```
helper/setup.sh --rebuild
```

Use the `--release` flag to build without debug output (currently only affects logging).

## Building & Running

To build and launch the application, run:

```bash
helper/run.sh
```

This script handles compilation and execution automatically.

## Project Structure

```
WavePDF/
├── src/            # Source code
├── helper/         # Helper scripts for build & setup
├── build/          # Generated build, make & conan files (ignored in git)
└── README.md
```
