#include "image.h"
#include <cstddef>
#include <cstring>
#include <fstream>
#include <climits>
#include <cmath>
#include <algorithm>
#include <vector>
#include "glog/logging.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif // STB_IMAGE_IMPLEMENTATION
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif // STB_IMAGE_WRITE_IMPLEMENTATION

#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define CLIP3(x, a, b) MIN2(MAX2(a,x), b)

using namespace std;

Image::Image(const string &image_path) {
  int ret = 0;
  ret = read(image_path, 3);
  if (ret == 0) {
    is_empty_ = false;
  }
}

Image::Image(const string &image_path, const int &required_channel) {
  int ret = 0;
  ret = read(image_path, required_channel);
  if (ret == 0) {
    is_empty_ = false;
  }
}

Image::Image(const std::vector<unsigned char>& data) {
  int ret = 0;
  ret = read(data);
  if (ret == 0) {
    is_empty_ = false;
  }
}

Image::Image(const Image &origin_image) {
  int ret = 0;
  width_ = origin_image.width();
  height_ = origin_image.height();
  channel_ = origin_image.channel();
  create(origin_image.width(), origin_image.height(), origin_image.channel(), origin_image.data());
  is_empty_ = false;
}

Image::Image(const int &width, const int &height, const int &channel) : width_(width), height_(height), channel_(channel){
  create(width, height, channel);
  is_empty_ = false;
}

Image::Image(const int &width, const int &height, const int &channel, unsigned char *data) : width_(width), height_(height), channel_(channel){
  create(width, height, channel, data);
  is_empty_ = false;
}

Image::~Image() {
  if (data_ != nullptr) {
    delete[] data_;
    data_ = nullptr;
  }
}

int Image::width() const {
  return width_;
}

int Image::height()const {
  return height_;
}

int Image::channel() const {
  return channel_;
}

int Image::stride() const {
  return width_ * channel_;
}

int Image::size() const {
  return sizeof(unsigned char) * width_ * height_ * channel_;
}

int Image::image_size() const {
  return width_ * height_;
}

bool Image::is_empty() const {
  return is_empty_;
}

unsigned char *Image::data() const{
  return data_;
}

void Image::create(const int &width, const int &height, const int &channel) {
  if (! is_empty_) {
    LOG(ERROR) << "can not call crate function from a not empty image!";
    return;
  }
  int num_pixel = width * height * channel;
  data_ = new unsigned char [num_pixel];
  memset(data_, 255, sizeof (unsigned char ) * num_pixel);
  width_ = width;
  height_ = height;
  channel_ = channel;
  is_empty_ = false;
}

void Image::create(const int &width, const int &height, const int &channel, unsigned char *data) {
  if (! is_empty_) {
    LOG(ERROR) << "can not call crate function from a not empty image!";
    return;
  }
  int num_pixel = width * height * channel;
  data_ = new unsigned char [num_pixel];
  memcpy(data_, data, sizeof(unsigned char) * num_pixel);
  width_ = width;
  height_ = height;
  channel_ = channel;
  is_empty_ = false;
}

void Image::change_channel(int channel1, int channel2) {
  unsigned char tmp = 0;
  unsigned char *channel1_ptr = data_ + channel1;
  unsigned char *channel2_ptr = data_ + channel2;
  for (int i = 0; i < width_; ++i) {
    for (int j = 0; j < height_; ++j) {
      tmp = *channel1_ptr;
      *channel1_ptr = *channel2_ptr;
      *channel2_ptr = tmp;
      channel1_ptr += channel_;
      channel2_ptr += channel_;
    }
  }
}

