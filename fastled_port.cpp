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


uint8_t scale8(uint8_t i, fract8 scale)
{
    return ((uint16_t)i * (uint16_t)(scale) ) >> 8;
}


uint8_t scale8_video(uint8_t i, fract8 scale)
{
    uint8_t j = (((int)i * (int)scale) >> 8) + ((i&&scale)?1:0);
    return j;
}


uint16_t scale16(uint16_t i, fract16 scale)
{
    uint16_t result;
    result = ((uint32_t)(i) * (1+(uint32_t)(scale))) / 65536;
    return result;
}


void nscale8x3(uint8_t& r, uint8_t& g, uint8_t& b, fract8 scale)
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


uint8_t map8(uint8_t in, uint8_t rangeStart, uint8_t rangeEnd)
{
    uint8_t rangeWidth = rangeEnd - rangeStart;
    uint8_t out = scale8( in, rangeWidth);
    out += rangeStart;
    return out;
}


uint8_t lsrX4(uint8_t dividend)
{ 
    return dividend >>= 4;
}  


uint8_t sin8(uint8_t theta)
{
    uint8_t offset = theta;
    if( theta & 0x40 ) {
        offset = (uint8_t)255 - offset;
    }
    offset &= 0x3F; // 0..63

    uint8_t secoffset  = offset & 0x0F; // 0..15
    if( theta & 0x40) ++secoffset;

    uint8_t section = offset >> 4; // 0..3
    uint8_t s2 = section * 2;
    const uint8_t* p = b_m16_interleave;
    p += s2;
    uint8_t b   =  *p;
    ++p;
    uint8_t m16 =  *p;

    uint8_t mx = (m16 * secoffset) >> 4;

    int8_t y = mx + b;
    if( theta & 0x80 ) y = -y;

    y += 128;

    return static_cast<uint8_t>(y);;
}


uint16_t sin16(uint16_t theta)
{
    static const uint16_t base[] =
    { 0, 6393, 12539, 18204, 23170, 27245, 30273, 32137 };
    static const uint8_t slope[] =
    { 49, 48, 44, 38, 31, 23, 14, 4 };

    uint16_t offset = (theta & 0x3FFF) >> 3; // 0..2047
    if( theta & 0x4000 ) offset = 2047 - offset;

    uint8_t section = offset / 256; // 0..7
    uint16_t b   = base[section];
    uint8_t  m   = slope[section];

    uint8_t secoffset8 = (uint8_t)(offset) / 2;

    uint16_t mx = m * secoffset8;
    int16_t  y  = mx + b;

    if( theta & 0x8000 ) y = -y;

    return static_cast<uint16_t>(y);
}


uint16_t beat88b(uint32_t time, accum88 beats_per_minute_88, uint32_t timebase)
{
    // BPM is 'beats per minute', or 'beats per 60000ms'.
    // To avoid using the (slower) division operator, we
    // want to convert 'beats per 60000ms' to 'beats per 65536ms',
    // and then use a simple, fast bit-shift to divide by 65536.
    //
    // The ratio 65536:60000 is 279.620266667:256; we'll call it 280:256.
    // The conversion is accurate to about 0.05%, more or less,
    // e.g. if you ask for "120 BPM", you'll get about "119.93".
    return ((time - timebase) * beats_per_minute_88 * 280) >> 16;
}

uint16_t beat16(uint32_t time, accum88 beats_per_minute, uint32_t timebase)
{
    // Convert simple 8-bit BPM's to full Q8.8 accum88's if needed
    if(beats_per_minute < 256) beats_per_minute <<= 8;
    return beat88b(time, beats_per_minute, timebase);
}

uint8_t beat8(uint32_t time, accum88 beats_per_minute, uint32_t timebase)
{
    return beat16(time, beats_per_minute, timebase) >> 8;
}


