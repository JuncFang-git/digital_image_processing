#include <iostream>
#include "glog/logging.h"
#include "opencv2/opencv.hpp"
#include "image.h"

void free_bug_test() {
  // for opencv class
//  cv::Mat cv_image(cv::Size(10, 10), CV_8UC3);
//  unsigned char * ptr_cv_data = cv_image.data;
//  LOG(INFO) << (void *)(cv_image.data) << " , " << (int)*cv_image.data;
//  LOG(INFO) << (void *)ptr_cv_data << " , " << (int)*ptr_cv_data;
//
//  delete[] ptr_cv_data;
//  LOG(INFO) << "----------";
//  LOG(INFO) << (void *)cv_image.data << " , " << (int)*cv_image.data;
//  LOG(INFO) << (void *)ptr_cv_data << " , " << (int)*ptr_cv_data;

  // for Image class
  Image image(10, 10, 3);
  unsigned char * ptr_data = image.data();
  LOG(INFO) << (void *)image.data() << " , " << (int)*image.data();
  LOG(INFO) << (void *)ptr_data << " , " << (int)*ptr_data;

  delete[] ptr_data;

  LOG(INFO) << (void *)image.data() << " , " << (int)*image.data();
  LOG(INFO) << (void *)ptr_data << " , " << (int)*ptr_data;
}
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
void gray_stretch_test(Image &input) {
  LOG(INFO) << "Begin " << __func__ << " ...";
  Image input_copy(input);
  input_copy.bgr2gray();
  input_copy.gray_stretch(100, 50, 150, 200);
  input_copy.write_jpg("../demo/result/gray_stretch.png", 100);
  LOG(INFO) << "End " << __func__ << " ...";
}
void histogram_equalize_test(Image &input) {
  LOG(INFO) << "Begin " << __func__ << " ...";
  Image input_copy(input);
  input_copy.bgr2gray();
  input_copy.histogram_equalize();
  input_copy.write_jpg("../demo/result/histogram_equalize.png", 100);
  LOG(INFO) << "End " << __func__ << " ...";
}
int main() {
  std::cout << "hello" << std::endl;
  Image input_image("../demo/test_image/lenna.png");
  // free_bug_test
//  free_bug_test();
  // reverse_color
  reverse_color_test(input_image);
  // bgr2gray_test
  bgr2gray_test(input_image);
  // threshold_binary_test
  threshold_binary_test(input_image);
  // gray_stretch_test
  gray_stretch_test(input_image);
  // histogram_equalize_test
  histogram_equalize_test(input_image);
}