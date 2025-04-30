# 🎞️ Frame-by-Frame GIF Creator (C++)

A simple yet powerful terminal-based application that lets you:

- 📷 Capture stills from your **webcam**
- 🖼️ Load images from a **folder**
- 🎨 Apply filters (e.g., grayscale)
- 🔁 Reverse playback order
- 💾 Export as a looping GIF!

## 🚀 Features

| Feature                | Description                                   |
|------------------------|-----------------------------------------------|
| Webcam Input           | Capture frames live from your webcam          |
| Folder Import          | Load existing images from your local folder   |
| Grayscale Filter       | Optionally apply a grayscale filter           |
| Reverse Playback       | Reverse frame order before exporting          |
| GIF Export             | Export assembled frames into a `.gif` file    |
| Exit Anytime           | Type `exit` anytime to quit and view summary  |
| Terminal UI            | Colored feedback and validation in terminal   |

---

## 🧰 Tech Stack

- **C++17**
- [OpenCV](https://opencv.org/) – For capturing and processing images
- [`gif_writer.h`](https://github.com/charlietangora/gif-h) – Lightweight GIF encoder
- Cross-platform (Windows, Linux, macOS)

---

## 📦 Installation

### 1. Clone this repository

```bash
git clone https://github.com/YSPlop/gif-creator-cpp.git
cd gif-creator-cpp
```

### 2. Install OpenCV

**Linux (Ubuntu):**
```bash
sudo apt update
sudo apt install libopencv-dev
```

**macOS (Homebrew):**
```bash
brew install opencv
```

**Windows:**  
Use vcpkg or install OpenCV manually and configure paths in CMake.

### 3. Build the project

```bash
mkdir build && cd build
cmake ..
make
./GifCreator
```

---

## 🎮 How to Use

Launch the app and follow the menu:

1. Choose `1` to capture webcam frames (press **SPACE** to capture, **ESC** to stop).
2. Choose `2` to load image files from a folder.
3. Apply grayscale or reverse filters as needed.
4. Choose `5` to export the final GIF (set filename + delay).
5. Type `exit` at any time to quit and see a summary.

---

## 🧠 Why This Project?

✅ Creative and visual  
✅ Uses computer vision + image processing  
✅ Demonstrates file I/O, UI, and multimedia  
✅ Great C++ showcase project for **media/software roles**

---

## 📸 Example Output

![demo](demo.gif)

> Demo GIF above created using webcam + grayscale filter.

---

## 📄 License

MIT License © 2025 [Your Name]