uint16_t beatsin88(uint32_t time, accum88 beats_per_minute_88, 
                   uint16_t lowest, uint16_t highest,
                   uint32_t timebase, uint16_t phase_offset)
{
    uint16_t beat = beat88b(time, beats_per_minute_88, timebase);
    uint16_t beatsin = (sin16(beat + phase_offset) + 32768);
    uint16_t rangewidth = highest - lowest;
    uint16_t scaledbeat = scale16(beatsin, rangewidth);
    uint16_t result = lowest + scaledbeat;
    return result;
}

uint16_t beatsin16(uint32_t time, accum88 beats_per_minute, 
                   uint16_t lowest, uint16_t highest,
                   uint32_t timebase, uint16_t phase_offset)
{
    uint16_t beat = beat16(time, beats_per_minute, timebase);
    uint16_t beatsin = (sin16(beat + phase_offset) + 32768);
    uint16_t rangewidth = highest - lowest;
    uint16_t scaledbeat = scale16(beatsin, rangewidth);
    uint16_t result = lowest + scaledbeat;
    return result;
}

uint8_t beatsin8(uint32_t time, accum88 beats_per_minute, 
                 uint8_t lowest, uint8_t highest,
                 uint32_t timebase, uint8_t phase_offset)
{
    uint8_t beat = beat8(time, beats_per_minute, timebase);
    uint8_t beatsin = sin8(beat + phase_offset);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatsin, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
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

void fill_solid(struct CRGB * targetArray, int numToFill,
                 const struct CRGB& color)
{
    for(int i = 0; i < numToFill; ++i) {
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


CRGB ColorFromPalette(const CRGBPalette16& pal, 
                      uint8_t index, uint8_t brightness, 
                      TBlendType blendType)
{
    if (blendType == LINEARBLEND_NOWRAP) {
        index = map8(index, 0, 239);  // Blend range is affected by lo4 blend 
                                      // of values, remap to avoid wrapping.
    }

    // hi4 = index >> 4;
    uint8_t hi4 = lsrX4(index);
    uint8_t lo4 = index & 0x0F;
    
    // const CRGB* entry = &(pal[0]) + hi4;
    // since hi4 is always 0..15, hi4 * sizeof(CRGB) can be a single-byte value,
    // instead of the two byte 'int' that avr-gcc defaults to.
    // So, we multiply hi4 X sizeof(CRGB), giving hi4XsizeofCRGB;
    uint8_t hi4XsizeofCRGB = hi4 * sizeof(CRGB);
    // We then add that to a base array pointer.
    const CRGB* entry = (CRGB*)((uint8_t*)(&(pal[0])) + hi4XsizeofCRGB);
    
    uint8_t blend = lo4 && (blendType != NOBLEND);
    
    uint8_t red1   = entry->r;
    uint8_t green1 = entry->g;
    uint8_t blue1  = entry->b;
       
    if(blend) { 
        if(hi4 == 15) { entry = &(pal[0]); } 
        else { ++entry; }

        uint8_t f2 = lo4 << 4;
        uint8_t f1 = 255 - f2;
        
        // rgb1.nscale8(f1);
        uint8_t red2 = entry->r;
        red1 = scale8(red1, f1);
        red2 = scale8(red2, f2);
        red1 += red2;

        uint8_t green2 = entry->g;
        green1 = scale8(green1, f1);
        green2 = scale8(green2, f2);
        green1 += green2;

        uint8_t blue2 = entry->b;
        blue1 = scale8(blue1,  f1);
        blue2 = scale8(blue2,  f2);
        blue1 += blue2;
    }
    
    if(brightness != 255) {
        if(brightness) {
            ++brightness; // Adjust for rounding.
            // Now, since brightness is nonzero, 
            // we don't need the full scale8_video logic;
            // we can just to scale8 and then add one 
            // (unless scale8 fixed) to all nonzero inputs.
            if(red1) {
                red1 = scale8(red1, brightness);
                ++red1;
            }
            if(green1) {
                green1 = scale8(green1, brightness);
                ++green1;
            }
            if(blue1) {
                blue1 = scale8(blue1, brightness);
                ++blue1;
            }
        } else {
            red1 = 0;
            green1 = 0;
            blue1 = 0;
        }
    }
    return CRGB(red1, green1, blue1);
}


////////////////////////////////////////////////////////////////////////////////