#ifndef PACKAGE_MANAGER_HPP
#define PACKAGE_MANAGER_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <filesystem> // C++17 for filesystem operations
#include "json.hpp"

using json = nlohmann::json;

class PackageManager
{
private:
    std::string configFileName = "TegenConfig.json";

    // Helper function to get the current working directory
    std::string getCurrentDirectory()
    {
        return std::filesystem::current_path().string();
    }

    // Helper function to check if TegenConfig.json exists in the current directory
    bool configExists()
    {
        std::filesystem::path filePath = std::filesystem::current_path() / configFileName;
        return std::filesystem::exists(filePath);
    }

    // Helper function to load the JSON configuration
    json loadConfig()
    {
        std::filesystem::path filePath = std::filesystem::current_path() / configFileName;
        std::ifstream file(filePath);
        if (!file.good())
            return json({});
        json config;
        file >> config;
        return config;
    }

    // Helper function to save the JSON configuration
    void saveConfig(const json &config)
    {
        std::filesystem::path filePath = std::filesystem::current_path() / configFileName;
        std::ofstream file(filePath);
        file << config.dump(4); // Pretty print with 4 spaces
    }

    // Helper function to prompt for user input
    std::string prompt(const std::string &message, const std::string &defaultValue = "")
    {
        std::string input;
        std::cout << message << (defaultValue.empty() ? ": " : " [" + defaultValue + "]: ");
        std::getline(std::cin, input);
        return input.empty() ? defaultValue : input;
    }

    // Helper function to execute a system command and handle paths with spaces
    void executeCommand(const std::string &command)
    {
        int result = std::system(command.c_str());
        if (result != 0)
        {
            throw std::runtime_error("Command failed: " + command);
        }
    }

public:
    // Initialize a new TegenConfig.json file in the current directory
    void init()
    {
        if (configExists())
        {
            std::cout << "TegenConfig.json already exists in the current directory." << std::endl;
            return;
        }

        json config;
        std::cout << "Initializing TegenConfig.json and CMake project..." << std::endl;

        // Prompt for project details
        config["name"] = prompt("Enter project name", "my-package");
        config["version"] = prompt("Enter project version", "1.0.0");
        config["author"] = prompt("Enter author name", "Anonymous");
        config["license"] = prompt("Enter license type", "MIT");
        config["description"] = prompt("Enter project description", "A C++ project");
        config["dependencies"] = json::object(); // Initialize dependencies as an empty object

        // Save the TegenConfig.json
        saveConfig(config);
        std::cout << "Initialized TegenConfig.json in " << getCurrentDirectory() << " with the following details:" << std::endl;
        std::cout << config.dump(4) << std::endl;

        // Create CMake project structure

        // Create 'src' and 'include' directories
        std::filesystem::create_directory("src");
        std::filesystem::create_directory("include");

        // Create a basic main.cpp in the 'src' folder
        std::ofstream mainCpp("src/main.cpp");
        mainCpp << "#include <iostream>" << std::endl;
        mainCpp << std::endl;
        mainCpp << "int main() {" << std::endl;
        mainCpp << "    std::cout << \"Hello, World!\" << std::endl;" << std::endl;
        mainCpp << "    return 0;" << std::endl;
        mainCpp << "}" << std::endl;
        mainCpp.close();

        // Create a basic CMakeLists.txt
        std::ofstream cmakeLists("CMakeLists.txt");
        cmakeLists << "cmake_minimum_required(VERSION 3.10)" << std::endl;
        cmakeLists << "project(" << config["name"].get<std::string>() << " VERSION " << config["version"].get<std::string>() << ")" << std::endl;
        cmakeLists << std::endl;
        cmakeLists << "set(CMAKE_CXX_STANDARD 17)" << std::endl;
        cmakeLists << std::endl;
        cmakeLists << "include_directories(include)" << std::endl;
        cmakeLists << "add_executable(" << config["name"].get<std::string>() << " src/main.cpp)" << std::endl;
        cmakeLists.close();

        std::cout << "CMake project structure created:" << std::endl;
        std::cout << "- src/main.cpp" << std::endl;
        std::cout << "- include/ (empty for now)" << std::endl;
        std::cout << "- CMakeLists.txt" << std::endl;

        // Inform user that CMake needs to be installed
        std::cout << std::endl;
        std::cout << "Note: You need to have CMake installed on your system to build and install the project." << std::endl;
        std::cout << "You can download CMake from https://cmake.org/download/" << std::endl;
        std::cout << std::endl;

        // Instructions for building and installing
        std::cout << "To build and install your project, follow these steps:" << std::endl;
        std::cout << "1. Create a build directory and navigate to it:" << std::endl;
        std::cout << "   mkdir build && cd build" << std::endl;
        std::cout << "2. Run CMake to configure the project:" << std::endl;
        std::cout << "   cmake .." << std::endl;
        std::cout << "3. Build the project using the following command:" << std::endl;
        std::cout << "   cmake --build . --target install" << std::endl;
        std::cout << "4. After the build completes, run your project from the build directory:" << std::endl;
        std::cout << "   ./<project-name>" << std::endl;
        std::cout << std::endl;

        std::cout << "Your project is now ready for building and installation!" << std::endl;
    }

