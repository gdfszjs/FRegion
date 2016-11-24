/* ColorGradient 
	code from:
	http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
*/
	
#ifndef COLOR_GRADIENT_H_
#define COLOR_GRADIENT_H_

class ColorGradient
{
private:
  struct ColorPoint  // Internal class used to store colors at different points in the gradient.
  {
    double r,g,b;      // Red, green and blue values of our color.
    double val;        // Position of our color along the gradient (between 0 and 1).
    ColorPoint(double red, double green, double blue, double value)
      : r(red), g(green), b(blue), val(value) {}
  };
  vector<ColorPoint> color;      // An array of color points in ascending value.
 
public:
  //-- Default constructor:
  ColorGradient()  {  createDefaultHeatMapGradient();  }
 
  //-- Inserts a new color point into its correct position:
  void addColorPoint(double red, double green, double blue, double value)
  {
    for(int i=0; i<(int)(color.size()); i++)  {
      if(value < color[i].val) {
        color.insert(color.begin()+i, ColorPoint(red,green,blue, value));
        return;  }}
    color.push_back(ColorPoint(red,green,blue, value));
  }
 
  //-- Inserts a new color point into its correct position:
  void clearGradient() { color.clear(); }
 
  //-- Places a 5 color heapmap gradient into the "color" vector:
  void createDefaultHeatMapGradient()
  {
    color.clear();
    color.push_back(ColorPoint(0, 0, 1,   0.0));      // Blue.
    color.push_back(ColorPoint(0, 1, 1,   0.25));     // Cyan.
    color.push_back(ColorPoint(0, 1, 0,   0.5));      // Green.
    color.push_back(ColorPoint(1, 1, 0,   0.75));     // Yellow.
    color.push_back(ColorPoint(1, 0, 0,   1.0));      // Red.
  }
 
  //-- Inputs a (value) between 0 and 1 and outputs the (red), (green) and (blue)
  //-- values representing that position in the gradient.
  void getColorAtValue(const double value, double &red, double &green, double &blue)
  {
    if(color.size()==0)
      return;
 
    for(int i=0; i<(int)(color.size()); i++)
    {
      ColorPoint &currC = color[i];
      if(value < currC.val)
      {
        ColorPoint &prevC  = color[ max(0,i-1) ];
        double valueDiff    = (prevC.val - currC.val);
        double fractBetween = (valueDiff==0) ? 0 : (value - currC.val) / valueDiff;
        red   = (prevC.r - currC.r)*fractBetween + currC.r;
        green = (prevC.g - currC.g)*fractBetween + currC.g;
        blue  = (prevC.b - currC.b)*fractBetween + currC.b;
        return;
      }
    }
    red   = color.back().r;
    green = color.back().g;
    blue  = color.back().b;
    return;
  }
};

#endif // COLOR_GRADIENT_H_