# Prerequisites & Dependencies

* g++
* cmake
* boost
* glew
* glfw3
* glm
* YAML-CPP

# Compile

```bash
mkdir build && cd build
cmake .. && make
```

# Run CLI

Render to file

```bash
osrlib --yaml testdata/scene.yaml --output bunny.ppm
```

Run viewer

```bash
osrlib --yaml testdata/scene.yaml
```
