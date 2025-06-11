# Use official GCC image as the base (has g++ compiler)
FROM gcc:latest

# Set working directory inside container
WORKDIR /app

# Copy your source code into the container
COPY . .

# Compile the server.cpp file into an executable named 'server'
RUN g++ -o server server.cpp

# Expose port 9909 (your server's listening port)
EXPOSE 9909

# Command to run your server executable
CMD ["./server"]
