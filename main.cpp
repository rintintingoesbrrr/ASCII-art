#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION // get the STB image read implementation 
#include <stb_image.h>

// function to Apply sharpening matrix convolution
std::vector<uint8_t> applySharpeningFilter(const uint8_t* img, int w, int h) {
std::vector<uint8_t> filtered(w * h);

// Sharpening kernel
int kernel[3][3] = {
    { 0, -1,  0},
    {-1,  5, -1},
    { 0, -1,  0}
};

for (int y = 1; y < h - 1; y++) {
    for (int x = 1; x < w - 1; x++) {
        int sum = 0;
        
        // Apply 3x3 kernel
        for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                int pixel_idx = (y + ky) * w + (x + kx);
                sum += img[pixel_idx] * kernel[ky + 1][kx + 1];
            }
        }
        
        // Clamp values to 0-255 range
        sum = std::max(0, std::min(255, sum));
        filtered[y * w + x] = static_cast<uint8_t>(sum);
    }
}

// Handle borders by copying original values
for (int x = 0; x < w; x++) {
    filtered[x] = img[x]; // Top row
    filtered[(h-1) * w + x] = img[(h-1) * w + x]; // Bottom row
}
for (int y = 0; y < h; y++) {
    filtered[y * w] = img[y * w]; // Left column
    filtered[y * w + (w-1)] = img[y * w + (w-1)]; // Right column
}

return filtered;
}

// function to increase contrast
std::vector<uint8_t> increaseContrast(const uint8_t* img, int w, int h, float contrastFactor) {
  std::vector<uint8_t> contrasted(w * h);
  
  for (int i = 0; i < w * h; i++) {
      // Apply contrast formula: new_pixel = ((old_pixel - 128) * factor) + 128
      int newValue = static_cast<int>(((img[i] - 128) * contrastFactor) + 128);
      
      // Clamp to 0-255 range
      newValue = std::max(0, std::min(255, newValue));
      contrasted[i] = static_cast<uint8_t>(newValue);
  }
  
  return contrasted;
}

// function to resize image using nearest neighbor
std::vector<uint8_t> resizeImage(const std::vector<uint8_t>& img, int oldW, int oldH, int newW, int newH) {
 std::vector<uint8_t> resized(newW * newH);
 
 float scaleX = static_cast<float>(oldW) / newW;
 float scaleY = static_cast<float>(oldH) / newH;
 
 for (int y = 0; y < newH; y++) {
     for (int x = 0; x < newW; x++) {
         int oldX = static_cast<int>(x * scaleX);
         int oldY = static_cast<int>(y * scaleY);
         
         // Clamp to bounds
         oldX = std::min(oldX, oldW - 1);
         oldY = std::min(oldY, oldH - 1);
         
         resized[y * newW + x] = img[oldY * oldW + oldX];
     }
 }
 
 return resized;
}

// preprocess pipeline: apply filter (sharpen or contrast) then resize
std::vector<uint8_t> preProcess(const uint8_t* img, int w, int h, int newW, int newH, bool useSharpen, float contrastFactor = 1.5f) {
 std::vector<uint8_t> filtered;
 
 if (useSharpen) {
     // Apply sharpening filter
     filtered = applySharpeningFilter(img, w, h);
 } else {
     // Apply contrast enhancement
     filtered = increaseContrast(img, w, h, contrastFactor);
 }
 
 // Resize the filtered image
 std::vector<uint8_t> processed = resizeImage(filtered, w, h, newW, newH);
 
 return processed;
}

// set relation bewtween pixel and ascci character 
char relateNumChar(int charnum){
// first approach mapping down the 255 int values 
std::vector<char> chars = {'@', '#', '$', '%', '&', '8', 'B', 'W', 'm', 'w', 'q', 'p', 'd', 'b', 'k', 'h', 'a', 'o', 'n', 'u', 'v', 'z', 's', 'c', 'r', '\\', '/', ')', '(', '}', '{', ']', '[', '?', '7', '1', 't', 'f', 'j', '|', 'I', 'l', '!', 'i', '>', '<', '=', '+', '_', '-', '^', '"', '~', ';', ':', ' ', '\'', ',', '.'};
int ascci_index = (charnum*(chars.size()-1) / 255);
return chars[ascci_index];
}

