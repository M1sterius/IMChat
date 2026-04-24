# IMChat
A small instant messaging app I built in C++ to learn about networking,
boost::asio and databases.

It’s a client-server setup with a custom json-based protocol, 
a small GUI and PostgreSQL database.

## Features
- Real-time messaging between multiple clients
- Login system with username + password (hashed on client)
- Chat history is stored and loaded from the database
- Shows currently connected users
- Messages are timestamped and synced from the server
- Basic UI with scrolling chat and input box

## UI showcase
![Login](images/login.png)

![Main UI](images/main.png)

## Tech used
- Modern C++ (std=c++20)
- boost::asio for asynchronous networking
- Dear ImGui for simple UI (with utf8 input support)
- libpqxx for PostgreSQL database connection in C++
- nlohmann::json for message serialization, fmt for formatted logging

## Quick architecture overview
- The server accepts TCP connections and keeps track of clients
- Messages are sent as structured JSON with a small header
- When a user logs in:
  - Server checks credentials in PostgreSQL
  - Sends back chat history + active users
- When someone sends a message:
  - It gets saved to the database
  - Then broadcast to all other connected clients

## Building and running the project
Clone the repository using `git clone https://github.com/M1sterius/IMChat.git`.
Make sure you have installed `postgresql` dev package, `cmake`, `ninja` and a C++20 compatible compiler.
If you're running a wayland linux distribution, make sure to install `libwayland-dev` package using
`sudo apt install libwayland-dev libxkbcommon-dev xorg-dev`

Generate build files by running this command in the root project directory.
`cmake -S . -B CMakeBuild -DCMAKE_BUILD_TYPE=Debug`

Compile the project with.
`cmake -C CMakeBuild`

Executables for server and client will be placed in `Bin/Server/<Debug/Release>` and `Bin/Client/<Debug/Release>`.

To run the server you must firstly configure the database using the schema provided in `db/schema.sql`.
After creating and configuring the database, make sure to set `IMCHAT_POSTGRESQL_USER`, `IMCHAT_DB_NAME` and `IMCHAT_DB_PASSWORD`
environment variables.