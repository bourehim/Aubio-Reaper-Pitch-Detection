# Aubio Reaper Pitch Detection


Create debug build:
```sh
mkdir -p build/linux
cd build/linux
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cmake --build .
```



Result will be located at `build/linux/reaper_minimal_plugin.so`


#### Windows

Create debug build:
```batch
mkdir build\win
cd build\win
cmake ..\..
cmake --build .
```

Result will be located at `build/win/reaper_minimal_plugin.dll`