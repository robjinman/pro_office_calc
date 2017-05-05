#include <iostream>
#include <fstream>


using namespace std;


struct __attribute__ ((packed)) BmpFileHeader {
  BmpFileHeader(uint32_t s)
    : size(s) {}

  char type[2] = {'B', 'M'};
  uint32_t size;
  uint16_t reserved1 = 0;
  uint16_t reserved2 = 0;
  uint32_t offset = 54;
};

struct __attribute__ ((packed)) BmpImgHeader {
  BmpImgHeader(uint32_t w, uint32_t h)
    : width(w), height(h) {}

  uint32_t size = 40;
  uint32_t width;
  uint32_t height;
  uint16_t planes = 1;
  uint16_t bitCount = 24;
  uint32_t compression = 0;
  uint32_t imgSize = 0;
  uint32_t xPxPerMetre = 0;
  uint32_t yPxPerMetre = 0;
  uint32_t colMapEntriesUsed = 0;
  uint32_t numImportantColours = 0;
};

struct __attribute__ ((packed)) BmpHeader {
  BmpHeader(uint32_t imgW, uint32_t imgH)
    : fileHdr(54 + imgW * imgH * 3),
      imgHdr(imgW, imgH) {}

  BmpFileHeader fileHdr;
  BmpImgHeader imgHdr;
};

int main(int argc, char** argv) {
  if (argc < 3) {
    cout << "Usage: " << argv[0] << " w h\n";
    return EXIT_SUCCESS;
  }

  uint32_t w = stoi(argv[1]);
  uint32_t h = stoi(argv[2]);

  uint8_t img[h][w][3];

  for (uint32_t i = 0; i < w; ++i) {
    for (uint32_t j = 0; j < h; ++j) {
      img[j][i][0] = rand() % 256;
      img[j][i][1] = rand() % 256;
      img[j][i][2] = rand() % 256;
    }
  }

  BmpHeader hdr(w, h);

  ofstream fout("out.bmp", ofstream::binary);
  fout.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
  fout.write(reinterpret_cast<char*>(img), sizeof(img));

  return EXIT_SUCCESS;
}
