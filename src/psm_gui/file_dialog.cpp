#include "file_dialog.h"

#include <string>
#include <vector>

// Platform-specific includes
#ifdef _WIN32
#include <commdlg.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#else
#include <cstdlib>
#include <cstring>
#endif

namespace psm_gui {

std::string OpenImageFileDialog() {
  std::string filePath;

#ifdef _WIN32
  // Windows implementation
  OPENFILENAMEA ofn;
  char szFile[260] = {0};

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter =
      "Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All Files\0*.*\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

  if (GetOpenFileNameA(&ofn)) {
    filePath = ofn.lpstrFile;
  }
#elif defined(__APPLE__)
  // macOS implementation using CoreFoundation
  CFURLRef fileURL = NULL;
  CFStringRef message = CFSTR("Select an image file");
  CFStringRef title = CFSTR("Open");

  CFMutableArrayRef fileTypes =
      CFArrayCreateMutable(kCFAllocatorDefault, 5, &kCFTypeArrayCallBacks);
  CFArrayAppendValue(fileTypes, CFSTR("png"));
  CFArrayAppendValue(fileTypes, CFSTR("jpg"));
  CFArrayAppendValue(fileTypes, CFSTR("jpeg"));
  CFArrayAppendValue(fileTypes, CFSTR("bmp"));
  CFArrayAppendValue(fileTypes, CFSTR("tga"));

  // Create the dialog
  fileURL = NULL;

  // Convert CFURL to string
  if (fileURL != NULL) {
    CFStringRef filePathCF =
        CFURLCopyFileSystemPath(fileURL, kCFURLPOSIXPathStyle);
    if (filePathCF) {
      char buffer[1024];
      if (CFStringGetCString(filePathCF, buffer, sizeof(buffer),
                             kCFStringEncodingUTF8)) {
        filePath = buffer;
      }
      CFRelease(filePathCF);
    }
    CFRelease(fileURL);
  }

  CFRelease(fileTypes);
#else
  // Linux implementation using zenity
  FILE* pipe = popen(
      "zenity --file-selection --title=\"Select an image file\" "
      "--file-filter=\"Image files | *.png *.jpg *.jpeg *.bmp *.tga\"",
      "r");
  if (pipe) {
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
      // Remove trailing newline
      size_t len = strlen(buffer);
      if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
      }
      filePath = buffer;
    }
    pclose(pipe);
  }
#endif

  return filePath;
}

}  // namespace psm_gui
