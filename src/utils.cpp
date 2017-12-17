#include <cmath>
#include <limits>
#include <vector>
#include <sstream>
#include <QRect>
#include <QRectF>
#include "utils.hpp"


using std::string;
using std::stringstream;
using std::istream;
using std::ostream;
using std::vector;


#ifdef DEBUG
ostream& operator<<(ostream& os, const QRect& rect) {
  os << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height();
  return os;
}
ostream& operator<<(ostream& os, const QRectF& rect) {
  os << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height();
  return os;
}
ostream& operator<<(ostream& os, const QPoint& p) {
  os << p.x() << ", " << p.y();
  return os;
}
ostream& operator<<(ostream& os, const QPointF& p) {
  os << p.x() << ", " << p.y();
  return os;
}
#endif

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

vector<string> splitString(const string& s, char delim) {
  vector<string> v;
  stringstream ss(s);

  string tok;
  while (std::getline(ss, tok, delim)) {
    v.push_back(tok);
  }

  return v;
}