int Image::read(const string &image_path, const int &required_channel) {
  // assert
  if (required_channel != 1 && required_channel != 3 && required_channel != 4) {
    LOG(ERROR) << "Image::read need required_channel in [1, 3, 4], but got " << required_channel;
    return -1;
  }
  //get image format is RGB
  unsigned char *read_data = stbi_load(image_path.c_str(), &width_, &height_, &channel_, required_channel);
  if (read_data == nullptr) {
    LOG(ERROR) << "Read image error at \"" << image_path << "\"\n";
    return -1;
  }
  channel_ = required_channel;
  // copy data
  create(width_, height_, channel_, read_data);
  free(read_data);
  read_data = nullptr;
  if (channel_ == 3 || channel_ == 4) {
    // change channel from RGB/RGBA to BGR/BGRA
    change_channel(0, 2);
  }
  is_empty_ = false;
  return 0;
}

int Image::read(const std::vector<unsigned char>& buf) {
  unsigned char *read_data = stbi_load_from_memory(buf.data(), buf.size(), &width_, &height_, &channel_, 3);
  if (read_data == nullptr) {
    LOG(ERROR) << "Read image error" << buf.size();
    return -1;
  }
  channel_ = 3;
  // copy data
  create(width_, height_, channel_, read_data);
  free(read_data);
  read_data = nullptr;
  // change channel to BGR
  change_channel(0, 2);
  is_empty_ = false;
  return 0;
}

void Image::write_jpg(const string &save_path, int quality) {
  if (channel_ != 4 && channel_ != 3 && channel_ != 1) {
    LOG(ERROR) << "Image::write_jpg can be saved just when channel in [1, 3, 4], but got" << channel_;
    return;
  }
  if (channel_ != 1) {
    // change channel to RGB/RGBA
    change_channel(0, 2);
  }
  stbi_write_jpg(save_path.c_str(), width_, height_, channel_, data_, quality);
  if (channel_ != 1) {
    // change channel to BGR/BGRA
    change_channel(0, 2);
  }
}

vector<char> Image::get_bytes(const string &tmp_save_path) {
  write_jpg(tmp_save_path, 80); // save as rgb
  std::ifstream input(tmp_save_path.c_str(), std::ios::binary);
  std::vector<char> bytes(
      (std::istreambuf_iterator<char>(input)),
      (std::istreambuf_iterator<char>()));
  input.close();
  return bytes;
}

void Image::add_alpha() {
  if (channel_ != 3) {
    LOG(ERROR) << "Only 3 channel image can add alpha channel, but got " << channel_ << " channel image";
    return;
  }
  auto *tmp_data = new unsigned char [width_ * height_ * 4];
  memset(tmp_data, 255, width_ * height_ * 4 * 1);
//  memcpy(tmp_data, data_, width_ * height_ * 3);
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      tmp_data[(i * width_ + j) * 4    ] = data_[(i * width_ + j) * 3    ];
      tmp_data[(i * width_ + j) * 4 + 1] = data_[(i * width_ + j) * 3 + 1];
      tmp_data[(i * width_ + j) * 4 + 2] = data_[(i * width_ + j) * 3 + 2];
      tmp_data[(i * width_ + j) * 4 + 3] = 255;
    }
  }
  delete[] data_;
  data_ = tmp_data;
  channel_ = 4;
}

void Image::delete_alpha() {
  if (channel_ != 4) {
    LOG(ERROR) << "Only 4 channel image can delete alpha channel, but got " << channel_ << " channel image";
    return;
  }
  auto *tmp_data = new unsigned char [width_ * height_ * 3];
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      tmp_data[(i * width_ + j) * 3    ] = data_[(i * width_ + j) * 4    ];
      tmp_data[(i * width_ + j) * 3 + 1] = data_[(i * width_ + j) * 4 + 1];
      tmp_data[(i * width_ + j) * 3 + 2] = data_[(i * width_ + j) * 4 + 2];
    }
  }
  delete[] data_;
  data_ = tmp_data;
  channel_ = 3;
}