// function to generate ASCII art string
std::string generateAsciiArt(const std::vector<uint8_t>& processedImg, int newW, int newH) {
   std::string asciiArt;
   
   for (int y = 0; y < newH; y++) {
       for (int x = 0; x < newW; x++) {
           int idx = y * newW + x;
           asciiArt += relateNumChar(int(processedImg[idx]));
       }
       asciiArt += '\n';
   }
   
   return asciiArt;
}

// function to export ASCII art to file
bool exportToFile(const std::string& asciiArt, const std::string& filename) {
   std::ofstream file(filename);
   
   if (!file.is_open()) {
       std::cout << "Error: Could not create file '" << filename << "'" << std::endl;
       return false;
   }
   
   file << asciiArt;
   file.close();
   
   std::cout << "ASCII art exported to '" << filename << "'" << std::endl;
   return true;
}

// Text-based user interface
void userInterface() {
   std::string imagePath;
   // Default dimensions optimized for Google Docs page with no borders and Courier New 11pt font
   int newW = 90;  // Default width (characters)
   int newH = 65;  // Default height (lines)
   int filterChoice;
   float contrastFactor = 1.5f;
   char exportChoice, customSizeChoice;
   std::string exportFilename;
   
   std::cout << "=== ASCII Art Generator ===" << std::endl;
   std::cout << "Enter image path: ";
   std::getline(std::cin, imagePath);
   
   std::cout << "Use default size (90x65 - optimized for Google Docs with Courier New 11pt)? (y/n): ";
   std::cin >> customSizeChoice;
   
   if (customSizeChoice == 'n' || customSizeChoice == 'N') {
       std::cout << "Enter output width (characters): ";
       std::cin >> newW;
       
       std::cout << "Enter output height (lines): ";
       std::cin >> newH;
   }
   
   std::cout << "Choose filter:" << std::endl;
   std::cout << "1. Sharpening" << std::endl;
   std::cout << "2. Contrast Enhancement" << std::endl;
   std::cout << "Enter choice (1 or 2): ";
   std::cin >> filterChoice;
   
   bool useSharpen = (filterChoice == 1);
   
   if (!useSharpen) {
       std::cout << "Enter contrast factor (1.0 = no change, 2.0 = double contrast): ";
       std::cin >> contrastFactor;
   }
   
   std::cout << "Export to text file? (y/n): ";
   std::cin >> exportChoice;
   
   if (exportChoice == 'y' || exportChoice == 'Y') {
       std::cout << "Enter filename (e.g., output.txt): ";
       std::cin >> exportFilename;
   }
   
   // Load image
   int w, h, comp;
   int channels = 1;
   const uint8_t* img = stbi_load(imagePath.c_str(), &w, &h, &comp, channels);
   
   if (img == nullptr) {
       std::cout << "Error: Could not load image '" << imagePath << "'" << std::endl;
       return;
   }
   
   std::cout << "\nOriginal image size: " << w << "x" << h << std::endl;
   std::cout << "Output size: " << newW << "x" << newH << std::endl;
   std::cout << "Filter: " << (useSharpen ? "Sharpening" : "Contrast Enhancement") << std::endl;
   if (!useSharpen) {
       std::cout << "Contrast factor: " << contrastFactor << std::endl;
   }
   std::cout << "\nGenerating ASCII art...\n" << std::endl;
   
   // Apply preprocessing pipeline
   std::vector<uint8_t> processedImg = preProcess(img, w, h, newW, newH, useSharpen, contrastFactor);
   
   // Generate ASCII art string
   std::string asciiArt = generateAsciiArt(processedImg, newW, newH);
   
   // Print ASCII art to terminal
   std::cout << asciiArt << std::endl;
   
   // Export to file if requested
   if (exportChoice == 'y' || exportChoice == 'Y') {
       exportToFile(asciiArt, exportFilename);
   }
   
   // Free the image memory
   stbi_image_free((void*)img);
}

int main() {
   userInterface();
   return 0;
}