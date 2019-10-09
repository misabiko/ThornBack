# ThornBack

An(other) attempt at making a Minecraft clone on the Godot Engine.
The name doesn't mean anything.

The current texture pack is [New Start by loufisch](https://www.planetminecraft.com/texture_pack/new-start---by-loufisch/).

## Compiling
Most of this follows the guide here: https://docs.godotengine.org/en/3.1/tutorials/plugins/gdnative/gdnative-cpp-example.html

- `git clone https://github.com/misabiko/ThornBack.git`
- `git submodule update --init --recursive`
- `cd godot-cpp`
- `godot --gdnative-generate-json-api api.json`
- `scons p=<your platform> generate_bindings=yes　bits=64`
    - You can install scons with pip
    - You also need the Visual Studio compiler, if Visual Studio isn't installed you can install the build tool individually here https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2017
    - You can add `-j<number of threads>` to the command to speed up the process
- Launch "x64 Native Tools Command Prompt" or "vcvars64.bat"
    - Mine was found in `C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat`
    - but  the location can change depending on VS's version
    - I suggest making a search at `C:\Program Files (x86)\Microsoft Visual Studio`
    - To my experience, this doesn't work in PowerShell since it immediately exits the shell
- `cd {ThornBack}/Project/GDNative`
- `scons p=<your platform> bits=64`
    - Again with `-j`
- Download https://www.planetminecraft.com/texture_pack/new-start---by-loufisch/ and extract as Project/newstart_textures
- Launch Godot, Import Project/project.godot
- Fix Dependencies → Fix Broken (top right button)
- Launch Scenes/World.tscn