#define SATURATE_CAST_SHORT(X) (short)::std::min(::std::max((int)(X + (X >= 0.f ? 0.5f : -0.5f)), SHRT_MIN), SHRT_MAX);
void Image::resize_bilinear_c3(int target_width, int target_height) {
  if (channel_ == 3) {
    auto target_data = new unsigned char [target_width * target_height * 3];
    const int INTER_RESIZE_COEF_BITS = 11;
    const int INTER_RESIZE_COEF_SCALE = 1 << INTER_RESIZE_COEF_BITS;
    double scale_x = (double)width_ / target_width;
    double scale_y = (double)height_ / target_height;
    int *buf = new int[target_width + target_height + target_width + target_height + 3 * target_width + 3];

    int *x_ofs = buf;//new int[w];
    int *y_ofs = buf + target_width;//new int[h];
    short *i_alpha = (short*)(buf + target_width + target_height);//new short[w * 2];
    short *i_beta = (short*)(buf + target_width + target_height + target_width);//new short[h * 2];
    float fx;
    float fy;
    int sx;
    int sy;

    for (int dx = 0; dx < target_width; dx++) {
      fx = (float)((dx + 0.5) * scale_x - 0.5);
      sx = static_cast<int>(floor(fx));
      fx -= sx;
      if (sx < 0) {
        sx = 0;
        fx = 0.f;
      }
      if (sx >= width_ - 1) {
        sx = width_ - 2;
        fx = 1.f;
      }
      x_ofs[dx] = sx * 3;
      float a0 = (1.f - fx) * INTER_RESIZE_COEF_SCALE;
      float a1 = fx  * INTER_RESIZE_COEF_SCALE;
      i_alpha[dx * 2] = SATURATE_CAST_SHORT(a0);
      i_alpha[dx * 2 + 1] = SATURATE_CAST_SHORT(a1);
    }

    for (int dy = 0; dy < target_height; dy++) {
      fy = (float)((dy + 0.5) * scale_y - 0.5);
      sy = static_cast<int>(floor(fy));
      fy -= sy;
      if (sy < 0) {
        sy = 0;
        fy = 0.f;
      }
      if (sy >= height_ - 1) {
        sy = height_ - 2;
        fy = 1.f;
      }
      y_ofs[dy] = sy;
      float b0 = (1.f - fy) * INTER_RESIZE_COEF_SCALE;
      float b1 = fy  * INTER_RESIZE_COEF_SCALE;
      i_beta[dy * 2] = SATURATE_CAST_SHORT(b0);
      i_beta[dy * 2 + 1] = SATURATE_CAST_SHORT(b1);
    }

    // loop body
    short* rows0 = (short*)(buf + 2 * (target_width + target_height));
    short* rows1 = rows0 + 3 * target_width + 2;
    int prev_sy1 = -2;
    for (int dy = 0; dy < target_height; dy++) {
      int sy = y_ofs[dy];
      if (sy == prev_sy1) {
        // reuse all rows
      }
      else if (sy == prev_sy1 + 1) {
        // hresize one row
        short* rows0_old = rows0;
        rows0 = rows1;
        rows1 = rows0_old;
        const unsigned char *S1 = data_ + stride() * (sy + 1);
        const short* i_alphap = i_alpha;
        short* rows1p = rows1;
        for (int dx = 0; dx < target_width; dx++) {
          int sx = x_ofs[dx];
          short a0 = i_alphap[0];
          short a1 = i_alphap[1];

          const unsigned char* S1p = S1 + sx;
          rows1p[0] = (S1p[0] * a0 + S1p[3] * a1) >> 4;
          rows1p[1] = (S1p[1] * a0 + S1p[4] * a1) >> 4;
          rows1p[2] = (S1p[2] * a0 + S1p[5] * a1) >> 4;

          i_alphap += 2;
          rows1p += 3;
        }
      } else {
        // hresize two rows
        const unsigned char *S0 = data_ + stride() * (sy);
        const unsigned char *S1 = data_ + stride() * (sy + 1);

        const short* i_alphap = i_alpha;
        short* rows0p = rows0;
        short* rows1p = rows1;
        for (int dx = 0; dx < target_width; dx++) {
          int sx = x_ofs[dx];
          short a0 = i_alphap[0];
          short a1 = i_alphap[1];
          const unsigned char* S0p = S0 + sx;
          const unsigned char* S1p = S1 + sx;
          rows0p[0] = (S0p[0] * a0 + S0p[3] * a1) >> 4;
          rows0p[1] = (S0p[1] * a0 + S0p[4] * a1) >> 4;
          rows0p[2] = (S0p[2] * a0 + S0p[5] * a1) >> 4;
          rows1p[0] = (S1p[0] * a0 + S1p[3] * a1) >> 4;
          rows1p[1] = (S1p[1] * a0 + S1p[4] * a1) >> 4;
          rows1p[2] = (S1p[2] * a0 + S1p[5] * a1) >> 4;
          i_alphap += 2;
          rows0p += 3;
          rows1p += 3;
        }
      }
      prev_sy1 = sy;
      // vresize
      short b0 = i_beta[0];
      short b1 = i_beta[1];
      short* rows0p = rows0;
      short* rows1p = rows1;
      unsigned char* Dp = target_data + (target_width * 3) * (dy);
      int nn = 0;
      int remain = (target_width * 3) - (nn << 3);
      for (; remain; --remain) {
        *Dp++ = (unsigned char)(((short)((b0 * (short)(*rows0p++)) >> 16) + (short)((b1 * (short)(*rows1p++)) >> 16) + 2) >> 2);
      }
      i_beta += 2;
    }
    delete[] buf;
    // change data
    delete[] data_;
    data_ = target_data;
    target_data = nullptr;
    width_ = target_width;
    height_ = target_height;
  }
}
int Image::fast_mean_filter_c3(const int &radius_) {
  if (radius_ == 0) {
    return 0;
  }
  // adjust radius
  int short_side = MIN2(width_, height_);
  int radius = MIN2(radius_, short_side / 3);

  int unit = 3, t = 0, t1 = 0;
  int i, j, k;
  int block = (radius << 1) + 1;
  int win_size = block * block;
  long sum_b = 0, sum_g = 0, sum_r = 0;
  unsigned char *p_src = data_;
  // malloc temp data
  int *temp = (int*)malloc(sizeof(int)* width_ * unit);
  if (temp == nullptr) {
    LOG(ERROR) << "Image fast_mean_filter malloc error!";
    return -1;
  }
  memset(temp,0,sizeof(int) * width_ * unit);
  // malloc temp result
  auto *temp_res = (unsigned char *)malloc(sizeof(unsigned char) * width_ * height_ * channel_);
  if (temp_res == nullptr) {
    LOG(ERROR) << "Image fast_mean_filter malloc error!";
    return -1;
  }
  memset(temp_res,0,sizeof(unsigned char) * width_ * height_ * channel_);

  for (k = -radius; k <= radius; k++) {
    for (j = 0; j< width_; j++) {
      t = j * unit;
      t1 = abs(k) * width_ * channel_;
      temp[t + 0] += p_src[t + 0 + t1];
      temp[t + 1] += p_src[t + 1 + t1];
      temp[t + 2] += p_src[t + 2 + t1];
    }
  }
  for (i = 0; i < height_; i++) {
    sum_b = sum_g = sum_r = 0;
    for (j = -radius; j <= radius; j++) {
      t = abs(j) * unit;
      sum_b += temp[t + 0];
      sum_g += temp[t + 1];
      sum_r += temp[t + 2];
    }
    for (j = 0; j < width_ - 1; j++) {
      t = j * unit + i * channel_ * width_;
      temp_res[t + 0] = (int)(sum_b / win_size);
      temp_res[t + 1] = (int)(sum_g / win_size);
      temp_res[t + 2] = (int)(sum_r / win_size);
      t = abs(j - radius) * unit;
      t1 = (j + radius + 1) % width_ * unit;
      sum_b = sum_b - temp[t] + temp[t1];
      sum_g = sum_g - temp[t + 1] + temp[t1 + 1];
      sum_r = sum_r - temp[t + 2] + temp[t1 + 2];
    }
    t = (width_ - 1) * unit + i * channel_ * width_;
    temp_res[t + 0] = (int)(sum_b / win_size);
    temp_res[t + 1] = (int)(sum_g / win_size);
    temp_res[t + 2] = (int)(sum_r / win_size);
    for (k = 0; k < width_; k++) {
      t = k * unit + abs(i - radius) *  channel_ * width_;
      t1 = k * unit + (i + radius + 1) % height_ *  channel_ * width_;
      temp[k * unit] = temp[k * unit] - p_src[t] + p_src[t1];
      temp[k * unit + 1] = temp[k * unit + 1] - p_src[t + 1] + p_src[t1 + 1];
      temp[k * unit + 2] = temp[k * unit + 2] - p_src[t + 2] + p_src[t1 + 2];
    }
  }
  i = height_ - 1;
  for (j = -radius; j <= radius; j++) {
    t = abs(j) * unit;
    sum_b += temp[t];
    sum_g += temp[t + 1];
    sum_r += temp[t + 2];
  }
  for (j = 0; j < width_ - 1; j++) {
    t = j * unit + i * channel_ * width_;
    temp_res[t + 0] = (int)(sum_b / win_size);
    temp_res[t + 1] = (int)(sum_g / win_size);
    temp_res[t + 2] = (int)(sum_r / win_size);
    t = abs(j - radius) * unit;
    t1 = (j + radius + 1) % width_ * unit;
    sum_b = sum_b - temp[t] + temp[t1];
    sum_g = sum_g - temp[t + 1] + temp[t1 + 1];
    sum_r = sum_r - temp[t + 2] + temp[t1 + 2];
  }
  t = (width_ - 1) * unit + i * channel_ * width_;
  temp_res[t + 0] = (int)(sum_b / win_size);
  temp_res[t + 1] = (int)(sum_g / win_size);
  temp_res[t + 2] = (int)(sum_r / win_size);
  memcpy(data_, temp_res, sizeof(unsigned char) * width_ * height_ * channel_);
  free(temp_res);
  free(temp);
  return 0;
}

