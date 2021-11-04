# Arduino LED Matrix DFS Algorithm Visualizer 

## Functionality
This project is a DFS Algorithm Visualizer on a WS2812B 16x16 LED Matrix using Arduino. A maze is generated and drawn on the LED matrix, and then a joystick may then be used to
select and start and end location in the maze. Then the DFS algorithm will run - picking a random direction to go at each node until it has found the end location or all nodes 
have been visited. If the end location was found, the exact traversal of the algorithm will be traced through in an animation before showing the solution. Note that since this is 
DFS, the shortest path is not guaranteed. 

## Limitations
Due to the Arduino Uno's limited callstack and RAM, the maze cannot be generated upon startup on the Uno. Thus, I have written a seperate script to generate an easily copy and
pasteable maze for the Arduino code. A 16x16 maze generator averages nearly 600-700 call stacks if done recursively, and the same number applies for the amount of loop iterations
in an iterative apporach. 

## Algorithm in Action
![](TSl4smH.gif)

## Circuit
<img src="https://i.imgur.com/JbfJXmj.jpg" width="300" /> <img src="https://i.imgur.com/80zrv0c.jpg" width="500" />
