#ifndef PACKAGE_MANAGER_HPP
#define PACKAGE_MANAGER_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <filesystem> // C++17 for filesystem operations
#include <chrono>
#include <system_error>
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

        // Instructions for building and running with Tegen
        std::cout << "To build and run your project using Tegen, follow these steps:" << std::endl;
        std::cout << "1. Install required dependencies:" << std::endl;
        std::cout << "   tegen install <package-name>" << std::endl;
        std::cout << "2. Build your project using Tegen:" << std::endl;
        std::cout << "   tegen build" << std::endl;
        std::cout << "3. After the build completes, run your project:" << std::endl;
        std::cout << "   tegen run" << std::endl;
        std::cout << std::endl;
        std::cout << "Your project is now ready to build and run with Tegen!" << std::endl;
    }

    // Install a new package from GitHub
    void install(const std::string &repository, const std::string &version = "")
    {
        if (!configExists())
        {
            std::cerr << "TegenConfig.json not found. Run 'init' first." << std::endl;
            return;
        }

        json config = loadConfig();
        std::filesystem::path projectDir = std::filesystem::current_path();
        std::filesystem::path modulesDir = projectDir / "TegenModules";
        std::filesystem::path projectInclude = projectDir / "include";
        std::filesystem::path projectLib = projectDir / "lib";

        std::filesystem::create_directories(modulesDir);
        std::filesystem::create_directories(projectInclude);
        std::filesystem::create_directories(projectLib);

        std::string osBranch;
#ifdef _WIN32
        osBranch = "WindowsBranch";
#elif __APPLE__
        osBranch = "MacBranch";
#else
        osBranch = "LinuxBranch";
#endif

        std::string resolvedVersion = version.empty() ? osBranch : version;

        if (config["dependencies"].contains(repository))
        {
            std::cout << "Repository " << repository << " is already installed with version "
                      << config["dependencies"][repository] << "." << std::endl;
            return;
        }

        try
        {
            std::cout << "Installing package: " << repository << " (branch/version: " << resolvedVersion << ")..." << std::endl;

            std::filesystem::path repoDir = modulesDir / repository;

            // Clone or update repo
            if (!std::filesystem::exists(repoDir))
            {
                executeCommand("git clone -b " + resolvedVersion + " https://github.com/TegenPackages/" + repository + ".git \"" + repoDir.string() + "\"");
            }
            else
            {
                std::cout << "Repository already cloned. Fetching latest changes..." << std::endl;
                executeCommand("git -C \"" + repoDir.string() + "\" fetch");
                executeCommand("git -C \"" + repoDir.string() + "\" checkout " + resolvedVersion);
                executeCommand("git -C \"" + repoDir.string() + "\" pull");
            }

            // -------------------- COPY HEADERS --------------------
            auto sourceIncludeDir = repoDir / "include";
            if (std::filesystem::exists(sourceIncludeDir))
            {
                std::cout << "Copying header files..." << std::endl;
                std::vector<std::filesystem::path> headerFiles;
                for (const auto &file : std::filesystem::recursive_directory_iterator(sourceIncludeDir))
                    if (file.is_regular_file())
                        headerFiles.push_back(file.path());

                size_t total = headerFiles.size();
                size_t count = 0;
                for (const auto &file : headerFiles)
                {
                    auto relative = std::filesystem::relative(file, sourceIncludeDir);
                    auto target = projectInclude / relative;
                    std::filesystem::create_directories(target.parent_path());
                    std::filesystem::copy_file(file, target, std::filesystem::copy_options::overwrite_existing);
                    count++;
                    int percent = int((count * 100) / total);
                    std::cout << "\rHeaders [" << std::string(percent / 2, '#') << std::string(50 - percent / 2, ' ')
                              << "] " << percent << "% (" << count << "/" << total << ")" << std::flush;
                }
                std::cout << std::endl;
            }

            // -------------------- COPY LIBS --------------------
            auto libDir = repoDir / "lib";
            while (std::filesystem::exists(libDir) && std::filesystem::is_directory(libDir) && std::distance(std::filesystem::directory_iterator(libDir), std::filesystem::directory_iterator{}) == 1)
            {
                libDir = *std::filesystem::directory_iterator(libDir);
            }

            if (std::filesystem::exists(libDir))
            {
                std::cout << "Copying library files..." << std::endl;
                std::vector<std::filesystem::path> libFiles;
                for (const auto &file : std::filesystem::recursive_directory_iterator(libDir))
                    if (file.is_regular_file() && (file.path().extension() == ".a" || file.path().extension() == ".lib"))
                        libFiles.push_back(file.path());

                size_t total = libFiles.size();
                size_t count = 0;
                for (const auto &file : libFiles)
                {
                    auto target = projectLib / file.filename();
                    std::filesystem::copy_file(file, target, std::filesystem::copy_options::overwrite_existing);
                    count++;
                    int percent = int((count * 100) / total);
                    std::cout << "\rLibraries [" << std::string(percent / 2, '#') << std::string(50 - percent / 2, ' ')
                              << "] " << percent << "% (" << count << "/" << total << ")" << std::flush;
                }
                std::cout << std::endl;
            }

            // -------------------- UPDATE CMakeLists.txt --------------------
            std::filesystem::path cmakeFile = projectDir / "CMakeLists.txt";
            std::ofstream cmakeOut(cmakeFile, std::ios::app);
            cmakeOut << "\n# Added by Tegen for " << repository << "\n";
            cmakeOut << "include_directories(include)\n";

            // Link copied libs
            for (const auto &libFile : std::filesystem::directory_iterator(projectLib))
            {
                if (libFile.path().extension() == ".lib" || libFile.path().extension() == ".a")
                {
                    cmakeOut << "target_link_libraries(${PROJECT_NAME} PRIVATE \"" << libFile.path().string() << "\")\n";
                }
            }

#ifdef _WIN32
            cmakeOut << "target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 mswsock advapi32)\n";
#endif

            cmakeOut.close();

            // -------------------- UPDATE CONFIG --------------------
            config["dependencies"][repository] = resolvedVersion;
            saveConfig(config);

            // -------------------- CLEAN UP --------------------
            std::error_code ec;
            removeFolderRecursively(modulesDir);
            if (ec)
                std::cerr << "Warning: could not remove temporary folder " << modulesDir << ": " << ec.message() << std::endl;

            std::cout << "Package " << repository << " successfully installed, integrated, and cleaned up!" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to install package: " << e.what() << std::endl;
        }
    }

    void removeFolderRecursively(const std::filesystem::path &folder)
    {
        std::error_code ec;

#ifdef _WIN32
        for (auto &entry : std::filesystem::recursive_directory_iterator(folder, std::filesystem::directory_options::skip_permission_denied))
        {
            std::filesystem::permissions(entry, std::filesystem::perms::owner_all, std::filesystem::perm_options::add, ec);
            // ignore ec, just try to make it writable
        }
#endif

        std::filesystem::remove_all(folder, ec);
        if (ec)
        {
            std::cerr << "Warning: Failed to completely remove " << folder << ": " << ec.message() << std::endl;
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

    void run()
    {
        if (!configExists())
        {
            // Red for errors
#ifdef _WIN32
            std::cerr << "\x1B[31m"; // Red
#endif
            std::cerr << "TegenConfig.json not found in the current directory. Run 'init' first." << std::endl;
#ifdef _WIN32
            std::cerr << "\x1B[0m"; // Reset
#endif
            return;
        }

        json config = loadConfig();
        std::string projectName = config["name"].get<std::string>();

        // Yellow for info
#ifdef _WIN32
        std::cout << "\x1B[33m"; // Yellow
#endif
        std::cout << "Running the project..." << std::endl;
#ifdef _WIN32
        std::cout << "\x1B[0m"; // Reset
#endif

        // Build cross-platform executable path
        std::filesystem::path buildPath = std::filesystem::current_path() / "build" / projectName;
#ifdef _WIN32
        buildPath += ".exe";
#endif

        if (!std::filesystem::exists(buildPath))
        {
#ifdef _WIN32
            std::cerr << "\x1B[31m"; // Red
#endif
            std::cerr << "Executable not found: " << buildPath << std::endl;
            std::cerr << "Make sure the project is built before running." << std::endl;
#ifdef _WIN32
            std::cerr << "\x1B[0m"; // Reset
#endif
            return;
        }

        std::string command = "\"" + buildPath.string() + "\"";

        auto start = std::chrono::high_resolution_clock::now();
        int result = std::system(command.c_str());
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        if (result != 0)
        {
#ifdef _WIN32
            std::cerr << "\x1B[31m"; // Red
#endif
            std::cerr << "Failed to run the project. Make sure it's built correctly." << std::endl;
#ifdef _WIN32
            std::cerr << "\x1B[0m"; // Reset
#endif
        }
        else
        {
#ifdef _WIN32
            std::cout << "\x1B[32m"; // Green
#endif
            std::cout << "Project finished successfully in " << duration << " ms." << std::endl;
#ifdef _WIN32
            std::cout << "\x1B[0m"; // Reset
#endif
        }
    }
};

#endif
