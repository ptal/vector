# Vector data structure

The code will be compiled with the following command to obtain the best performance:

```
g++ test/test_vector.cpp -Iinclude -Wall -Werror -Wextra -Wpedantic -std=c++26 -march=native -DNDEBUG -O3 -o test_vector
```

We might replace `test_vector.cpp` with our own tests, so make sure your implementation of `vector` matches the interface provided.

Make sure your code compile with this command before submitting for review.

For debugging your code, you can compile with this command:
```
g++ test/test_vector.cpp -Iinclude -Wall -Wextra -Wpedantic -std=c++23 -g -o test_vector
```
