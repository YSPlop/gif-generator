// main.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include "gif_writer.h"

namespace fs = std::filesystem;

#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

void captureFromWebcam(std::vector<cv::Mat>& frames) {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << RED << "Cannot open webcam." << RESET << std::endl;
        return;
    }

    std::cout << YELLOW << "Press SPACE to capture a frame, ESC to stop." << RESET << std::endl;
    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        cv::imshow("Webcam", frame);
        char key = (char)cv::waitKey(30);
        if (key == 27) break; // ESC
        else if (key == 32) { // SPACE
            frames.push_back(frame.clone());
            std::cout << GREEN << "Captured frame " << frames.size() << RESET << std::endl;
        }
    }
    cap.release();
    cv::destroyAllWindows();
}

void loadImagesFromFolder(const std::string& folder, std::vector<cv::Mat>& frames) {
    std::string folderPath = folder;
    
    // Handle relative paths
    if (!fs::path(folder).is_absolute()) {
        // Get the current working directory and append the relative path
        folderPath = fs::current_path().string() + "/" + folder;
    }
    
    if (!fs::exists(folderPath)) {
        std::cerr << RED << "Path does not exist: " << folderPath << RESET << std::endl;
        return;
    }
    
    // Check if the path is a file (not a directory)
    if (fs::is_regular_file(folderPath)) {
        cv::Mat img = cv::imread(folderPath);
        if (!img.empty()) {
            frames.push_back(img);
            std::cout << GREEN << "Loaded: " << fs::path(folderPath).filename() << RESET << std::endl;
        } else {
            std::cerr << RED << "Could not load image: " << folderPath << RESET << std::endl;
        }
        return;
    }
    
    // Check if the path is a directory
    if (!fs::is_directory(folderPath)) {
        std::cerr << RED << "Path is not a directory or a valid image file: " << folderPath << RESET << std::endl;
        return;
    }

    // Process directory contents
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry.path())) {
            cv::Mat img = cv::imread(entry.path().string());
            if (!img.empty()) {
                frames.push_back(img);
                std::cout << GREEN << "Loaded: " << entry.path().filename() << RESET << std::endl;
            }
        }
    }
    
    if (frames.empty()) {
        std::cout << YELLOW << "No valid images found in: " << folderPath << RESET << std::endl;
    }
}

void applyGrayscale(std::vector<cv::Mat>& frames) {
    for (auto& frame : frames) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR); // Keep 3-channel for gif writer
    }
    std::cout << CYAN << "Applied grayscale to all frames." << RESET << std::endl;
}

void reverseFrames(std::vector<cv::Mat>& frames) {
    std::reverse(frames.begin(), frames.end());
    std::cout << CYAN << "Reversed frame order." << RESET << std::endl;
}

void exportGIF(const std::string& filename, const std::vector<cv::Mat>& frames, int delay) {
    if (frames.empty()) {
        std::cerr << RED << "No frames to export." << RESET << std::endl;
        return;
    }

    // Create a copy of frames to avoid modifying the originals
    std::vector<cv::Mat> processedFrames;
    
    // Get dimensions from the first frame
    int width = frames[0].cols;
    int height = frames[0].rows;
    
    // Pre-process all frames to ensure consistent format
    for (const auto& frame : frames) {
        cv::Mat resized;
        // Ensure all frames have the same dimensions
        cv::resize(frame, resized, cv::Size(width, height));
        
        // Ensure the frame is in the correct format (8-bit, 3-channel)
        cv::Mat normalized;
        if (resized.type() != CV_8UC3) {
            resized.convertTo(normalized, CV_8UC3);
        } else {
            normalized = resized.clone();
        }
        
        // Convert to RGB (GIF writer expects RGB, not BGR)
        cv::Mat rgb;
        cv::cvtColor(normalized, rgb, cv::COLOR_BGR2RGB);
        
        processedFrames.push_back(rgb);
    }
    
    // Initialize GIF writer
    GifWriter g = {0};  // Initialize with null file pointer
    if (!GifBegin(&g, filename.c_str(), width, height, delay)) {
        std::cerr << RED << "Failed to initialize GIF writer." << RESET << std::endl;
        return;
    }

    // Write each frame
    for (const auto& frame : processedFrames) {
        if (!GifWriteFrame(&g, frame.data, width, height, delay)) {
            std::cerr << RED << "Failed to write frame to GIF." << RESET << std::endl;
            GifEnd(&g);
            return;
        }
    }

    // Finalize the GIF
    GifEnd(&g);
    std::cout << GREEN << "Exported to " << filename << RESET << std::endl;
}

