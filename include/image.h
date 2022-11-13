#ifndef IMAGE_H_
#define IMAGE_H_
#include <string>
#include <vector>

using std::string;
using std::vector;

class Image {
 public:
  explicit Image() = default;
  /*!
   * Read a image from image_path by stb_imread. All image will read as bgr mode.
   * If read failed, some log will output by glog, and is_empty() will return true.
   * @param image_path
   */
  explicit Image(const string &image_path);
  /*!
   * Read image from @param image_path and channel will set as @param required_channel according stb_imread rule.
   * @param image_path
   * @param required_channel
   */
  explicit Image(const string &image_path, const int &required_channel);
  /*!
   * decode image from a iostream buffer.
   * @param buf
   */
  explicit Image(const std::vector<unsigned char>& buf);
  Image(const Image &origin_image);
  Image(const int &width, const int &height, const int &channel);
  Image(const int &width, const int &height, const int &channel, unsigned char *data);
  ~Image();
  int read(const string &image_path, const int &required_channel);
  int read(const std::vector<unsigned char>& buf);
  void create(const int &width, const int &height, const int &channel);
  void create(const int &width, const int &height, const int &channel, unsigned char *data);
  int width() const;
  int height() const;
  int channel() const;
  int stride() const;
  int size() const;
  int image_size() const;
  bool is_empty() const;
  unsigned char *data() const;
  void write_jpg(const string &save_path, int quality);
  vector<char> get_bytes(const string &tmp_save_path);
  void add_alpha();
  void delete_alpha();
  void resize_bilinear_c3(int target_width, int target_height);
  int fast_mean_filter(const int &radius);
  void laplace_sharpen(const int &ratio);
  void lut_filter_512(const Image &lut_image, const int &ratio);
  void change_channel(int channel1, int channel2);
  void reverse_color();
  void threshold_binary(const int &threshold);
  void bgr2gray();
  void gray_stretch(const int &x1, const int &y1, const int &x2, const int &y2);
  void histogram_equalize();

 private:
  unsigned char *data_ = nullptr;
  float *hsv_data_ = nullptr;
  int width_;
  int height_;
  int channel_;
  bool is_empty_ = true;
  int fast_mean_filter_c1(const int &radius);
  int fast_mean_filter_c3(const int &radius);
  void bgr2hsv();
};
#endif //IMAGE_H_