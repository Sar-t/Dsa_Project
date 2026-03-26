# LINUX_File_Management_System_Emulator <!-- omit in toc -->


This program simulates a simple Linux file system with basic shell command functionalities. It provides a structure to manage files and directories, akin to a simplified Linux environment. Below, you'll find detailed descriptions of the functionalities and commands available in this simulation.


**Table of Contents**
- [Overview:](#overview)
- [Class Descriptions:](#class-descriptions)
- [Functions:](#functions)
- [Commands:](#commands)
- [Usage:](#usage)
- [Compilation and Execution:](#compilation-and-execution)


## Overview
The program is designed to simulate a file system using a tree structure, where each node represents either a file or a directory. It supports various commands such as ls, cd, pwd, mkdir, touch, rm, cp, mv, chmod, find, stat, edit, cat, and more, mimicking a Linux shell.
  
## Class Descriptions
TreeNode
The TreeNode class represents a node in the file system, which can either be a file or a directory.

- Attributes:
  - name: The name of the file or directory.
  - contents: The contents of the file (used for files only).
  - type: The type of node ('d' for directory, '-' for file).
  - cdate: Creation date of the node.
  - mdate: Last modified date of the node.
  - permission: Permission of the node in numeric form.
  - parent: Pointer to the parent node.
  - link: Pointer to the next sibling node.
  - child: Pointer to the first child node.
- Methods:
  - get_permission(): Returns the permission in string format (e.g., "rwx").


## Functions  
Main Functions
- linux_tree(TreeNode root):* Initializes the file system with some default directories and files.
- print_help(): Prints the help message with a list of available commands.
- print_tree(TreeNode root, string prev):* Recursively prints the contents of the directory in a tree-like format.
- print_ls(TreeNode pwd):* Lists the contents of the current directory.
- print_stat(TreeNode root, TreeNode pwd, string path):** Prints metadata of a file or directory.
- pwd_str(TreeNode root, TreeNode pwd):** Returns the current working directory as a string.
- find_names(TreeNode root, TreeNode pwd, string name):** Finds files or directories by name.
- find_node(TreeNode root, TreeNode pwd, string path):** Finds a node by its path.
- find_on_pwd(TreeNode pwd, string name):* Finds a node in the current working directory by name.
- split(string str, char delim): Splits a string by a delimiter.
- join(list<string> str, char delim): Joins a list of strings by a delimiter.
- split_name(string str): Splits a path into directory and name.
- cd(TreeNode root, TreeNode pwd, string path):** Changes the current directory.
- create(TreeNode root, TreeNode pwd, string path, char type):** Creates a new file or directory.
- remove(TreeNode root, TreeNode pwd, string path):** Removes a file or directory.
- dupl(TreeNode root, TreeNode pwd, string src, string dst, int keep):** Copies or moves a file or directory.
- edit(TreeNode root, TreeNode pwd, string path):** Edits the contents of a file.
- cat(TreeNode root, TreeNode pwd, string path):** Prints the contents of a file.
- chmod(TreeNode root, TreeNode pwd, string path, string new_modes):** Changes the permissions of a file or directory.
- clear_screen(): Clears the console screen.

## Commands
 The following commands are supported by the simulated file system:

- help: Prints the help message with a list of available commands.
- ls: Lists contents of the current directory.
- tree: Lists contents of the current directory in a tree-like format.
- pwd: Prints the current working directory.
- cd DIR: Changes directory to DIR.
- find N: Finds file or directory named N.
- stat P: Prints metadata of file or directory at path P.
- mkdir D: Creates a directory named D.
- touch F: Creates a file named F.
- rm P: Removes the file or directory at path P.
- rmdir P: Removes the directory at path P.
- cp S D: Copies file or directory from S to D.
- mv S D: Moves file or directory from S to D.
- edit P: Edits the file at path P.
- cat P: Prints the contents of the file at path P.
- chmod M P: Changes permissions of the file at path P to mode M.
- clear: Clears the console screen.
- exit: Exits the shell.

## Usage
- Listing Files and Directories:

  - ls lists the contents of the current directory.
  - tree displays the directory structure in a tree format.
    
- Navigating Directories:

  - pwd shows the current directory path.
  - cd DIR changes the current directory to DIR.
    
- Managing Files and Directories:

  - mkdir D creates a new directory named D.
  - touch F creates a new file named F.
  - rm P or rmdir P removes a file or directory at path P.
 
- File Operations:

  - cp S D copies a file or directory from S to D.
  - mv S D moves a file or directory from S to D.
  - edit P edits the contents of the file at path P.
  - cat P prints the contents of the file at path P.
    
- File and Directory Information:

  - stat P prints the metadata of a file or directory at path P.
  - find N searches for a file or directory named N.
 
- Permission Management:

  - chmod M P changes the permissions of the file at path P to mode M.
    
- Miscellaneous:

  - clear clears the console screen.
  - exit exits the shell.

## Compilation and Execution:
To compile and run the program, follow these steps:

- Compile the program:

    - g++ -o fs_simulator fs_simulator.cpp
   
- Run the program:

   - ./fs_simulator


 - First install curl
 - then compile like this
```g++ main.cpp -o main -I C:\curl\include -L C:\curl\lib -lcurl -lws2_32 -lssl -lcrypto```