bool isExitCommand(const std::string& input) {
    std::string lower = input;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return std::tolower(c); });
    return lower == "exit";
}

void showSummary(const std::vector<cv::Mat>& frames) {
    std::cout << MAGENTA << "\n📊 Final Summary:" << RESET << std::endl;
    std::cout << CYAN << "Frames captured/loaded: " << frames.size() << RESET << std::endl;
}

void clearMemory(std::vector<cv::Mat>& frames, GifWriter& writer) {
    // Clear the frames vector
    frames.clear();
    
    // Free any memory allocated by the vector
    std::vector<cv::Mat>().swap(frames);
    
    // Close the GIF writer if it's open
    if (writer.f != nullptr) {
        try {
            GifEnd(&writer);
        } catch (...) {
            std::cerr << RED << "Error while closing GIF writer" << RESET << std::endl;
        }
        writer.f = nullptr;  // Set to nullptr after closing
    }
    
    // Force garbage collection
    try {
        cv::destroyAllWindows();
    } catch (...) {
        std::cerr << RED << "Error while destroying windows" << RESET << std::endl;
    }
    
    std::cout << BLUE << "Memory cleared from previous render" << RESET << std::endl;
}

int main() {
    std::vector<cv::Mat> frames;
    GifWriter writer = {0};  // Initialize with null file pointer
    std::string input;

    try {
        while (true) {
            std::cout << BLUE << "\n📷 GIF Creator Menu:\n"
                      << "1. Capture from Webcam\n"
                      << "2. Upload from Folder\n"
                      << "3. Apply Grayscale Filter\n"
                      << "4. Reverse Frame Order\n"
                      << "5. Export as GIF\n"
                      << "6. Exit\n> " << RESET;

            std::getline(std::cin, input);
            if (isExitCommand(input) || input == "6") {
                showSummary(frames);
                break;
            }

            if (input == "1") {
                // Before loading new images, clear previous data
                clearMemory(frames, writer);
                
                captureFromWebcam(frames);
            } else if (input == "2") {
                // Before rendering new GIF, clear previous data
                clearMemory(frames, writer);
                
                std::cout << YELLOW << "Enter folder path: " << RESET;
                std::string folder;
                std::getline(std::cin, folder);
                if (isExitCommand(folder)) { showSummary(frames); break; }
                loadImagesFromFolder(folder, frames);
            } else if (input == "3") {
                applyGrayscale(frames);
            } else if (input == "4") {
                reverseFrames(frames);
            } else if (input == "5") {
                std::cout << YELLOW << "Enter filename (e.g., output.gif): " << RESET;
                std::string filename;
                std::getline(std::cin, filename);
                if (isExitCommand(filename)) { showSummary(frames); break; }

                std::cout << YELLOW << "Enter frame delay in hundredths of a second (e.g., 10): " << RESET;
                std::string delay_str;
                std::getline(std::cin, delay_str);
                if (isExitCommand(delay_str)) { showSummary(frames); break; }

                try {
                    int delay = std::stoi(delay_str);
                    exportGIF(filename, frames, delay);
                } catch (...) {
                    std::cerr << RED << "Invalid delay value. Must be a number." << RESET << std::endl;
                }
            } else if (input == "q" || input == "Q") {
                // Clean up before exiting
                clearMemory(frames, writer);
                break;
            } else {
                std::cout << RED << "Invalid input. Please choose a number from 1 to 6." << RESET << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << RED << "Fatal exception caught: " << e.what() << RESET << std::endl;
    } catch (...) {
        std::cerr << RED << "Fatal unknown exception caught" << RESET << std::endl;
    }
    
    // Final cleanup
    try {
        clearMemory(frames, writer);
    } catch (...) {
        std::cerr << RED << "Error during final cleanup" << RESET << std::endl;
    }
    
    return 0;
}