    // Install a new package from GitHub
    void install(const std::string &repository, const std::string &version = "")
    {
        if (!configExists())
        {
            std::cerr << "TegenConfig.json not found in the current directory. Run 'init' first." << std::endl;
            return;
        }

        json config = loadConfig();
        std::filesystem::path modulesDir = std::filesystem::current_path() / "TegenModules";
        std::filesystem::path includeDir = modulesDir / "include";
        std::filesystem::path libDir = modulesDir / "lib";

        // Ensure TegenModules directories exist
        if (!std::filesystem::exists(modulesDir))
            std::filesystem::create_directory(modulesDir);
        if (!std::filesystem::exists(includeDir))
            std::filesystem::create_directory(includeDir);
        if (!std::filesystem::exists(libDir))
            std::filesystem::create_directory(libDir);

        std::string resolvedVersion = version.empty() ? "main" : version;

        if (config["dependencies"].contains(repository))
        {
            std::cout << "Repository " << repository << " is already installed with version "
                      << config["dependencies"][repository] << "." << std::endl;
            return;
        }

        try
        {
            std::cout << "Installing package: " << repository << " (version: " << resolvedVersion << ")..." << std::endl;

            // Update the URL to point to the organization "TegenPackages"
            std::string url = "https://github.com/TegenPackages/" + repository + "/archive/refs/heads/" + resolvedVersion + ".zip";
            std::filesystem::path zipFilePath = modulesDir / (repository + ".zip");
            std::filesystem::path extractDir = modulesDir / repository;

            // Download the repository as a zip file
            std::string downloadCommand = "curl -L -o \"" + zipFilePath.string() + "\" " + url;
            executeCommand(downloadCommand);

            // Unzip the repository
            std::string unzipCommand = "unzip -o \"" + zipFilePath.string() + "\" -d \"" + modulesDir.string() + "\"";
            executeCommand(unzipCommand);

            // Move headers to TegenModules/include
            auto sourceIncludeDir = extractDir / (repository + "-" + resolvedVersion) / "include";
            if (std::filesystem::exists(sourceIncludeDir))
            {
                for (const auto &file : std::filesystem::recursive_directory_iterator(sourceIncludeDir))
                {
                    if (file.is_regular_file())
                    {
                        auto relativePath = std::filesystem::relative(file.path(), sourceIncludeDir);
                        auto targetPath = includeDir / relativePath;
                        std::filesystem::create_directories(targetPath.parent_path());
                        std::filesystem::copy_file(file.path(), targetPath, std::filesystem::copy_options::overwrite_existing);
                    }
                }
            }

            // Move libraries to TegenModules/lib
            auto sourceLibDir = extractDir / (repository + "-" + resolvedVersion) / "lib";
            if (std::filesystem::exists(sourceLibDir))
            {
                for (const auto &file : std::filesystem::directory_iterator(sourceLibDir))
                {
                    if (file.is_regular_file())
                    {
                        auto targetPath = libDir / file.path().filename();
                        std::filesystem::copy_file(file.path(), targetPath, std::filesystem::copy_options::overwrite_existing);
                    }
                }
            }

            // Add to TegenConfig.json
            config["dependencies"][repository] = resolvedVersion;
            saveConfig(config);

            // Clean up zip file
            std::filesystem::remove(zipFilePath);

            std::cout << "Package " << repository << " successfully installed." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to install package: " << e.what() << std::endl;
        }
    }

    // List all dependencies
    void listDependencies()
    {
        if (!configExists())
        {
            std::cerr << "TegenConfig.json not found in the current directory." << std::endl;
            return;
        }

        json config = loadConfig();
        std::cout << "Dependencies:" << std::endl;

        for (const auto &[key, value] : config["dependencies"].items())
        {
            std::cout << "  - " << key << ": " << value << std::endl;
        }
    }

    // Build the project using CMake
    void build()
    {
        if (!configExists())
        {
            std::cerr << "TegenConfig.json not found in the current directory. Run 'init' first." << std::endl;
            return;
        }

        std::cout << "Building the project..." << std::endl;

        // Create build directory if it doesn't exist
        std::filesystem::create_directory("build");

        // Run cmake to configure the project
        executeCommand("cmake -S . -B build");

        // Run make to build the project
        executeCommand("cmake --build build");

        std::cout << "Build completed successfully. The project is located in the 'build/' directory." << std::endl;
    }

    // Run the built project from the build directory
    void run()
    {
        if (!configExists())
        {
            std::cerr << "TegenConfig.json not found in the current directory. Run 'init' first." << std::endl;
            return;
        }

        json config = loadConfig();
        std::string projectName = config["name"].get<std::string>();

        // Run the built project from the 'build' directory
        std::cout << "Running the project..." << std::endl;
        executeCommand("./build/" + projectName);
    }
};

#endif
