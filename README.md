# Shield Challenge Problem- Robot Path Planning

## Deliverables
1. Implement a library that helps BB-8 navigate a map of obstacles. For simplicity, assume BB-8 is a circle on the map with a given radius and all obstacles on the map also have locations and radii. Your customer is other developers on the team, so make sure the library is functional and easy to use. You may use the programming language you think is appropriate for the task.
  At the minimum the library should include the following abilities:
  * Take the initial parameters of map size (M x N) and robot radius.
  * Add obstacles to the current map using as a parameter a list of circular obstacles, with each having a center location and radius.
  * Create the best safe path given a start and end location on the current map, taking into consideration the robot radius.
  * Save the current map to a file.
  * Load a map from a file.
  * Visualize a map and path to the screen.
2. Provide an example application to demo the usage of this library. This includes the build files, directions for building, and directions for use such
3. Please explain your reasoning for why you chose your implementation.

## Approach
After spending time researching various path planning algorithms, I chose to go with the A* search algorithm. There were a number of factors that led me to select this approach. Since this problem deals with a static environment with full view of all obstacles, it made sense to select from global algorithms as opposed to local. I first looked at the Dijkstra algorithm due to its general robustness, but the A*'s advantages introduced by using heuristics as a 'best-first' search approach was appealing for improved efficiency. Additionally, given to the various heuristics that may be applied, it leaves open the possibility of further exploration for optimization. The tradeoff of such an approach is its relatively high memory complexity, but given that this is currently meant for running on a PC this isn't a concern for now.

## Additional Considerations
In the interest of time, it is not necessary to implement the following, but please briefly (< 150 words total) answer how you would adjust your solution to address these issues.
1. Performance - What is the bottleneck in your library? How could you go about improving performance in the future?
  
2. Modularity - How would you handle additional planning algorithms and new methods of updating the map?
3. Cross-Platform - How would you alter your library to support multiple Operating Systems or multiple processors with different instruction sets?

## Solutions
The example application is demonstrated in the code under `src/Main.cpp`. The motion planning solution itself is implemented in `MotionPlanning.h` Other supporting functionality has been implemented as a modular library in various classes contained in `src/`.

## Building
Build scripts used to configure CMake have been provided for building on both Linux (`build.sh`) and Windows (`build.bat`) for convenience. Note that the paths contained in `build.bat` may need to be modified for your local install of Visual Studio. The usage is provided if run with the `-h` / `--help` option, but in general it should be sufficient to run as follows:
```
./build.sh
```
or
```
.\build.bat
```
Note that the build artifacts may be found under `build/` (created by `build.sh`) or `win_build\` (created by `build.bat`), and the generated binary (`save-bb8`) and test binary (`save-bb8.Test`) will be found under the respective build directories.

## Unit Testing
The [Catch testing framework](https://github.com/catchorg/Catch2) is used in this project for its simplicity (header-only library). Dependencies are located in `catch/`. Unfortunately, due to time constraints unit testing was minimal on this project, given the time commitment involved with the solution. The `Cell` class was unit tests as a sample of testing practices, and is found under `src/test/`. The generated test executable is under the system-specific build directory as `save-bb8.Test`, and may be run with the following:
```
./save-bb8.Test
```

## Running
For best performance, the solution may be run directly by calling the compiled executable with some additional command line arguments. Due to time constraints, robust command parsing with a `--help` option was not implemented, so a usage guide is included here. The solution may be run without any visualizations generated with the following (see more below for generating visualization):

```
# ./<this-executable> <M> <N> <robot-radius> <pre-configured-case>
./save-bb8 100 250 6 5
```
where positional arguments are as follows:
* M:= The number of rows in the task space, in unit cells
* N:= The number of columns in the task space, in unit cells
* robot-radius:= The robot's radius, in unit cells
* pre-configured case:= There are five pre-configured obstacle cases that may be selected from. These are scaled based on M and N. The pre-configured cases are described below.

### Pre-configured Cases
In each case, the robot is attempting to start from the lower left corner with a goal in the upper right. The following configurations are provided for demonstration.
Case 1- Simple, obstacle-free space
Case 2- Impossible solution, with an obstacle spanning the minimum dimension of the task space.
Case 3- A simple case with two obstacles in the opposite corners from the start and goal.
Case 4- A complex case with many obstacles to navigate around.
Case 5- A maze of obstacles, forcing the robot to take a winding path.

Note that sample outputs from the above cases may be found under the `samples/` directory.

### Outputs
The solution generates two output files, found in the `output/` directory which is generated upon running, within your working directory. The files are `config-space.txt` and `solution-path.txt`, and include plain-text data of their respective fields. The contents are as follows:

#### config-space.txt- 
* The first row indicates the robot's radius used to generate the configuration space.
* The second and third rows are, respectively, the number of columns and rows.
* The remainder of the file contains a table of values representing the state of each cell in the configuration space. For reference, 0 represents free space accessible to the robot, 1 represents obstacles, and 2 represents padding added around each object and the boundaries to account for the robot's radius.

#### solution-path.txt- 
This file contains the solution path for the robot, moving from the start position to the goal. It is composed of two columns, with the first representing the x-index and the second the y-index.

## Visualization
To avoid introducing additional dependencies to the project, visualization is achieved by running a simple python script which wraps the `save-bb8` executable. The script is found at the top level in `run-save-bb8.py`. It may be executed with the same command line arguments as `save-bb8` described above, as follows:
```
# ./<this-executable> <M> <N> <robot-radius> <pre-configured-case>
./run-save-bb8.py 100 250 6 5
```
If graphics forwarding isn't supported in the environment you're running Python from, the visualization is saved in `solution.png`.

## Code Documentation
In-code documentation was added on an as-needed basis where the code may have needed more explanation / context, but was left otherwise sparse to avoid cluttering the code. 

## Future Work
Some enhancements were identified during development which were not included in this exercise for sake of time. These were called out with a `TODO: FUTURE WORK` comment.