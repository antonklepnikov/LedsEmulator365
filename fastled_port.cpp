////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     Porting the CHSV, CRGB types and 
     some functions from the FastLed library  ***/


////////////////////////////////////////////////////////////////////////////////

#include "fastled_port.h"


////////////////////////////////////////////////////////////////////////////////

void prandom_init()
{
	std::srand(std::time(nullptr));
}


int prandom_range(int upper)
{	
	if(upper > RAND_MAX) { upper = RAND_MAX; }
	double u = upper;
	return 1 + static_cast<int>((u * rand() / (RAND_MAX + 1.0)));
}


int check_8(int val)
{
    if(val > 255) { return 255; }
    if(val < 0) { return 0; }
    return val;
}


uint8_t qadd8( uint8_t i, uint8_t j)
{
    unsigned int t = i + j;
    if( t > 255) t = 255;
    return t;
}


uint8_t qsub8( uint8_t i, uint8_t j)
{
    int t = i - j;
    if( t < 0) t = 0;
    return t;
}


uint8_t qmul8( uint8_t i, uint8_t j)
{
    unsigned p = (unsigned)i * (unsigned)j;
    if( p > 255) p = 255;
    return p;
}


uint8_t scale8( uint8_t i, fract8 scale)
{
    return ((uint16_t)i * (uint16_t)(scale) ) >> 8;
}


void nscale8x3( uint8_t& r, uint8_t& g, uint8_t& b, fract8 scale)
{
    r = ((int)r * (int)(scale) ) >> 8;
    g = ((int)g * (int)(scale) ) >> 8;
    b = ((int)b * (int)(scale) ) >> 8;
}


void nscale8x3_video( uint8_t& r, uint8_t& g, uint8_t& b, fract8 scale)
{
    uint8_t nonzeroscale = (scale != 0) ? 1 : 0;
    r = (r == 0) ? 0 : (((int)r * (int)(scale) ) >> 8) + nonzeroscale;
    g = (g == 0) ? 0 : (((int)g * (int)(scale) ) >> 8) + nonzeroscale;
    b = (b == 0) ? 0 : (((int)b * (int)(scale) ) >> 8) + nonzeroscale;
}


uint8_t scale8_video(uint8_t i, fract8 scale)
{
    uint8_t j = (((int)i * (int)scale) >> 8) + ((i&&scale)?1:0);
    return j;
}


////////////////////////////////////////////////////////////////////////////////
/// hsv2rgb_rainbow()///////////////////////////////////////////////////////////

// Sometimes the compiler will do clever things to reduce
// code size that result in a net slowdown, if it thinks that
// a variable is not used in a certain location.
// This macro does its best to convince the compiler that
// the variable is used in this location, to help control
// code motion and de-duplication that would result in a slowdown.
#define FORCE_REFERENCE(var)  asm volatile( "" : : "r" (var) )

#define K255 255
#define K171 171
#define K170 170
#define K85  85