int Image::fast_mean_filter_c1(const int &radius_) {
  if (radius_ == 0) {
    return 0;
  }
  // adjust radius
  int short_side = MIN2(width_, height_);
  int radius = MIN2(radius_, short_side / 3);

  int unit = 1, t = 0, t1 = 0;
  int i, j, k;
  int block = (radius << 1) + 1;
  int win_size = block * block;
  long sum_pixel;
  unsigned char *p_src = data_;
  // malloc temp data
  int *temp = (int*)malloc(sizeof(int)* width_ * unit);
  if (temp == nullptr) {
    LOG(ERROR) << "Image fast_mean_filter malloc error!";
    return -1;
  }
  memset(temp,0,sizeof(int) * width_ * unit);
  // malloc temp result
  auto *temp_res = (unsigned char *)malloc(sizeof(unsigned char) * width_ * height_ * channel_);
  if (temp_res == nullptr) {
    LOG(ERROR) << "Image fast_mean_filter malloc error!";
    return -1;
  }
  memset(temp_res,0,sizeof(unsigned char) * width_ * height_ * channel_);

  for (k = -radius; k <= radius; k++) {
    for (j = 0; j< width_; j++) {
      t = j * unit;
      t1 = abs(k) * width_ * channel_;
      temp[t + 0] += p_src[t + 0 + t1];
    }
  }
  for (i = 0; i < height_; i++) {
    sum_pixel = 0;
    for (j = -radius; j <= radius; j++) {
      t = abs(j) * unit;
      sum_pixel += temp[t + 0];
    }
    for (j = 0; j < width_ - 1; j++) {
      t = j * unit + i * channel_ * width_;
      temp_res[t + 0] = (int)(sum_pixel / win_size);
      t = abs(j - radius) * unit;
      t1 = (j + radius + 1) % width_ * unit;
      sum_pixel = sum_pixel - temp[t] + temp[t1];
    }
    t = (width_ - 1) * unit + i * channel_ * width_;
    temp_res[t + 0] = (int)(sum_pixel / win_size);
    for (k = 0; k < width_; k++) {
      t = k * unit + abs(i - radius) *  channel_ * width_;
      t1 = k * unit + (i + radius + 1) % height_ *  channel_ * width_;
      temp[k * unit] = temp[k * unit] - p_src[t] + p_src[t1];
    }
  }
  i = height_ - 1;
  for (j = -radius; j <= radius; j++) {
    t = abs(j) * unit;
    sum_pixel += temp[t];
  }
  for (j = 0; j < width_ - 1; j++) {
    t = j * unit + i * channel_ * width_;
    temp_res[t + 0] = (int)(sum_pixel / win_size);
    t = abs(j - radius) * unit;
    t1 = (j + radius + 1) % width_ * unit;
    sum_pixel = sum_pixel - temp[t] + temp[t1];
  }
  t = (width_ - 1) * unit + i * channel_ * width_;
  temp_res[t + 0] = (int)(sum_pixel / win_size);
  memcpy(data_, temp_res, sizeof(unsigned char) * width_ * height_ * channel_);
  free(temp_res);
  free(temp);
  return 0;
}

