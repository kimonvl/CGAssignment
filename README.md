# GraphicsAssignment

```cmd
git clone https://github.com/kimonvl/CGAssignment.git
premake5 vs2019
```

# Premake5

The assignment is using [premake5](https://premake.github.io/) in order to generate the necessary project files. Thus you will need to have downloaded and be able to run `premake` from the command line before you start building and using the project. You can download a prebuild binary either from their [webpage](https://premake.github.io/download) or the [github page](https://premake.github.io/download).
<br><br>
After you have downloaded the latest prebuild binary you need to setup the appropriate enviroment in order to be able to run the premake from the command line:
<br>

* _Open **File Explorer**._
* _Right Click: **This PC** and Select: **Properties**._
* _Click: **Advanced system settings**._
* _Click: on the button **Enviroment Variables**._
* _Add to variable: **Path** the path to your local copy of premake5.exe ._
    - _If the variable __Path__ doesn't exist create it by pressing the __New...__ button._

<br>

_If you need more information about how to use premake I will suggest you looking at their [documentation](https://premake.github.io/docs/)._

# C++17 compiler

You will neeed a modern C++ compiler with support for `C++17` as well as Windowns API calls. The recomended version of Visual Studio (see bellow) should be covering you.