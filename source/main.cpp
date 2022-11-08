#include <iostream>
#include "glog/logging.h"
#include "image.h"

void reverse_color_test(Image &input) {
  LOG(INFO) << "hello";
  input.reverse_color();
  LOG(INFO) << "hello";
  input.write_jpg("../demo/result/reverse_color.png", 100);
}

int main() {
  std::cout << "hello" << std::endl;
  Image input_image("../demo/test_image/lenna.png");
  LOG(INFO) << "hello";
  // reverse_color
  reverse_color_test(input_image);
  std::cout << "hello" << std::endl;
}