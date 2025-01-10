# ZeroZero

Examples for the [ZeroZero engine](https://github.com/HenriMichelon/zero_zero)

[User documentation](https://henrimichelon.github.io/ZeroZero/)

**How to use**
- Clone this repository and the [ZeroZero repository](https://github.com/HenriMichelon/zero_zero)
- Create a `.env.cmake` file in your project with a variable called `Z0_PROJECT_DIR` to reference the ZeroZero cloned directory, for example :
  `set(Z0_PROJECT_DIR "C:/Users/MyUser/Documents/GitHub/zero_zero")`
- If needed, adjust the `CMakeLists.txt` settings to your needs
- `cmake -B build -G Ninja -D CMAKE_BUILD_TYPE=Release`
- `cmake --build build`

**Note**

Due to his size the `interior_station_pack.zres` file is not included in this repository.

It can be generated with Blender and the [ZeroZero's Blender Add-on](https://henrimichelon.github.io/ZeroZero/md_003_blender_add_on.html) from the `interior_station_pack.blend` source file.
