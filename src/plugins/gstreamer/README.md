# Apertus GStreamerPlugin

## Install

Linux (Ubuntu/Debian)
```sh
sudo apt install gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-libav
```

macOS (Homebrew)
```sh
brew install gst-plugins-base gst-plugins-good gst-plugins-bad gst-libav
```

Windows (MSYS2)
```sh
pacman -S mingw-w64-x86_64-gst-plugins-base mingw-w64-x86_64-gst-plugins-good mingw-w64-x86_64-gst-plugins-bad
```

## Optional dependencies

### libCurl

Linux (Ubuntu/Debian)
```sh
sudo apt install libcurl4-openssl-dev
```

macOS (Homebrew)
```sh
brew install curl
```

## Debug

```sh
cd build
GST_DEBUG=4 ./apertus
```


## Important notes

### URI Format for GStreamer

eventService->Trigger("PlayAudio", "file:///Users/aklen/Music/Ableton/Projects/647 Project/export/647.mp3");
