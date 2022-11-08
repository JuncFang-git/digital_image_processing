#include <iostream>
#include "image.h"

void reverse_color_test(Image &input) {
  input.reverse_color();
  input.write_jpg("../demo/result/reverse_color.png", 100);
}

int main() {
  Image input_image("../demo/test_image/lenna.png");
  // reverse_color
  reverse_color_test(input_image);
  std::cout << "hello" << std::endl;
}