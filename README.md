# Cluster
This project is made as for self-learning purposes, which has the base of DPP & SQLite statements and readable in the code base.

## Features
- Blacklist specific users by ID
- Notify admins of blacklist violations
- Lightweight and efficient C++ implementation

## Requirements  
- C++17 or later: Ensure your compiler supports modern C++ features.  
- A Discord developer account and bot token: Register your bot at https://discord.com/developers/applications.  
- libDPP library: Used for interacting with Discord's API.  
  - Install via vcpkg: `vcpkg install dpp`  
  - Or build from source: https://github.com/brainboxdotcc/DPP  
- SQLite library: Required for storing blacklist data persistently.  
  - Install via vcpkg: `vcpkg install sqlite3`  
  - Or follow the installation instructions: https://sqlite.org/download.html  
- CMake or Make: Build tools for compiling the project.

## Usage
- Use `/blacklist <userID> <reason>` to add a user to the blacklist.
- Use `!unblacklist <userID>` to remove a user from the blacklist.
- Use `!list` to view all blacklisted users.

## License
This project is licensed under the [GNU General Public License v3.0](./LICENSE).  
You are free to use, modify, and distribute this software as long as you adhere to the terms of the license.

## Contributing
Contributions are welcome! Please fork this repository, make your changes, and submit a pull request.

## Disclaimer
This bot is intended for educational and administrative purposes only. Use it responsibly and ensure compliance with Discord's [Terms of Service](https://discord.com/terms).
