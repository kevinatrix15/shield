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
   
   Upon inspection using both VTune and gprof, the primary bottleneck of the entire application is IO, in particular the writing of the map to disk found in ConfigSpaceIO::write(). This was not a surprising finding given the fact that data is currently being written in ASCII format and the inherent cost of converting data from memory to a string to write. This could be significantly reduced going forward by instead adding binary writing and reading capability, avoiding this conversion altogether.

   The more interesting bottleneck with regards to the A* algorithm was found to be ConfigurationSpace::getAccessibleNbrs(), which is called for each considered and is used to find each of the accessible neighboring nodes. In its current form, the function starts with an empty vector, and emplaces neighbors onto the vector if they're accessible. Running with a map with dimensions 4500x4500, this function took 71% of the run time, the bulk of which was spent in std::vector::_Emplace_reallocate() to reallocate the vector in memory and copy data as more neighbors are emplaced. Since we have a fixed stencil size, this could be significantly optimized by using a std::array of the stencil size and avoid the reallocation altogether.
  
2. Modularity - How would you handle additional planning algorithms and new methods of updating the map?

    Assuming we wish to have a runtime-configurable selection of search algorithms, one way this could be supported is through polymorphism by defining an interface class with a `searchPath()` method which simply takes a `start` and `goal` cell positions as is currently done, returning the path of cells traversed if a solution is found. The implementations between algorithms would be compared to identify common functionality to be extracted to avoid code duplication.

    As for specifying and updating the map- It's likely in a real-world robotic application, the map wouldn't be fully known at the start of the search, and would instead be updated from sensor data in real-time. To support this, I would add an `update()` method to the `ConfigurationSpace` class which could take in obstacle position information. This would be generalized to take in other means for defining the obstacles besides specifying circles. In such a case, I would consider other dynamic search algorithms to update the map as the search progresses. 

3. Cross-Platform - How would you alter your library to support multiple Operating Systems or multiple processors with different instruction sets?

    One consideration for supporting cross-platform environments is the build system. I have chosen to build my application using CMake to enable cross-platform builds, and have proven it out on Linux and Windows. According to [online documentation](https://wiki.ros.org/rosbuild/CMakeLists), it should also support ROS commonly used for robotics.

    The other major consideration would be proper utilization of resources for the given compute architecture. I would perform a careful analysis of my application to understand the resource utilization (e.g., compute, memory, disk, etc.) to determine whether I would be faced with limitations with a given platform. This may require further optimizations to e.g., select a search algorithm with lower memory complexity, or adjust my selected data structures to improve utilization. Other considerations might come when considering parallelization of the solution to consider communication protocols or programming paradigms.

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

![alt text](https://github.com/kevinatrix15/shield/blob/main/samples/solution-cas1.png?raw=true)

Case 2- Impossible solution, with an obstacle spanning the minimum dimension of the task space.

![alt text](https://github.com/kevinatrix15/shield/blob/main/samples/solution-cas2.png?raw=true)

Case 3- A simple case with two obstacles in the opposite corners from the start and goal.

![alt text](https://github.com/kevinatrix15/shield/blob/main/samples/solution-cas3.png?raw=true)

Case 4- A complex case with many obstacles to navigate around.

![alt text](https://github.com/kevinatrix15/shield/blob/main/samples/solution-cas4.png?raw=true)

Case 5- A maze of obstacles, forcing the robot to take a winding path.

![alt text](https://github.com/kevinatrix15/shield/blob/main/samples/solution-cas5.png?raw=true)

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