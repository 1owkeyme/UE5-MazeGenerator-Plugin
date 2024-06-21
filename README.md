# UE5 Maze Generator Plugin

<img src="https://media3.giphy.com/media/8FDhJzbSLQvhmDVeTW/giphy.gif?cid=790b76114036e4ee69b9f731981f1a7704870fdb8ff19adc&rid=giphy.gif&ct=g" height="222"/>

## Table of Contents

- [UE5 Maze Generator Plugin](#ue5-maze-generator-plugin)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Enable Plugin](#enable-plugin)
  - [Quick Start](#quick-start)
  - [Generation Algorithms](#generation-algorithms)
  - [Limitations](#limitations)
  - [Notes](#notes)

---

## About

This is a plugin for UE5 that allows you to create various mazes using the following parameters:

- Size
- Seed
- Generation algorithm

This plugin supports pathfinding.

---

## Enable Plugin

Before enabling plugin ensure you have installed it in engine of appropriate version inside Epic Games launcher:

![](./Images/install_to_engine.png)

---

Open plugins list **Edit->Plugins**:

![](./Images/click_plugins.png)

---

Search for MazeGenerator in search box and check the checkbox against MazeGenerator plugin. Then restart your project.

![](./Images/enable_plugin.png)

---

Make sure plugins are shown in content browser. You can toggle visibility as follows:

![](./Images/show_plugin.png)

## Quick Start

To create your first maze create Blueprint class based on `Maze` class:

![](./Images/create_maze.png)

---

In the newly created Blueprint class select _Class Defaults_ and play with available parameters:

![](./Images/class_defaults.png)

_Note: size can be specified only on instances(i.e. on actors placed on level)_

Set `Floor` and `Wall` Static Meshes under _Details_ panel.

---

Instantiate Maze by dragging Blueprint into level and play with parameters:

![](./Images/place_on_level.png)

## Generation Algorithms

- [Recursive Backtracker](http://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking.html)
- [Recursive Division](http://weblog.jamisbuck.org/2011/1/12/maze-generation-recursive-division-algorithm)
- [Hunt-and-Kill](http://weblog.jamisbuck.org/2011/1/24/maze-generation-hunt-and-kill-algorithm.html)
- [Sidewinder](http://weblog.jamisbuck.org/2011/2/3/maze-generation-sidewinder-algorithm.html)
- [Kruskal's](http://weblog.jamisbuck.org/2011/1/3/maze-generation-kruskal-s-algorithm.html)
- [Eller's](http://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm.html)
- [Prim's](http://weblog.jamisbuck.org/2011/1/10/maze-generation-prim-s-algorithm.html)

## Limitations

Unfortunately, Unreal Engine Reflection System doesn't support 2D arrays, so legally they can't be exposed to the editor.

If you want to work directly with the maze grid or path, inherit from the `Maze` class and expand the functionality directly using C++.

## Notes

- Generated mazes are perfect
- Under the plugin content folder is an example of a `Maze` Blueprint with some logic
- Instances of `Maze` have _Randomize_ button
- Source code of the plugin can be found under _Plugins/MazeGenerator/Source/MazeGenerator_
