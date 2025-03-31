# Flow-Maker

A C++ console-based application with file handling that allows users to create, execute, and manage interactive workflows.

A project for the Faculty of Automatic Control and Computers, University Politehnica of Bucharest.

Users can define steps such as text input, number input, arithmetic operations, file inputs, and more. Workflows can be saved, loaded, and deleted using a CSV-based storage system.

You can create custom workflows with predefined step types, save and load workflows from CSV files, execute predefined flows or user-created ones, add various step types like text input, number input, calculations, and file handling, interactive menu-driven interface for easy navigation.

Concepts used:
-> Object-Oriented Programming (OOP) – Implemented using classes and objects

-> Inheritance & Polymorphism – Different step types inherit from a base FlowStep class

-> Encapsulation – Data and methods are grouped within relevant classes

-> Templates – Used in CalculusStep for arithmetic operations with different data types

-> Exception Handling – C++ exceptions are used to manage errors gracefully

-> File Handling (CSV Storage) – Workflows are saved and retrieved from a CSV file

-> Algorithms & Iterators – Used for searching, sorting, and managing workflow steps