void hsv2rgb_rainbow(const CHSV& hsvPixel, CRGB& rgbPixel)
{
    // Yellow has a higher inherent brightness than
    // any other color; 'pure' yellow is perceived to
    // be 93% as bright as white.  In order to make
    // yellow appear the correct relative brightness,
    // it has to be rendered brighter than all other
    // colors.
    // Level Y1 is a moderate boost, the default.
    // Level Y2 is a strong boost.
    const uint8_t Y1 = 0;
    const uint8_t Y2 = 1;
    
    uint8_t hue = hsvPixel.h;
    uint8_t sat = hsvPixel.s;
    uint8_t val = hsvPixel.v;
    
    uint8_t offset = hue & 0x1F; // 0..31
    
    // offset8 = offset * 8
    uint8_t offset8 = offset;
    {
        offset8 <<= 3;
    }
    
    uint8_t third = scale8( offset8, (256 / 3)); // max = 85
    
    uint8_t r, g, b;
    
    if( ! (hue & 0x80) ) {
        // 0XX
        if( ! (hue & 0x40) ) {
            // 00X
            //section 0-1
            if( ! (hue & 0x20) ) {
                // 000
                //case 0: // R -> O
                r = K255 - third;
                g = third;
                b = 0;
                FORCE_REFERENCE(b);
            } else {
                // 001
                //case 1: // O -> Y
                if( Y1 ) {
                    r = K171;
                    g = K85 + third ;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
                if( Y2 ) {
                    r = K170 + third;
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    g = K85 + twothirds;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
            }
        } else {
            //01X
            // section 2-3
            if( !  (hue & 0x20) ) {
                // 010
                //case 2: // Y -> G
                if( Y1 ) {
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    r = K171 - twothirds;
                    g = K170 + third;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
                if( Y2 ) {
                    r = K255 - offset8;
                    g = K255;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
            } else {
                // 011
                // case 3: // G -> A
                r = 0;
                FORCE_REFERENCE(r);
                g = K255 - third;
                b = third;
            }
        }
    } else {
        // section 4-7
        // 1XX
        if( ! (hue & 0x40) ) {
            // 10X
            if( ! ( hue & 0x20) ) {
                // 100
                //case 4: // A -> B
                r = 0;
                FORCE_REFERENCE(r);
                //uint8_t twothirds = (third << 1);
                uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                g = K171 - twothirds; //K170?
                b = K85  + twothirds;
                
            } else {
                // 101
                //case 5: // B -> P
                r = third;
                g = 0;
                FORCE_REFERENCE(g);
                b = K255 - third;
                
            }
        } else {
            if( !  (hue & 0x20)  ) {
                // 110
                //case 6: // P -- K
                r = K85 + third;
                g = 0;
                FORCE_REFERENCE(g);
                b = K171 - third;
                
            } else {
                // 111
                //case 7: // K -> R
                r = K170 + third;
                g = 0;
                FORCE_REFERENCE(g);
                b = K85 - third;
                
            }
        }
    }
    
    // Scale down colors if we're desaturated at all
    // and add the brightness_floor to r, g, and b.
    if( sat != 255 ) {
        if( sat == 0) {
            r = 255; b = 255; g = 255;
        } else {
            uint8_t desat = 255 - sat;
            desat = scale8_video( desat, desat);

            uint8_t satscale = 255 - desat;
            //satscale = sat; // uncomment to revert to pre-2021 saturation behavior

            //nscale8x3_video( r, g, b, sat);
            if( r ) r = scale8( r, satscale) + 1;
            if( g ) g = scale8( g, satscale) + 1;
            if( b ) b = scale8( b, satscale) + 1;
            
            uint8_t brightness_floor = desat;
            r += brightness_floor;
            g += brightness_floor;
            b += brightness_floor;
        }
    }
    
    // Now scale everything down if we're at value < 255.
    if( val != 255 ) {
        
        val = scale8_video( val, val);
        if( val == 0 ) {
            r=0; g=0; b=0;
        } else {
            // nscale8x3_video( r, g, b, val);
            if( r ) r = scale8( r, val) + 1;
            if( g ) g = scale8( g, val) + 1;
            if( b ) b = scale8( b, val) + 1;
        }
    }
    
    // Here we have the old AVR "missing std X+n" problem again
    // It turns out that fixing it winds up costing more than
    // not fixing it.
    // To paraphrase Dr Bronner, profile! profile! profile!
    //asm volatile(  ""  :  :  : "r26", "r27" );
    //asm volatile (" movw r30, r26 \n" : : : "r30", "r31");
    rgbPixel.r = r;
    rgbPixel.g = g;
    rgbPixel.b = b;
}


////////////////////////////////////////////////////////////////////////////////
/// CRGB::SetParity()///////////////////////////////////////////////////////////

void CRGB::SetParity(uint8_t parity)
{
    uint8_t curparity = GetParity();
    if(parity == curparity) { return; }
    if(parity) {
        // Going 'up'
        if((b > 0) && (b < 255)) {
            if(r == g && g == b) { 
                ++r; 
                ++g; 
            }
             ++b;
         } 
         else if((r > 0) && (r < 255)) { ++r; } 
         else if((g > 0) && (g < 255)) { ++g; } 
         else {
            if(r == g && g == b) {
                r ^= 0x01;
                g ^= 0x01;
            }
            b ^= 0x01;
            }
        } else {
            // Going 'down'
            if(b > 1) {
                if(r == g && g == b) {
                    --r;
                    --g;
                }
                --b;
            } 
            else if(g > 1) { --g; } 
            else if(r > 1) { --r; } 
            else {
                if(r == g && g == b) {
                    r ^= 0x01;
                    g ^= 0x01;
                }
                b ^= 0x01;
            }
        }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void fill_solid( struct CRGB * targetArray, int numToFill,
                 const struct CRGB& color)
{
    for( int i = 0; i < numToFill; ++i) {
        targetArray[i] = color;
    }
}


void fill_rainbow(struct CRGB * targetArray, int numToFill,
                  uint8_t initialhue, uint8_t deltahue) 
{
    CHSV pixel;
    pixel.h = initialhue;
    pixel.s = 240;
    pixel.v = 255;
    for(int i = 0; i < numToFill; ++i) {
        targetArray[i] = pixel;
        pixel.h += deltahue;
    }
}


////////////////////////////////////////////////////////////////////////////////
