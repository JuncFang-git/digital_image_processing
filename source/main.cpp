#include <iostream>
#include "glog/logging.h"
#include "image.h"

void reverse_color_test(const Image &input) {
  LOG(INFO) << "Begin " << __func__ << " ...";
  Image input_copy(input);
  input_copy.reverse_color();
  input_copy.write_jpg("../demo/result/reverse_color.png", 100);
  LOG(INFO) << "End " << __func__ << " ...";
}
void bgr2gray_test(Image &input) {
  LOG(INFO) << "Begin " << __func__ << " ...";
  Image input_copy(input);
  input_copy.bgr2gray();
  input_copy.write_jpg("../demo/result/bgr2gray.png", 100);
  LOG(INFO) << "End " << __func__ << " ...";
}
void threshold_binary_test(Image &input) {
  LOG(INFO) << "Begin " << __func__ << " ...";
  Image input_copy(input);
  input_copy.bgr2gray();
  input_copy.threshold_binary(122);
  input_copy.write_jpg("../demo/result/threshold_binary.png", 100);
  LOG(INFO) << "End " << __func__ << " ...";
}

int main() {
  std::cout << "hello" << std::endl;
  Image input_image("../demo/test_image/lenna.png");
  // reverse_color
  reverse_color_test(input_image);
  // bgr2gray_test
  bgr2gray_test(input_image);
  // threshold_binary_test
  threshold_binary_test(input_image);
}