int Image::fast_mean_filter(const int &radius) {
  int ret = 0;
  if (channel_ == 1) {
    ret = fast_mean_filter_c1(radius);
  }
  else if (channel_ == 3) {
    ret = fast_mean_filter_c3(radius);
  }
  return ret;
}

void Image::laplace_sharpen(const int &ratio) {
  Image tmp_image(width_, height_, channel_, data_);
  int stride = width_ * channel_;
  int k = ratio * 128 / 100;
  int nk = 128 - k;
  for (int i = 1; i < height_ - 1; i++) {
    for (int j = 1; j < width_ - 1; j++) {
      int pos = i * stride + j * 3;
      int b = 5 * tmp_image.data()[pos] - (tmp_image.data()[pos - 3] + tmp_image.data()[pos + 3] + tmp_image.data()[pos - stride] + tmp_image.data()[pos + stride]);
      int g = 5 * tmp_image.data()[pos + 1] - (tmp_image.data()[pos - 3 + 1] + tmp_image.data()[pos + 3 + 1] + tmp_image.data()[pos - stride + 1] + tmp_image.data()[pos + stride + 1]);
      int r = 5 * tmp_image.data()[pos + 2] - (tmp_image.data()[pos - 3 + 2] + tmp_image.data()[pos + 3 + 2] + tmp_image.data()[pos - stride + 2] + tmp_image.data()[pos + stride + 2]);
      data_[pos] = CLIP3((data_[pos] * nk + b * k) >> 7, 0, 255);
      data_[pos + 1] = CLIP3((data_[pos + 1] * nk + g * k) >> 7, 0, 255);
      data_[pos + 2] = CLIP3((data_[pos + 2] * nk + r * k) >> 7, 0, 255);
    }
  }
}

