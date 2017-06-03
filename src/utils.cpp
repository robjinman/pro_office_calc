#include <cmath>
#include <limits>
#include "utils.hpp"


using std::string;
using std::istream;
using std::ostream;


string readString(istream& is) {
  int nBytes = 0;
  is.read(reinterpret_cast<char*>(&nBytes), sizeof(nBytes));

  char buf[nBytes];
  is.read(buf, nBytes);

  return string(buf, nBytes);
}

void writeString(ostream& os, const string& s) {
  int nBytes = s.size();

  os.write(reinterpret_cast<const char*>(&nBytes), sizeof(nBytes));
  os.write(s.data(), nBytes);
}
