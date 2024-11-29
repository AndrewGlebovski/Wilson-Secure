# Add executables
add_executable(Client client.cpp)
add_executable(Server server.cpp)

# Find packages
find_package(OpenSSL REQUIRED)

# Link 
target_link_libraries(Client PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(Server PRIVATE OpenSSL::SSL OpenSSL::Crypto)
