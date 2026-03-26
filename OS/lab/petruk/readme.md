run linux.cpp:
  g++ -std=c++17 -pthread linux.cpp -o linux
  ./linux
run tests:
  chmod +x run_tests.sh
  ./run_tests.sh