void Image::lut_filter_512(const Image &lut_image, const int &ratio) {
  int r, g,  b, offset, pos, nx, ny, k_;
  int stride = width_ * channel_;
  offset = stride - (width_ * 3);
  unsigned char *p_src = data_;
  int k = ratio * 128 / 100;
  int nk = 128 - k;
  for(int i = 0; i < height_; i++) {
    for(int j = 0; j < width_; j++) {
      b = p_src[0];
      g = p_src[1];
      r = p_src[2];
      k_ = (b >> 2);
      nx = (int)(r >> 2) + ((k_ - ((k_ >> 3) << 3)) << 6);
      ny = (int)(((b >> 5) << 6) + (g >> 2));
      pos = (nx * 3) + (ny * 512 * 3);
      b = lut_image.data()[pos + 0];
      g = lut_image.data()[pos + 1];
      r = lut_image.data()[pos + 2];
      p_src[0] = CLIP3((b * k + p_src[0] * nk) >> 7, 0, 255);
      p_src[1] = CLIP3((g * k + p_src[1] * nk) >> 7, 0, 255);
      p_src[2] = CLIP3((r * k + p_src[2] * nk) >> 7, 0, 255);
      p_src += 3;
    }
    p_src  +=  offset;
  }
}

void Image::reverse_color() {
  // assert
  if (channel_ != 3 && channel_ != 1) {
    LOG(ERROR) << "Not support " << channel_ << " channel image at reverse_color! Skip it!";
    return;
  }
  int pos = 0;
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      for (int k = 0; k < channel_; k ++) {
        pos = i * this->stride() + j * channel_ + k;
        data_[pos] = 255 - data_[pos];
      }
    }
  }
}
void Image::threshold_binary(const int &threshold) {
  // assert
  if (channel_ != 1) {
    LOG(ERROR) <<  "Image::threshold_binary need image channel is 1, but got " << channel_ << "! Skip it!";
    return;
  }

  // threashold
  int pos = 0;
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
        pos = i * this->stride() + j * channel_;
        data_[pos]  = data_[pos] < threshold ? 0 : 255;
    }
  }
}
void Image::bgr2gray() {
  // assert
  if (data_ == nullptr) {
    LOG(ERROR) << "Image::to_gray got an empty image! Skip it!";
    return;
  }

  //
  auto tmp_data_ = new unsigned char [width_ * height_];
  int pos_original = 0;
  int pos_gray = 0;
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      pos_original = i * this->stride() + j * channel_;
      pos_gray = i * width_ + j;
      // 心理学公式 Gray = 0.3*R + 0.59*G + 0.11*B 的位移写法
      tmp_data_[pos_gray] = (28 * data_[pos_original + 0] + 151 * data_[pos_original + 1] + 77 * data_[pos_original + 2]) >> 8; // average rgb
    }
  }

  // free and re-point
  delete data_;
  data_ = tmp_data_;
  tmp_data_ = nullptr;

  // set
  channel_ = 1;
}
void Image::gray_stretch(const int &x1, const int &y1, const int &x2, const int &y2) {
  // assert
  if (channel_ != 1) {
    LOG(ERROR) << "Image::contrast_stretching_gray need channel is 1, but got " << channel_ << ". Skip it!";
    return;
  }

  // gray_stretch
  int pos = 0;
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      pos = i * this->stride() + j * channel_;
      if (data_[pos] < x1) {
        data_[pos] = y1 / x1 * data_[pos];
      }
      else if (data_[pos] < x2) {
        data_[pos] = (y2 - y1) / (x2 - x1) * (data_[pos] - x1) + y1;
      }
      else {
        data_[pos] = (255 - y2) / (255 - x2) * (data_[pos] - x2) + y2;
      }
    }
  }
}
void Image::histogram_equalize() {
  // assert
  if (channel_ != 1) {
    LOG(ERROR) << "Image::histogram_equalize need channel is 1, but got " << channel_ << ". Skip it!";
    return;
  }

  int gray_static[256]{}; //每个灰度值的统计像素个数
  double gray_accumulation[256]{}; //从0到当前灰度值的累计值像素个数
  double gray_equal[256]{}; //变换函数

  // 统计灰度
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      int value = data_[i * width_ + j];
      gray_static[value]++;
    }
  }

  // 统计累积值
  gray_accumulation[0] = gray_static[0];
  for (int i = 1; i < 256; i++) {
    gray_accumulation[i] = gray_accumulation[i-1] + gray_static[i];
  }

  // 计算变换函数
  for (int i = 0; i < 256; i++) {
    gray_equal[i] = 255.f / (width_ * height_) * gray_accumulation[i];
  }

  // 进行映射
  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      unsigned char old_value = data_[i * width_ + j];
      data_[i * width_ + j] = gray_equal[old_value];
    }
  }
}
