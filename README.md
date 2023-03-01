# UE5 Maze Generator Plugin

<img src="https://media3.giphy.com/media/8FDhJzbSLQvhmDVeTW/giphy.gif?cid=790b76114036e4ee69b9f731981f1a7704870fdb8ff19adc&rid=giphy.gif&ct=g" height="222"/>

## Table of Contents

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

1. Open plugins list **Edit->Plugins**:

![image](https://user-images.githubusercontent.com/111310807/222064589-3038b999-3a34-4e9d-b194-72f5498b8acd.png)


2. Search for MazeGenerator in search box and check the checkbox. Then restart your project.

![image](https://user-images.githubusercontent.com/111310807/222064484-10c72947-3979-42da-a926-55bb98e6eb89.png)

3. Make sure plugins are shown in content browser. You can toggle visibility as follows:

![image](https://user-images.githubusercontent.com/111310807/222065237-c0bc2b18-0d3c-4238-8a52-fd55f3a9b139.png)


---

## Quick Start

To create your first maze create Blueprint class based on `Maze` class:

![](./Images/derive_from.jpg)

---

In the newly created Blueprint class select _Class Defaults_:

![](./Images/class_defaults.jpg)

---

Set `Floor` and `Wall` Static Meshes in _Details_ panel:

![](./Images/expand_cells.jpg)

![](./Images/set_cells.jpg)

---

Instantiate Maze by dragging Blueprint into level and play with parameters:

![](./Images/change_params.jpg)

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
- Source code of the plugin can be found under  _Plugins/MazeGenerator/Source/MazeGenerator_
