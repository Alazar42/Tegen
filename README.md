# Tegen

![Tegen Logo](https://i.ibb.co/QYtzFXx/Tegen.png)

Tegen is a C++ package manager and project management tool designed to simplify the process of managing dependencies, building, and running projects. It helps you create and manage C++ projects with ease by automating common tasks such as installation of packages, building projects using CMake, and managing project configurations.

## Features

- Initialize and configure a new C++ project with `TegenConfig.json`.
- Install and manage dependencies from GitHub repositories.
- Easily build and run C++ projects using CMake.
- Automatically organize and install dependencies in the `TegenModules` directory.

## Installation

To use Tegen, you'll need to have CMake installed on your system. You can download it from the official website: [CMake Downloads](https://cmake.org/download/).

You can then install Tegen by cloning the repository and building it manually:

```bash
git clone https://github.com/Alazar42/Tegen.git
cd Tegen
mkdir build && cd build
cmake ..
make
sudo make install
```

## Usage

### Initialize a New Project

To initialize a new project, navigate to your project directory and run:

```bash
tegen init
```

This will generate a `TegenConfig.json` file with project details, and it will set up the necessary CMake project structure, including `src`, `include`, and `CMakeLists.txt`.

### Install Dependencies

To install a dependency, run:

```bash
tegen install <repository>
```

For example:

```bash
tegen install celeris
```

You can also specify a version:

```bash
tegen install celeris main
```

### List Dependencies

To list all the dependencies in your project, run:

```bash
tegen list
```

### Build the Project

To build your project using CMake, run:

```bash
tegen build
```

### Run the Project

To run the built project, execute:

```bash
tegen run
```

## Contributing

If you'd like to contribute to Tegen, please follow these steps:

1. Fork the repository.
2. Create a new branch.
3. Make your changes and test them.
4. Submit a pull request with a detailed description of your changes.

## License

Tegen is open-source and licensed under the GNU General Public License v3.0 . See the [LICENSE](LICENSE) file for more information.
